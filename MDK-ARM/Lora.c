#include "Lora.h"
uint8_t readRegLora_8Bit(uint8_t address){
	uint8_t result=0;
	HAL_GPIO_WritePin(GPIOA, LORA_NSS_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	while ( HAL_SPI_Transmit(&hspi2,&address,1,100) != HAL_OK );
	while ( HAL_SPI_Receive(&hspi2,&result,1,100) != HAL_OK );
	HAL_GPIO_WritePin(GPIOA, LORA_NSS_Pin, GPIO_PIN_SET);
	return result;
}
/*****************************************************************/
void readRegLora(uint8_t address, uint8_t *receiveData, uint8_t length){
	HAL_GPIO_WritePin(GPIOA, LORA_NSS_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	while ( HAL_SPI_Transmit(&hspi2,&address,1,100) != HAL_OK );
	while ( HAL_SPI_Receive(&hspi2,receiveData,length,100) != HAL_OK );
	HAL_GPIO_WritePin(GPIOA, LORA_NSS_Pin, GPIO_PIN_SET);
}

/*****************************************************************/
void writeRegLora(uint8_t address, uint8_t data){
	uint8_t add;
	add =	address | 0x80;
	HAL_GPIO_WritePin(GPIOA, LORA_NSS_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	while ( HAL_SPI_Transmit(&hspi2,&add,1,100) != HAL_OK );
	while ( HAL_SPI_Transmit(&hspi2,&data,1,100) != HAL_OK );
	HAL_GPIO_WritePin(GPIOA, LORA_NSS_Pin, GPIO_PIN_SET);
}

/*****************************************************************/
void writeToFIFO(uint8_t *data, uint8_t length){
	uint8_t add= REG_FIFO | 0x80;
	HAL_GPIO_WritePin(GPIOA, LORA_NSS_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	while ( HAL_SPI_Transmit(&hspi2,&add,1,100) != HAL_OK );
	while ( HAL_SPI_Transmit(&hspi2,data,length,100) != HAL_OK );
	HAL_GPIO_WritePin(GPIOA, LORA_NSS_Pin, GPIO_PIN_SET);
}

/*****************************************************************/
void LoraSetFreq(uint32_t frequency){
	uint64_t frf = ((uint64_t)frequency << 19) / 32000000;
	writeRegLora(REG_FRF_MSB, (uint8_t)(frf >> 16));
	writeRegLora(REG_FRF_MID, (uint8_t)(frf >> 8));
	writeRegLora(REG_FRF_LSB, (uint8_t)(frf >> 0));
}

/*****************************************************************/
void LoraSleep(void){
	writeRegLora(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_SLEEP);
}

/*****************************************************************/
void LoraIdle(void){
	writeRegLora(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
}

//**********************************************************
uint16_t LoraBegin(void){
	
	//Blink Reset Pin
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8,GPIO_PIN_SET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8,GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8,GPIO_PIN_SET);
	HAL_Delay(10);
	
	//Blink Slave Selection
	HAL_GPIO_WritePin(GPIOA, LORA_NSS_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOA, LORA_NSS_Pin, GPIO_PIN_SET);
	HAL_Delay(10);

	//Check Version
	uint8_t version = readRegLora_8Bit(REG_VERSION);
  if (version != 0x12) return 0;
	
	//put in sleep mode
	LoraSleep();
	
	//Set Frequency
	LoraSetFreq(FREQUENCY);
	
	//Set base address FIFO
	writeRegLora(REG_FIFO_TX_BASE_ADDR, 0);
	writeRegLora(REG_FIFO_RX_BASE_ADDR, 0);
	
	//set explicitheader mode
	//writeRegLora(REG_MODEM_CONFIG_1, readRegLora_8Bit(REG_MODEM_CONFIG_1) & 0xfe);
	LoraExplicitHeaderMode();
	// set LNA boost
	writeRegLora(REG_LNA, readRegLora_8Bit(REG_LNA) | 0x03);
	
	// set auto AGC
  writeRegLora(REG_MODEM_CONFIG_3, 0x04);
	
	//ocpTrim
	writeRegLora(REG_OCP, 0x20 | (0x1F & 0x0B));
	
	//TX power
	writeRegLora(REG_PA_CONFIG, PA_BOOST | (17 - 2));
	
	//put in standby Mode
	LoraIdle();
	return 1;
}

//*****************************************************
void LoraEnd(void){
	LoraSleep();
}

//****************************************************
void LoraSetLdoFlag(void){
	// Section 4.1.1.5
  long symbolDuration = 1000 / ( LoraGetSignalBW() / (1L << LoraGetSpreadingFactor()) ) ;
	uint8_t config3 = readRegLora_8Bit(REG_MODEM_CONFIG_3);
  // Section 4.1.1.6
  if(symbolDuration > 16) config3 = config3 | 0x08;
	else  config3 = config3 & 0xF7;

 // bitWrite(config3, 3, ldoOn);
  writeRegLora(REG_MODEM_CONFIG_3, config3);
}


//****************************************************
//set Spreading Factor
void LoraSetSpreadingFactor(uint8_t sf){
	if (sf < 6) {
			sf = 6;
		} else if (sf > 12) {
			sf = 12;
		}

		if (sf == 6) {
			writeRegLora(REG_DETECTION_OPTIMIZE, 0xc5);
			writeRegLora(REG_DETECTION_THRESHOLD, 0x0c);
		} else {
			writeRegLora(REG_DETECTION_OPTIMIZE, 0xc3);
			writeRegLora(REG_DETECTION_THRESHOLD, 0x0a);
		}

  writeRegLora(REG_MODEM_CONFIG_2, (readRegLora_8Bit(REG_MODEM_CONFIG_2) & 0x0f) | ((sf << 4) & 0xf0));
  LoraSetLdoFlag();
}

//******************************************************
uint16_t LoraGetSpreadingFactor(void){
	return readRegLora_8Bit(REG_MODEM_CONFIG_2) >> 4;
}

//***************************************************
//get Signal Band Width
uint32_t LoraGetSignalBW(void){
	uint8_t bw = (readRegLora_8Bit(REG_MODEM_CONFIG_1) >> 4);
  switch (bw) {
    case 0: return 7800;
    case 1: return 10400;
    case 2: return 15600;
    case 3: return 20800;
    case 4: return 31250;
    case 5: return 41700;
    case 6: return 62500;
    case 7: return 125000;
    case 8: return 250000;
    case 9: return 500000;
  }
	return 0;
}

//*****************************************************
//set Signal BandWidth
void LoraSetSignalBW(uint32_t bandwidth){
int bw;

  if (bandwidth <= 7.8E3) {
    bw = 0;
  } else if (bandwidth <= 10400) {
    bw = 1;
  } else if (bandwidth <= 15600) {
    bw = 2;
  } else if (bandwidth <= 20800) {
    bw = 3;
  } else if (bandwidth <= 31250) {
    bw = 4;
  } else if (bandwidth <= 41700) {
    bw = 5;
  } else if (bandwidth <= 62500) {
    bw = 6;
  } else if (bandwidth <= 125000) {
    bw = 7;
  } else if (bandwidth <= 250000) {
    bw = 8;
  } else /*if (bandwidth <= 250E3)*/ {
    bw = 9;
  }
  writeRegLora(REG_MODEM_CONFIG_1, (readRegLora_8Bit(REG_MODEM_CONFIG_1) & 0x0f) | (bw << 4));
  LoraSetLdoFlag();
}

//********************************************************
//set Coding Rate
void LoraSetCodingRate(int denominator){
if (denominator < 5) {
    denominator = 5;
  } else if (denominator > 8) {
    denominator = 8;
  }
  int cr = denominator - 4;
	writeRegLora(REG_MODEM_CONFIG_1, (readRegLora_8Bit(REG_MODEM_CONFIG_1) & 0xf1) | (cr << 1));
}

//***********************************************************
//Lora is Transmitting
uint8_t LoraIsTransmitting(void){
	if ((readRegLora_8Bit(REG_OP_MODE) & MODE_TX) == MODE_TX) {
    return 1;
  }

  if (readRegLora_8Bit(REG_IRQ_FLAGS) & IRQ_TX_DONE_MASK) {
    // clear IRQ's
    writeRegLora(REG_IRQ_FLAGS, IRQ_TX_DONE_MASK);
  }

  return 0;
}

//***********************************************************
//Set imlicit Header Mode
void LoraImplicitHeaderMode(void){
	 writeRegLora(REG_MODEM_CONFIG_1, readRegLora_8Bit(REG_MODEM_CONFIG_1) | 0x01);
}

//***********************************************************
//Set explicit Header Mode
void LoraExplicitHeaderMode(void){
	writeRegLora(REG_MODEM_CONFIG_1, readRegLora_8Bit(REG_MODEM_CONFIG_1) & 0xFE);
}

/********************************************************/
// set Lora into Rx mode : Continous or Single
void LoraRxMode(uint8_t RxMode){
		writeRegLora(REG_OP_MODE, MODE_LONG_RANGE_MODE | RxMode);
}
/********************************************************/
// set Lora into Rx mode : Continous or Single
void LoraTxMode(){
		writeRegLora(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_TX);
}

/**************************************************************/
void LoraRead(uint8_t *arrayReadData){
	uint8_t packetLength = 0;
	uint8_t i;
	uint8_t irqFlag = readRegLora_8Bit(REG_IRQ_FLAGS);
	writeRegLora(REG_IRQ_FLAGS, irqFlag);
	if((irqFlag & 0x40) != 0){
		packetLength = readRegLora_8Bit(REG_RX_NB_BYTES);
		writeRegLora(REG_FIFO_ADDR_PTR, readRegLora_8Bit(REG_FIFO_RX_CURRENT_ADDR));		
		for(i = 0; i <packetLength; i++){
		arrayReadData[i]=readRegLora_8Bit(REG_FIFO);
		}
		//powerReadInt = ( arrayReadData[0]-48)*10 + (arrayReadData[1] -48);
		//neu  doc dc
	  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET);
			HAL_Delay(200);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);
			HAL_Delay(200);
	}
}
/**************************************************************/
//Transfer Data have first address = buffer, and length = uint8_t Size
//return Size transfered		
uint8_t LoraTransfer(uint8_t *buffer, uint8_t size){
	 // reset FIFO address and paload length
  writeRegLora(REG_FIFO_ADDR_PTR, 0);
  writeRegLora(REG_PAYLOAD_LENGTH, 0);
	writeToFIFO(buffer,size);
	writeRegLora(REG_PAYLOAD_LENGTH, size);
	// put in TX mode
  LoraTxMode();
	// wait for TX done
  while ((readRegLora_8Bit(REG_IRQ_FLAGS) & IRQ_TX_DONE_MASK) == 0) {}
    // clear IRQ's
  writeRegLora(REG_IRQ_FLAGS, IRQ_TX_DONE_MASK);
		return 1;
}

//convert read data to int

