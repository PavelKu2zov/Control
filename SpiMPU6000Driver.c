#include "stm32f4xx.h"
#include "Init.h"
#include "SpiMPU6000Driver.h"


/*
*Читаем данные по SPI1
*reg_addr - адрес регистра MPU6000
*data - буфер, куда складывать данные
*len - длина читаемых данных
*/
int8_t ReadDataSPI1(uint8_t reg_addr, uint8_t *data, uint16_t len)
{
  if (len > 64)
    return (-1);
  
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
  
  DMA_Cmd(DMA2_Stream0, DISABLE);//RX
  DMA_Cmd(DMA2_Stream3, DISABLE);//TX
  //сбрасывам флаг RXE считыванием DR
  SPI1->DR;
  
  uint8_t temp[10];//[len+1];
  uint8_t temp2[10];//[len+1];
  temp[0] = reg_addr;
  temp[0] |=  0x80;
  GPIO_ResetBits(GPIOA, GPIO_Pin_4);
  
  for(int i=0;i<3;i++);
  

  DMA_ClearFlag(DMA2_Stream0, DMA_FLAG_TCIF0);
  DMA_ClearFlag(DMA2_Stream3, DMA_FLAG_TCIF3);     
  DMA_SetCurrDataCounter(DMA2_Stream0, len+1);
  DMA_SetCurrDataCounter(DMA2_Stream3, len+1);
  DMA2_Stream0->M0AR = (uint32_t)temp2;//RX
  DMA2_Stream3->M0AR = (uint32_t)temp;//TX
  

  DMA_Cmd(DMA2_Stream0, ENABLE);//RX
  DMA_Cmd(DMA2_Stream3, ENABLE);//TX
  
  //while (DMA_GetFlagStatus(DMA2_Stream0, DMA_FLAG_TCIF0) != SET);
while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);

  for(int i=0;i<15;i++);
  GPIO_SetBits(GPIOA, GPIO_Pin_4);
  
  memcpy(data,&temp2[1],len);
  
  return (len);
}


/*
*запись данных (один байт)в MPU6000 по адресу  reg_addr
*reg_addr - адрес регистра куда пишем
data - данные которые пишем? 
*/
int8_t WriteDataSPI1(uint8_t reg_addr, uint8_t data)
{

  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
  
  DMA_Cmd(DMA2_Stream3, DISABLE);//TX
      
  uint8_t temp[2];
  temp[0] = reg_addr;
  temp[0] &= 0x7f;  
  temp[1] = data;
 
  GPIO_ResetBits(GPIOA, GPIO_Pin_4);
  
  for(int i=0;i<3;i++);
  

  DMA_ClearFlag(DMA2_Stream3, DMA_FLAG_TCIF3);     
  DMA_SetCurrDataCounter(DMA2_Stream3, 2);
  DMA2_Stream3->M0AR = (uint32_t)temp;//TX
  DMA_Cmd(DMA2_Stream3, ENABLE);//TX
  
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
  
  for(int i=0;i<15;i++);
  GPIO_SetBits(GPIOA, GPIO_Pin_4);
  
  return (1);
  
}