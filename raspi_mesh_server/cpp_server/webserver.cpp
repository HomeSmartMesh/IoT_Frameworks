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

#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPClientSession.h"

#include "Poco/Net/DatagramSocket.h"
#include "Poco/Net/SocketAddress.h"

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

using Poco::Net::HTTPRequest;
using Poco::Net::HTTPClientSession;
using Poco::Net::ConnectionRefusedException;

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
		for (auto &param : request){std::cout << "        " + param.first + "\t" + param.second << std::endl;}
		std::cout << "wbs> ---> HTTP Page Request Handler" << std::endl;
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

class PostHandler: public HTTPRequestHandler
{
public:
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
    {
		for (auto &param : request){std::cout << "        " + param.first + "\t" + param.second << std::endl;}
		std::cout << "wbs> ---> HTTP POST Handler" << std::endl;
		//handle request
		std::cout << "wbs> POST Body:" << std::endl;
		std::cout << request.stream().rdbuf() << std::endl;
		//prepare the response
        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");
        std::ostream& ostr = response.send();
		ostr << "POST handled within the cpp streamer";
    }
};


class WebSocketRequestHandler: public HTTPRequestHandler
{
public:
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
    {
		std::cout << "wbs> ---> Websocket Upgrade Request" << std::endl;
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
			bool isResp = false;
            do
            {
				//5 ms poll cycle
				if(ws.poll(5000, Poco::Net::Socket::SELECT_READ || Poco::Net::Socket::SELECT_ERROR))
				{
					start_time = utl::get_start();
					isResp = true;
					n = ws.receiveFrame(buffer, sizeof(buffer), flags);
					if(n!=0)
					{
						std::cout << Poco::format("wbs>  Frame received (length=%d, flags=0x%x) from ", n, unsigned(flags)) << ClientKey << std::endl;
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
					if(isResp)
					{
						std::cout << "wbs> websocket response to "<< ClientKey <<" sent within "<< utl::get_stop(start_time) << std::endl;
						isResp = false;
					}
					else
					{
						std::cout << "wbs> broadcast to client "<< ClientKey << std::endl;
					}
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
		HTTPRequestHandler* Handler = NULL;
		std::cout << "wbs> ------------- => New HTTP request" << std::endl;
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
        
		if(request.has("Upgrade") && Poco::icompare(request["Upgrade"], "websocket") == 0)
		{
            Handler = new WebSocketRequestHandler;
		}
        else if(request.getMethod().compare("POST") == 0)
		{
            Handler = new PostHandler;
		}
        else
		{
            Handler = new PageRequestHandler;
		}
		return Handler;
    }
};



webserver_c::webserver_c(strmap &v_conf)
{
	p_srv = NULL;
	conf = v_conf;

	if(utl::exists(conf,"websocket_url"))
	{
		std::cout << "wsm> =>" << " websocket_url = " << conf["websocket_url"] << std::endl;
		
		URI uri(conf["websocket_url"]);
		int port = uri.getPort();
		std::cout << "    starting server at port : " << port << std::endl;
		p_srv = new HTTPServer(new RequestHandlerFactory, port);
		
		p_srv->start();
	}
	else
	{
		std::cout << "wsm>" << " X : 'websocket_url' parameter not provided, Webserver will not be started" << std::endl;
	}
	
	if(utl::exists(conf,"HTTP_POST"))
	{
		client.uri = URI(conf["HTTP_POST"]);
		std::string path(client.uri.getPathAndQuery());
		if (path.empty()) path = "/";
		
		
		//client.session.setHost(uri.getHost());
		//client.session.setPort(uri.getPort());
		client.request.setMethod(HTTPRequest::HTTP_POST);
		client.request.setURI(path);
		client.request.setVersion(HTTPRequest::HTTPMessage::HTTP_1_1);
		
		std::cout << "wsm> => " << "HTTP_POST => host(" << client.uri.getHost()
										<< ") port(" << client.uri.getPort()
										<< ") path(" << path
										<< ")" << std::endl;
		client.isReady = true;
	}
	else
	{
		std::cout << "wsm>" << " X : No HTTP Post Client" << std::endl;
		client.isReady = false;
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

//HTTP POST to another server
void webserver_c::post(std::string &update)
{
	if(client.isReady)
	{
		try
		{
			HTTPClientSession session(client.uri.getHost(), client.uri.getPort());
			client.request.setContentLength(update.length());
			std::ostream& post_stream = session.sendRequest(client.request);
			post_stream << update;
			std::cout << "wbs> Posted update: " << update.length() << " Bytes" << std::endl;
		}
		catch(ConnectionRefusedException& exc)
		{
			std::cout <<"wbs> Post Fail :"<< exc.displayText() << std::endl;
		}
	}
}

const char GROUP_1_ALL_OFF	= 0x46;//	           70
const char GROUP_2_ALL_ON	= 0x47;//	           71	(SYNC/PAIR RGB+W Bulb within 2 seconds of Wall Switch Power being turned ON)
const char GROUP_1_ALL_ON   = 0x45;//	           69	(SYNC/PAIR RGB+W Bulb within 2 seconds of Wall Switch Power being turned ON)
const char GROUP_2_ALL_OFF	= 0x48;//	           72
const char GROUP_3_ALL_ON	= 0x49;//	           73	(SYNC/PAIR RGB+W Bulb within 2 seconds of Wall Switch Power being turned ON)
const char GROUP_3_ALL_OFF	= 0x4A;//	           74
const char GROUP_4_ALL_ON	= 0x4B;//	           75	(SYNC/PAIR RGB+W Bulb within 2 seconds of Wall Switch Power being turned ON)
const char GROUP_4_ALL_OFF	= 0x4C;//	           76

const char BRIGHTNESS       = 0x4E;

void webserver_c::sendLight()
{
	std::cout << " => send Light" << std::endl;

	Poco::Net::SocketAddress local_Add(Poco::Net::IPAddress(), 8820);
    Poco::Net::DatagramSocket dSocket(local_Add);
	Poco::Net::SocketAddress lightbridge_Add("10.0.0.8", 8899);
	
	unsigned char message[3];
	message[0] = GROUP_4_ALL_ON;//all on 
	message[1] = 0;	//Max Bright Val
	message[2] = 0x55;	

	//3x times	
	for(int i=0;i<3;i++)
	{
		int sentB = dSocket.sendTo(message, 3, lightbridge_Add);
		std::cout << "sent> " << sentB << std::endl;
		usleep(200000);
	}

	message[0] = BRIGHTNESS;	//Brightness
	message[1] = 27;	//Max Bright Val
	message[2] = 0x55;	
	//3x times	
	for(int i=0;i<3;i++)
	{
		int sentB = dSocket.sendTo(message, 3, lightbridge_Add);
		std::cout << "sent> " << sentB << std::endl;
		usleep(200000);
	}

}