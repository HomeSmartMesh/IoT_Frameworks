#include "mbed.h"

#include "rfmesh.h"
#include "dimm.h"
#include "utils.h"

//------------------------------------- CONFIG -----------------------------------------
const uint8_t CHANNEL = 10;
const uint8_t NODEID = 25;
//--------------------------------------------------------------------------------------

Serial   rasp(PB_10, PB_11, 115200);
DigitalOut myled(PC_13);

Ticker tick_call;
//nRF Modules 1:Gnd, 2:3.3v, 3:ce,  4:csn, 5:sck, 6:mosi, 7:miso, 8:irq 
//RFPIO Layout !!!!
RfMesh hsm(&rasp,           PA_5,  PB_12, PB_13, PB_15, PB_14, PA_4);

//                rel,  sync,   ch1,2,   3,    4
Dimm dimmer(&rasp,PB_4,PB_5,   PA_8,PA_9,PA_10,PA_11,   PA_15,PB_3,PA_3,PA_2 );


void the_ticker()
{
    myled = !myled;
}

void rf_message_received(uint8_t *data,uint8_t size)
{
    //rasp.printf("rf>Rx message Handler :");
    //print_tab(&rasp,data,size);
    if(data[0x01] == 0x1B)//pid : light
    {
        dimmer.handle_message(data,size);
    }
}

void init()
{
    
    uint8_t * p_UID = (uint8_t*) 0x1FFFF7E8;
	
	rasp.printf("Light Dimmer> U_ID: ");
	print_tab(&rasp,p_UID,12);
	rasp.printf("Light Dimmer> Node ID: %d\r",NODEID);

    rasp.printf("Hello Light Dimmer\r");


    tick_call.attach(&the_ticker,1);


    dimmer.init();//here irq is enabled

    hsm.init(CHANNEL);
    rasp.printf("Light Dimmer listening at channel %d\r",CHANNEL);
    hsm.setNodeId(NODEID);
    
    hsm.print_nrf();

    hsm.attach(&rf_message_received,RfMesh::CallbackType::Message);

}

int main() 
{

    init();

    while(1) 
    {
        wait(10);
        rasp.printf("Nb int : %d\r",dimmer.intCount);
        dimmer.intCount = 0;
    }
}
