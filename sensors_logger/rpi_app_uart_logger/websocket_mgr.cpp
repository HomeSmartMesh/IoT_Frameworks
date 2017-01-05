

#include "websocket_mgr.hpp"

//for stdout
#include <iostream>

using easywsclient::WebSocket;

easywsclient::WebSocket::pointer wsp;

websocket_manager_c::websocket_manager_c()
{
	wsp = NULL;
	ws_monitor_count = 0;
}

bool websocket_manager_c::config(strmap &v_conf)
{
	conf = v_conf;
	if(utl::exists(conf,"websocket_url"))
	{
		std::cout << "wsm>" << "websocket_url = " << conf["websocket_url"] << std::endl;
		wsp = WebSocket::from_url(conf["websocket_url"]);
	}
	return (wsp!=NULL);
}

void websocket_manager_c::sendLines(std::vector<std::string> &lines)
{
	if(wsp)
	if(wsp->getReadyState() != WebSocket::CLOSED) 
	{
		for(std::string cl : lines)
		{
			wsp->send(cl);
		}
	}
	
}

void websocket_manager_c::run()
{
	if(wsp)
	{
		if(wsp->getReadyState() != WebSocket::CLOSED)
		{
			//WebSocket::pointer wsp = &*ws; // <-- because a unique_ptr cannot be copied into a lambda
			wsp->poll();
			wsp->dispatch([](const std::string & message) 
			{
				std::cout << "wsm>" << message << std::endl;
			}			);
		}
		else
		{
			//TODO, WARNING, scalability, check memory leaks
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
				std::cout << "wsm>"  << "websocket_url = " << conf["websocket_url"] << std::endl;
				wsp = WebSocket::from_url(conf["websocket_url"]);
				if(!wsp)
				{
					std::cout << "wsm>"  << "could not open websocket url" << std::endl;
					std::cout << "wsm>"  << "Continuing without websocket, will be checked later..." << std::endl;
				}
			}
		}
	}
	
}