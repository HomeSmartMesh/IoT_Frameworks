#include "mbed.h"

#include "rfmesh.h"
#include "suart.h"
#include "protocol.h"
#include "utils.h"


Serial   rasp(PB_10, PB_11, 115200);
Proto    prf(&rasp);
DigitalOut myled(PC_13);
Ticker tick_call;
//nRF Modules 1:Gnd, 2:3.3v, 3:ce,  4:csn, 5:sck, 6:mosi, 7:miso, 8:irq 
RfMesh hsm(&rasp,           PC_15, PA_4, PA_5,   PA_7,  PA_6,    PA_0);

suart com(&rasp);

uint8_t payload[32];


void uart_message_received(uint8_t *data,uint8_t size)
{
    
    uint8_t *buffer = data;
    if(strbegins(buffer,"rgb") == 0)
    {
        //rgb NodeId R G B
        //rgb 0x03 0x00 0x00 0x00
        //rgb 0x03 0x0F 0x06 0x08
        //rgb 0x0B 0x55 0x66 0xbb
        uint8_t TargetNodeId = get_hex(buffer,4);
        uint8_t R = get_hex(buffer,9);
        uint8_t G = get_hex(buffer,14);
        uint8_t B = get_hex(buffer,19);
        

		uint8_t nbret = hsm.send_rgb(TargetNodeId,R,G,B);
		if(nbret == 0)
		{
			rasp.printf("send_rgb fail\r");
		}
		else
		{
			rasp.printf("send_rgb success in %d retries\r",nbret);
		}

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

void rf_broadcast_catched(uint8_t *data,uint8_t size)
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

	hsm.init();//left to the user for more flexibility on memory management
	rasp.printf("stm32_dongle> Started listening\n");

	hsm.nrf.selectChannel(2);
	
	hsm.setNodeId(22);

	/*hsm.setRetries(20);
	hsm.setAckDelay(400);
	
	hsm.print_nrf();
	*/

	hsm.attach(&rf_broadcast_catched,RfMesh::CallbackType::Broadcast);

    com.attach(&uart_message_received);

}

int main() 
{
    init();

	hsm.print_nrf();

	hsm.broadcast_reset();
    
    while(1) 
    {
		wait_ms(1000);
		if(hsm.nRFIrq.read() == 0)
		{
			rasp.printf("irq pin Low, missed interrupt, re init()\n");
			hsm.init();
		}
	}
}
