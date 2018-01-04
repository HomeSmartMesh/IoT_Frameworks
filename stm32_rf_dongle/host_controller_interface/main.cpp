#include "mbed.h"

#include "rfmesh.h"
#include "suart.h"
#include "protocol.h"
#include "utils.h"

//------------------------------------- CONFIG -----------------------------------------
#define FLASH_HEADER	0x0800FFF0
#define F_NODEID	*(uint8_t *) FLASH_HEADER
#define F_CHANNEL	*(uint8_t *) (FLASH_HEADER+0x01)

#define RF_BOARD_DONGLE 1
#define RF_BOARD_PIO 	0
//--------------------------------------------------------------------------------------
Serial   rasp(PB_10, PB_11, 115200);

#if (RF_BOARD_DONGLE == 1)
	uint8_t spi_module = 1;
	//nRF Modules 1:Gnd, 2:3.3v, 3:ce,  4:csn, 5:sck, 6:mosi, 7:miso, 8:irq 
	RfMesh hsm(&rasp,spi_module,           PC_15, PA_4, PA_5,   PA_7,  PA_6,    PA_0);
#elif (RF_BOARD_PIO == 1)
uint8_t spi_module = 2;
#endif

Proto    prf(&rasp);
DigitalOut myled(PC_13);
Ticker tick_call;

suart com(&rasp);

uint8_t payload[32];

bool is_rgb_toSend = false;
bool is_heat_toSend = false;
bool is_msg_toSend = false;
bool rf_rx_functions[4] = {false,false,false,false};
uint8_t msg_size = 0;
uint8_t tab_send[32];

void handle_cmd(uint8_t cmd,uint8_t param_size,uint8_t *params)
{
	switch(cmd)
	{
		case rf::exec_cmd::status :
		{
			hsm.print_nrf();
		}
		break;
		case rf::exec_cmd::send :
		{
			for(uint8_t i = 0;i<param_size;i++)
			{
				tab_send[i] = params[i];
			}
			is_msg_toSend = true;
		}
		break;
		case rf::exec_cmd::channel :
		{
			uint8_t chan_to_set = params[0];
			hsm.nrf.selectChannel(chan_to_set);
			rasp.printf("channel:%d\n",hsm.nrf.getChannel());
		}
		break;
		case rf::exec_cmd::set_rx :
		{
			uint8_t index = params[0];
			rf_rx_functions[index] = params[1];
			rasp.printf("rx_function:%d;status:%u\n",index,rf_rx_functions[index]);
		}
		break;
		case rf::exec_cmd::test_rf :
		{
			uint8_t target = params[0];
			rasp.printf("todo:send_ping\n");
		}
		break;

		default:
		{
			rasp.printf("unhandled cmd:0x%X\r\n",cmd);
		}
	}
}


void binary_message_received(uint8_t *data,uint8_t size)
{
	uint8_t pid = data[1];
	//rasp.printf("pid:0x%X\r\n",pid);
	switch(pid)
	{
		case rf::pid::exec_cmd:
		{
			uint8_t param_size = data[0] - 3;
			uint8_t cmd = data[2];
			uint8_t *params = &data[3];
			handle_cmd(cmd,param_size,params);
		}
		break;
		default:
		{
			rasp.printf("_bin:");
			print_tab(&rasp,data,data[0]);
		}
	}
}

//IMPORTANT !!
//if you want to send a message that waits for an acknowledge, then it should be in an interruptible wait
//call from main is working
void text_message_received(uint8_t *data,uint8_t size)
{
	uint8_t *buffer = data;
    if(strbegins(buffer,"msg") == 0)
    {
        //msg size payload
        //light from 23 to 15 @ 2000
		//msg 0x06 0x7B 0x17 0x19 0x07 0xD0
		uint8_t charpos = 4;
		msg_size = get_hex(buffer,charpos);
		tab_send[0] = msg_size;
		for(int i=1;i<msg_size;i++)
		{
			charpos+=5;
			tab_send[i] = get_hex(buffer,charpos);
		}
		
		is_msg_toSend = true;
    }
    else
    {
		rasp.printf("unhandled:");
		print_tab(&rasp,data,size);
    }
}

void rf_sniffed(uint8_t *data,uint8_t size)
{
	if(!rf_rx_functions[0]) return;

	#if(SEND_BINARY == 1)
		rasp.putc('b');
		rasp.putc(size+1);//size included
		for(int i=0;i<size;i++)
		{
			rasp.putc(data[i]);//write without callback was protected
		}
	#else
		if(data[0] < 31)
		{
			rasp.printf("raw:");
			print_tab(&rasp,data,data[0]+2);//using size
		}
	#endif
}

void rf_broadcast(uint8_t *data,uint8_t size)
{
	if(!rf_rx_functions[1]) return;

	rasp.printf("bcast:");
	print_tab(&rasp,data,data[0]);
}

void rf_message(uint8_t *data,uint8_t size)
{
	if(!rf_rx_functions[2]) return;

	rasp.printf("msg:");
	print_tab(&rasp,data,data[0]);
}

void rf_response(uint8_t *data,uint8_t size)
{
	if(!rf_rx_functions[3]) return;

	rasp.printf("resp:");
	print_tab(&rasp,data,data[0]);
}

void the_ticker()
{
	myled = !myled;
}

void init()
{
    rasp.printf("startup:Hello from the RF Host Controller Interface\n");

    tick_call.attach(&the_ticker,1);

	hsm.init(F_CHANNEL);//left to the user for more flexibility on memory management
	rasp.printf("startup:listening to Mesh RF;channel:%d\n",F_CHANNEL);

	hsm.setNodeId(F_NODEID);

	hsm.setRetries(5);
	hsm.setAckDelay(100);
	rasp.printf("retries:5;delay:100 ms\n");
	
	//hsm.print_nrf();

    hsm.attach(&rf_sniffed,RfMesh::CallbackType::Sniff);
    hsm.attach(&rf_broadcast,RfMesh::CallbackType::Broadcast);
    hsm.attach(&rf_message,RfMesh::CallbackType::Message);
    hsm.attach(&rf_response,RfMesh::CallbackType::Response);

    com.attach_txt(&text_message_received);
    com.attach_bin(&binary_message_received);
}

int main() 
{
	uint8_t * p_UID = (uint8_t*) 0x1FFFF7E8;
	
	rasp.printf("U_ID: ");
	print_tab(&rasp,p_UID,12);
	rasp.printf("NodeID: %d\n",F_NODEID);
	init();

	//hsm.print_nrf();

	hsm.broadcast(rf::pid::reset);
    
    while(1) 
    {
		wait_ms(1);
		if(hsm.nRFIrq.read() == 0)
		{
			rasp.printf("stm32_event:irq pin Low, missed interrupt, re init()\n");
			hsm.init(F_CHANNEL);
		}
		if(is_msg_toSend)
		{
			uint8_t nbret = hsm.send_msg(tab_send);
			if(nbret == 0)
			{
				rasp.printf("send_msg:fail:msg:");
				print_tab(&rasp,tab_send,tab_send[0]+2);
				rasp.printf("\r\n");
			}
			else
			{
				wait_ms(100);
				rasp.printf("send_msg:success;retries:%d\r\n",nbret);
			}
			
			is_msg_toSend = false;
		}
	}
}
