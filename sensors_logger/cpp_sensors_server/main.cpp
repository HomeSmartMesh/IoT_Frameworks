/*
The MIT License (MIT)

Copyright (c) 2017 Wassim Filali

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

___________________________________________________________________________________
 dependencies :
 - Poco,Boost filesystem : using The Boost Software License 1.0
 - bme_280 : with datasheet's associated rights see bme280_server.hpp
 - json.hpp MIT License Copyright (c) 2013-2017 Niels Lohmann <http://nlohmann.me>.

___________________________________________________________________________________

 start date : 20.01.2017

 webserver for managing
 * HTTP server
 * HTTP update as a client
 * Websocket server to broadcast sensors data
 * Websocket client to retreive data from other Servers
 
 
*/


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

#include "mqtt.hpp"

#include "webserver.hpp"

#include <assert.h>
#include <string>
#include <memory>

#include "db_mgr.hpp"

#include "json.hpp"
using json = nlohmann::json;

#include "log.hpp"

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
void localActions(NodeMap_t &measures,webserver_c &l_wbs)
{
	const int Rx_NodeId = 10;
	if(measures.find(Rx_NodeId) != measures.end())
	{
		if(measures[Rx_NodeId].find("Reset") != measures[Rx_NodeId].end() )
		{
			auto &vals = measures[Rx_NodeId]["Reset"];
			if(!vals.empty() && vals[0].value == 1)
			{
				l_wbs.sendLight();
				std::cout << "localActions> Node[Rx_NodeId]['Reset']:1" << std::endl;
			}
		}
	}
}

void send_RGB_Status(Serial &l_str)
{
	char text[31];
	char TargetNodeId = 0x12;
	unsigned char R = 20;
	unsigned char G = 5;
	unsigned char B = 50;
	int nbWrite = sprintf(text,"rgb 0x%02x 0x%02x 0x%02x 0x%02x\r",TargetNodeId,R,G,B);
	l_str.send(text,nbWrite);
	std::string s(text);
	std::cout << "rgb> " << s << std::endl;
}

int main( int argc, char** argv )
{
	std::cout << "______________________Config______________________" << std::endl;
	strmap conf;
	utl::args2map(argc,argv,conf);//here is checked './configfile.txt'

	Log::config(conf);

	webserver_c		wbs(conf);	//websocket manager : broadcast() and respond()
	
	Serial 			stream(conf);	// - process serial port stream : - calibrate sensors values
									// - provides ready to store measures MAP of Nodes.Sensors.Values,Timestamp
									// - If not configured to be used then the .update() polling is neutral

	mqtt_c			mqtt(conf,stream);	//MQTT client app wrapper, will attempt connection on creation if params provided
	
	db_manager_c	dbm(conf);	//adds values to files and memory db, answers requests
	
	dbm.load();
	
	//#2 issue, it is likely that someone else is using the port in parallel
	//discard first trash buffer if available right after opening the port
	//this discard measure is not enough as ibberish appears still
	
	stream.update();
	
	std::cout << "______________________Main Loop______________________" << std::endl;
#ifdef RGB_STATUS_TEST
	std::cout << "----------> send RGB" << std::endl;
	send_RGB_Status(stream);
#endif

	while (1) 
	{
		if(stream.update())
		{
			NodeMap_t measures = stream.processBuffer();
			if(measures.size() != 0)
			{
				localActions(measures,wbs);
				dbm.addMeasures(measures);	//save into the data base (memory db & files db)
				std::string jMeasures = utl::stringify(measures,"update");//data type is "update"
				wbs.broadcast(jMeasures);
				std::string jMeasures2 = utl::stringify2(measures,"update");//data type is "update"
				wbs.post(jMeasures2);//for another webserver if configured

				mqtt.publish_measures(measures);
			}
		}

		//run() contains the loop needed to process certain QoS messages and reconnect if connection lost
		mqtt.run();
		
		//5 ms : this is an unnneccessary load if the processing grows up
		usleep(5000);
		
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
