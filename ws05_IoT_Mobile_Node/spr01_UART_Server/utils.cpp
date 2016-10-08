

#include "utils.hpp"
#include <time.h>

//for file
#include <fstream>
#include <iostream>

using namespace std;

std::string TakeParseToLast(std::string &str,char sep)
{
	size_t first = str.find_last_of(sep);
	std::string Parsed = str.substr(0 , first);
	str = str.substr(first+1 ,str.length());
	return Parsed;
}

std::string TakeParseTo(std::string &str,char sep)
{
	size_t first = str.find_first_of(sep);
	std::string Parsed = str.substr(0 , first);
	str = str.substr(first+1 ,str.length());
	return Parsed;
}

std::string utl::ParseRemTill(std::string &str,char sep,bool &found)
{
	size_t first = str.find_first_of(sep);
	if(first!=std::string::npos)
	{
		found = true;
	}
	else
	{
		found = false;
	}
	std::string Parsed = str.substr(0 , first);
	str = str.substr(first+1 ,str.length());
	return Parsed;
}

void utl::args2map( int argc, char** argv ,strmap &params)
{
	std::string exepath(*argv);//placed here before pointer is shifted
	while(argc--)
	{
		std::string argv_str(*argv++);
		//to debug parameters list
		//std::cout << "[" << argv_str << "]" << std::endl;
		std::string arg_name = TakeParseTo(argv_str,'=');
		params[arg_name] = argv_str;
	}
	//once all params in cmd line parsed check if a config file is available
	std::ifstream 	configfile;
	if(!utl::exists(params,"configfile"))//the parameter 'configfile' was not passed, so check the default one
	{
		exepath = TakeParseToLast(exepath,'/');
		params["configfile"] = exepath + "/configfile.txt";
	}
	configfile.open(params["configfile"].c_str());
	if(configfile.is_open())
	{
		std::cout << "configfile = "<< params["configfile"] << std::endl;
		string line;
		while ( getline (configfile,line) )
		{
			line = TakeParseTo(line,'#');//allow comment character
			std::string arg_name = TakeParseTo(line,'=');
			if(!utl::exists(params,arg_name))//if the new parameter from the file was not already provided in cmd line
			{
			params[arg_name] = TakeParseTo(line,'\r');//take the windows line ending out
			}//else ignore it and keep precedence for cmd line
			else
			{
				std::cout << arg_name << " => command line parameter takes over definition in file" << std::endl;
			}
		}
	}
	else
	{
		std::cout << "No Config file in cmd line and not found in exe dir: " <<  params["configfile"] << endl;
	}
	
}

bool utl::exists(const strmap &params,const std::string param)
{
	return params.find(param) != params.end();
}

std::string	utl::getTime()
{
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];

	time (&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer,80,"%Hh%Mmn%Ss",timeinfo);
	std::string str(buffer);
	return str;	
}
std::string	utl::getDay()
{
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];

	time (&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer,80,"%Y.%m.%d",timeinfo);
	std::string str(buffer);
	return str;	
}
