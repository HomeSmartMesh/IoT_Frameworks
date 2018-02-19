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
uint8_t msg_size = 0;
uint8_t tab_send[32];

void binary_message_received(uint8_t *data,uint8_t size)
{
	rasp.printf("bin:");
	print_tab(&rasp,data,32);
	//data[0] : 
		//0x00 : forward binary frame to RF
		//0x01 : dimmer, ... (avoid having to know source node id)
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
    else if(strbegins(buffer,"rgb") == 0)
    {
        //rgb NodeId R G B
        //rgb 0x03 0x00 0x00 0x00
        //rgb 0x03 0x0F 0x06 0x08
        //rgb 0x0B 0x55 0x66 0xbb
        tab_send[0] = get_hex(buffer,4);
        tab_send[1] = get_hex(buffer,9);
        tab_send[2] = get_hex(buffer,14);
        tab_send[3] = get_hex(buffer,19);
		
		is_rgb_toSend = true;
    }
    else if(strbegins(buffer,"heat") == 0)
    {
        //heat NodeId val
        //heat 0x18 0x00
        //heat 0x18 0x01
        //heat 0x18 0x09
        //heat 0x18 0x0A
        tab_send[0] = get_hex(buffer,5);
        tab_send[1] = get_hex(buffer,10);
		
		is_heat_toSend = true;
    }
    else if(strbegins(buffer,"help") == 0)
    {
        rasp.printf("see help in https://github.com/wassfila/IoT_Frameworks");
    }
    else
    {
        rasp.printf("not known Command, type 'help' for info\r\n");
		rasp.printf("txt:");
		print_tab(&rasp,data,size);
        rasp.printf("not known Command, type 'help' for info\r\n");
    }
}

void rf_broadcast_catched(uint8_t *data,uint8_t size)
{
	rasp.printf("NodeId:%u;",data[rf::ind::source]);
	rasp.printf("ttl:%u;",data[rf::ind::control]&0x0F);

	switch(data[rf::ind::pid])
	{
		case rf::pid::alive:
			{
				rasp.printf("status:Alive\r");
			}
			break;
		case rf::pid::reset:
			{
				rasp.printf("event:Reset\r");
			}
			break;
		case rf::pid::gesture:
			{
				rasp.printf("gesture:%d\r",data[rf::ind::bcst_payload]);
			}
			break;
		case rf::pid::light:
			{
				prf.print_light(data+rf::ind::bcst_payload);
			}
			break;
		case rf::pid::heat:
			{
				rasp.printf("heat:%u\r",data[rf::ind::bcst_payload]);
			}
			break;
		case rf::pid::magnet:
			{
				prf.print_magnet(data+rf::ind::bcst_payload);
			}
			break;
		case rf::pid::bme280:
			{
				prf.print_bme280(data+rf::ind::bcst_payload);
			}
			break;
		case rf::pid::light_rgb:
			{
				prf.print_light_rgb(data+rf::ind::bcst_payload);
			}
			break;
		case rf::pid::proximity:
			{
				rasp.printf("proximity:%u\r",data[rf::ind::bcst_payload]);
			}
			break;
		case rf::pid::button:
			{
				rasp.printf("button:%u\r",data[rf::ind::bcst_payload]);
			}
			break;
		default :
			{
                rasp.printf("pid:Unknown;size:%u:rx:",size);
                for(int i=0;i<size;i++)
                {
                    rasp.printf("0x%0x ",data[i]);
                }
                if(size < 31)
                {
                    rasp.printf(";crc:0x%0x 0x%0x",data[size],data[size+1]);
                }
                rasp.printf("\n");
			}
			break;
	}

}

void the_ticker()
{
	myled = !myled;
}

void init()
{
    rasp.printf("stm32_dongle> Hello from the RF UART Interface\n");

    tick_call.attach(&the_ticker,1);

	hsm.init(F_CHANNEL);//left to the user for more flexibility on memory management
	rasp.printf("stm32_dongle> listening to Mesh 2.0 on channel %d\n",F_CHANNEL);

	hsm.setNodeId(F_NODEID);

	hsm.setRetries(5);
	hsm.setAckDelay(100);
	rasp.printf("stm32_dongle> config: 5 retrise, 100 ms wait\r\n");
	
	//hsm.print_nrf();

	hsm.attach(&rf_broadcast_catched,RfMesh::CallbackType::Broadcast);

    com.attach_txt(&text_message_received);
    com.attach_bin(&binary_message_received);
}

int main() 
{
	uint8_t * p_UID = (uint8_t*) 0x1FFFF7E8;
	
	rasp.printf("stm32_dongle> U_ID: ");
	print_tab(&rasp,p_UID,12);
	rasp.printf("\r\n");
	rasp.printf("stm32_dongle> Node ID: %d\r",F_NODEID);
	rasp.printf("\r\n");
	init();

	//hsm.print_nrf();

	hsm.broadcast(rf::pid::reset);
    
    while(1) 
    {
		wait_ms(100);
		if(hsm.nRFIrq.read() == 0)
		{
			rasp.printf("irq pin Low, missed interrupt, re init()\n");
			hsm.init(F_CHANNEL);
		}
		//send_rgb() is only allowed to be called from main as it uses the wait_ms function which fails from ISRs context
		//wait_ms() is required to wait for the acknowlege and keep a simple result in the function return
		if(is_msg_toSend)
		{
			uint8_t nbret = hsm.send_msg(tab_send);
			if(nbret == 0)
			{
				rasp.printf("stm32> send_msg fail : ");
				print_tab(&rasp,tab_send,tab_send[0]+2);
				rasp.printf("\r\n");
			}
			else
			{
				rasp.printf("stm32> send_msg success in %d retries\r\n",nbret);
			}
			
			is_msg_toSend = false;
		}
		if(is_rgb_toSend)
		{
			uint8_t nbret = hsm.send_rgb(tab_send[0],tab_send[1],tab_send[2],tab_send[3]);
			if(nbret == 0)
			{
				rasp.printf("send_rgb fail\r");
			}
			else
			{
				rasp.printf("send_rgb success in %d retries\r",nbret);
			}
			is_rgb_toSend = false;
		}
		if(is_heat_toSend)
		{
			uint8_t nbret = hsm.send_byte(rf::pid::heat,tab_send[0],tab_send[1]);//pid, dest, val
			if(nbret == 0)
			{
				rasp.printf("send_heat fail\r");
			}
			else
			{
				rasp.printf("send_heat success in %d retries\r",nbret);
			}
			
			is_heat_toSend = false;
		}
	}
}
