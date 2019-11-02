#include "common.h"
#include "ax25.h"
#include "util/crc16.h"

unsigned short crc16(unsigned char* data_p, unsigned char length);
int Data_Length = 34;
void make_ax25_frame_from_data(uint8_t *frame, uint8_t *data)
{
	uint8_t i, buf[80], size = 0;
	uint16_t crc;
	//char tp[9] = "PRATHA";
	frame[size++] = AX_FLAG;
	
	memcpy(buf, "CQ    ", AX_ADDR_SIZE);
	for(i = 0; i < AX_ADDR_SIZE; i++)
	frame[i + size] = buf[i] ;
	size += AX_ADDR_SIZE;
	frame[size++] = 0b01100000;
	
	memcpy(buf, "VU2BUG", AX_ADDR_SIZE);
	for(i = 0; i < AX_ADDR_SIZE; i++)
	frame[i + size] = buf[i] ;
	size += AX_ADDR_SIZE;
	frame[size++] = 0b01101000;
	
	memcpy(buf, "RELAY ", AX_ADDR_SIZE);
	for(i = 0; i < AX_ADDR_SIZE; i++)
	frame[i + size] = buf[i] ;
	size += AX_ADDR_SIZE;
	frame[size++] = 0b01100001;
	
	frame[size++] = AX_CONTROL;
	frame[size++] = AX_PID;
	
	memcpy(buf, data, Data_Length);
	for(i = 0; i < Data_Length; i++)
	frame[i + size] = buf[i] ;
	size += Data_Length;

	//tp = "VU2BUG";
	crc = crc16(frame+1,Data_Length+23);
	
	memcpy(buf, &crc, 2);
	for(i = 0; i < 2; i++)
	frame[i + size] = buf[i] ;
	size += 2;
	
	frame[size++] = AX_FLAG;
	
}

unsigned short crc16(unsigned char* data_p, unsigned char length){
	unsigned char x;
	unsigned short crc = 0xFFFF;

	while (length--){
		x = crc >> 8 ^ *data_p++;
		x ^= x>>4;
		crc = (crc << 8) ^ ((unsigned short)(x << 12)) ^ ((unsigned short)(x <<5)) ^ ((unsigned short)x);
	}
	return crc;
}

//Onward functions not used
uint16_t update_crc_16(uint8_t byte, uint16_t crc)
{
	uint8_t bit, crcbit, j;
	
	for (j = 0; j < 8; j++)
	{
		bit = byte & 0x01;
		crcbit = crc & 0x01;
		crc = crc >> 1;
		if (bit != crcbit)
		crc ^= CRC_CCIT_REV;
		byte = byte >> 1;
	}
	
	return crc;
}

uint16_t crc16_update(uint16_t crc, uint8_t a)
{
	int i;

	crc ^= a;
	for (i = 0; i < 8; ++i)
	{
		if (crc & 1)
		crc = (crc >> 1) ^ 0xA001;
		else
		crc = (crc >> 1);
	}

	return crc;
}

uint16_t calculate_crc_16(uint8_t *data, uint8_t size)
{
	uint16_t crc = 0xFFFF;
	uint8_t i;
	for(i = 0; i < size; i++)
	crc = update_crc_16(data[i], crc);
	return ~crc;
}


