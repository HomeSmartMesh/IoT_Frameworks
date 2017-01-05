

#include "db_mgr.hpp"

//for stdout
#include <iostream>

//Linux dependency
#include <sys/stat.h>

#include <ctime>

#include <boost/filesystem.hpp>
using namespace boost::filesystem;
using std::cout;

db_manager_c::db_manager_c()
{

}

bool db_manager_c::config(strmap &v_conf)
{
	conf = v_conf;
	if(utl::exists(conf,"dbpath"))
	{
		dbpath = conf["dbpath"];
	}
	return true;
}

bool db_manager_c::splitPath2Names(std::string path,int &year,int &month,int &NodeId,std::string &SensorName)
{
	bool res = false;
	strvect texts = utl::split(path,'/');
	int length = texts.size();
	if(length > 3)
	{
		year = std::stoi(texts[length-3]);
		month = std::stoi(texts[length-2]);
		if((month >= 1) || (month <= 12))
		{
			strvect node_sensor = utl::split(texts[length-1],'_');
			if(node_sensor.size() == 2)
			{
				std::string NodeName = node_sensor[0];
				if(NodeName.find("NodeId") == 0)
				{
					std::string name = "NodeId";
					utl::remove(name,NodeName);
					NodeId = std::stoi(NodeName);

					strvect file_ext = utl::split(node_sensor[1],'.');
					if(file_ext.size() == 2)
					{
						if(file_ext[1].find("txt") == 0)
						{
							SensorName = file_ext[0];
							res = true;
						}
					}
				}
			}
		}
		
	}
	return res;
}

void db_manager_c::load()
{
	if(utl::exists(conf,"dbloadpaths"))
	{
		cout << "loading files from dbloadpaths" << std::endl;
		path p(conf["dbloadpaths"]);
		
		try
		{
			if (exists(p) && is_directory(p))
			{
				for (directory_entry& x : directory_iterator(p))
				{
					cout << "    " << x.path() << '\n'; 
					if(is_directory(x.path()))
						for (directory_entry& f : directory_iterator(x.path()))
						{
							std::string filename = f.path().string();
							cout << "        " << filename << std::endl;
							//get Node Id and params
							int year,month;
							std::string SensorName;
							int NodeId;
							if (splitPath2Names(filename,year,month,NodeId,SensorName))
							{
								//cout << "          " << year << " ; " << month << " ; " << "NodeId" << NodeId << " ; " << SensorName << std::endl;

								std::tm timeinfo;
								timeinfo.tm_year = year - 1900;//standard say so
								timeinfo.tm_mon = month - 1;//as mon start from 0
								timeinfo.tm_isdst = 1;//true time saving applies
								
								std::ifstream ifile;
								ifile.open(filename.c_str(), std::ios::in );
								std::string line;
								while (std::getline(ifile, line))
								{
									strvect cells = utl::split(line,'\t');
									if(cells.size() == 3)//3 columns expected
									{
										std::string &day_txt = cells[0];
										std::string &time_txt = cells[1];
										std::string &value_txt = cells[2];

										sensor_measure_t Measure;
										Measure.value = std::stof(value_txt);

										timeinfo.tm_mday = std::stoi(day_txt);
										strvect timevals = utl::split(time_txt,':');
										if(timevals.size() == 3)
										{
											timeinfo.tm_hour = std::stoi(timevals[0]);
											timeinfo.tm_min = std::stoi(timevals[1]);
											timeinfo.tm_sec = std::stoi(timevals[2]);
										}
										else
										{
											std::cout << "Error: unexpected time format" << std::endl;
										}
										Measure.time = std::mktime(&timeinfo);

										Nodes[NodeId][SensorName].push_back(Measure);
										
										//std::cout << "timeinfo[" << timeinfo.tm_year << "," << timeinfo.tm_mon <<  "," << timeinfo.tm_mday << " - ";
										//std::cout << timeinfo.tm_hour << "," << timeinfo.tm_min <<  "," << timeinfo.tm_sec << "]  ";
										//std::cout << "print time: " << Measure.time << " : ";
										//utl::printTime(Measure.time);

									}
									else
									{
										std::cout << "Error: 3 columns expected" << std::endl;
									}
								}
							}
							
						}
				}
			}
			else
			  cout << p << " does not exist\n";
		}

		catch (const filesystem_error& ex)
		{
			cout << ex.what() << '\n';
		}
	}
}

void db_manager_c::print()
{
	for(auto const& sensorsTables : Nodes)
	{
		int NodeId = sensorsTables.first;
		std::string NodeName = "NodeId" + std::to_string(NodeId);
		std::cout << NodeName << std::endl;
		for(auto const& Table : sensorsTables.second) 
		{
			std::string SensorName = Table.first;
			std::cout << "\tSensor: " << SensorName << std::endl;
			for(auto const& Measure : Table.second) 
			{
				std::cout << "\t\ttime: " << utl::getTime(Measure.time) << std::endl;
				std::cout << "\t\tval: " << Measure.value << std::endl;
			}
		}
	}
}

//The year folder should be manually created !!!!
//"dbpath/year/month/NodeName_SensorName.txt"
void db_manager_c::addMeasures(NodeMap_t &NodesSensorsVals)
{
	if(dbpath.empty())
	{
		return;
	}
	for(auto const& sensorsTables : NodesSensorsVals) 
	{
		int NodeId = sensorsTables.first;
		std::string NodeName = "NodeId" + std::to_string(NodeId);
		std::cout << "dbm>" << NodeName << std::endl;
		for(auto const& Table : sensorsTables.second) 
		{
			std::string SensorName = Table.first;
			std::cout << "dbm>" << "\tSensor: " << SensorName << std::endl;
			for(auto const& Measure : Table.second) 
			{
				//--------------------------first add it to the memory DB--------------------------
				Nodes[NodeId][SensorName].push_back(Measure);
				//--------------------------then to cout--------------------------
				std::cout << "dbm>" << "\t\ttime: " << utl::getTime(Measure.time) << std::endl;
				std::cout << "dbm>" << "\t\tval: " << Measure.value << std::endl;
				//--------------------------then save it to the db files--------------------------
				//TODO this year month setting could be triggered on event to update it once.
				std::string text_year,text_month,text_day;
				utl::getYearMonthDay(Measure.time,text_year,text_month,text_day);
				std::string filepath = dbpath + text_year + "/" + text_month;
				std::string filename = filepath + "/" + NodeName + "_" + SensorName + ".txt";
				std::ofstream &ofile = Files[filename];
				//minimal test to evaluate file open only once on usual loop
				if(ofile.is_open())
				{
					std::string text_time = utl::getTime(Measure.time);
					ofile << text_day << "\t" << text_time << "\t" << Measure.value << std::endl;
				}
				else//come here only on exceptions for first time call
				{
					std::cout << "dbm>" << ">>> Opening file: " << filename << std::endl;
					ofile.open(filename.c_str(), (std::ios::out|std::ios::app) );
					if(!ofile.is_open())
					{
						std::cout << "dbm>" << "could not open sensor file: " << filename << std::endl;
						struct stat buffer;
						if(stat(filepath.c_str(), &buffer) != 0)//then Month directory does not exist
						{
							std::cout << "dbm>" << ">>> Directory does not exist: " << filepath << std::endl;
							mkdir(filepath.c_str(),ACCESSPERMS);
							if(stat(filepath.c_str(), &buffer) == 0)
							{
								std::cout << "dbm>" << ">>> Directory created,retry open file" << std::endl;
								ofile.open(filename.c_str(), (std::ios::out|std::ios::app) );
								if(ofile.is_open())
								{
									std::string text_time = utl::getTime(Measure.time);
									ofile << text_day << "\t" << text_time << "\t" << Measure.value << std::endl;
								}
								else
								{
									std::cout << "dbm>" << ">>> Still could not open sensor file !!!: " << filename << std::endl;
									//=> Error don't know what's goig on ?
								}
							}
							else
							{
								std::cout << "dbm>" << ">>> Directory still does not exist !!!! : " << filepath << std::endl;
							}
						}
					}
				}
			}
		}
	}
}

void db_manager_c::parseLines(std::vector<std::string> &lines)
{
	for(std::string line : lines)
	{
		
	}	
}
