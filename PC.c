#include "stm32f4xx.h"
#include "bmi160.h"
#include "Init.h"

extern PIDTypeDef PID;
extern PRMTypedef PRM;
extern struct bmi160_dev sensor; 
extern struct bmi160_sensor_data accel;
extern struct bmi160_sensor_data gyro;
extern int8_t rslt; 
char  BuffTxUsart1[SIZE_BUFF_DMA_UART1_TX];
char  BuffRxUsart1[SIZE_BUFF_DMA_UART1_RX];



/*-----------------------Отправка пакета по usart1 на комп-----------------*/
void SendMesPC()
{
uint8_t tempBuf[64];
uint16_t tempSize=0;

tempBuf[0] = 0x10;
tempBuf[1] = PRM.GasValue;//BuffRxSpi1[1]>>8;     //об/мин
tempBuf[2] = 0;//TenzoData>>8;
tempBuf[3] = 0;//TenzoData;
tempBuf[4] = accel.x>>8;    //мгновенные значения акселераторов      
tempBuf[5] = accel.x;     
tempBuf[6] = accel.y>>8;          
tempBuf[7] = accel.y;
tempBuf[8] = accel.z>>8;          
tempBuf[9] = accel.z;
tempBuf[10] = gyro.x>>8;    //мгновенные значения гироскопов    
tempBuf[11] = gyro.x;     
tempBuf[12] = gyro.y>>8;          
tempBuf[13] = gyro.y;
tempBuf[14] = gyro.z>>8;          
tempBuf[15] = gyro.z;
                                            
memcpy(&tempBuf[16],&PID.alfa_gyro,4);  //угол после фильтра
memcpy(&tempBuf[20],&PID.alfa_acc,4);
memcpy(&tempBuf[24],&PID.alfa,4);

tempBuf[28] = 0x10;
tempBuf[29] = 0x03;

//byte stafing
tempSize = 0;
BuffTxUsart1[tempSize++] = tempBuf[0];

//дублируем 0x10
     for(int i = 1; i < 28; i++)//количество полезных данных + 1 
     {
       BuffTxUsart1[tempSize++] = tempBuf[i];
       if ( tempBuf[i] == 0x10 )
         BuffTxUsart1[tempSize++] = 0x10;
     }
  BuffTxUsart1[tempSize++] = 0x10;
  BuffTxUsart1[tempSize++] = 0x03;

  DMA_Cmd(DMA2_Stream7, DISABLE);     
  DMA_ClearFlag(DMA2_Stream7, DMA_FLAG_TCIF7);
  DMA_SetCurrDataCounter(DMA2_Stream7, tempSize);
  DMA2_Stream7->M0AR = (uint32_t)&BuffTxUsart1;
  DMA_Cmd(DMA2_Stream7, ENABLE);
 }

  /*-------------------прием данных по UART1 с компа -------------------*/
/*
* *buff     -> указатель на входной буфер     
* return -1 -> нет приема пакета
* return >0 -> есть пакет
*/
int GetMesPC(uint8_t *buff)
{
  static Circular_buffTypeDef Buff_USART1_RX = 
  {
   BuffRxUsart1,//*p_rd;
   BuffRxUsart1,//*p_wr;
   &DMA2_Stream2->NDTR,//*p_cndtr;
   BuffRxUsart1,//*StartAdr;
   SIZE_BUFF_DMA_UART1_RX//Size;
  };
  static UartStreamTypeDef UART_RX={StopUART,0,0,0};
  static float Kp_tm_float;
  static float Ki_tm_float;
  static float Kd_tm_float;
  static uint32_t *Kp_tm  = ((uint32_t*)&Kp_tm_float);
  static uint32_t *Ki_tm  = ((uint32_t*)&Ki_tm_float);
  static uint32_t *Kd_tm  = ((uint32_t*)&Kd_tm_float);
  uint8_t ch;
  
   uint32_t p_cndtr_tm;
   uint32_t size_recieve_bytes;
// Buff_USART1_RX.p_rd = BuffRxUsart1;
// Buff_USART1_RX.p_wr = BuffRxUsart1;
// Buff_USART1_RX.p_cndtr = &DMA2_Stream2->NDTR;
// Buff_USART1_RX.StartAdr = (char*)DMA2_Stream2->M0AR;
// Buff_USART1_RX.Size = SIZE_BUFF_DMA_UART1_RX;
  
  
  p_cndtr_tm = *Buff_USART1_RX.p_cndtr;
if (( Buff_USART1_RX.Size - p_cndtr_tm) >= (uint32_t)(Buff_USART1_RX.p_rd - Buff_USART1_RX.StartAdr))
     size_recieve_bytes = (Buff_USART1_RX.Size - p_cndtr_tm) - 
                              (uint32_t)(Buff_USART1_RX.p_rd - Buff_USART1_RX.StartAdr);
else
     size_recieve_bytes = (Buff_USART1_RX.Size - (uint32_t)(Buff_USART1_RX.p_rd - Buff_USART1_RX.StartAdr))+
                             (uint32_t)(Buff_USART1_RX.Size - p_cndtr_tm);

  if ( size_recieve_bytes != 0) 
	//Parsing(&size_recieve_bytes);
  {
        //чтение из кольцевого буфера
      while(size_recieve_bytes!=0)
      {
      ch = *Buff_USART1_RX.p_rd;
      size_recieve_bytes -= 1;
      
      if ((uint32_t)(Buff_USART1_RX.p_rd - Buff_USART1_RX.StartAdr+1) == Buff_USART1_RX.Size)
            Buff_USART1_RX.p_rd = Buff_USART1_RX.StartAdr;
      else
            Buff_USART1_RX.p_rd++;

    //разбор пакета
      switch(UART_RX.Mode)
      {
              case StopUART:
                    if (ch == 0x10)
                    {
                            UART_RX.Mode = WorkUART;
                            UART_RX.ByteCounter = 0;
                            UART_RX.Flag_Simb_0x10 = 0;
                            Kp_tm_float=0;
                            Ki_tm_float=0;
                            Kd_tm_float=0;
                    }
                    break;
              case WorkUART:
                    if ((ch == 0x10)&&(UART_RX.Flag_Simb_0x10!=1))
                        {
                          UART_RX.Flag_Simb_0x10 = 1;
                          break;	
                        }
                    else if (ch == 0x03) // конец пакета
                          {
                            UART_RX.Mode = StopUART;
                            return (1);
                            break;						
                          }
                      
                    else  
                          //Прием байтов данных
                          {
                            UART_RX.Flag_Simb_0x10 = 0;
                            *buff = ch;
                             buff++;
//                             if (UART_RX.ByteCounter == 0)
//                               {
//                                  if ( ch > 100)		
//                                          PRM.GasValue = 100;  
//                                  else
//                                          PRM.GasValue = ch;
//                                 
//                                  UART_RX.ByteCounter++;
//                               }
//                             else if ((UART_RX.ByteCounter >= 1)&&(UART_RX.ByteCounter <= 4))
//                               {
//                                    *Kp_tm |= ch << ((3-(UART_RX.ByteCounter-1))*8);
//                                    if (UART_RX.ByteCounter == 4) 
//                                      PID.Kp = Kp_tm_float;
//                                    UART_RX.ByteCounter++;
//    //                                if (isnan(Kp)!=0)//если NaN
//    //                                    Kp = Kp_tm_float;
//                               }
//                             else if ((UART_RX.ByteCounter >= 5)&&(UART_RX.ByteCounter <= 8))
//                               {
//                                    *Ki_tm |= ch << ((3-(UART_RX.ByteCounter-5))*8);
//                                    if (UART_RX.ByteCounter == 8) 
//                                      //PID.Ki = Ki_tm_float;
//                                    UART_RX.ByteCounter++;
//                               }
//                             else if ((UART_RX.ByteCounter >= 9)&&(UART_RX.ByteCounter <= 12))
//                               {
//                                    *Kd_tm |= ch << ((3-(UART_RX.ByteCounter-9))*8);
//                                    if (UART_RX.ByteCounter == 12) 
//                                      //PID.Kd = Kd_tm_float;
//                                    UART_RX.ByteCounter++;
//                               }
                          }
                          break;

                                    
              
      }
      
      }
      
      
      }

}