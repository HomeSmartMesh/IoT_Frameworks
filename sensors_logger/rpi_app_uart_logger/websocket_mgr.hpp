/*
author : wassfila
date : 02.01.2017

websocket manager


*/

#include "easywsclient.hpp"

//for strmap
#include "utils.hpp"
#include <string>
#include <vector>


class websocket_manager_c
{
public:
	websocket_manager_c
	{
		wsp = NULL;
	};
public:
	void config(strmap &v_conf);
	void sendLines(std::vector<std::string> &lines);
	void handle_messages();
private:
	WebSocket::pointer wsp;
	strmap conf;

};

