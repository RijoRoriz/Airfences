/*
	defines.h - files with all the defines of the field on Airfences project
	Created by Ricardo Roriz and Pedro Costa, 12 December 2016.
	Last Update by Ricardo Roriz, 4 February 2017
*/

#ifndef DEFINES_H_
#define DEFINES_H_

#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <iostream>
#include <string.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <wiringPiI2C.h>

#include <mqueue.h>
#include <pthread.h>
#include <semaphore.h>
#include <iomanip>

#define MQSENSORS      "/mq_sensors"
#define MQSENSORSLEN   2
#define MQGPS          "/mq_gps"
#define MQGPSLEN       24
#define MQWIFI         "/mq_wifi"
#define MQ_WIFI_R      "/mq_wifi_R"
#define MQWIFILEN      200
#define MQRFCOM        "/mq_rf"
#define MQRFCOMLEN     32
#define BUFF_LEN       33
#define MAX_MSG_LEN    10000

#define TIMEOUT_RF_RECEIVER  4


#define OUTZONE     4
#define REDZONE     3
#define YELLOWZONE  2
#define GREENZONE   1


/*******************************************************************************
                            RF - NRF905
*******************************************************************************/
#define FIELDADDR0 0xFF
#define FIELDADDR1 0xFF
#define FIELDADDR2 0x00
#define FIELDADDR3 0x01
#define FIELDADDRT 1;

// Registers
#define NRF905_WRITE_CONF		     0x00
#define NRF905_READ_CONF			  0x10
#define NRF905_WRITE_TX_PAYLOAD	  0x20
#define NRF905_READ_TX_PAYLO		  0x21
#define NRF905_WRITE_TX_ADDR		  0x22
#define NRF905_READ_TX_ADDR		  0x23
#define NRF905_READ_RX_PAYLOAD	  0x24

// Pins - wiringpi
#define TX_EN		21	// TX / RX mode pin - GPIO5
#define CE			29	// T/R enable pin - GPIO21
#define CD			25	// Carrier detect pin - GPIO26
#define AM			23  // Adressmatch pin - GPIO13
#define DR			24  // T/X dataReady pin - GPIO19
#define MISO		13  // SPI OUT - GPIO9
#define MOSI 		12  // SPI IN - GPIO10
#define SCK			14  // SPI CLOCK - GPIO11
#define CSN			10  // SPI slave select pin, send new commands - GPIO8
#define PWR_RF 	0  // Power NRF905 RASPBERRY PI 3

// Configurations
#define NRF905_PAYLOAD_SIZE 		32
#define NRF905_STATE_IDLE 			0
#define NRF905_STATE_TX					1
#define NRF905_STATE_RX					2
#define NRF905_ADDR_SIZE				0x01
#define NRF905_TIMEOUT 					180000 // 3 min

#endif /*DEFINES_*/
