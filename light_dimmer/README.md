# Light Dimmer
## Used Boards
- [ServoMotors Controller PCB](https://github.com/wassfila/STM8_IoT_Boards)

<img src="https://github.com/wassfila/IoT_Frameworks/blob/master/joystick_servo/ServoController.JPG" height=300>

- [4CH AC Light Dimmer Triac](https://www.facebook.com/krida.electronics)

<img src="https://github.com/wassfila/IoT_Frameworks/blob/master/light_dimmer/DimmingCircuit.jpg" height=300>

# Servo Board Modif
- STM8S module modified : 3.3V regulator removed, joined 3.3 to 5V

# Sync Signal
<img src="https://github.com/wassfila/IoT_Frameworks/blob/master/light_dimmer/SYNC_Signal.png" height=300>

<img src="https://github.com/wassfila/IoT_Frameworks/blob/master/light_dimmer/SYNC_Signal_Shift.png" height=300>

# Program Flow
## Dimming Triggers Control
- Sync Pulse Interrupt : starts the Timer1
- Timer 1 step : 1 us
- Timer 1 with 4 Channels Output compare Interrupts generation
- Pulse of 10 us Trigger generated during the interrupt

## Current sensing
- ACS712 ELCTR-05B-T => 185 mV/A
- 0A => 2.5 V => ADC_VAL = 512
- 1A => 2.685 V => ADC_VAL = 550

# Consumption
- total Boards consumption (Servo Boards + Dimmer Logic 5V part + ASC712) : 27 mA

# Current signal
- Noisiny sensor ACS712 : with no current => min_c = -17 ; max_c = 14 ; 37u => 1A
