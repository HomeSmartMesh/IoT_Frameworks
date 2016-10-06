

#include "utils.hpp"
#include <time.h>

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
	while(argc--)
	{
		std::string argv_str(*argv++);
		std::string arg_name = TakeParseTo(argv_str,'=');
		params[arg_name] = argv_str;
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
