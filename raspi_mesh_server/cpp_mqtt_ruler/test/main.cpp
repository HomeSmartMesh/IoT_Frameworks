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

 start date : 04.03.2017

 test for simplest mosquitto transactions
 
 
*/


//for usleep() sleep()
#include <mosquittopp.h>
//#include <mosquitto.h>
//for printf
#include <stdio.h>
//for stdout
#include <iostream>
//for file
#include <fstream>

#include <string>
#include <cstring>

class mqtt_test : public mosqpp::mosquittopp
{
	public:
		mqtt_test(const char *id, const char *host, int port);
		~mqtt_test();

		void say_hello();
		void on_connect(int rc);
		void on_message(const struct mosquitto_message *message);
		void on_subscribe(int mid, int qos_count, const int *granted_qos);
};

mqtt_test::mqtt_test(const char *id, const char *host, int port) : mosquittopp(id)
{
	int keepalive = 60;
	connect(host, port, keepalive);
	std::cout << "Construction" << std::endl;
};

mqtt_test::~mqtt_test()
{
}

void mqtt_test::on_connect(int rc)
{
	std::cout << "My MQTT client is Connected (" <<  rc << ")" << std::endl;
	if(rc == 0)
	{
		//subscribe(NULL, "test/topic");
	}
}

void mqtt_test::on_message(const struct mosquitto_message *message)
{
		std::cout << "Received message with size " << message->payloadlen << std::endl;
}

void mqtt_test::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
	std::cout << "My MQTT client is Subscribed id "  << mid << std::endl;
}

void mqtt_test::say_hello()
{
	std::string hello_str = "Hello from my test";
	publish(NULL, "test/topic", hello_str.size(), hello_str.c_str());
	std::cout << "published hello message "  << hello_str << std::endl;
}


int main( int argc, char** argv )
{
	std::cout << "Mosquitto test" << std::endl;

	mosqpp::lib_init();

	mqtt_test MyMqtt("TestId","localhost",1883);

	MyMqtt.say_hello();

	int haveToReconnect;
	while(true)
	{
		haveToReconnect = MyMqtt.loop();
		if(haveToReconnect)
		{
			MyMqtt.reconnect();
		}
	}
	mosqpp::lib_cleanup();

	return 0;
}
