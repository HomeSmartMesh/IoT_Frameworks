#include "mbed.h"

#include "rfmesh.h"
#include "utils.h"
#include "protocol.h"

//------------------------------------- CONFIG -----------------------------------------
const uint8_t CHANNEL = 10;
const uint8_t NODEID = 24;
//--------------------------------------------------------------------------------------


Serial   rasp(PB_10, PB_11, 115200);
DigitalOut myled(PC_13);
Ticker tick_call;

DigitalOut heater(PB_13);
const uint8_t HEAT_MAX = 10;

//nRF Modules 1:Gnd, 2:3.3v, 3:ce,  4:csn, 5:sck, 6:mosi, 7:miso, 8:irq 
RfMesh hsm(&rasp,           PC_15, PA_4, PA_5,   PA_7,  PA_6,    PA_0);

uint8_t heat_val = 0;


void rf_message_to_me(uint8_t *data,uint8_t size)
{
	if(data[rfi_pid] == rf_pid_heat)
	{
		heat_val = data[4];//heat_val payload : Size Pid  SrcId TrgId  HeatVal CRC
		rasp.printf("stm32_heater> (From RF) Set Heat Val to %d> ",heat_val);
	}
	else
	{
		rasp.printf("RX(%d)> ",size);
		print_tab(&rasp,data,size);
	}

}

void the_ticker()
{
	static uint8_t tick_cycle = 0;
	static uint8_t l_heat_val = 0;

	if(tick_cycle == 0)
	{
		l_heat_val = heat_val;//cache it for teh next cycle

		if(l_heat_val != 0)
		{
			heater = 1;
		}
	}
	else if(tick_cycle == l_heat_val)
	{
		heater = 0;
	}

	myled = !myled;//alive

	tick_cycle++;
	if(tick_cycle == HEAT_MAX)
	{
		tick_cycle = 0;
	}
}

void init()
{
	heater = 0;
    rasp.printf("stm32_heater> Hello from the RF Heater\n");

    tick_call.attach(&the_ticker,1);

	hsm.init(CHANNEL);//left to the user for more flexibility on memory management
	rasp.printf("stm32_heater> Started listening\n");

	hsm.setNodeId(NODEID);

	hsm.setRetries(10);
	hsm.setAckDelay(400);
	
	//hsm.print_nrf();

	hsm.attach(&rf_message_to_me,RfMesh::CallbackType::Message);//matching the node id, safe wit hACK

}

const float one_minute = 60;

void run_heater_program()
{

	heat_val = 10;
	rasp.printf("stm32_heater> Level 10 : for 20 min\r");
	wait(20 * one_minute);

	heat_val = 5;
	rasp.printf("stm32_heater> Level 5 : for 10 min\r");
	wait(10 * one_minute);

	heat_val = 2;
	rasp.printf("stm32_heater> Level 2 : for 60 min\r");
	wait(60 * one_minute);
}

int main() 
{

	uint8_t * p_UID = (uint8_t*) 0x1FFFF7E8;

	rasp.printf("stm32_heater> U_ID: ");
	print_tab(&rasp,p_UID,12);
	rasp.printf("stm32_heater> Node ID: %d\r",NODEID);
	

    init();

	hsm.print_nrf();

	hsm.broadcast_reset();
	
	
	run_heater_program();

	heat_val = 0;rasp.printf("stm32_heater> Program Over\r");
	
	while(1) 
    {
		if(heat_val > 0)
		{
			heat_val--;
			hsm.broadcast_heat(heat_val);
			rasp.printf("stm32_heater> heat_val down to %d\r",heat_val);
		}
		rasp.printf("stm32_heater> wait 10 min\r");
		wait(10 * one_minute);// 10 minutes
	}
}
