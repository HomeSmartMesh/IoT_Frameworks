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
#include "webserver.hpp"

#include "Poco/URI.h"
//for sleep
#include <unistd.h>

#include <map>
#include <string>
#include <mutex>

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Net/NetException.h"
#include "Poco/Thread.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Format.h"
#include <iostream>

#include <chrono>
#include <thread>

#include "Poco/Process.h"
#include "Poco/Timespan.h"

using Poco::URI;
using Poco::Net::ServerSocket;
using Poco::Net::WebSocket;
using Poco::Net::WebSocketException;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::Timestamp;
using Poco::Thread;
using Poco::ThreadPool;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;

using Poco::Timespan;


typedef std::list<std::string> Messages_t;
typedef std::map<std::string,Messages_t> MessagesMap_t;


class SafeMessaging_c
{
public:
	void push(const std::string &Key, const std::string &message);
	void pull(const std::string &Key, std::string &message);
	void poll_any(std::string &Key, std::string &message);
	void push_for_all(const std::string &message);
	void remove(const std::string &Key);
private:	
	MessagesMap_t 	Messages;
	std::mutex 		messages_mutex;
};

void SafeMessaging_c::push(const std::string &Key, const std::string &message)
{
	std::lock_guard<std::mutex> guard(messages_mutex);
	Messages[Key].push_back(message);
}
void SafeMessaging_c::pull(const std::string &Key, std::string &message)
{
	std::lock_guard<std::mutex> guard(messages_mutex);
	auto &messagelist = Messages[Key];
	if(!messagelist.empty())
	{
		message = messagelist.front();
		messagelist.pop_front();
	}
}
void SafeMessaging_c::remove(const std::string &Key)
{
	std::lock_guard<std::mutex> guard(messages_mutex);
	auto it = Messages.find(Key);
	Messages.erase(it);
}

void SafeMessaging_c::poll_any(std::string &Key, std::string &message)
{
	std::lock_guard<std::mutex> guard(messages_mutex);
	for(auto &msg : Messages)
	{
		if(!msg.second.empty())
		{
			Key = msg.first;
			message = msg.second.front();
			msg.second.pop_front();
			//break; but return rather
			return;
		}
	}
}

void SafeMessaging_c::push_for_all(const std::string &message)
{
	std::lock_guard<std::mutex> guard(messages_mutex);
	for(auto &msg : Messages)
	{
		//do not care who the first is, push for all
		msg.second.push_back(message);
	}
}


SafeMessaging_c requests;
SafeMessaging_c responses;


class PageRequestHandler: public HTTPRequestHandler
{
public:
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
    {
		std::cout << "------------- => HTTP Request" << std::endl;
        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");
        std::ostream& ostr = response.send();
        ostr << "<html>";
        ostr << "<head>";
        ostr << "<title>WebSocketServer</title>";
        ostr << "</head>";
        ostr << "<body>";
        ostr << "  <h1>WebSocket Server, Websocket connection expected</h1>";
        ostr << "</body>";
        ostr << "</html>";
    }
};


class WebSocketRequestHandler: public HTTPRequestHandler
{
public:
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
    {
		std::cout << "wbs> ------------- => Websocket Request" << std::endl;
        try
        {
            WebSocket ws(request, response);
            std::cout << "wbs>  WebSocket connection established." << std::endl;
			
			std::string ClientKey = request.clientAddress().toString();
			
			std::string message;
            char buffer[2048];
            int flags = 0;//init is important as receive is not blocking
            int n = 0;
			utl::time_u start_time;
            do
            {
				//5 ms poll cycle
				if(ws.poll(5000, Poco::Net::Socket::SELECT_READ || Poco::Net::Socket::SELECT_ERROR))
				{
					start_time = utl::get_start();
					n = ws.receiveFrame(buffer, sizeof(buffer), flags);
					if(n!=0)
					{
						std::cout << Poco::format("wbs>  Frame received (length=%d, flags=0x%x).", n, unsigned(flags)) << std::endl;
						message.append(buffer,n);
						if(n == 2)
						{
							if( (buffer[0] == 3) && (buffer[1] == 233) )//likely to be a close connection notification, to be checked TODO
								n = 0;
						}
						else if(flags & Poco::Net::WebSocket::FRAME_FLAG_FIN)
						{
							requests.push(ClientKey,message);
							message.clear();
						}
					}
				}
				//now handle transmissions from the shared Map in this loop
				std::string wsResponse;//string constructor with empty content
				responses.pull(ClientKey,wsResponse);
				if(!wsResponse.empty())
				{
					ws.sendFrame(wsResponse.c_str(), wsResponse.size());//default flags are text_frame
					std::cout << "wbs> websocket response sent within "<< utl::get_stop(start_time) << std::endl;
				}
            }
            while ( (n > 0) || ((flags & WebSocket::FRAME_OP_BITMASK) != WebSocket::FRAME_OP_CLOSE) );
            std::cout << "wbs> WebSocket with Client "<< ClientKey <<" closed." << std::endl;
			requests.remove(ClientKey);
			responses.remove(ClientKey);
        }
        catch (WebSocketException& exc)
        {
            std::cout <<"wbs> "<< exc.displayText() << std::endl;
            switch (exc.code())
            {
            case WebSocket::WS_ERR_HANDSHAKE_UNSUPPORTED_VERSION:
                response.set("Sec-WebSocket-Version", WebSocket::WEBSOCKET_VERSION);
                // fallthrough
            case WebSocket::WS_ERR_NO_HANDSHAKE:
            case WebSocket::WS_ERR_HANDSHAKE_NO_VERSION:
            case WebSocket::WS_ERR_HANDSHAKE_NO_KEY:
                response.setStatusAndReason(HTTPResponse::HTTP_BAD_REQUEST);
                response.setContentLength(0);
                response.send();
                break;
            }
        }
    }
};


class RequestHandlerFactory: public HTTPRequestHandlerFactory
{
public:
    HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request)
    {
		std::cout << "wbs> ------------- => Request Factory" << std::endl;
        //Application& app = Application::instance();
        //app.logger().information("Request from " 
		std::cout << "wbs>  Request from " 
            + request.clientAddress().toString()
            + " -- method: "
            + request.getMethod()
            + " -- uri: "
            + request.getURI()
            + " -- version: "
            + request.getVersion()
			<< std::endl;
        //request is a MAP
		//for (auto &param : request){std::cout << "  " + param.first + "\t" + param.second << std::endl;}
        
        if(request.find("Upgrade") != request.end() && Poco::icompare(request["Upgrade"], "websocket") == 0)
            return new WebSocketRequestHandler;
        else
            return new PageRequestHandler;
    }
};



webserver_c::webserver_c(strmap &v_conf)
{
	p_srv = NULL;
	conf = v_conf;
}

void webserver_c::startServer()
{
	if(utl::exists(conf,"websocket_url"))
	{
		std::cout << "wsm>" << " websocket_url = " << conf["websocket_url"] << std::endl;
		
		URI uri(conf["websocket_url"]);
		int port = uri.getPort();
		std::cout << "    starting server at port : " << port << std::endl;
		p_srv = new HTTPServer(new RequestHandlerFactory, port);
		
		p_srv->start();
	}
	else
	{
		std::cout << "wsm>" << " => 'websocket_url' parameter not provided, Webserver will not be started" << std::endl;
	}
}

void webserver_c::broadcast(std::string &update)
{
	responses.push_for_all(update);
}

void webserver_c::respond(std::string &response)
{
	responses.push(currentclient,response);
}

std::string webserver_c::poll()
{
	std::string request;
	
	requests.poll_any(currentclient,request);

	return request;
}

