
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
	utl::args2map(argc,argv,conf);
	Serial 		ser;

	if(utl::exists(conf,"logfile"))
	{
		std::cout << "logfile = " << conf["logfile"] << std::endl;
		ser.start_logfile(conf["logfile"]);
	}
	if(utl::exists(conf,"logout"))
	{
		std::cout << "logout = " << conf["logout"] << std::endl;
		ser.isLogOut = true;//by default
		if(conf["logout"] == "0")
		{
			ser.isLogOut = false;
		}
	}
	if(utl::exists(conf,"port"))
	{
		std::cout << "port = " << conf["port"] << std::endl;
		ser.start(conf["port"]);
	}
	else
	{
		help_arguments();
		exit(1);
	}
	
	

	//#2 issue, it is likely that someone else is using the port in parallel
	//discard first trash buffer if available right after opening the port
	//this discard measure is not enough as ibberish appears still
	ser.update();
	
	while (1) 
	{
		if(ser.update())
		{
			ser.logBuffer();
		}
		sleep(1);
		
	}

	return 0;
}
