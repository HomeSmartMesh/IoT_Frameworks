

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

void db_manager_c::addMeasures(NodeMap_t &NodesSensorsVals)
{
	for(auto const& sensorsTables : NodesSensorsVals) 
	{
		std::cout << "Node: " << sensorsTables.first << std::endl;
		for(auto const& Table : sensorsTables.second) 
		{
			std::cout << "\tSensor: " << Table.first << std::endl;
			for(auto const& Measure : Table.second) 
			{
				std::cout << "\t\ttime: " << utl::getTime(Measure.time) << std::endl;
				std::cout << "\t\tval: " << Measure.value << std::endl;
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