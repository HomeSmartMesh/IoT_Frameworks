#include "mbed.h"

#include "rfmesh.h"
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

DigitalOut myled(PC_13);
Ticker tick_call;

void rf_sniffed(uint8_t *data,uint8_t size)
{
	rasp.printf("sniff: ");    print_tab(&rasp,data,size);
}

void the_ticker()
{
    myled = !myled;
}

void init()
{

    uint8_t * p_UID = (uint8_t*) 0x1FFFF7E8;
	
	rasp.printf("stm32 sniffer> U_ID: ");
	print_tab(&rasp,p_UID,12);
	rasp.printf("stm32 sniffer> Node ID: %d\r",F_NODEID);

    tick_call.attach(&the_ticker,1);

    hsm.init(F_CHANNEL);
    rasp.printf("stm32 sniffer> listening to Mesh 2.0 at channel %d\r",F_CHANNEL);
    hsm.setNodeId(F_NODEID);

    hsm.attach(&rf_sniffed,RfMesh::CallbackType::Sniff);

}

int main() 
{
    init();

    while(1) 
    {
        wait_ms(1);
    }
}
