#include "stm32f4xx.h"
#include "Init.h"
#include "math.h"
extern uint8_t  BuffRxUsart1[SIZE_BUFF_DMA_UART1_RX];
extern Circular_buffTypeDef Buff_USART1_RX;
extern PRMTypedef PRM;
extern PIDTypeDef PID;
extern float Kp,Ki,Kd;

 void Parsing(uint32_t *size)
{
  static UartStreamTypeDef UART_RX={StopUART,0,0,0};
  static float Kp_tm_float;
  static float Ki_tm_float;
  static float Kd_tm_float;
  static uint32_t *Kp_tm  = ((uint32_t*)&Kp_tm_float);
  static uint32_t *Ki_tm  = ((uint32_t*)&Ki_tm_float);
  static uint32_t *Kd_tm  = ((uint32_t*)&Kd_tm_float);
  uint8_t ch;
  
  //чтение из кольцевого буфера
  while(*size!=0)
  {
  ch = *Buff_USART1_RX.p_rd;
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
                        break;						
                      }
                  
                else  
                      //Прием байтов данных
                      {
                        UART_RX.Flag_Simb_0x10 = 0;
                         if (UART_RX.ByteCounter == 0)
                           {
                              if ( ch > 100)		
                                      PRM.GasValue = 100;  
                              else
                                      PRM.GasValue = ch;
                              UART_RX.ByteCounter++;
                           }
                         else if ((UART_RX.ByteCounter >= 1)&&(UART_RX.ByteCounter <= 4))
                           {
                                *Kp_tm |= ch << ((3-(UART_RX.ByteCounter-1))*8);
                                if (UART_RX.ByteCounter == 4) 
                                  PID.Kp = Kp_tm_float;
                                UART_RX.ByteCounter++;
//                                if (isnan(Kp)!=0)//если NaN
//                                    Kp = Kp_tm_float;
                           }
                         else if ((UART_RX.ByteCounter >= 5)&&(UART_RX.ByteCounter <= 8))
                           {
                                *Ki_tm |= ch << ((3-(UART_RX.ByteCounter-5))*8);
                                if (UART_RX.ByteCounter == 8) 
                                  PID.Ki = Ki_tm_float;
                                UART_RX.ByteCounter++;
                           }
                         else if ((UART_RX.ByteCounter >= 9)&&(UART_RX.ByteCounter <= 12))
                           {
                                *Kd_tm |= ch << ((3-(UART_RX.ByteCounter-9))*8);
                                if (UART_RX.ByteCounter == 12) 
                                  PID.Kd = Kd_tm_float;
                                UART_RX.ByteCounter++;
                           }
                      }
                      break;

				
	  
  }
  *size -= 1;
  }
	
	
}