#include "mbed.h"

#include "rfmesh.h"
#include "Servo.h"
#include "suart.h"

#define BOARD_IS_RF_DONGLE 1

Serial   rasp(PB_10, PB_11, 115200);
DigitalOut myled(PC_13);
Ticker tick_call;
//nRF Modules 1:Gnd, 2:3.3v, 3:ce,  4:csn, 5:sck, 6:mosi, 7:miso, 8:irq 
#if BOARD_IS_RFPIO == 1
//RfMesh mesh(&rasp,           PA_5,  PB_12, PB_13, PB_15, PB_14, PA_4);
#elif BOARD_IS_RF_DONGLE ==1
RfMesh mesh(&rasp,           PC_15, PA_4, PA_5,   PA_7,  PA_6,    PA_0);
#endif

suart com(&rasp);

void the_ticker()
{
    myled = !myled;
    
}

bool got_message = false;
uint8_t payload[32];

void uart_message_received(uint8_t *data)
{
    //rasp.printf("stm32_dongle> OK message received with %d Bytes\n",data[0]);
    uint8_t size = data[0];
    for(int i=0;i<size;i++)
    {
        payload[i] = data[i];
    }
    got_message = true;
}

void rf_message_received(uint8_t *data,uint8_t size)
{
    rasp.printf("stm32_dongle>rf> Rx message (%d bytes) unused in dongle\n",size);
}

void init()
{
    rasp.printf("stm32_dongle> Hello PIO Dongle\n");

    tick_call.attach(&the_ticker,1);

    mesh.init();//left to the user for more flexibility on memory management

    mesh.nrf.selectChannel(22);

    mesh.attach(&rf_message_received,RfMesh::CallbackType::Message);

    com.attach(uart_message_received);

}

int main() 
{
    init();

    while(1) 
    {
        wait_ms(1);
        if(got_message)
        {
            rasp.printf("stm32_dongle> start transmission\n");
            mesh.nrf.start_transmission(payload,payload[0]);
            got_message = false;
        }
    }
}
