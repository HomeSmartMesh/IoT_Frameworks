#include "mbed.h"

#include "rfmesh.h"
#include "protocol.h"
#include "utils.h"


//--------------------------------------------------------------------------------------
//APDS9960 (Colorlight sensor, gesture)
#define USE_APDS_SENSOR 		1
#define USE_APDS_LIGHT 			1
#define USE_APDS_GESTURE 		0
#define USE_APDS_PROXIMITY 		0

#define SEND_ALIVE  			1
#define APDS_SEND_RGB_DEMO 		0

#define LOOP_MS_WAIT			10

#define ALIVE_PERIOD			300
#define ALIVE_OFFSET			0

#define LIGHT_PERIOD 			300
#define LIGHT_OFFSET			100

#define TEST_RUN 		0
#define TEST_TARGET 	27
#define TEST_CHANNEL 	2

#define TEST_PERIOD				100
#define TEST_OFFSET				 50

#define TICKER_SEC 0.1

//------------------------------------- CONFIG -----------------------------------------
#define FLASH_HEADER	0x0800FFF0
//RF
#define F_NODEID	*(uint8_t *) FLASH_HEADER
#define F_CHANNEL	*(uint8_t *) (FLASH_HEADER+0x01)
//--------------------------------------------------------------------------------------
Serial   pc(PB_10, PB_11, 115200);

#include "ws2812B.h"
ws2812B rgb_led(PB_13);

#include "glibr.h"
glibr gsensor(PB_7,PB_6);
bool gsensor_available = false;

uint8_t spi_module = 1;
//nRF Modules 1:Gnd, 2:3.3v, 3:ce,  4:csn, 5:sck, 6:mosi, 7:miso, 8:irq 
RfMesh hsm(&pc,spi_module,           PC_15, PA_4, PA_5,   PA_7,  PA_6,    PA_0);

Proto    prf(&pc);
DigitalOut myled(PC_13);

Ticker tick_call;
uint8_t led_count = 0;

uint8_t payload[32];

bool is_rgb_toSend = false;
bool is_msg_toSend = false;
bool is_rf_request = false;
uint8_t rf_requester = 0;
uint8_t msg_size = 0;
uint8_t tab_send[32];

uint8_t cmd_to_exec;//0 if no command to execute
uint8_t cmd_params[32];
uint8_t cmd_params_size;

uint32_t ticks = 0;
bool is_newtick = false;

void handle_cmd(uint8_t cmd)
{
	switch(cmd)
	{
		case rf::exec_cmd::send :
		{
			for(uint8_t i = 0;i<cmd_params_size;i++)
			{
				tab_send[i] = cmd_params[i];
			}
			is_msg_toSend = true;
		}
		break;
		case rf::exec_cmd::set_channel :
		{
			uint8_t chan_to_set = cmd_params[0];
			hsm.nrf.selectChannel(chan_to_set);
		}
		break;
		case rf::exec_cmd::get_channel :
		{
			pc.printf("channel:%d\n",hsm.nrf.getChannel());
		}
		break;
		case rf::exec_cmd::set_retries :
		{
			uint8_t nb_retries = cmd_params[0];
			hsm.setRetries(nb_retries);
			pc.printf("nb_retries:%d\n",nb_retries);
		}
		break;
		case rf::exec_cmd::set_ack_delay :
		{
			uint16_t ack_delay = ((uint16_t)cmd_params[0] <<8) + cmd_params[1];
			hsm.setAckDelay(ack_delay);
			pc.printf("ack_delay_ms:%u\n",ack_delay);
		}
		break;
		case rf::exec_cmd::test_rf :
		{
			uint8_t target = cmd_params[0];
			uint8_t channel = cmd_params[1];
			uint8_t nb_ping = cmd_params[2];
			uint8_t nb_success = hsm.test_rf(target,channel,nb_ping);
			if(is_rf_request)
			{
				hsm.send_byte(rf::pid::test_rf_resp,rf_requester,nb_success);
			}
			else
			{
				pc.printf("target:%u;chan:%u;res:%u;nbping:%u\n",
							target,channel,nb_success,nb_ping);
			}
		}
		break;
		default:
		{
			pc.printf("unhandled cmd:0x%X\r\n",cmd);
		}
	}
}

//100 ms
void the_ticker()
{
	ticks++;
	is_newtick = true;
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

void rf_message(uint8_t *data,uint8_t size)
{
	
	if(data[rf::ind::pid] == rf::pid::rgb)
	{
		uint8_t red = data[5];
		uint8_t green = data[6];
		uint8_t blue = data[7];
		rgb_led.set(red,green,blue);
		pc.printf("R:%u;G:%u;B:%u\r\n",red,green,blue);
	}

	if(data[rf::ind::pid] == rf::pid::exec_cmd)
	{
		cmd_params_size = data[rf::ind::size] - rf::ind::p2p_payload;
		cmd_to_exec = data[rf::ind::p2p_payload];
		is_rf_request = true;
		rf_requester = data[rf::ind::source];
		//pc.printf("exec_cmd:0x%02X\n",cmd_to_exec);
		for(uint8_t i = 0; i< cmd_params_size;i++)
		{
			cmd_params[i] = data[rf::ind::p2p_payload+1+i];
		}
		//print_tab(&pc,cmd_params,cmd_params_size);
	}
}
void rf_broadcast(uint8_t *data,uint8_t size)
{
	if(F_NODEID == 57)//57 is IoT action 2
	{
		if(data[rf::ind::pid] == rf::pid::acceleration)
		{
			uint8_t red = data[5];		//from x
			uint8_t green = data[7];	//from y
			uint8_t blue = data[9];		//from z
			rgb_led.set(red,green,blue);
			pc.printf("R:%u;G:%u;B:%u\r\n",red,green,blue);
		}
	}
}

void init()
{
	uint8_t * p_UID = (uint8_t*) 0x1FFFF7E8;
	
	pc.printf("stm32_action> U_ID: ");
	print_tab(&pc,p_UID,12);
	pc.printf("stm32_action> Node ID: %d\r",F_NODEID);

	tick_call.attach(&the_ticker,TICKER_SEC);

	hsm.init(F_CHANNEL);//left to the user for more flexibility on memory management
	hsm.nrf.setMode(nrf::Mode::Rx);//not set by default as to check power consemption with hci
	pc.printf("rfmode:%d;channel:%d\n",hsm.nrf.getMode(),hsm.nrf.getChannel());

	hsm.attach(&rf_message,RfMesh::CallbackType::Message);
	hsm.attach(&rf_broadcast,RfMesh::CallbackType::Broadcast);

	hsm.setNodeId(F_NODEID);

	hsm.setRetries(10);
	hsm.setAckDelay(400);
	
	//hsm.print_nrf();

	#if( 	(USE_APDS_SENSOR == 1) || (USE_APDS_GESTURE == 1) || \
			(USE_APDS_PROXIMITY == 1) || (USE_APDS_LIGHT == 1) )
		bool res;
	#endif
	#if(USE_APDS_SENSOR == 1)
		gsensor_available = gsensor.ginit();
		pc.printf("apds> ginit %u\r\n",gsensor_available);
	#endif
	#if(USE_APDS_GESTURE == 1)
		res = gsensor.enableGestureSensor();
		pc.printf("apds> Gesture sensor enable: %u\r\n",res);
	#endif
	#if(USE_APDS_PROXIMITY == 1)
		res = gsensor.enableProximitySensor();
		pc.printf("apds> Proximity sensor enable: %u\r\n",res);
	#endif
	#if(USE_APDS_LIGHT == 1)
		res = gsensor.enableLightSensor();
		pc.printf("apds> Light sensor enable: %u\r\n",res);
	#endif

}

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
			//pc.printf("NodeId:%u;gesture:%u\r\n",F_NODEID,gest);//expected at rx gateway side
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
		//pc.printf("NodeId:%u;proximity:%u\n",F_NODEID,val);//slows down the 10 ms loop
		#if(APDS_SEND_RGB_DEMO == 1)
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
	static uint16_t light_count = LIGHT_OFFSET;

	if(light_count != 0)
	{
		light_count--;
		//do nothing
	}
	else
	{
		light_count = LIGHT_PERIOD;//refill the counter
		if(gsensor_available)
		{
			uint16_t light_rgb[4];
			gsensor.readAmbientLight(light_rgb[0]);
			gsensor.readRedLight(light_rgb[1]);
			gsensor.readGreenLight(light_rgb[2]);
			gsensor.readBlueLight(light_rgb[3]);
			hsm.broadcast_light_rgb(light_rgb);
			//expected at rx gateway side
			pc.printf("NodeId:%u;light:%u;red:%u,green:%u;blue:%u\r\n",F_NODEID,light_rgb[0],light_rgb[1],light_rgb[2],light_rgb[3]);
			//59 is IoT action 4
		}
	}


}
#endif

void alive_log()
{
	static uint16_t alive_count = ALIVE_OFFSET;
	if(alive_count != 0)
	{
		alive_count--;
		return;
	}

	hsm.broadcast(rf::pid::alive);

	alive_count = ALIVE_PERIOD;
}

void test_channel()
{
	#if (TEST_RUN == 1)
		static uint16_t test_count = TEST_OFFSET;

		if(test_count != 0)
		{
			test_count--;
			return;
		}
		pc.printf("testing channel\n");
		uint8_t nb_success = hsm.test_rf(TEST_TARGET,TEST_CHANNEL);
		pc.printf("nb_success:%d\n",nb_success);
		float col_g = nb_success;
		col_g = col_g * 255 / 100;
		float col_r = 100-nb_success;
		col_r = col_r * 255 / 100;
		uint8_t green = col_g;
		uint8_t red = col_r;
		pc.printf("colors: red:%u, green:%u\n",red,green);
        rgb_led.set(col_r,col_g,0x00);
        wait(1.0);
        rgb_led.set(0x00,0x00,0x00);
		test_count = TEST_PERIOD;
	#endif
}

void cyclic_rf_send()
{
	if(!is_newtick)
	{
		return;
	}
	is_newtick = false;

	#if(USE_APDS_LIGHT == 1)
		apds_log_light_colors();
	#endif

	#if (SEND_ALIVE == 1)
		alive_log();
	#endif
	
	#if (TEST_RUN == 1)
		test_channel();
	#endif
}

void poll_colors()
{
	if(gsensor_available)
	{
		uint16_t light_rgb[4];
		gsensor.readRedLight(light_rgb[1]);
		gsensor.readGreenLight(light_rgb[2]);
		gsensor.readBlueLight(light_rgb[3]);
		rgb_led.set(light_rgb[1]>>3,light_rgb[2]>>3,light_rgb[3]>>3);
		if(F_NODEID == 58)//58 is IoT action 3 flat
		{
			static int div = 0;
			if(((div++) % 10) == 0)
			{
				hsm.send_rgb(59,light_rgb[1]>>3,light_rgb[2]>>3,light_rgb[3]>>3);
				pc.printf("sending rgb to 59\r\n");
			}
		}
	}
}

int main() 
{
	init();

	test_RGB();

	hsm.broadcast(rf::pid::reset);

	pc.printf("main>into the while\n");
    
    while(1) 
    {
		wait_ms(LOOP_MS_WAIT);

		rf_bridge_delegate();//cyclic check if bridge has to send from main context
		
		if(hsm.nRFIrq.read() == 0)
		{
			pc.printf("irq pin Low, missed interrupt, re init()\n");
			hsm.init(F_CHANNEL);
			hsm.broadcast(rf::pid::reset);//notify if this issue happens
			wait(5);//avoid fast cyclic reset
		}

		if(cmd_to_exec)
		{
			handle_cmd(cmd_to_exec);
			cmd_to_exec = 0;
			is_rf_request = false;
		}

		cyclic_rf_send();

		poll_colors();

		#if(USE_APDS_PROXIMITY == 1)
			apds_poll_proximity();
		#endif

		#if(USE_APDS_GESTURE == 1)
			apds_poll_gesture();
		#endif

	}
}
