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

DigitalOut debug_pio(PB_13);

Proto    prf(&rasp);
DigitalOut myled(PC_13);
Ticker tick_call;
uint8_t led_count = 0;

uint8_t payload[32];

bool is_rgb_toSend = false;
bool is_heat_toSend = false;
bool is_msg_toSend = false;
uint8_t msg_size = 0;
uint8_t tab_send[32];


void the_ticker()
{
	if(led_count != 0)
	{
		myled = 0;
		led_count--;
	}
	else
	{
		myled = 1;
	}
}

void rf_sniffed(uint8_t *data,uint8_t size)
{
	led_count = 1;
}

void init()
{
	uint8_t * p_UID = (uint8_t*) 0x1FFFF7E8;
	
	rasp.printf("stm32_bridge> U_ID: ");
	print_tab(&rasp,p_UID,12);
	rasp.printf("stm32_bridge> Node ID: %d\r",F_NODEID);

	tick_call.attach(&the_ticker,0.1);

	hsm.init(F_CHANNEL);//left to the user for more flexibility on memory management
	hsm.setBridgeMode();
	rasp.printf("stm32_bridge> listening to Mesh 2.0 on channel %d in bridge Mode\n",F_CHANNEL);

	hsm.setNodeId(F_NODEID);

    hsm.attach(&rf_sniffed,RfMesh::CallbackType::Sniff);

	hsm.setRetries(10);
	hsm.setAckDelay(400);
	
	//hsm.print_nrf();

}

int main() 
{

	init();


	hsm.broadcast(rf::pid::reset);
    
	uint16_t alive_count = 520;

    while(1) 
    {
		wait_ms(100);
		if(hsm.nRFIrq.read() == 0)
		{
			rasp.printf("irq pin Low, missed interrupt, re init()\n");
			hsm.init(F_CHANNEL);
		}
		if(alive_count == 0)
		{
			hsm.broadcast(rf::pid::alive);
			alive_count = 520;//~60 sec
		}
		else
		{
			alive_count--;
		}
	}
}
