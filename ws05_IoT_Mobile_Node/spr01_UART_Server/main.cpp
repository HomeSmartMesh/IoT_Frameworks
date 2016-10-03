
//for usleep
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
		std::cout << "help file missing reinstall SW\r\n";
	}
	
}


int main( int argc, char** argv )
{
	strmap conf;
	utl::args2map(argc,argv,conf);
	Serial 		ser;

	if(utl::exists(conf,"port"))
	{
		std::cout << "port was parsed as : " << conf["port"] << "\r\n";
		ser.start(conf["port"]);
	}
	else
	{
		help_arguments();
		exit(1);
	}
	if(utl::exists(conf,"logfile"))
	{
		std::cout << "logfile was parsed as : " << conf["logfile"] << "\r\n";
		ser.start_logfile(conf["logfile"]);
	}
	
	

	
	//std::cout << MAGENTA << "Colored " << CYAN << "Text" << RESET << std::endl;
	
	while (1) 
	{
		//display Received log
		if(ser.update())
		{
			ser.log();
			ser.print();
		}
		usleep (10000);
		
	}

	return 0;
}
