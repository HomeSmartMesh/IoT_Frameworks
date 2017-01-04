

#include "db_mgr.hpp"

//for stdout
#include <iostream>

//Linux dependency
#include <sys/stat.h>

db_manager_c::db_manager_c()
{

}

bool db_manager_c::config(strmap &conf)
{
	//conf = v_conf;
	if(utl::exists(conf,"dbpath"))
	{
		dbpath = conf["dbpath"];
	}
	return true;
}

void db_manager_c::load()
{
	
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
		std::string NodeName = "NodeId" + std::to_string(sensorsTables.first);
		std::cout << NodeName << std::endl;
		for(auto const& Table : sensorsTables.second) 
		{
			std::string SensorName = Table.first;
			std::cout << "\tSensor: " << SensorName << std::endl;
			for(auto const& Measure : Table.second) 
			{
				std::cout << "\t\ttime: " << utl::getTime(Measure.time) << std::endl;
				std::cout << "\t\tval: " << Measure.value << std::endl;
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
					std::cout << ">>> Opening file: " << filename << std::endl;
					ofile.open(filename.c_str(), (std::ios::out|std::ios::app) );
					if(!ofile.is_open())
					{
						std::cout << "could not open sensor file: " << filename << std::endl;
						struct stat buffer;
						if(stat(filepath.c_str(), &buffer) != 0)//then Month directory does not exist
						{
							std::cout << ">>> Directory does not exist: " << filepath << std::endl;
							mkdir(filepath.c_str(),ACCESSPERMS);
							if(stat(filepath.c_str(), &buffer) == 0)
							{
								std::cout << ">>> Directory created,retry open file" << std::endl;
								ofile.open(filename.c_str(), (std::ios::out|std::ios::app) );
								if(ofile.is_open())
								{
									std::string text_time = utl::getTime(Measure.time);
									ofile << text_day << "\t" << text_time << "\t" << Measure.value << std::endl;
								}
								else
								{
									std::cout << ">>> Still could not open sensor file !!!: " << filename << std::endl;
									//=> Error don't know what's goig on ?
								}
							}
							else
							{
								std::cout << ">>> Directory still does not exist !!!! : " << filepath << std::endl;
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