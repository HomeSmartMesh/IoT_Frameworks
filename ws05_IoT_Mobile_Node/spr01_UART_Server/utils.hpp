
#ifndef __UTILS__
#define __UTILS__

#include <string>
#include <map>


typedef std::map<std::string,std::string> strmap;

namespace utl
{
	void args2map( int argc, char** argv ,strmap &params);
	bool exists(const strmap &params,const std::string param);
	
}

#endif /*__UTILS__*/