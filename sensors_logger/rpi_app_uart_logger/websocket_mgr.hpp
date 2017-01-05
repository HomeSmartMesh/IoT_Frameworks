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
	websocket_manager_c();
public:
	bool 		config(strmap &v_conf);
	void 		sendLines(std::vector<std::string> &lines);
	void 		send(std::string &response);
	std::string poll();
	void 		check_connection();
private:
	
	strmap conf;
	int ws_monitor_count;

};

