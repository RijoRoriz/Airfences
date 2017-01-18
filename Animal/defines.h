#ifndef DEFINES_H_
#define DEFINES_H_

#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <iostream>

#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <wiringPiI2C.h>

#include <mqueue.h>
#include <pthread.h>

#define MQBATTEMP      "/mq_batTemp"
#define MQBATTEMPLEN   6
#define MQGPS          "/mq_gps"
#define MQGPSLEN       24
#define MQWIFI         "/mq_wifi"
#define MQWIFILEN      200
#define MQRFCOM        "/mq_rf"
#define MQRFCOMLEN     32

#define MIN_BATTERY_LEVEL 1.5

#define OUTZONE     4
#define REDZONE     3
#define YELLOWZONE  2
#define GREENZONE   1

/*******************************************************************************
                              ADC - ADS1015
*******************************************************************************/

#define ADS1015_ADDRESS                 (0x48)    // 1001 000 (ADDR = GND)

//POINTER REGISTER
#define ADS1015_REG_POINTER_CONVERT     (0x00)
#define ADS1015_REG_POINTER_CONFIG      (0x01)
#define ADS1015_REG_POINTER_LOWTHRESH   (0x02)
#define ADS1015_REG_POINTER_HITHRESH    (0x03)

//CONFIG REGISTER

#define ADS1015_REG_CONFIG_OS_SINGLE    (0x8000)  // Write: Set to start a single-conversion

#define ADS1015_REG_CONFIG_MUX_SINGLE_0 (0x4000)  // Single-ended AIN0
#define ADS1015_REG_CONFIG_MUX_SINGLE_1 (0x5000)  // Single-ended AIN1
#define ADS1015_REG_CONFIG_MUX_SINGLE_2 (0x6000)  // Single-ended AIN2
#define ADS1015_REG_CONFIG_MUX_SINGLE_3 (0x7000)  // Single-ended AIN3

#define ADS1015_REG_CONFIG_PGA_MASK     (0x0E00)
#define ADS1015_REG_CONFIG_PGA_6_144V   (0x0000)  // +/-6.144V range = Gain 2/3
#define ADS1015_REG_CONFIG_PGA_4_096V   (0x0200)  // +/-4.096V range = Gain 1
#define ADS1015_REG_CONFIG_PGA_2_048V   (0x0400)  // +/-2.048V range = Gain 2 (default)
#define ADS1015_REG_CONFIG_PGA_1_024V   (0x0600)  // +/-1.024V range = Gain 4
#define ADS1015_REG_CONFIG_PGA_0_512V   (0x0800)  // +/-0.512V range = Gain 8
#define ADS1015_REG_CONFIG_PGA_0_256V   (0x0A00)  // +/-0.256V range = Gain 16

#define ADS1015_REG_CONFIG_MODE_MASK    (0x0100)
#define ADS1015_REG_CONFIG_MODE_CONTIN  (0x0000)  // Continuous conversion mode
#define ADS1015_REG_CONFIG_MODE_SINGLE  (0x0100)  // Power-down single-shot mode (default)

#define ADS1015_REG_CONFIG_DR_MASK      (0x00E0)
#define ADS1015_REG_CONFIG_DR_128SPS    (0x0000)  // 128 samples per second
#define ADS1015_REG_CONFIG_DR_250SPS    (0x0020)  // 250 samples per second
#define ADS1015_REG_CONFIG_DR_490SPS    (0x0040)  // 490 samples per second
#define ADS1015_REG_CONFIG_DR_920SPS    (0x0060)  // 920 samples per second
#define ADS1015_REG_CONFIG_DR_1600SPS   (0x0080)  // 1600 samples per second (default)
#define ADS1015_REG_CONFIG_DR_2400SPS   (0x00A0)  // 2400 samples per second
#define ADS1015_REG_CONFIG_DR_3300SPS   (0x00C0)  // 3300 samples per second

#define ADS1015_REG_CONFIG_CMODE_MASK   (0x0010)
#define ADS1015_REG_CONFIG_CMODE_TRAD   (0x0000)  // Traditional comparator with hysteresis (default)
#define ADS1015_REG_CONFIG_CMODE_WINDOW (0x0010)  // Window comparator

#define ADS1015_REG_CONFIG_CPOL_MASK    (0x0008)
#define ADS1015_REG_CONFIG_CPOL_ACTVLOW (0x0000)  // ALERT/RDY pin is low when active (default)
#define ADS1015_REG_CONFIG_CPOL_ACTVHI  (0x0008)  // ALERT/RDY pin is high when active

#define ADS1015_REG_CONFIG_CLAT_MASK    (0x0004)  // Determines if ALERT/RDY pin latches once asserted
#define ADS1015_REG_CONFIG_CLAT_NONLAT  (0x0000)  // Non-latching comparator (default)
#define ADS1015_REG_CONFIG_CLAT_LATCH   (0x0004)  // Latching comparator

#define ADS1015_REG_CONFIG_CQUE_MASK    (0x0003)
#define ADS1015_REG_CONFIG_CQUE_1CONV   (0x0000)  // Assert ALERT/RDY after one conversions
#define ADS1015_REG_CONFIG_CQUE_2CONV   (0x0001)  // Assert ALERT/RDY after two conversions
#define ADS1015_REG_CONFIG_CQUE_4CONV   (0x0002)  // Assert ALERT/RDY after four conversions
#define ADS1015_REG_CONFIG_CQUE_NONE    (0x0003)  // Disable the comparator and put ALERT/RDY in high state (default)

/*******************************************************************************
                            RF - NRF905
*******************************************************************************/
#define FIELDADDR0 0x00
#define FIELDADDR1 0x00
#define FIELDADDR2 0x00
#define FIELDADDR3 0x00

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
#define PWR_RF 	22  // Power NRF905 RASPBERRY PI B+

// Configurations
#define NRF905_PAYLOAD_SIZE 		32
#define NRF905_STATE_IDLE 			0
#define NRF905_STATE_TX					1
#define NRF905_STATE_RX					2
#define NRF905_ADDR_SIZE				0x01
#define NRF905_TIMEOUT 					180000 // 3 min

#endif /*DEFINES_*/
