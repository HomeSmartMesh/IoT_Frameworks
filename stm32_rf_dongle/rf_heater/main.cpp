#include "mbed.h"

#include "rfmesh.h"
#include "utils.h"
#include "protocol.h"

//------------------------------------- CONFIG -----------------------------------------
#define FLASH_HEADER	0x0800FFF0
#define F_NODEID	*(uint8_t *) FLASH_HEADER
#define F_CHANNEL	2
//#define F_CHANNEL	*(uint8_t *) (FLASH_HEADER+0x01)
//--------------------------------------------------------------------------------------
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

DigitalOut heater(PB_13);
const uint8_t HEAT_CYCLE = 10;
const uint8_t DURATION_MAX = 60;//60 minutes
const uint32_t WATCH_SEC_MAX = 3700;

uint8_t heat_mode = 0;
uint8_t heat_val = 0;
uint32_t state_duration_cycles = 0;
uint8_t tick_cycle = 0;

void heat_wave_mn(uint8_t val,uint32_t minutes)
{
	//Clamp on max 1 hour

	if(val > HEAT_CYCLE)
	{
		minutes = 0;
		rasp.printf("stm32_heater> heat_wave value unexpected, switching off\r");
		heat_val = 0;
		state_duration_cycles = 0;
	}
	else if(val == 0)
	{
		minutes = 0;
		rasp.printf("stm32_heater> heat_wave value 0, switching off\r");
		heat_val = 0;
		state_duration_cycles = 0;
	}
	else if(minutes > DURATION_MAX)
	{
		minutes = 0;
		rasp.printf("stm32_heater> heat_wave duration unexpected, switching off\r");
		heat_val = 0;
		state_duration_cycles = 0;
	}
	else if(minutes == 0)
	{
		rasp.printf("stm32_heater> heat_wave with 0 duration, switching off\r");
		heat_val = 0;
		state_duration_cycles = 0;
	}
	else// 0 <= minutes <= 60
	{
		uint32_t cycles = minutes * 6;//one cycle is 10 sec
		rasp.printf("stm32_heater> heat_wave @ %d for %lu cycles\r",val,cycles);
		state_duration_cycles = cycles;
		heat_val = val;
	}
	hsm.broadcast_byte(rf::pid::heat,heat_val);
}

void rf_message_to_me(uint8_t *data,uint8_t size)
{
	rasp.printf("RX(%d)> ",size);
	print_tab(&rasp,data,size);
	if(data[rf::ind::pid] == rf::pid::heat)
	{
		uint8_t request_heat_val 		= data[rf::ind::p2p_payload+0];//payload[0] => heat val
		uint8_t request_heat_time_mn 	= data[rf::ind::p2p_payload+1];//payload[1] => heat mn
		tick_cycle = 0;//restart a new cycle for immidiate application
		rasp.printf("stm32_heater> RF request heat_wave_mn(%d,%u)\r",request_heat_val,request_heat_time_mn);
		heat_wave_mn(request_heat_val,request_heat_time_mn);
	}

}

//called every heat cycle of HEAT_CYCLE seconds
void handle_cycle()
{
	//Transitions
	if(state_duration_cycles == 0)
	{
		if(heat_val != 0)
		{
			heat_val = 0;
			rasp.printf("stm32_heater> heating over\n");
		}
	}
	else
	{
		rasp.printf("stm32_heater> heating @ %d for %d more cycles\n",heat_val,state_duration_cycles);
		state_duration_cycles--;
	}
}

void the_ticker()
{
	static uint32_t heat_watch = WATCH_SEC_MAX;//a bit 
	static uint8_t alive_count = 0;
	static uint8_t l_heat_val = 0;

	//heat max on watchdog
	if(heat_val != 0)
	{
		if(heat_watch != 0)
		{
			heat_watch--;
		}
	}
	else//it's off : reset watchdog
	{
		heat_watch = WATCH_SEC_MAX;
	}

	if(tick_cycle == 0)
	{
		l_heat_val = heat_val;//cache it for teh next cycle

		if(l_heat_val != 0)
		{
			heater = 1;
		}
		else
		{
			heater = 0;//was forgotten !! important, as next is else only and avoid a tiny pulse in case of val of 0
		}
	}
	else if(tick_cycle == l_heat_val)
	{
		heater = 0;
	}

	myled = !myled;//alive

	//rasp.printf("tick %d ; heat_output %d ; heat val %d\r",tick_cycle,heater.read(),l_heat_val);

	tick_cycle++;
	if(tick_cycle == HEAT_CYCLE)
	{
		tick_cycle = 0;
		handle_cycle();
	}
	
	if(alive_count == 0)
	{
		alive_count = 60;
		//hsm.broadcast(rf::pid::alive);
		hsm.broadcast_byte(rf::pid::heat,heat_val);
	}
	else
	{
		alive_count--;
	}
}

void init()
{
	heater = 0;
    rasp.printf("stm32_heater> Hello from the RF Heater\n");

    tick_call.attach(&the_ticker,1);

	hsm.init(F_CHANNEL);//left to the user for more flexibility on memory management
	hsm.nrf.setMode(nrf::Mode::Rx);//not set by default as to check power consemption with hci
	rasp.printf("mode:%d;channel:%d\n",hsm.nrf.getMode(),hsm.nrf.getChannel());

	hsm.setNodeId(F_NODEID);
	rasp.printf("stm32_heater> Started listening\n");

	hsm.setRetries(10);
	hsm.setAckDelay(400);
	
	//hsm.print_nrf();

	hsm.attach(&rf_message_to_me,RfMesh::CallbackType::Message);//matching the node id, safe wit hACK

}

const float one_minute = 60;

void wait_mn(uint8_t min)
{
	wait(min * one_minute);
}
void wait_hr(uint8_t hour)
{
	for(int i=0;i<hour;i++)
	{
		wait_mn(60);
	}
}

int main() 
{

	uint8_t * p_UID = (uint8_t*) 0x1FFFF7E8;

	rasp.printf("stm32_heater> U_ID: ");
	print_tab(&rasp,p_UID,12);
	rasp.printf("stm32_heater> Node ID: %d\r",F_NODEID);
	

    init();

	hsm.print_nrf();

	hsm.broadcast(rf::pid::reset);
	wait(1);
	
	heat_wave_mn(10,3);

	while(1) 
    {
		wait(1);
	}
}
