#include "mbed.h"

#include "rfmesh.h"
#include "Servo.h"
#include "suart.h"

Serial   rasp(PB_10, PB_11, 115200);
DigitalOut myled(PC_13);
Ticker tick_call;
//nRF Modules 1:Gnd, 2:3.3v, 3:ce,  4:csn, 5:sck, 6:mosi, 7:miso, 8:irq 
//RFPIO Layout !!!!
RfMesh mesh(&rasp,           PA_5,  PB_12, PB_13, PB_15, PB_14, PA_4);

Servo ser_m11(PA_8);
Servo ser_m12(PA_9);
Servo ser_m13(PA_10);
Servo ser_m14(PA_11);
Servo ser_m21(PA_15);
Servo ser_m22(PB_3);
Servo ser_m31(PB_4);
Servo *msr[7];

DigitalOut pio_u1(PB_5);
DigitalOut pio_u2(PB_6);
DigitalOut pio_u3(PB_7);
DigitalOut pio_u4(PB_8);
DigitalOut pio_u5(PB_9);
DigitalOut pio_u6(PB_1);
DigitalOut pio_u7(PB_0);
DigitalOut pio_u8(PA_7);
DigitalOut *mio[8];

void the_ticker()
{
    myled = !myled;
    
}

void rf_message_received(uint8_t *data,uint8_t size)
{
    rasp.printf("rf>Rx message Handler : 0x");
    for(int i = 0; i < size; i++)
    {
        rasp.printf(" %02x",data[i]);
    }
    rasp.printf("\r\n");
}

void init()
{
    wait_ms(100);
    
    msr[0] = &ser_m11;
    msr[1] = &ser_m12;
    msr[2] = &ser_m13;
    msr[3] = &ser_m14;
    msr[4] = &ser_m21;
    msr[5] = &ser_m22;
    msr[6] = &ser_m31;

    mio[0] = &pio_u1;
    mio[1] = &pio_u2;
    mio[2] = &pio_u3;
    mio[3] = &pio_u4;
    mio[4] = &pio_u5;
    mio[5] = &pio_u6;
    mio[6] = &pio_u7;
    mio[7] = &pio_u8;
    

    rasp.printf("Hello RFPIO\n");

    tick_call.attach(&the_ticker,1);

    mesh.init();//left to the user for more flexibility on memory management

    rasp.printf("selectChannel(22)\n");
    mesh.nrf.selectChannel(22);

    mesh.attach(&rf_message_received,RfMesh::CallbackType::Message);

}

int main() 
{
    init();

    while(1) 
    {
        for(int i=0;i<7;i++)
        {
            (*msr[i]) = 1;
        }
        for(int i=0;i<7;i++)
        {
            (*mio[i]) = 1;
        }
        wait(1.0);
        for(int i=0;i<7;i++)
        {
            (*msr[i]) = 0;
        }
        for(int i=0;i<7;i++)
        {
            (*mio[i]) = 0;
        }
        wait(1.0);
    }
}
