#include "mbed.h"

#include "rfmesh.h"
#include "suart.h"
#include "protocol.h"

Serial   rasp(PB_10, PB_11, 115200);
Proto    prf(&rasp);
DigitalOut myled(PC_13);
Ticker tick_call;
//nRF Modules 1:Gnd, 2:3.3v, 3:ce,  4:csn, 5:sck, 6:mosi, 7:miso, 8:irq 
RfMesh mesh(&rasp,           PC_15, PA_4, PA_5,   PA_7,  PA_6,    PA_0);

suart com(&rasp);

uint8_t payload[32];

uint8_t get_hex_char(uint8_t c)
{
	uint8_t res = 0;
	if(c <= '9')
	{
		res = c - '0';
	}
	else if(c <= 'F')
	{
		res = c - 'A' + 10;
	}
	else if(c <= 'f')
	{
		res = c - 'a' + 10;
	}
	return res;
}

uint8_t get_hex(uint8_t* buffer,uint8_t pos)
{
	uint8_t hex_val;
	pos+=2;//skip "0x"
	hex_val = get_hex_char(buffer[pos++]);
	hex_val <<= 4;
	hex_val |= get_hex_char(buffer[pos]);
	return hex_val;
}

uint8_t strbegins (uint8_t * s1, const char * s2)
{
    for(; *s1 == *s2; ++s1, ++s2)
        if(*s2 == 0)
            return 0;
    return (*s2 == 0)?0:1;
}

void uart_message_received(uint8_t *data,uint8_t size)
{
    
    uint8_t *buffer = data;
    if(strbegins(buffer,"rgb") == 0)
    {
        //rgb NodeId R G B
        //rgb 0x00 0x00 0x00 0x00
        //rgb 0x13 0x55 0x66 0xbb
        uint8_t TargetNodeId = get_hex(buffer,4);
        uint8_t R = get_hex(buffer,9);
        uint8_t G = get_hex(buffer,14);
        uint8_t B = get_hex(buffer,19);
        

        //mesh.nrf.start_transmission(payload,payload[0]);

        rasp.printf("NodeId:16;NodeDest:%d;R:%u;G:%u;B:%u\r",
                    TargetNodeId,R,G,B);
    }
    else if(strbegins(buffer,"help") == 0)
    {
        rasp.printf("see help in https://github.com/wassfila/IoT_Frameworks");
    }
    else
    {
        rasp.printf("Unknown Command, type 'help' for info");
    }
}

void rf_message_received(uint8_t *data,uint8_t size)
{

	switch(data[rfi_pid])
	{
		case rf_pid_0xF5_alive:
			{
				rasp.printf("NodeId:%d;is_Alive\r",data[rfi_src]);
			}
			break;
		case rf_pid_0xC9_reset:
			{
				rasp.printf("NodeId:%d;was:Reset\r",data[rfi_src]);
			}
			break;
		case rf_pid_0xBB_light:
			{
				prf.rx_light(data[rfi_src],data+rfi_broadcast_payload_offset);
			}
			break;
		case rf_pid_0xC5_magnet:
			{
				prf.rx_magnet(data[rfi_src],data);
			}
			break;
		case rf_pid_0xE2_bme280:
			{
				prf.bme280_rx_measures(data[rfi_src],data+3);
			}
			break;
		default :
			{
                rasp.printf("RX(%d)> ",size);
                for(int i=0;i<size;i++)
                {
                    rasp.printf("0x%0x ",data[i]);
                }
                if(size < 31)
                {
                    rasp.printf(" - 0x%0x 0x%0x",data[size],data[size+1]);
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

    mesh.init();//left to the user for more flexibility on memory management

    mesh.nrf.selectChannel(2);

    mesh.attach(&rf_message_received,RfMesh::CallbackType::Message);

    com.attach(&uart_message_received);

}

int main() 
{
    init();

    //transmission example
    //mesh.nrf.start_transmission(payload,payload[0]);
    
    while(1) 
    {
        wait_ms(1);
    }
}
