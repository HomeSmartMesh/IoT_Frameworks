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
#define USE_RGB_LED 1
//APDS9960 (Colorlight sensor, gesture)
#define USE_APDS_SENSOR 0
#define USE_APDS_GESTURE 0
#define USE_APDS_PROXIMITY 0
#define USE_APDS_LIGHT 0

#define USE_BME_SENSOR 0

#define USE_BME_VI_SENSOR 1

#define SEND_ALIVE  1
#define BRIDGE_MODE 1
#define RGB_DEMO 	0

#define ALIVE_SEC		5380
#define LOOP_MS_WAIT	10
#define LOG_LOGHT_COUNT 860
//--------------------------------------------------------------------------------------
//TODO should have a board as a target
#define RF_BOARD_DONGLE 1
#define RF_BOARD_PIO 	0
//--------------------------------------------------------------------------------------
Serial   pc(PB_10, PB_11, 115200);

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
	RfMesh hsm(&pc,spi_module,           PC_15, PA_4, PA_5,   PA_7,  PA_6,    PA_0);
#elif (RF_BOARD_PIO == 1)
uint8_t spi_module = 2;
#endif

#if(USE_BME_SENSOR == 1)
	#include "BME280.h"
	BME280 bme(PB_7,PB_6);
#endif
#if(USE_BME_VI_SENSOR == 1)
	#include "bme280.h"
	I2C i2c(PB_7,PB_6);
	BME280 bme280(&pc,&i2c);
#endif


DigitalOut debug_pio(PB_13);

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
bool is_sametick = false;

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

#if(USE_BME_SENSOR == 1)
void bme_send_temperature()
{
	float temp = bme.getTemperature();
	if((temp > -20) && (temp<100))
	{
		int16_t temp_i = temp * 100;
		hsm.broadcast_int16(rf::pid::temperature,temp_i);
		pc.printf("Temperature:%d\n",temp_i);
	}
	else
	{
		pc.printf("Temperature out of range\n");
	}
}

void bme_send_humidity()
{
	float hum = bme.getHumidity();
	int16_t hum_i = hum * 100;
	hsm.broadcast_int16(rf::pid::humidity,hum_i);
	pc.printf("Humidity:%d\n",hum_i);
}

void bme_send_pressure()
{
	float press = bme.getPressure();
	int16_t press_i = press * 100;
	hsm.broadcast_int16(rf::pid::pressure,press_i);
	pc.printf("Pressure:%d\n",press_i);
}
#endif

//100 ms
void the_ticker()
{
	ticks++;
	is_sametick = false;
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
			pc.printf("R:%u;G:%u;B:%u\r\n",red,green,blue);
		}
	#endif
	
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

void init()
{
	uint8_t * p_UID = (uint8_t*) 0x1FFFF7E8;
	
	pc.printf("stm32_bridge> U_ID: ");
	print_tab(&pc,p_UID,12);
	pc.printf("stm32_bridge> Node ID: %d\r",F_NODEID);

	tick_call.attach(&the_ticker,0.1);

	hsm.init(F_CHANNEL);//left to the user for more flexibility on memory management
	hsm.nrf.setMode(nrf::Mode::Rx);//not set by default as to check power consemption with hci
	pc.printf("mode:%d;channel:%d\n",hsm.nrf.getMode(),hsm.nrf.getChannel());

    hsm.attach(&rf_sniffed,RfMesh::CallbackType::Sniff);
	hsm.attach(&rf_message,RfMesh::CallbackType::Message);

#if (BRIDGE_MODE == 1)
	hsm.setBridgeMode();
	pc.printf("stm32_bridge> listening in bridge Mode\n");
#else
	pc.printf("stm32_bridge> Not in bridge Mode !!! Just a node\n");
#endif

	hsm.setNodeId(F_NODEID);


	hsm.setRetries(10);
	hsm.setAckDelay(400);
	
	//hsm.print_nrf();

	bool res;
	#if(USE_APDS_SENSOR == 1)
		res = gsensor.ginit();
		pc.printf("apds> ginit %u\r\n",res);
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

	#if(USE_BME_VI_SENSOR == 1)
		bme280.measure();
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
	static uint16_t light_count = LOG_LOGHT_COUNT;// ~ 10 s

	if(light_count == 0)
	{
		uint16_t light_rgb[4];
		gsensor.readAmbientLight(light_rgb[0]);
		gsensor.readRedLight(light_rgb[1]);
		gsensor.readGreenLight(light_rgb[2]);
		gsensor.readBlueLight(light_rgb[3]);
		hsm.broadcast_light_rgb(light_rgb);
		//expected at rx gateway side
		//pc.printf("NodeId:%u;light:%u;red:%u,green:%u;blue:%u\r\n",F_NODEID,light_rgb[0],light_rgb[1],light_rgb[2],light_rgb[3]);

		light_count = LOG_LOGHT_COUNT;//~10 sec
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
    
	uint16_t alive_count = ALIVE_SEC;

    while(1) 
    {
		wait_ms(LOOP_MS_WAIT);
		if(cmd_to_exec)
		{
			handle_cmd(cmd_to_exec);
			cmd_to_exec = 0;
			is_rf_request = false;
		}
		
		#if(USE_APDS_LIGHT == 1)
			apds_log_light_colors();
		#endif

		#if(USE_APDS_PROXIMITY == 1)
			apds_poll_proximity();
		#endif

		#if(USE_APDS_GESTURE == 1)
			apds_poll_gesture();
		#endif

		#if(USE_BME_SENSOR == 1)
		//100ms x 100 => 10 s
		if(!is_sametick)
		{
			if(((ticks) % 100) == 0)
			{
				bme_send_temperature();
				bme_send_humidity();
				bme_send_pressure();
			}
			is_sametick = true;
		}
		#endif

		if(hsm.nRFIrq.read() == 0)
		{
			pc.printf("irq pin Low, missed interrupt, re init()\n");
			hsm.init(F_CHANNEL);
			hsm.broadcast(rf::pid::reset);//notify if this issue happens
		}
		if(alive_count == 0)
		{
			#if (SEND_ALIVE == 1)
			hsm.broadcast(rf::pid::alive);
			#endif
			//pc.printf("NodeId:%u;status:Alive\r\n",F_NODEID);//expected at rx gateway side
			alive_count = ALIVE_SEC;
		}
		else
		{
			alive_count--;
		}
		rf_bridge_delegate();//cyclic check if bridge has to send from main context
	}
}
