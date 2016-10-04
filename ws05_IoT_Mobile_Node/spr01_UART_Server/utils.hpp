
#ifndef __UTILS__
#define __UTILS__

#include <string>
#include <map>


typedef std::map<std::string,std::string> strmap;

namespace utl
{
	void args2map( int argc, char** argv ,strmap &params);
	bool exists(const strmap &params,const std::string param);
	std::string ParseRemTill(std::string &str,char sep,bool &found);
	std::string	getTime();
	std::string	getDay();
	
}

#endif /*__UTILS__*/