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
 - bme_280 with datasheet's associated rights see bme280_server.hpp
___________________________________________________________________________________

 start date : 16.07.2016

 serial port cpp wrapper
 * update() to get data from the serial port buffer
 * collect data into lines and parses each completed line
 * calls the bme_280 as main sensors are calibrated from within that library
 * transforms the registers values into ready to use sensors map Nodes.Sensors.Values,Timestamp
 
*/


//for ios::out,... #issue once placed in the end of the includes, it does not recognise cout part of std::
#include <iostream>
#include <fstream>
#include <string>

#include "serial.hpp"
//for getTime
#include "utils.hpp"

#include "bme280_server.hpp"

#include <stdio.h>
#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>


#ifdef CUSTOM_SERIAL_PORT_SPEED_UNDER_INVESTIGATION
//for serial_struct
#include <serial.h>
//for warnx()
#include <err.h>



using namespace std;

void set_custom_speed(int fd,int rate)
{
	struct serial_struct serinfo;
	/* Custom divisor */
	serinfo.reserved_char[0] = 0;
	if (ioctl(fd, TIOCGSERIAL, &serinfo) < 0)
		return -1;
	serinfo.flags &= ~ASYNC_SPD_MASK;
	serinfo.flags |= ASYNC_SPD_CUST;
	serinfo.custom_divisor = (serinfo.baud_base + (rate / 2)) / rate;
	if (serinfo.custom_divisor < 1) 
		serinfo.custom_divisor = 1;
	if (ioctl(fd, TIOCSSERIAL, &serinfo) < 0)
		return -1;
	if (ioctl(fd, TIOCGSERIAL, &serinfo) < 0)
		return -1;
	if (serinfo.custom_divisor * rate != serinfo.baud_base) {
		warnx("actual baudrate is %d / %d = %f",
			  serinfo.baud_base, serinfo.custom_divisor,
			  (float)serinfo.baud_base / serinfo.custom_divisor);
	}	
}
#endif /*CUSTOM_SERIAL_PORT_SPEED_UNDER_INVESTIGATION*/

int set_interface_attribs (int fd, speed_t baudrate, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf ("error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, baudrate);
        cfsetispeed (&tty, baudrate);
		
		cfmakeraw(&tty);						//very important to avoid swapping CR to LF

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars

        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 0;            // timeout : 100 ms per unit

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                printf ("error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}


LogBuffer_c::LogBuffer_c()
{
	newLine = true;//must start with a timestamp on the first write;
	plinebuf = linebuf;//points on the beginning of the line buffer
}

Serial::Serial()
{
	isReady = false;
}
Serial::Serial(strmap &conf)
{
	if(!config(conf))
	{
		std::cout << "str> X :Serial Port not configured, will not be used" << std::endl;
		
	}
}

bool Serial::config(strmap &conf)
{
	bool res = true;
	
	//conf = v_conf;
	
	exepath = conf["exepath"];
	
	//logfile : log into a file------------------------------------------------------
	if(utl::exists(conf,"logfile"))
	{
		std::cout << "str> logfile = " << conf["logfile"] << std::endl;
		start_logfile(conf["logfile"]);
	}

	//logout is on by default, only a 0 stops it------------------------------------
	if(utl::exists(conf,"logout"))
	{
		std::cout << "str> logout = " << conf["logout"] << std::endl;
		isLogOut = true;//by default
		if(conf["logout"] == "0")
		{
			isLogOut = false;
		}
	}
	
	//serial port config------------------------------------------------------------
	if(utl::exists(conf,"port"))
	{
		std::cout << "str> port = " << conf["port"] << std::endl;
		start(conf["port"]);
	}
	else
	{
		res = false;
	}
	
	
	std::string NodesList = conf["SensorNodes"];
	strvect NodesIds = utl::split(NodesList,';');
	
	
	for(std::string str : NodesIds)
	{
		std::cout << "str> Line: " << str << std::endl;
		std::string fullfilepath = exepath + "/NodeId" + str + ".txt";
		int l_Id = std::stoi(str);
		NodesMeasures[l_Id].load_calib_data(fullfilepath);
	}
	
	isReady = res;
	return isReady;
}

void Serial::start_logfile(std::string fileName)
{
	logfile.open(fileName.c_str(), (std::ios::out|std::ios::app) );
	if(!logfile.is_open())
	{
		printf("could not open log file:%s\r\n",fileName.c_str());
	}
}

void Serial::start(std::string port_name,bool s_500)
{
	std::string strlog;
	
	isLogFile = true;
	isLogOut = true;
	
	fd = open (port_name.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
	if (fd >= 0)
	{
		strlog+= "port "+port_name+" is open @";
		if(s_500)
		{
			set_interface_attribs (fd, B500000, 0);
			strlog+="B500000";
		}
		else
		{
			set_interface_attribs (fd, B115200, 0);  // set speed to 115,200 bps, 8n1 (no parity)
			strlog+="B115200";
		}
	}
	else
	{
		strlog+="error "+std::to_string(errno)+" opening "+port_name+" : "+strerror(errno);
	}
	log(strlog);
}

bool LogBuffer_c::update(int fd)
{
	bool res = false;
	n = read (fd, buf, sizeof buf);  // read up to 100 characters if ready to read
	if(n > 0)
	{
		//std::cout << "n " << n << std::endl;
		res = true;
		//as we want to print it here, we make sure it is null terminated
		if(n < sizeof buf)
		{
			buf[n] = '\0';//null terminated string
		}
		else
		{
			buf[(sizeof buf)-1] = '\0';//must insert a null terminated string, otherwise not safe to print nor search,...
			printf("Warning : Slow app Max Buffer reached, loss of data !!!\r\n");
		}
	}
	else
	{
		//std::cout << "Nothing" << std::endl;
	}
	return res;
}

bool Serial::update()
{
	if(isReady)
	{
		return logbuf.update(fd);//update the content of the local buffer from the serial device
	}
	else
	{
		return false;
	}
}

void Serial::log(const std::string &str)
{
	std::string d = utl::getDay();
	std::string t = utl::getTime();

	if(isLogOut)
	{
		std::cout <<"str> "<< d << "\t" << t << "\t";
		std::cout << str << std::endl;
	}
	if(isLogFile && logfile.is_open())
	{
		logfile << d << "\t" << t << "\t";
		logfile << str << std::endl;
	}
	if(isLogFile && logfile.is_open())
	{
		logfile.flush();
	}
}

void Serial::logBuffer()
{
	//std::cout << "[nb lines]" << logbuf.currentlines.size() << std::endl;
	for(std::string cl : logbuf.currentlines)
	{
		log(cl);
	}
}

void Serial::clearBuffer()
{
	logbuf.currentlines.clear();
}

void LogBuffer_c::lastLinesDiscardOld()
{
	const int nbSecondsToRetain = 2;
	std::time_t right_now;
	std::time(&right_now);
	std::time_t oldTime = right_now - nbSecondsToRetain;
	bool isDone = false;
	while(!lastLines.empty() && !isDone)
	{
		auto &vLine = lastLines.front();
		if(vLine.time<=oldTime)
		{
			//std::cout << "lastline_Remove>" << vLine.line << std::endl;
			lastLines.pop_front();
		}
		else
		{
			isDone = true;
		}
	}
}

bool LogBuffer_c::lastLinesCheck(std::string &line)
{
	bool isFound = false;	
	//-------------------check duplicate
	if(!lastLines.empty())
	{
		//more likely to be in the back
		for(std::list<sensor_line_t>::reverse_iterator rit=lastLines.rbegin();
			(rit!=lastLines.rend() && !isFound);
			++rit)
		{
			if(utl::compare(line,(*rit).line))
			{
				isFound = true;
				//std::cout << "lastline_Found>" << line << std::endl;
			}
		}
	}
	return isFound;
}

void LogBuffer_c::lastLinesAdd(std::string &line)
{
	lastLines.push_back({time_now,line});
	//std::cout << "lastline_add>" << line << std::endl;
}

void Serial::processLine(NodeMap_t &nodes)
{
	//replace end of line by end of string
	(*logbuf.plinebuf) = '\0';
	std::string logline(logbuf.linebuf);
	utl::replace(logline,',',';');
	//reset the line buffer pointer to the beginning of the line
	logbuf.plinebuf = logbuf.linebuf;
	
	
	strmap notif_map;
	utl::str2map( logline, notif_map);
	if(utl::exists(notif_map,"NodeId"))
	{
		std::string t_Id = notif_map["NodeId"];
		int l_Id = std::stoi(t_Id);
		if(utl::exists(notif_map,"RTX"))//If this is a retransmitted frame
		{
			utl::TakeParseTo(logline,';');//remove the first section "RTX:ttl;"
			logbuf.lastLinesDiscardOld();
			bool isDuplicate = logbuf.lastLinesCheck(logline);
			//Here should be taken out the duplicaes,
			// and release it for normal processing otherwise
			if(isDuplicate)
			{
				std::cout << "ser> Discarded Duplicate: "<< logline << std::endl;
				return;
			}
		}
		logbuf.lastLinesAdd(logline);
		
		if(utl::exists(notif_map,"BME280"))
		{
			if(NodesMeasures.find(l_Id) != NodesMeasures.end())
			if(NodesMeasures[l_Id].isReady)
			{
				NodesMeasures[l_Id].set_all_measures_Text(notif_map["BME280"]);
				
				sensor_measure_t temperature,humidity,pressure;
				temperature.time = logbuf.time_now;
				humidity.time = logbuf.time_now;
				pressure.time = logbuf.time_now;

				temperature.value = NodesMeasures[l_Id].get_float_temperature();
				humidity.value = NodesMeasures[l_Id].get_float_humidity();
				pressure.value = NodesMeasures[l_Id].get_float_pressure();
				
				nodes[l_Id]["Temperature"].push_back(temperature);
				nodes[l_Id]["Humidity"].push_back(humidity);
				nodes[l_Id]["Pressure"].push_back(pressure);
				
				logbuf.currentlines.push_back(	logbuf.day + "\t" + logbuf.time + "\t" 
										+ "NodeId:" + std::to_string(l_Id)
										+ ";Temperature:" + NodesMeasures[l_Id].get_temperature());
										
				logbuf.currentlines.push_back(	logbuf.day + "\t" + logbuf.time + "\t" 
										+ "NodeId:" + std::to_string(l_Id)
										+ ";Humidity:" + NodesMeasures[l_Id].get_humidity());
				logbuf.currentlines.push_back(	logbuf.day + "\t" + logbuf.time + "\t" 
										+ "NodeId:" + std::to_string(l_Id)
										+ ";Pressure:" + NodesMeasures[l_Id].get_pressure());
			}
			else
			{
				std::cout << "str> Error> SensorId"<<l_Id<<" calib files not loaded" << std::endl;
			}
		}
		else if(utl::exists(notif_map,"Light"))
		{
			sensor_measure_t light;
			light.time = logbuf.time_now;

			std::string t_light = notif_map["Light"];
			int l_light = std::stoi(t_light);
			light.value = l_light;
			
			nodes[l_Id]["Light"].push_back(light);

			//yes it is a generic log
			logbuf.currentlines.push_back(	logbuf.day + "\t" + logbuf.time + "\t" + logline);
		}
		else//other logs that do not need pre-formatting
		{
			logbuf.currentlines.push_back(	logbuf.day + "\t" + logbuf.time + "\t" + logline);
		}
	}
}

//we use Serial::buf for data and Serial::n for data size
//isReady is protected by the update that has to change the .n value
NodeMap_t Serial::processBuffer()
{
	NodeMap_t nodes;
	
	clearBuffer();//return only the last gathered data
	
	//std::cout << "DBG" << std::endl;
	if(logbuf.n>0)
	{
		char * buf_w = logbuf.buf;
		char * buf_end = logbuf.buf + logbuf.n;
		
		while(buf_w != buf_end)
		{
			bool isp = isprint(*buf_w);

			//Timestamping : avoid empty lines do not create a new timestamp if the char is a line ending
			if(logbuf.newLine && isp)
			{
				time(&logbuf.time_now);//take a timestamp
				logbuf.day = utl::getDay(logbuf.time_now);
				logbuf.time = utl::getTime(logbuf.time_now);
				logbuf.newLine = false;
			}

			//Process characters
			if( ((*buf_w) == '\n') || ((*buf_w) == 10) || ((*buf_w) == 13))//only allowed printable character
			{
				logbuf.newLine = true;
				(*logbuf.plinebuf) = (*buf_w);
				processLine(nodes);
			}
			else if(isp)//skip the CR and any other control
			{
				(*logbuf.plinebuf++) = (*buf_w);
			}
			//else non printable characters other than '\n' are discarded
			buf_w++;
		}
	}
	if(!logbuf.currentlines.empty())
	{
		std::cout << "ser> Processed " << logbuf.currentlines.size() << "Line(s)" << std::endl;
	}
	return nodes;
}

void Serial::send(char* buffer,int size)
{
	write(fd,buffer,size);
}
