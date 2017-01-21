

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

#include "webserver.hpp"

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

int main( int argc, char** argv )
{
	std::cout << "______________________Config______________________" << std::endl;
	strmap conf;
	utl::args2map(argc,argv,conf);//here is checked './configfile.txt'

	webserver_c		wbs(conf);	//websocket manager : broadcast() and respond()
	
	wbs.startServer();
	
	Serial 			stream(conf);	// - process serial port stream : - calibrate sensors values
									// - provides ready to store measures MAP of Nodes.Sensors.Values,Timestamp
									// - If not configured to be used then the .update() polling is neutral

	
	db_manager_c	dbm(conf);	//adds values to files and memory db, answers requests
	
	dbm.load();
	
	//#2 issue, it is likely that someone else is using the port in parallel
	//discard first trash buffer if available right after opening the port
	//this discard measure is not enough as ibberish appears still
	
	stream.update();
	
	std::cout << "______________________Main Loop______________________" << std::endl;
	
	while (1) 
	{
		if(stream.update())
		{
			NodeMap_t measures = stream.processBuffer();
			if(measures.size() != 0)
			{
				dbm.addMeasures(measures);	//save into the data base (memory db & files db)
				std::string jMeasures = utl::stringify(measures,"update");//data type is "update"
				wbs.broadcast(jMeasures);
			}
		}
		
		usleep(100000);//100 ms : this is an unnneccessary load if the processing grows up
		
		std::string request = wbs.poll();
		
		if(!request.empty())
		{
			std::string response;
			dbm.handle_request(request,response);
			if(!response.empty())
			{
				wbs.respond(response);
			}
		}
		
	}

	return 0;
}
