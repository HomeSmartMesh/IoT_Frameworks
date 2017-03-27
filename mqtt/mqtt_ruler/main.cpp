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
 - Mosquitto

___________________________________________________________________________________

 start date : 27.03.2017

 Ruler for mosquitto subscribe and publish
 
*/


//for usleep() sleep()
#include <unistd.h>

#include <mosquittopp.h>
//for printf
#include <stdio.h>
//for stdout
#include <iostream>
//for file
#include <fstream>

#include <string>
#include <cstring>

//for config
#include "utils.hpp"
//for Log::cout
#include "log.hpp"


typedef struct RGB_data
{
	int R,G,B;
	int NodeId;
}RGB_data_t;

class mqtt_c : public mosqpp::mosquittopp
{
	public:
		mqtt_c(strmap &conf);
        void run();
		void on_connect(int rc);
		void on_message(const struct mosquitto_message *message);
		void on_subscribe(int mid, int qos_count, const int *granted_qos);
        bool isReady;

        void publish_humidity_status(int NodeId,float humidity);
	
};

void mqtt_c::publish_humidity_status(int NodeId,float humidity)
{
	if(NodeId == 7)//bathroom
	{
		std::string topic = "Nodes/12/RGB";
		unsigned char red = 0, green = 0, blue = 5;
		if(humidity > 50)
		{
			float red_f = 255 * (humidity - 50)/50;
			if (red_f > 255)
			{
				red_f = 255;
			}
			red = static_cast<unsigned char>(red_f);
		}
		char value[10];
		snprintf(value, sizeof value, "#%02x%02x%02x", red, green, blue);
		int status = publish(NULL,topic.c_str(),7,value);// # + 6 chars
		if(status == MOSQ_ERR_SUCCESS)
		{
			Log::cout << "mqtt" << "\t" << "posted: " << value << "\ton topic:" << topic << Log::Debug();
		}
		else
		{
			Log::cout << "mqtt" << "\t" << "publish Fail" << Log::Error();
		}

	}

}

//one per app 
mqtt_c::mqtt_c(strmap &conf) : mosquittopp("hsm_ruler")
{
    isReady = false;
	//logfile : log into a file------------------------------------------------------
	if(utl::exists(conf,"mqtt_host"))
	{
        if(utl::exists(conf,"mqtt_port"))
        {
        	mosqpp::lib_init();
            isReady = true;
            int keepalive = 60;
            int port = std::stoi(conf["mqtt_port"]);
            int res = connect(conf["mqtt_host"].c_str(), port, keepalive);
            if(res == MOSQ_ERR_SUCCESS)
            {
                Log::cout << "mqtt"<<"\t"<<"connecting to " << conf["mqtt_host"] << " : " << conf["mqtt_port"] << Log::Info();
            }
            else
            {
                Log::cout << "mqtt"<<"\t"<<"X Failed to connect" << Log::Error();
            }
        }
	}
    if(!isReady)
    {
        Log::cout << "mqtt"<<"\t"<<"X Not Configured, will not be used" << Log::Info();
    }

};

void mqtt_c::on_connect(int rc)
{
    Log::cout << "mqtt"<<"\t"<<"connected id(" << rc << ")" << Log::Info();

    subscribe(NULL,"Nodes/+/Humidity");
}

void mqtt_c::on_message(const struct mosquitto_message *message)
{
    std::string msg(static_cast<const char*>(message->payload) );
    std::string topic(message->topic);
    if(topic.find("Nodes/") == 0)
    {
		std::string Text = topic;
		//the topic is "Node/6/Humidity"
		utl::TakeParseTo(Text,'/');//remove first section 
		std::string Id = utl::TakeParseTo(Text,'/');//take the second element
		int NodeId = std::stoi(Id);
		float humidity_val = std::stof(msg);
		publish_humidity_status(NodeId,humidity_val);
		Log::cout << "mqtt"<<"\t"<<"=> NodeId:"<< NodeId << " Humidity: "<< humidity_val << Log::Debug();
    }
    else
    {
        Log::cout << "mqtt"<<"\t"<<"unexpected Topic : "<< topic << Log::Debug();
        Log::cout << "mqtt"<<"\t"<<"=> "<< msg<< Log::Debug();
    }
}

void mqtt_c::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
    Log::cout << "mqtt"<<"\t"<<"subscribed to message id(" << mid << ")" << Log::Info();
}


void mqtt_c::run()
{
    if(isReady)
    {
        int status = loop(0);//immediate return, 
        if(status != MOSQ_ERR_SUCCESS)
        {
            reconnect();
        }
    }
}

int main( int argc, char** argv )
{

	std::cout << "______________________Config______________________" << std::endl;
	strmap conf;
	utl::args2map(argc,argv,conf);//here is checked './configfile.txt'

	Log::config(conf);

	mqtt_c			mqtt(conf);	//MQTT client app wrapper, will attempt connection on creation if params provided

	mosqpp::lib_init();

	int haveToReconnect;
	while(true)
	{
		mqtt.run();
		usleep(5000);
	}

	return 0;
}
