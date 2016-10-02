/** @file ClockUartLed.h
 *
 * @author Wassim FILALI taken over from http://blog.mark-stevens.co.uk/
 *
 * @compiler IAR STM8
 *
 *
 * $Date: 20.09.2015
 * $Revision:
 *
 */


#define delay_1us(); delay(0);

#define delay_10us(); delay(18);

#define delay_50us(); delay(98);

#define delay_100us(); delay(198);

#define delay_1ms(); delay(1998);

#define delay_10ms(); delay(19998);


void InitialiseUART();

void putc(char c);

void UARTPrintf(char const *message);

void UARTPrintfLn(char const *message);

void UARTPrintfHex(unsigned char val);

void UARTPrintfHexTable(unsigned char *pval,unsigned char length);

void UARTPrintfHexLn(unsigned char val);

void UARTPrintf_sint(signed int num);

void UARTPrintf_uint(unsigned int num);

void delay(unsigned int n);

void delay_1ms_Count(unsigned int n);


// L E D 
void Initialise_TestLed_GPIO_B5();
void Test_Led_On();
void Test_Led_Off();

