
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

    std::unique_ptr<WebSocket> ws(WebSocket::from_url("ws://192.168.1.9:8126/foo"));
    assert(ws);
    ws->send("goodbye");
    ws->send("hello");
	
	
	
	strmap conf;
	Serial 		ser;
	ser.exepath = utl::args2map(argc,argv,conf);

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
	
	ser.NodesMeasures.resize(8);
	std::string fullfilepath = ser.exepath + "/calib_data_node_6.txt";
	ser.NodesMeasures[6].load_calib_data(fullfilepath);
	fullfilepath = ser.exepath + "/calib_data_node_7.txt";
	ser.NodesMeasures[7].load_calib_data(fullfilepath);
	

	//#2 issue, it is likely that someone else is using the port in parallel
	//discard first trash buffer if available right after opening the port
	//this discard measure is not enough as ibberish appears still
	ser.update();
	
	while (1) 
	{
		if(ser.update())
		{
			//std::cout << "updated" << std::endl;
			ser.processBuffer();
			ser.logBuffer();
			for(std::string cl : ser.logbuf.currentlines)
			{
				ws->send(cl);
			}
			ser.clearBuffer();
		}
		usleep(100000);//100 ms
		//std::cout << "sleep" << std::endl;
		if(ws->getReadyState() != WebSocket::CLOSED) 
		{
			WebSocket::pointer wsp = &*ws; // <-- because a unique_ptr cannot be copied into a lambda
			ws->poll();
			ws->dispatch([wsp](const std::string & message) {
				printf(">>> %s\n", message.c_str());
				//if (message == "world") { wsp->close(); }
			});
		}
	}

	return 0;
}
