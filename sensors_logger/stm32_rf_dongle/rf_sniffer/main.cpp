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

void rf_sniffed(uint8_t *data,uint8_t size)
{
	rasp.printf("sniff: ");
	print_tab(&rasp,data,size);
}

void the_ticker()
{
    myled = !myled;
}

void init()
{
    rasp.printf("stm32 sniffer> Hi\n");

    tick_call.attach(&the_ticker,1);

    hsm.init();//left to the user for more flexibility on memory management

	hsm.nrf.selectChannel(2);
	
	hsm.setNodeId(22);

	hsm.setRetries(20);
	hsm.setAckDelay(400);
	
    hsm.print_nrf();
	
    hsm.attach(&rf_sniffed,RfMesh::CallbackType::Sniff);

    com.attach(&uart_message_received);

}

int main() 
{
    init();

    //transmission example
    //hsm.nrf.start_transmission(payload,payload[0]);
    
    while(1) 
    {
        wait_ms(1);
    }
}
