#include "stm32f4xx.h"
#include "Init.h"
#include "bmi160.h"

 uint8_t BuffTxSpi3[SIZE_BUFF_DMA_SPI1];
 uint8_t BuffRxSpi3[SIZE_BUFF_DMA_SPI1];


int8_t ReadDataSpi3(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
  uint8_t tm;
    
  if (len == 0)
    return BMI160_READ_WRITE_LENGHT_INVALID;
  
  if ((len+1) > SIZE_BUFF_DMA_SPI1)
    return BMI160_READ_WRITE_LENGHT_INVALID;
  
 // memset(BuffRxSpi3,0x00,SIZE_BUFF_DMA_SPI1);

  //ждем когда освобосится SPI
  while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY) == SET);
  //сбрасываем флаг RXEN
  tm = SPI3_DR_8bit;
  //сброс chip select
  GPIO_ResetBits(GPIOD, GPIO_Pin_2); 
  //ждем установки TXE 
  while((SPI3->SR & SPI_I2S_FLAG_TXE) == (uint16_t)RESET );
  //пишем в TX buffer адрес
  SPI3_DR_8bit = reg_addr;
  for (int i=0; i < (len-1);i++)
  {
    //ждем установки TXE 
    while((SPI3->SR & SPI_I2S_FLAG_TXE) == (uint16_t)RESET );
    //пишем в TX buffer
    SPI3_DR_8bit = 0xaa;
    //ждем установки RXNE
    while((SPI3->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET );
    //читаем Rx buffer
    BuffRxSpi3[i] = SPI3_DR_8bit;
    //чтение последнего байта
    if (i == (len-2))
    {
      //ждем установки RXNE
      while((SPI3->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET );
      //читаем Rx buffer
      BuffRxSpi3[i+1] = SPI3_DR_8bit;
    }
  }
  
  //ждем когда освободится SPI
  while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY) == SET);
  GPIO_SetBits(GPIOD, GPIO_Pin_2); //установка CS
  
  memcpy(data,BuffRxSpi3+1,len-1);

  //taskEXIT_CRITICAL(); 
  
  return BMI160_OK;
}


int8_t WriteDataSpi3(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
  uint32_t timeout;
  
 // taskENTER_CRITICAL();
  
  if (len == 0)
    return BMI160_READ_WRITE_LENGHT_INVALID;
  
  if ((len+1) > SIZE_BUFF_DMA_SPI2)
    return BMI160_READ_WRITE_LENGHT_INVALID;
  
  //ждем когда освобосится SPI
  while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY) == SET);
  //сброс chip select
  GPIO_ResetBits(GPIOD, GPIO_Pin_2); 
  //ждем установки TXE 
  while((SPI3->SR & SPI_I2S_FLAG_TXE) == (uint16_t)RESET );
  //пишем в TX buffer адрес
  SPI3_DR_8bit = reg_addr;
  
  for (int i=0; i < len;i++)
  {
    //ждем установки TXE 
     while((SPI3->SR & SPI_I2S_FLAG_TXE) == (uint16_t)RESET );
    //пишем в TX buffer
    SPI3_DR_8bit = *data;
    data++;   
  }
  
  //ждем когда освободится SPI
  while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY) == SET);
  GPIO_SetBits(GPIOD, GPIO_Pin_2); //установка CS

// taskEXIT_CRITICAL(); 
  
  return BMI160_OK;
  
}
