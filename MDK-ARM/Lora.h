#ifndef __LORA_H
#define __LORA_H
#include "math.h"
#include "stdint.h"
#include "gpio.h"
#include "stm32f1xx_hal.h"
//#include "string.h"
//Remember Reset pin = High
extern  uint16_t powerReadInt;
// registers
#define REG_FIFO                 0x00
#define REG_OP_MODE              0x01
#define REG_FRF_MSB              0x06
#define REG_FRF_MID              0x07
#define REG_FRF_LSB              0x08
#define REG_PA_CONFIG            0x09
#define REG_OCP                  0x0b
#define REG_LNA                  0x0c
#define REG_FIFO_ADDR_PTR        0x0d
#define REG_FIFO_TX_BASE_ADDR    0x0e
#define REG_FIFO_RX_BASE_ADDR    0x0f
#define REG_FIFO_RX_CURRENT_ADDR 0x10
#define REG_IRQ_FLAGS            0x12
#define REG_RX_NB_BYTES          0x13
#define REG_PKT_SNR_VALUE        0x19
#define REG_PKT_RSSI_VALUE       0x1a
#define REG_MODEM_CONFIG_1       0x1d
#define REG_MODEM_CONFIG_2       0x1e
#define REG_PREAMBLE_MSB         0x20
#define REG_PREAMBLE_LSB         0x21
#define REG_PAYLOAD_LENGTH       0x22
#define REG_MODEM_CONFIG_3       0x26
#define REG_FREQ_ERROR_MSB       0x28
#define REG_FREQ_ERROR_MID       0x29
#define REG_FREQ_ERROR_LSB       0x2a
#define REG_RSSI_WIDEBAND        0x2c
#define REG_DETECTION_OPTIMIZE   0x31
#define REG_INVERTIQ             0x33
#define REG_DETECTION_THRESHOLD  0x37
#define REG_SYNC_WORD            0x39
#define REG_INVERTIQ2            0x3b
#define REG_DIO_MAPPING_1        0x40
#define REG_VERSION              0x42
#define REG_PA_DAC               0x4d

// modes
#define MODE_LONG_RANGE_MODE     0x80
#define MODE_SLEEP               0x00
#define MODE_STDBY               0x01
#define MODE_TX                  0x03
#define MODE_RX_CONTINUOUS       0x05
#define MODE_RX_SINGLE           0x06
#define IMPLICIT_HEADER						1
#define EXPLICIT_HEADER						0


// PA config
#define PA_BOOST                 0x80

#define PA_OUTPUT_RFO_PIN          0
#define PA_OUTPUT_PA_BOOST_PIN     1

// IRQ masks
#define IRQ_TX_DONE_MASK           0x08
#define IRQ_PAYLOAD_CRC_ERROR_MASK 0x20
#define IRQ_RX_DONE_MASK           0x40
#define FREQUENCY 433000000


#define MAX_PKT_LENGTH           255

extern SPI_HandleTypeDef hspi2;

//read 8bit the register address = (uint8_t address)
uint8_t readRegLora_8Bit(uint8_t address);

//read Burst register start at (uint8_t address) has Length = ( uint8_t length ) insert into (uint8_t *receiveData)
void readRegLora(uint8_t address, uint8_t *receiveData, uint8_t length);

//write Data = ( uint8_t data) to register address = (uint8_t address )
void writeRegLora(uint8_t address, uint8_t data);

//Write n byte into FIFO
void writeToFIFO(uint8_t *data, uint8_t length);

//Setting Frequency, Normal 433 Mhz
void LoraSetFreq(uint32_t frequency);

//Sleep Mode
void LoraSleep(void);

//Idle Mode
void LoraIdle(void);

//Setting OverLoad current protection
void LoraSetOCP(uint8_t mA);

//Begin Lora: Check version, return 0 if error.
uint16_t LoraBegin(void);

//set Spreading Factor
void LoraSetSpreadingFactor(uint8_t sf);
//get Spreading Factor
uint16_t LoraGetSpreadingFactor(void);

//set Signal BandWidth
void LoraSetSignalBW(uint32_t bandwidth);
//get Signal Band Width
uint32_t LoraGetSignalBW(void);


//set Coding Rate
void LoraSetCodingRate(int denominator);

//Lora is Transmitting
uint8_t LoraIsTransmitting(void);

//Set imlicit Header Mode
void LoraImplicitHeaderMode(void);

//Set explicit Header Mode
void LoraExplicitHeaderMode(void);

//**************************************************************/
// set Lora into Rx mode : Continous or Single
void LoraRxMode(uint8_t RxMode);
//**************************************************************/
void LoraTxMode(void);

/**************************************************************/
//Transfer Data have first address = buffer, and length = uint8_t Size
//return Size transfered
uint8_t LoraTransfer(uint8_t *buffer, uint8_t size);

/**************************************************************/
void LoraRead(uint8_t *arrayReadData);

/**************************************************************/
//uint16_t LoraReadInt(void);
#endif
