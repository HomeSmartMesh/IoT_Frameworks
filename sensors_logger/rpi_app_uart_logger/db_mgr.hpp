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

class Node_class_c
{
public:
	int Id;
	std::map<int,db_years_c> years;
	std::map<std::string, sensor_proc_days_c> days;
};

typedef std::vector<sensors_tables_t> db_years_t;

typedef std::map<int,db_years_t> Node_t;


class db_manager_c
{
public:
	db_manager_c();
public:
	bool config(strmap &conf);
	void addMeasures(sensors_tables_t &measures);
	void parseLines(std::vector<std::string> &lines);
public:
	std::map<int,Node_t> Nodes;
//private:
	
	//strmap conf;

};

