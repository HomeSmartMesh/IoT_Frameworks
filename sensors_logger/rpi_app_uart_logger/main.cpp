

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
	
	strmap conf;
	utl::args2map(argc,argv,conf);//here is checked './configfile.txt'

	Serial 		stream;
	websocket_manager_c wsm;
	db_manager_c		dbm;

	
	if(!stream.config(conf))
	{
		help_arguments();
		exit(1);
	}
	
	if(!wsm.config(conf))
	{
		std::cout << "could not open websocket url, will retry later..." << std::endl;
		help_arguments();
	}
	
	dbm.config(conf);
	
	dbm.load();
	
	//dbm.print();
	
	exit(0);
	
	
	//#2 issue, it is likely that someone else is using the port in parallel
	//discard first trash buffer if available right after opening the port
	//this discard measure is not enough as ibberish appears still
	stream.update();
	
	
	while (1) 
	{
		if(stream.update())
		{
			NodeMap_t NodesSensorsVals;
			
			NodesSensorsVals = stream.processBuffer();

			dbm.addMeasures(NodesSensorsVals);

			stream.logBuffer();

			wsm.sendLines(stream.logbuf.currentlines);
			
			stream.clearBuffer();
		}
		usleep(100000);//100 ms
		
		wsm.handle_messages();
	}

	return 0;
}
