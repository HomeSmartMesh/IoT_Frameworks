

#include "utils.hpp"


std::string ParseRemTill(std::string &str,char sep)
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
		std::string arg_name = ParseRemTill(argv_str,':');
		params[arg_name] = argv_str;
	}
}

bool utl::exists(const strmap &params,const std::string param)
{
	return params.find(param) != params.end();
}