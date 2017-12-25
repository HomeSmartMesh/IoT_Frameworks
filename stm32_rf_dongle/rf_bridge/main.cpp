#include "mbed.h"

#include "rfmesh.h"
#include "protocol.h"
#include "utils.h"


//------------------------------------- CONFIG -----------------------------------------
#define FLASH_HEADER	0x0800FFF0
//RF
#define F_NODEID	*(uint8_t *) FLASH_HEADER
#define F_CHANNEL	*(uint8_t *) (FLASH_HEADER+0x01)
//RGB LED
#define USE_RGB_LED 0
//APDS9960 (Colorlight sensor, gesture)
#define USE_APDS_SENSOR 1
#define USE_APDS_GESTURE 0
#define USE_APDS_PROXIMITY 1
#define USE_APDS_LIGHT 0

#define RGB_DEMO 0
//--------------------------------------------------------------------------------------
//TODO should have a board as a target
#define RF_BOARD_DONGLE 1
#define RF_BOARD_PIO 	0
//--------------------------------------------------------------------------------------
Serial   rasp(PB_10, PB_11, 115200);

#if(USE_RGB_LED == 1)
	#include "ws2812B.h"
	ws2812B rgb_led(PB_13);
#endif

#if(USE_APDS_SENSOR == 1)
	#include "glibr.h"
	glibr gsensor(PB_7,PB_6);
#endif

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

void rf_message(uint8_t *data,uint8_t size)
{
	#if(USE_RGB_LED == 1)
		if(data[rf::ind::pid] == rf::pid::rgb)
		{
			uint8_t red = data[5];
			uint8_t green = data[6];
			uint8_t blue = data[7];
			rgb_led.set(red,green,blue);
			rasp.printf("R:%u;G:%u;B:%u\r\n",red,green,blue);
		}
	#endif
}

void init()
{
	uint8_t * p_UID = (uint8_t*) 0x1FFFF7E8;
	
	rasp.printf("stm32_bridge> U_ID: ");
	print_tab(&rasp,p_UID,12);
	rasp.printf("stm32_bridge> Node ID: %d\r",F_NODEID);

	tick_call.attach(&the_ticker,0.1);

	hsm.init(F_CHANNEL);//left to the user for more flexibility on memory management

    hsm.attach(&rf_sniffed,RfMesh::CallbackType::Sniff);
	hsm.attach(&rf_message,RfMesh::CallbackType::Message);

	hsm.setBridgeMode();
	rasp.printf("stm32_bridge> listening to Mesh 2.0 on channel %d in bridge Mode\n",F_CHANNEL);

	hsm.setNodeId(F_NODEID);


	hsm.setRetries(10);
	hsm.setAckDelay(400);
	
	//hsm.print_nrf();

	bool res;
	#if(USE_APDS_SENSOR == 1)
		res = gsensor.ginit();
		rasp.printf("apds> ginit %u\r\n",res);
	#endif
	#if(USE_APDS_GESTURE == 1)
		res = gsensor.enableGestureSensor();
		rasp.printf("apds> Gesture sensor enable: %u\r\n",res);
	#endif
	#if(USE_APDS_PROXIMITY == 1)
		res = gsensor.enableProximitySensor();
		rasp.printf("apds> Proximity sensor enable: %u\r\n",res);
	#endif
	#if(USE_APDS_LIGHT == 1)
		res = gsensor.enableLightSensor();
		rasp.printf("apds> Light sensor enable: %u\r\n",res);
	#endif

}

#if(USE_RGB_LED == 1)
void test_RGB()
{
        rgb_led.set(0x0F,0x00,0x00);
        wait(0.3);
        rgb_led.set(0x00,0x0F,0x00);
        wait(0.3);
        rgb_led.set(0x00,0x00,0x0F);
        wait(0.3);
        rgb_led.set(0x00,0x00,0x00);
}
#endif

#if(USE_APDS_GESTURE == 1)
void apds_poll_gesture()
{
	uint8_t gest = rf::gest::none;
	if ( gsensor.isGestureAvailable() ) 
	{
		gest = (uint8_t) gsensor.readGesture();
		if(gest)
		{
			hsm.broadcast_byte(rf::pid::gesture,gest);
			//rasp.printf("NodeId:%u;gesture:%u\r\n",F_NODEID,gest);//expected at rx gateway side
		}
	}
}
#endif

#if(USE_APDS_PROXIMITY == 1)
void apds_poll_proximity()
{
	uint8_t val;
	gsensor.readProximity(val);
	if(val > 25)
	{
		led_count = 1;
		//rasp.printf("NodeId:%u;proximity:%u\n",F_NODEID,val);//slows down the 10 ms loop
		#if(RGB_DEMO == 1)
			uint8_t r = val;
			uint8_t g = 0;
			uint8_t b = 255-val;
			hsm.send_rgb(24,r,g,b,false);
		#else
			hsm.broadcast_byte(rf::pid::proximity,val);
		#endif
	}
}
#endif

#if(USE_APDS_LIGHT == 1)
void apds_log_light_colors()
{
	static uint16_t light_count = 86;// ~ 10 s

	if(light_count == 0)
	{
		uint16_t light_rgb[4];
		gsensor.readAmbientLight(light_rgb[0]);
		gsensor.readRedLight(light_rgb[1]);
		gsensor.readGreenLight(light_rgb[2]);
		gsensor.readBlueLight(light_rgb[3]);
		hsm.broadcast_light_rgb(light_rgb);
		//expected at rx gateway side
		//rasp.printf("NodeId:%u;light:%u;red:%u,green:%u;blue:%u\r\n",F_NODEID,light_rgb[0],light_rgb[1],light_rgb[2],light_rgb[3]);

		light_count = 860;//~10 sec
	}
	else
	{
		light_count--;
	}
}
#endif


int main() 
{

	init();

	#if(USE_RGB_LED==1)
	test_RGB();
	#endif

	hsm.broadcast(rf::pid::reset);
    
	uint16_t alive_count = 520;

    while(1) 
    {
		wait_ms(10);
		
		#if(USE_APDS_LIGHT == 1)
			apds_log_light_colors();
		#endif

		#if(USE_APDS_PROXIMITY == 1)
			apds_poll_proximity();
		#endif

		#if(USE_APDS_GESTURE == 1)
			apds_poll_gesture();
		#endif

		if(hsm.nRFIrq.read() == 0)
		{
			rasp.printf("irq pin Low, missed interrupt, re init()\n");
			hsm.init(F_CHANNEL);
			hsm.broadcast(rf::pid::reset);//notify if this issue happens
		}
		if(alive_count == 0)
		{
			hsm.broadcast(rf::pid::alive);
			//rasp.printf("NodeId:%u;status:Alive\r\n",F_NODEID);//expected at rx gateway side
			alive_count = 5200;//~60 sec
		}
		else
		{
			alive_count--;
		}
	}
}
