#include "stm32f4xx.h"
#include "Init.h"
extern char  BuffTxUsart1[SIZE_BUFF_DMA_UART1_TX];
extern char  BuffRxUsart1[SIZE_BUFF_DMA_UART1_RX];
/*
UASRT1 <-> на комп
SPI3 <-> BMI160
UASRT6 -> SBUS
*/
void Init()
{
  //clock PCLK1 - 42 MHz max
  //clock PCLK2 - 85 MHz max
  
  //RCC_PCLK2Config(RCC_HCLK_Div2);
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,  ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);//42 MHz
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);//42 MHz
  
  
  /*-------------------------Настройка пинов---------------------------------*/
  GPIO_DeInit(GPIOA);
  GPIO_DeInit(GPIOB);
  GPIO_DeInit(GPIOC);
  GPIO_DeInit(GPIOD);
  GPIO_InitTypeDef GPIO_InitStructre;
  
  /*Порты для USART1 на PC*/ 
  GPIO_InitStructre.GPIO_Pin = GPIO_Pin_10;//RX
  GPIO_InitStructre.GPIO_Mode = GPIO_Mode_IN; 
  GPIO_InitStructre.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructre);
  //GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
  
  GPIO_InitStructre.GPIO_Pin = GPIO_Pin_9;//TX
  GPIO_InitStructre.GPIO_Mode = GPIO_Mode_AF; 
  GPIO_InitStructre.GPIO_Speed = GPIO_Fast_Speed;
  GPIO_InitStructre.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructre.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructre);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
  
  /*Пины USART6 для S-BUS*/
  GPIO_InitStructre.GPIO_Pin = GPIO_Pin_6;//TX
  GPIO_InitStructre.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructre. GPIO_Speed = GPIO_Fast_Speed;
  GPIO_InitStructre.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructre.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructre);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
  
  /*порты для SPI1 на MPU6000*/
  GPIO_InitStructre.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;//sck,mosi
  GPIO_InitStructre.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructre.GPIO_Speed = GPIO_High_Speed;
  GPIO_InitStructre.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructre.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructre);
  
  GPIO_InitStructre.GPIO_Pin = GPIO_Pin_6;//miso
  GPIO_InitStructre.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructre.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructre);
  
  GPIO_InitStructre.GPIO_Pin = GPIO_Pin_4;//CS
  GPIO_InitStructre.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructre.GPIO_Speed = GPIO_High_Speed;
  GPIO_InitStructre.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructre.GPIO_PuPd =  GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructre);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
  
  /*порты для SPI3 на BMP160*/
  GPIO_InitStructre.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_12;//sck,mosi
  GPIO_InitStructre.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructre.GPIO_Speed = GPIO_High_Speed;
  GPIO_InitStructre.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructre.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructre);
  
  GPIO_InitStructre.GPIO_Pin = GPIO_Pin_11;//miso
  GPIO_InitStructre.GPIO_Mode = GPIO_Mode_AF;//GPIO_Mode_IN;
  GPIO_InitStructre.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructre);
  
  GPIO_InitStructre.GPIO_Pin = GPIO_Pin_2;//CS
  GPIO_InitStructre.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructre.GPIO_Speed = GPIO_High_Speed;
  GPIO_InitStructre.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructre.GPIO_PuPd =  GPIO_PuPd_UP;
  GPIO_Init(GPIOD, &GPIO_InitStructre);
  
  /*тестовый */
  GPIO_InitStructre.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructre.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructre.GPIO_Speed = GPIO_High_Speed;
  GPIO_InitStructre.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructre.GPIO_PuPd =  GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructre);
  
  /*----------------------------Настройка USART1 для PC-----------------------*/
  USART_DeInit(USART1);
  USART_InitTypeDef USART_InitStruct;
  USART_InitStruct.USART_BaudRate = 115200;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStruct.USART_HardwareFlowControl = DISABLE;
  
  USART_Init(USART1, &USART_InitStruct);
  USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
  USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
  
  /*----------------------------Настройка USART6 для S-BUS-----------------------*/
  USART_DeInit(USART6);
  USART_InitStruct.USART_BaudRate = 100000;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits = USART_StopBits_2;
  USART_InitStruct.USART_Parity = USART_Parity_Even;
  USART_InitStruct.USART_Mode = USART_Mode_Tx;
  USART_InitStruct.USART_HardwareFlowControl = DISABLE;
  USART_Init(USART6, &USART_InitStruct);
  USART_DMACmd(USART6, USART_DMAReq_Tx, ENABLE);
  
  /*--------------------------Настройка SPI1 для MPU6000---------------------*/
  SPI_I2S_DeInit(SPI1);
  SPI_InitTypeDef  SPI_InitStructure;
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);
  SPI_SSOutputCmd(SPI1, DISABLE);
  SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);
  SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
  SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
  
  /*--------------------------Настройка SPI3 для BMI160-----------------------*/
  SPI_I2S_DeInit(SPI3);
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI3, &SPI_InitStructure);
  SPI_SSOutputCmd(SPI3, DISABLE);
  SPI_NSSInternalSoftwareConfig(SPI3, SPI_NSSInternalSoft_Set);
  SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, DISABLE);
  SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Rx, DISABLE);
  
  /*--------------------Настройка TIMER6 для опроса BMI160-------------------*/
  TIM_DeInit(TIM6);
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;//clock 42 MHz
  TIM_TimeBaseInitStruct.TIM_Prescaler = 84;
  TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
  TIM_TimeBaseInitStruct.TIM_Period = 10000;// 50 Hz 
  TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;  
  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);
  TIM_ITConfig(TIM6, TIM_IT_Update, DISABLE);
  
  /*----------------Настройка TIMER7 для задержки времени в мс --------------*/
  TIM_DeInit(TIM7);//clock 42 MHz
  TIM_TimeBaseInitStruct.TIM_Prescaler = 42000;
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStruct.TIM_Period = 0xffff;
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;  
  TIM_TimeBaseInit(TIM7, &TIM_TimeBaseInitStruct);
  TIM_ARRPreloadConfig(TIM7, DISABLE);
  
  /*----------------------------Настройка DMA2 для USART1_RX------------------*/
   DMA_InitTypeDef DMA_InitStruct;
   DMA_StructInit(&DMA_InitStruct);
   DMA_InitStruct.DMA_Channel = DMA_Channel_4;
   DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
   DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)BuffRxUsart1;
   DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory;
   DMA_InitStruct.DMA_BufferSize = SIZE_BUFF_DMA_UART1_RX;
   DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
   DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
   DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
   DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
   DMA_InitStruct.DMA_Priority = DMA_Priority_High;
   DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;
   DMA_Init(DMA2_Stream2,&DMA_InitStruct);
  
   /*----------------------------Настройка DMA2 для USART1_TX------------------*/ 
   DMA_InitStruct.DMA_Channel = DMA_Channel_4;
   DMA_StructInit(&DMA_InitStruct);
   DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
   DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToPeripheral;
   DMA_InitStruct.DMA_BufferSize = SIZE_BUFF_DMA_UART1_TX;
   DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
   DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
   DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
   DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
   DMA_InitStruct.DMA_Priority = DMA_Priority_High;
   DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;
   DMA_Init(DMA2_Stream7,&DMA_InitStruct);
   
   /*----------------------------Настройка DMA2 для USART6_TX------------------*/ 
   DMA_StructInit(&DMA_InitStruct);
   DMA_InitStruct.DMA_Channel = DMA_Channel_5;
   DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&USART6->DR;
   DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToPeripheral;
   DMA_InitStruct.DMA_BufferSize = 25;
   DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
   DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
   DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
   DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
   DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
   DMA_InitStruct.DMA_Priority = DMA_Priority_High;
   DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;
   DMA_Init(DMA2_Stream6,&DMA_InitStruct);
   
   /*----------------------------Настройка DMA2 для SPI1_TX------------------*/ 
   DMA_DeInit(DMA2_Stream3);
   DMA_StructInit(&DMA_InitStruct);
   DMA_InitStruct.DMA_Channel = DMA_Channel_3;
   DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DR;
   DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToPeripheral;
   DMA_InitStruct.DMA_BufferSize = 0;
   DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
   DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
   DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
   DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
   DMA_InitStruct.DMA_Priority = DMA_Priority_High;
   DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;
   DMA_Init(DMA2_Stream3,&DMA_InitStruct);
   
   /*----------------------------Настройка DMA2 для SPI1_RX-------------------*/
   DMA_DeInit(DMA2_Stream0);
   DMA_StructInit(&DMA_InitStruct);
   DMA_InitStruct.DMA_Channel = DMA_Channel_3;
   DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DR;
   DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory;
   DMA_InitStruct.DMA_BufferSize = 0;
   DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
   DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
   DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
   DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
   DMA_InitStruct.DMA_Priority = DMA_Priority_High;
   DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;
   DMA_Init(DMA2_Stream0,&DMA_InitStruct);
   
   /*----------------------------Настройка NVIC------------------*/
   NVIC_InitTypeDef NVIC_InitStruct;
   NVIC_InitStruct.NVIC_IRQChannel = TIM6_DAC_IRQn;
   NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
   NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
   NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
   NVIC_Init(&NVIC_InitStruct);
     
   //DMA_Cmd(DMA2_Stream2, ENABLE);
   //DMA_Cmd(DMA2_Stream7, ENABLE);
   //DMA_Cmd(DMA2_Stream6, ENABLE);
   //DMA_Cmd(DMA2_Stream0, ENABLE);
   //DMA_Cmd(DMA2_Stream3, ENABLE);
   SPI_Cmd(SPI3, ENABLE);
   SPI_Cmd(SPI1, ENABLE);
   USART_Cmd(USART1, ENABLE);
   USART_Cmd(USART6, ENABLE);
   TIM_Cmd(TIM6, ENABLE);
   TIM_Cmd(TIM7, ENABLE);
}

void SPI_SetBaudRatePrescaler(SPI_TypeDef* SPIx, uint16_t BaudRatePrescaler)
{
  /* check the parameters */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));
  
  while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_BSY) == SET);
  
  SPI_Cmd(SPIx, DISABLE);
  uint16_t CR1_tm = SPIx->CR1;
  SPIx->CR1 = (CR1_tm & (~SPI_CR1_BR));
  SPIx->CR1 |= BaudRatePrescaler;
  SPI_Cmd(SPIx, ENABLE);
}