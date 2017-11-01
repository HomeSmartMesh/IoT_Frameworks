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
 - Poco is using The Boost Software License 1.0
___________________________________________________________________________________

 start date : 20.01.2017

 webserver for managing
 * HTTP server
 * HTTP update as a client
 * Websocket server to broadcast sensors data
 * Websocket client to retreive data from other Servers

*/


//for strmap
#include "utils.hpp"
#include <string>
#include <vector>

#include "Poco/Net/HTTPServer.h"
//#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/URI.h"

class client_c
{
public:
	client_c(){isReady = false;};
public:
	//Poco::Net::HTTPClientSession 	session;
	Poco::Net::HTTPRequest 			request;
	Poco::URI						uri;
	bool							isReady;
};

class webserver_c
{
public:
	webserver_c();
	webserver_c(strmap &v_conf);
public:
	void 		sendLines(std::vector<std::string> &lines);
	void 		broadcast(std::string &update);			//to all connected clients
	void 		post(std::string &update);				//HTTP POST to another server
	std::string poll();										//get a request from the first client which list is not empty
	void		sendLight();
															//could be improved by a round robin to avoid Dos attacks
	void 		respond(std::string &response);				//to the current client that sent the request
	std::string currentclient;								//the poll() respond() is linked to the currentclient
private:
	Poco::Net::HTTPServer *p_srv;
	client_c				client;
	strmap conf;

};

