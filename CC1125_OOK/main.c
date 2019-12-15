/*
 * CC1125_OOK.c
 *
 * Created: 31-10-2019 06:50:30 PM
 * Author : Hrithik
 */ 

#define F_CPU 2000000

#include<avr/io.h>
//#include "uart.h"
//#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include <inttypes.h>
#include <avr/interrupt.h>
//#include "uart_8_32.h"
#include "ax25.c"
#include "USART.h"
#include "usart.c"
#include "avr_compiler.h"
#include "pmic_driver.h"
#include "spi_driver.h"
#include "spi_driver.c"
#include "cc112x_spi.h"
//#include "cc112x_serial_mode_reg_config.h"
//#include <uart.c>

//Global variable
unsigned char data_transmit[61],data_receive[60],transmit_enable=0,receive_enable=1,SWITCH[8]="$SWITCH",switch_num=0,transmit_check=0;
unsigned char address[61]="PROPOSAL_ACCEPTED";
unsigned int pkt_length=48,check=0;
int transmitFlag=0;

SPI_Master_t spiMasterE; //Changed from spiMasterC to spiMasterE

//#define NUM_BYTES 3
#define USART USARTC0
USART_data_t USART_data;
uint8_t buffer[80];
//uint8_t receiveArray[NUM_BYTES];
USART_data_t USART_data;
char sendArray[61];
bool success;

//UART baudrate calculation
//#define F_CPU 8000000			// oscillator-frequency in Hz, also needs delay.h header (Defined in Project->Configuration options)
#define UART_BAUD_RATE 9600
//for Asynchronous Normal mode (U2x=0) formula will be following:
#define UART_BAUD_CALC (((F_CPU/(UART_BAUD_RATE*16UL))-1)) //16UL still remains regardless of the frequency of crystal

//ATXmega

#define SPICS 4		//4//Port E bit 4 (pin): SS->chip select for CC
#define SPIDO 5		//5//Port E bit 5 (pin): MOSI->data out(data to CC_SI)
#define SPIDI 6		//6//Port E bit 6 (pin): MISO->data in (data from CC_SO)
#define SPICLK 7	//7//Port E bit 7 (pin): SCK->clock for CC

//CC1125

#define CC_GPIO0   0	//Port E bit 0 (pin) (pin10 of CC)
#define CC_GPIO2   2	//Port E bit 2 (pin) (pin4 of CC)
#define CC_CSN     4	//4//(pin 11 of CC)-> chip select
#define CC_SI      5	//5//(pin 7 of CC)-> data input
#define CC_SO      6	//6//(pin 9 of CC)-> multiplexed with GD01:-> data ouptut
#define CC_SCLK    7	//7//(pin 8 of CC)-> clock input

// CC2500/CC1100/CC1101 STROBE, CONTROL AND STATUS REGISTER


/*
#define CCxxx0_IOCFG2       0x00        // GDO2 output pin configuration
#define CCxxx0_IOCFG1       0x01        // GDO1 output pin configuration
#define CCxxx0_IOCFG0       0x02        // GDO0 output pin configuration
#define CCxxx0_FIFOTHR      0x03        // RX FIFO and TX FIFO thresholds
#define CCxxx0_SYNC1        0x04        // Sync word, high byte
#define CCxxx0_SYNC0        0x05        // Sync word, low byte
#define CCxxx0_PKTLEN       0x06        // Packet length
#define CCxxx0_PKTCTRL1     0x07        // Packet automation control
#define CCxxx0_PKTCTRL0     0x08        // Packet automation control
#define CCxxx0_ADDR         0x09        // Device address
#define CCxxx0_CHANNR       0x0A        // Channel number
#define CCxxx0_FSCTRL1      0x0B        // Frequency synthesizer control
#define CCxxx0_FSCTRL0      0x0C        // Frequency synthesizer control
#define CCxxx0_FREQ2        0x0D        // Frequency control word, high byte
#define CCxxx0_FREQ1        0x0E        // Frequency control word, middle byte
#define CCxxx0_FREQ0        0x0F        // Frequency control word, low byte
#define CCxxx0_MDMCFG4      0x10        // Modem configuration
#define CCxxx0_MDMCFG3      0x11        // Modem configuration
#define CCxxx0_MDMCFG2      0x12        // Modem configuration
#define CCxxx0_MDMCFG1      0x13        // Modem configuration
#define CCxxx0_MDMCFG0      0x14        // Modem configuration
#define CCxxx0_DEVIATN      0x15        // Modem deviation setting
#define CCxxx0_MCSM2        0x16        // Main Radio Control State Machine configuration
#define CCxxx0_MCSM1        0x17        // Main Radio Control State Machine configuration
#define CCxxx0_MCSM0        0x18        // Main Radio Control State Machine configuration
#define CCxxx0_FOCCFG       0x19        // Frequency Offset Compensation configuration
#define CCxxx0_BSCFG        0x1A        // Bit Synchronization configuration
#define CCxxx0_AGCCTRL2     0x1B        // AGC control
#define CCxxx0_AGCCTRL1     0x1C        // AGC control
#define CCxxx0_AGCCTRL0     0x1D        // AGC control
#define CCxxx0_WOREVT1      0x1E        // High byte Event 0 timeout
#define CCxxx0_WOREVT0      0x1F        // Low byte Event 0 timeout
#define CCxxx0_WORCTRL      0x20        // Wake On Radio control
#define CCxxx0_FREND1       0x21        // Front end RX configuration
#define CCxxx0_FREND0       0x22        // Front end TX configuration
#define CCxxx0_FSCAL3       0x23        // Frequency synthesizer calibration
#define CCxxx0_FSCAL2       0x24        // Frequency synthesizer calibration
#define CCxxx0_FSCAL1       0x25        // Frequency synthesizer calibration
#define CCxxx0_FSCAL0       0x26        // Frequency synthesizer calibration
#define CCxxx0_RCCTRL1      0x27        // RC oscillator configuration
#define CCxxx0_RCCTRL0      0x28        // RC oscillator configuration
#define CCxxx0_FSTEST       0x29        // Frequency synthesizer calibration control
#define CCxxx0_PTEST        0x2A        // Production test
#define CCxxx0_AGCTEST      0x2B        // AGC test
#define CCxxx0_TEST2        0x2C        // Various test settings
#define CCxxx0_TEST1        0x2D        // Various test settings
#define CCxxx0_TEST0        0x2E        // Various test settings
*/

//Strobe commands
#define CCxxx0_SRES			0x30		//Reset chip
#define CCxxx0_SFSTXON		0x31		//Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1)
//If in RX/TX: Go to a wait state where only the synthesizer is running (for quick RX/TX turnaround)
#define CCxxx0_SXOFF		0x32		//Turn of crystal oscillator
#define CCxxx0_SCAL			0x33		//Calibrate frequency synthesizer and turn it off (enables quick start)
#define CCxxx0_SRX			0x34		//Enable RX. Perform calibration first if coming from IDLE and MCSM0.FS_AUTOCAL=1
#define CCxxx0_STX			0x35		//In IDLE state: Enable TX. Perform calibration first if MCSM0.FS_AUTOCAL=1.
//If in RX state and CCA is enabled: only go to TX if channel is clear


int main(void)
{
    SPI_Master_init();
	uint8_t d=0b01001100;
	while(true)
	{
		SPI_send8(d);
	}
}

