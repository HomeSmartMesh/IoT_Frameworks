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
 - Boost Filesystem : The Boost Software License 1.0
___________________________________________________________________________________

date : 02.01.2017

database manager : RAM and Files mirroring

 - on startup loads the db files into memory (memory usage management)
 - on sensors string parse:
   - add entry in memory structure
   - create new directory path, and write entry in file

*/

//for strmap
#include "utils.hpp"
#include <string>
#include <vector>
//for file
#include <fstream>

class proc_day_entrie_c
{
public:
	int	day;
	float	average;
	float	min;
	float	max;
	float	var;
};

typedef std::vector<proc_day_entrie_c> sensor_proc_days_c;

typedef std::vector<sensors_tables_t> db_years_c;

class Node_class_c
{
public:
	int Id;
	std::map<int,db_years_c> years;
	std::map<std::string, sensor_proc_days_c> days;
};

typedef std::vector<sensors_tables_t> db_years_t;

typedef std::map<int,db_years_t> Node_t;

typedef std::map<std::string,std::ofstream> db_files_list_t;

//typedef std::vector<db_files_list_t> db_files_months_t;
//typedef std::map<int,db_files_months_t> Files_Map_t;


class db_manager_c
{
public:
	db_manager_c();
	db_manager_c(strmap &v_conf);
public:
	bool config(strmap &v_conf);
	void load();
	void handle_request(const std::string &request,std::string &response);
	void addMeasures(NodeMap_t &NodesSensorsVals);
	void getMeasures(int NodeId,std::string SensorName, time_t start, time_t stop,NodeMap_t &ResVals);
	void getUpdate(NodeMap_t &ResVals);
	void print();
private:
	bool splitPath2Names(std::string path,int &year,int &month,int &NodeName,std::string &SensorName);
public:
	NodeMap_t 		Nodes;
	std::string 	dbpath;
	db_files_list_t	Files;
private:
	
	strmap conf;

};

