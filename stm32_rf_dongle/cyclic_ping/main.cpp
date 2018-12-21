#include "mbed.h"

#include "rfmesh.h"
#include "protocol.h"
#include "utils.h"

//------------------------------------- CONFIG -----------------------------------------
const uint8_t CHANNEL = 10;
const uint8_t NODEID = 65;
//--------------------------------------------------------------------------------------

Serial   rasp(PB_10, PB_11, 115200);
Proto    prf(&rasp);
DigitalOut myled(PC_13);
Ticker tick_call;
//nRF Modules 1:Gnd, 2:3.3v, 3:ce,  4:csn, 5:sck, 6:mosi, 7:miso, 8:irq 
RfMesh hsm(&rasp,1,           PC_15, PA_4, PA_5,   PA_7,  PA_6,    PA_0);

uint8_t payload[32];

void the_ticker()
{
    myled = !myled;
}

void init()
{

    uint8_t * p_UID = (uint8_t*) 0x1FFFF7E8;
	
	rasp.printf("pinger> U_ID: ");
	print_tab(&rasp,p_UID,12);
	rasp.printf("pinger> Node ID: %d\r",NODEID);

    rasp.printf("pinger> Hi\n");

    tick_call.attach(&the_ticker,1);

    hsm.init(CHANNEL);
    rasp.printf("pinger listening at channel %d\r",CHANNEL);
    hsm.setNodeId(NODEID);

	hsm.setRetries(0);
	hsm.setAckDelay(0);
	
    hsm.print_nrf();
	
}

int main() 
{
    init();

    while(1) 
    {
        wait_ms(100);
        hsm.broadcast(rf::pid::reset);
    }
}
