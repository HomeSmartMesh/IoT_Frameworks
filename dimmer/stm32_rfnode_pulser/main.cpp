#include "mbed.h"



Serial   rasp(PB_10, PB_11, 115200);
DigitalOut myled(PC_13);
Ticker tick_call;

DigitalOut pulser(PB_13);

void the_ticker()
{
	myled = !myled;
}

int main() 
{
    rasp.printf("stm32_pulser> Hello");
	
	tick_call.attach(&the_ticker,0.2);
	
    while(1) 
    {
		wait_ms(10);
		pulser = 1;
		wait_us(10);
		pulser = 0;
	}
}
