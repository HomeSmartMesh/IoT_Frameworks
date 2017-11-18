#include "mbed.h"

#include "rfmesh.h"
#include "protocol.h"
#include "utils.h"

//------------------------------------- CONFIG -----------------------------------------
const uint8_t CHANNEL = 10;
const uint8_t NODEID = 24;
//--------------------------------------------------------------------------------------


Serial   rasp(PB_10, PB_11, 115200);
Proto    prf(&rasp);
DigitalOut myled(PC_13);
Ticker tick_call;
//nRF Modules 1:Gnd, 2:3.3v, 3:ce,  4:csn, 5:sck, 6:mosi, 7:miso, 8:irq 
RfMesh hsm(&rasp,           PC_15, PA_4, PA_5,   PA_7,  PA_6,    PA_0);

uint8_t payload[32];

bool is_rgb_toSend = false;
bool is_heat_toSend = false;
bool is_msg_toSend = false;
uint8_t msg_size = 0;
uint8_t tab_send[32];


void the_ticker()
{
	myled = !myled;
}

void init()
{
    rasp.printf("stm32_bridge> Hello from the RF UART Interface\n");

    tick_call.attach(&the_ticker,1);

	hsm.init(CHANNEL);//left to the user for more flexibility on memory management
	rasp.printf("stm32_bridge> Started listening\n");

	hsm.setNodeId(NODEID);

	hsm.setRetries(10);
	hsm.setAckDelay(400);
	
	//hsm.print_nrf();

}

int main() 
{
	uint8_t * p_UID = (uint8_t*) 0x1FFFF7E8;
	
	rasp.printf("stm32_bridge> U_ID: ");
	print_tab(&rasp,p_UID,12);
	rasp.printf("stm32_bridge> Node ID: %d\r",NODEID);

	init();

	hsm.print_nrf();

	hsm.broadcast(rf::pid::reset);
    
    while(1) 
    {
		wait_ms(100);
		if(hsm.nRFIrq.read() == 0)
		{
			rasp.printf("irq pin Low, missed interrupt, re init()\n");
			hsm.init(CHANNEL);
		}
	}
}
