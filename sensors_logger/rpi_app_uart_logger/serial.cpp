
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
	measures.load_calib_data("/home/pi/IoT_Frameworks/sensors_logger/rpi_app_uart_logger/calib_data.txt");
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
		//https://github.com/wassfila/STM8_IoT_Base/issues/3
		char buferr[20];
		sprintf(buferr,"%d",errno);
		std::string err_str(buferr);
		strlog+="error "+err_str+" opening "+port_name+" : "+strerror(errno);
	}
	log(strlog);
}

bool LogBuffer_c::update(int fd)
{
	bool res = false;
	n = read (fd, buf, sizeof buf);  // read up to 100 characters if ready to read
	if(n > 0)
	{
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
	return res;
}

bool Serial::update()
{
	return logbuf.update(fd);//update the content of the local buffer from the serial device
}

void Serial::log(const std::string &str)
{
	std::string d = utl::getDay();
	std::string t = utl::getTime();
	logfile << d << "\t" << t << "\t";
	logfile << str << std::endl;

	std::cout << d << "\t" << t << "\t";
	std::cout << str << std::endl;

}

void Serial::processLine()
{
	(*logbuf.plinebuf) = '\0';
	std::string logline(logbuf.linebuf);
	//reset the line buffer pointer to the beginning of the line
	logbuf.plinebuf = logbuf.linebuf;
	
	if(isLogOut)
	{
		std::cout << logbuf.day << "\t" << logbuf.time << "\t" << logline;
	}
	if(isLogFile && logfile.is_open())
	{
		logfile << logbuf.day << "\t" << logbuf.time << "\t" << logline;
		logfile.flush();
	}
	
	strmap notif_map;
	utl::str2map( logline, notif_map);
	
	
	if(utl::exists(notif_map,"BME280"))
	{
		measures.set_all_measures_Text(notif_map["BME280"]);
		std::cout << "Temperature:\t" << measures.get_temperature() << std::endl;
		std::cout << "Humidity:\t" << measures.get_humidity() << std::endl;
		std::cout << "Pressure:\t" << measures.get_pressure() << std::endl;

		
	}
}

//we use Serial::buf for data and Serial::n for data size
void Serial::logBuffer()
{
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
				logbuf.day = utl::getDay();
				logbuf.time = utl::getTime();
				logbuf.newLine = false;
			}

			//Process characters
			if((*buf_w) == '\n')//only allowed printable character
			{
				logbuf.newLine = true;
				(*logbuf.plinebuf++) = (*buf_w);
				processLine();
			}
			else if(isp)//skip the CR and any other control
			{
				(*logbuf.plinebuf++) = (*buf_w);
			}
			//else non printable characters other than '\n' are discarded
			buf_w++;
		}
	}
}

void Serial::send(char* buffer,int size)
{
	write(fd,buffer,size);
}
