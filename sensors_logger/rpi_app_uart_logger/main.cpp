
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

#include "easywsclient.hpp"
#include <assert.h>
#include <string>
#include <memory>


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
    using easywsclient::WebSocket;
	
	WebSocket::pointer wsp = NULL;
	
	
	
	strmap conf;
	Serial 		ser;
	ser.exepath = utl::args2map(argc,argv,conf);//here is checked './configfile.txt'

	//logfile : log into a file------------------------------------------------------
	if(utl::exists(conf,"logfile"))
	{
		std::cout << "logfile = " << conf["logfile"] << std::endl;
		ser.start_logfile(conf["logfile"]);
	}

	//logout is on by default, only a 0 stops it------------------------------------
	if(utl::exists(conf,"logout"))
	{
		std::cout << "logout = " << conf["logout"] << std::endl;
		ser.isLogOut = true;//by default
		if(conf["logout"] == "0")
		{
			ser.isLogOut = false;
		}
	}
	
	//serial port config------------------------------------------------------------
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
	
	//websocket config---------------------------------------------------------------
	if(utl::exists(conf,"websocket_url"))
	{
		std::cout << "websocket_url = " << conf["websocket_url"] << std::endl;
		wsp = WebSocket::from_url(conf["websocket_url"]);
		if(!wsp)
		{
			std::cout << "could not open websocket url" << std::endl;
			help_arguments();
			std::cout << "Starting without websocket, will be checked later..." << std::endl;
		}
	}
	
	
	
	
	ser.NodesMeasures.resize(8);
	std::string fullfilepath = ser.exepath + "/calib_data_node_6.txt";
	ser.NodesMeasures[6].load_calib_data(fullfilepath);
	fullfilepath = ser.exepath + "/calib_data_node_7.txt";
	ser.NodesMeasures[7].load_calib_data(fullfilepath);
	

	//#2 issue, it is likely that someone else is using the port in parallel
	//discard first trash buffer if available right after opening the port
	//this discard measure is not enough as ibberish appears still
	ser.update();
	
	int ws_monitor_count = 0;
	while (1) 
	{
		if(ser.update())
		{
			//std::cout << "updated" << std::endl;
			ser.processBuffer();
			ser.logBuffer();
			//send all log lines through the websocket
			if(wsp)
			if(wsp->getReadyState() != WebSocket::CLOSED) 
			{
				for(std::string cl : ser.logbuf.currentlines)
				{
					wsp->send(cl);
				}
			}
			ser.clearBuffer();
		}
		usleep(100000);//100 ms
		
		//handle the websocket received messages
		if(wsp)
		{
			if(wsp->getReadyState() != WebSocket::CLOSED)
			{
				//WebSocket::pointer wsp = &*ws; // <-- because a unique_ptr cannot be copied into a lambda
				wsp->poll();
				wsp->dispatch([wsp](const std::string & message) 
				{
					std::cout << "ws_server>" << message << std::endl;
				}			);
			}
			else
			{
				wsp = 0;//kill the websocket so that it's checked later for reconnections
			}
		}
		else
		{
			ws_monitor_count++;
			if(ws_monitor_count == 100)// check every 10 s
			{
				ws_monitor_count = 0;
				if(utl::exists(conf,"websocket_url"))
				{
					std::cout << "websocket_url = " << conf["websocket_url"] << std::endl;
					wsp = WebSocket::from_url(conf["websocket_url"]);
					if(!wsp)
					{
						std::cout << "could not open websocket url" << std::endl;
						std::cout << "Continuing without websocket, will be checked later..." << std::endl;
					}
				}
			}
		}
	}

	return 0;
}
