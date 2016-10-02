
//for usleep
#include <unistd.h>
//for printf
#include <stdio.h>
//for stdout
#include <iostream>

//for abs
#include <cmath>

//for exit()
#include <cstdlib>

#include "serial.hpp"
#include "utils.hpp"

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
		std::cout << "argumentrequired 'port:/dev/ttyUSB0'\r\n";
		exit(1);
	}
	
	

	
	//std::cout << MAGENTA << "Colored " << CYAN << "Text" << RESET << std::endl;
	
	while (1) 
	{
		//display Received log
		if(ser.update())
		{
			ser.print();
		}
		usleep (10000);
		
	}

	return 0;
}
