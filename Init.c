#include "stm32f4xx.h"
#include "Init.h"

/*
UASRT1 <-> на комп
SPI3 <-> BMI160
UASRT3 -> SBUS
*/
void Init()
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  
  
  /*-------------------------Настройка пинов---------------------------------*/
  GPIO_DeInit(GPIOA);
  GPIO_DeInit(GPIOB);
  /*USART1*/
  GPIO_InitTypeDef GPIO_InitStructre;
  GPIO_InitStructre.GPIO_Pin = GPIO_Pin_10;//RX
  GPIO_InitStructre.GPIO_Mode = GPIO_Mode_IN; 
  GPIO_InitStructre.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructre);
  
  GPIO_InitStructre.GPIO_Pin = GPIO_Pin_9;//TX
  GPIO_InitStructre.GPIO_Mode = GPIO_Mode_AF; 
  GPIO_InitStructre.GPIO_Speed = GPIO_Fast_Speed;
  GPIO_InitStructre.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructre.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructre);
  
  /*----------------------------Настройка USART1------------------------------*/
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
  
  USART_Cmd(USART1, ENABLE);
  
  /*----------------------------Настройка DMA2 для USART1_RX------------------*/
   DMA_InitTypeDef DMA_InitStruct;
   DMA_InitStruct.DMA_Channel = DMA_Channel_4;
   DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
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
   
   DMA_Cmd(DMA2_Stream2, ENABLE);
   DMA_Cmd(DMA2_Stream7, ENABLE);
}