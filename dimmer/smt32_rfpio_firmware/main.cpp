#include "mbed.h"

#include "rfmesh.h"
#include "protocol.h"
#include "dimm.h"
#include "utils.h"

//------------------------------------- CONFIG -----------------------------------------
const uint8_t CHANNEL = 2;
const uint8_t NODEID = 25;
//TODO use flash config

#define RF_BOARD_DONGLE 0
#define RF_BOARD_PIO 	1

#define DEBUG_RFPIO 1
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

Serial   rasp(PB_10, PB_11, 115200);
DigitalOut myled(PC_13);

Ticker tick_call;

#if (RF_BOARD_DONGLE == 1)
	uint8_t spi_module = 1;
	//nRF Modules 1:Gnd, 2:3.3v, 3:ce,  4:csn, 5:sck, 6:mosi, 7:miso, 8:irq 
	RfMesh hsm(&rasp,spi_module, PC_15, PA_4, PA_5,   PA_7,  PA_6,    PA_0);
#elif (RF_BOARD_PIO == 1)
    uint8_t spi_module = 2;
    //nRF Modules 1:Gnd, 2:3.3v, 3:ce,  4:csn, 5:sck, 6:mosi, 7:miso, 8:irq 
    //RFPIO Layout !!!!
    RfMesh hsm(&rasp, spi_module, PA_5,  PB_12, PB_13, PB_15, PB_14, PA_4);
#endif

//channel from pins A2,A3 could not be assigned to pwm even after changing uart to UART1
//                rel,  sync,   ch1,2,   3,    4
Dimm dimmer(&rasp,PB_4,PB_5,   PA_8,PA_9,PA_10,PA_11,   PA_15,PB_3,PB_0,PB_1 );


void the_ticker()
{
    //myled = !myled;
    #if(DEBUG_RFPIO == 1)
    //rasp.printf("rf>alive\r\n");
    #endif
}

void rf_message_received(uint8_t *data,uint8_t size)
{
    //rasp.printf("rf_message_received()\r\n");
    //print_tab(&rasp,data,size);
    if(data[rf::ind::pid] == rf::pid::dimmer)
    {
        dimmer.handle_message(data,size);
    }
}

void startup_switchon()
{
    int vals[8];

    //switch on - still might jitter depending on phase
    //TODO might think to sync the relay with the ISR
    dimmer.relay = 0;

    //185 - 9900
    for(int i=0;i<18000;i++)
    {
        for(int j=0;j<8;j++)        
        {
            vals[j] = i - (j*1000);//first gets counter, second is shifted by 1000,...
            //the set level is protected against max so no issues for first overflowing
            //the set_level is also latch protected, so no influence on change light count
            dimmer.set_level(j,vals[j]);
        }
        wait_us(40);// ~ 
    }
}

void init()
{
    
    uint8_t * p_UID = (uint8_t*) 0x1FFFF7E8;
	
	rasp.printf("Light Dimmer> U_ID: ");
	print_tab(&rasp,p_UID,12);
	rasp.printf("Light Dimmer> Node ID: %d\r",NODEID);

    tick_call.attach(&the_ticker,1);


    dimmer.init();//here irq is enabled

    startup_switchon();

    hsm.init(CHANNEL);
    hsm.setNodeId(NODEID);
    rasp.printf("Light Dimmer listening at channel %d\r",CHANNEL);
    
    hsm.print_nrf();

    hsm.attach(&rf_message_received,RfMesh::CallbackType::Message);

}

int main() 
{
    myled = 1;//turn off

    init();//dimmer, hsm


    rasp.printf("Broadcasting reset\r\r");
    hsm.broadcast(rf::pid::reset);

    rasp.printf("starting loop\r\r");
    while(1) 
    {
        myled = 0;//on
        wait_ms(50);
        myled = 1;//off
        wait(10);
        rasp.printf("Nb int : %d\r",dimmer.intCount);
        dimmer.intCount = 0;
    }
}
