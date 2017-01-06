

//for usleep() sleep()
#include <unistd.h>
//for printf
#include <stdio.h>
//for stdout
#include <iostream>
//for file
#include <fstream>
//for abs
#include <cmath>

//for exit()
#include <cstdlib>


#include "serial.hpp"

#include "utils.hpp"

#include "websocket_mgr.hpp"

#include <assert.h>
#include <string>
#include <memory>

#include "db_mgr.hpp"

#include "json.hpp"
using json = nlohmann::json;


using namespace std;

void help_arguments()
{
	string line;
	ifstream helpFile("help.txt");
	if (helpFile.is_open())
	{
		while ( getline (helpFile,line) )
		{
		  cout << line << '\n';
		}
		helpFile.close();
	}
	else
	{
		std::cout << "help file missing reinstall SW" << std::endl;
	}
	
}

void json_test()
{
	json Nodes;
	sensor_measure_t m;
	time(&m.time);
	m.value = 19.5;
	//when using numbers Nodes[5], it converts it into an array with 4 times null,null,...
	Nodes["5"]["Temperature"]["Time"].push_back(m.time);
	Nodes["5"]["Temperature"]["Value"].push_back(m.value);
	m.value = 20.5;
	Nodes["5"]["Temperature"]["Time"].push_back(m.time);
	Nodes["5"]["Temperature"]["Value"].push_back(m.value);
	m.value = 980.5;
	Nodes["6"]["Pressure"]["Time"].push_back(m.time);
	Nodes["6"]["Pressure"]["Value"].push_back(m.value);
	
	std::cout << Nodes.dump(4) << std::endl;
	
}

int main( int argc, char** argv )
{
	strmap conf;
	utl::args2map(argc,argv,conf);//here is checked './configfile.txt'

	Serial 					stream;	//process serial port stream : calibrate sensor values
	websocket_manager_c 	wsm;	//websocket manager : send, receive, reconnections
	db_manager_c			dbm;	//adds values to files and memory db, answers requests

	
	if(!stream.config(conf))//if the configuration failes to get what is needed (serial port number)
	{
		help_arguments();	//then remind of the command line helps
		exit(1);			//and exit this application
	}
	
	if(!wsm.config(conf))
	{
		std::cout << "wsm>" << "could not open websocket url, will retry later..." << std::endl;
		help_arguments();
	}
	
	dbm.config(conf);
	
	dbm.load();
	
	//DEBUG: dbm.print();
	
	//#2 issue, it is likely that someone else is using the port in parallel
	//discard first trash buffer if available right after opening the port
	//this discard measure is not enough as ibberish appears still
	stream.update();
	
	
	while (1) 
	{
		if(stream.update())
		{
			NodeMap_t measures = stream.processBuffer();

			dbm.addMeasures(measures);	//save into the data base (memory db & files db)

			std::string jMeasures = utl::stringify(measures,"update");//data type is "update"
			
			wsm.send(jMeasures);
		}
		
		usleep(100000);//100 ms : this is an unnneccessary load if the processing grows up
		
		std::string request = wsm.poll();
		
		if(!request.empty())
		{
			std::string response;
			dbm.handle_request(request,response);
			wsm.send(response);
		}
		
		wsm.check_connection();//carefull !! loop count depend on time 100 ms
	}

	return 0;
}
