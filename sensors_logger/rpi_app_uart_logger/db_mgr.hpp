/*
author : wassfila
date : 02.01.2017

database manager

 - on startup loads the db files into memory (memory usage management)
 - on sesnors string parse:
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
public:
	bool config(strmap &v_conf);
	void load();
	void handle_request(const std::string &request,std::string &response);
	void addMeasures(sensors_tables_t &measures);
	void addMeasures(NodeMap_t &NodesSensorsVals);
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

