

#include "db_mgr.hpp"

//for stdout
#include <iostream>

db_manager_c::db_manager_c()
{

}

bool db_manager_c::config(strmap &conf)
{
	//conf = v_conf;
	if(utl::exists(conf,"websocket_url"))
	{
		std::cout << "websocket_url = " << conf["websocket_url"] << std::endl;
	}
	return true;
}
