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
 - sudo apt-get install libmosquitto-dev
 - sudo apt-get install libmosquittopp-dev
___________________________________________________________________________________

 start date : 04.03.2017

 mqtt application wrapper
 
*/

#include "mqtt.hpp"

//for printf
#include <stdio.h>
//for stdout
#include <iostream>

#include <string>
#include <cstring>

//for config
#include "utils.hpp"
//for Log::cout
#include "log.hpp"


//one per app 
mqtt_c::mqtt_c(strmap &conf) : mosquittopp("streamer")
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

void mqtt_c::on_connect(int rc)
{
    Log::cout << "mqtt"<<"\t"<<"connected id(" << rc << ")" << Log::Info();
}

void mqtt_c::on_message(const struct mosquitto_message *message)
{
    Log::cout << "mqtt"<<"\t"<<"received message with length(" << message->payloadlen << ")" << Log::Debug();
}

void mqtt_c::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
    Log::cout << "mqtt"<<"\t"<<"subscribed to message id(" << mid << ")" << Log::Info();
}

void mqtt_c::say_hello()
{
	std::string hello_str = "Hello from my test";
	publish(NULL, "test/topic", hello_str.size(), hello_str.c_str());
	std::cout << "published hello message "  << hello_str << std::endl;
}

void mqtt_c::publish_measures(NodeMap_t &NodesSensorsVals)
{
	if(!isReady)
	{
		return;
	}
	for(auto const& sensorsTables : NodesSensorsVals) 
	{
		int NodeId = sensorsTables.first;
		std::string Node = std::to_string(NodeId);
		for(auto const& Table : sensorsTables.second) 
		{
			std::string SensorName = Table.first;
			for(auto const& Measure : Table.second) 
			{
                std::string topic = "Nodes/" + Node + "/" + SensorName;
                std::string Value = std::to_string(Measure.value);
                int status = publish(NULL,topic.c_str(),Value.size(),Value.c_str());
                if(status == MOSQ_ERR_SUCCESS)
                {
                    Log::cout << "mqtt" << "\t" << "publish OK" << Log::Debug();
                }
                else
                {
                    Log::cout << "mqtt" << "\t" << "publish Fail" << Log::Error();
                }
			}
		}
	}
}