#include "stm32f4xx.h"
#include "bmi160.h"
#include "Init.h"
#include "Sbus.h"
#include "math.h"

void Parsing(uint32_t *size);

PRMTypedef PRM;
PIDTypeDef PID;

struct bmi160_dev sensor; 
struct bmi160_sensor_data accel;
struct bmi160_sensor_data gyro;

int8_t rslt;

void main()
{
 Init();

  PID.alfa = PID.alfa_acc = PID.alfa_g = PID.alfa_gyro = PID.alfa_gyro_old = PID.alfa_old =0;
  PID.Kp = 11.5;
  PID.Ki = 0.1;
  PID.Kd = 280;
  PID.P = 0;
  PID.I = 0;
  PID.D = 0;
  PID.e_old = 0;



 
  sensor.id = 0xd1;
 sensor.interface = BMI160_SPI_INTF;
 sensor.read = ReadDataSpi3;
 sensor.write = WriteDataSpi3;
 sensor.delay_ms = TimerBmi160;
 rslt = BMI160_OK;
 rslt = bmi160_init(&sensor);
 
 TimerBmi160(500);
 /* Select the Output data rate, range of accelerometer sensor */
  sensor.accel_cfg.odr = BMI160_ACCEL_ODR_25HZ;//BMI160_ACCEL_ODR_1600HZ;//
  sensor.accel_cfg.range = BMI160_ACCEL_RANGE_2G;
  sensor.accel_cfg.bw = BMI160_ACCEL_BW_OSR4_AVG1;//BMI160_ACCEL_BW_NORMAL_AVG4;//

  /* Select the power mode of accelerometer sensor */
  sensor.accel_cfg.power = BMI160_ACCEL_NORMAL_MODE;

  /* Select the Output data rate, range of Gyroscope sensor */
  sensor.gyro_cfg.odr = BMI160_GYRO_ODR_3200HZ;
  sensor.gyro_cfg.range = BMI160_GYRO_RANGE_2000_DPS;
  sensor.gyro_cfg.bw = BMI160_GYRO_BW_NORMAL_MODE;

  /* Select the power mode of Gyroscope sensor */
  sensor.gyro_cfg.power = BMI160_GYRO_NORMAL_MODE; 

  /*  Set the Power mode  */
  rslt = bmi160_set_power_mode(&sensor);
  
  /* Set the sensor configuration */
  rslt = bmi160_set_sens_conf(&sensor);
  
  rslt = bmi160_get_sensor_data(BMI160_ACCEL_SEL, &accel, NULL, &sensor);
  PID.alfa_acc = PI/2 - acos((float)accel.x/-17039);
  PID.alfa_gyro_old = PID.alfa_acc*180/PI;
  PID.alfa_old = PID.alfa_gyro_old;
 
 TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
for(;;)
{
if (GetMesPC())
{
  TIM_ITConfig(TIM6, TIM_IT_Update, DISABLE);
/                             if (UART_RX.ByteCounter == 0)
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
//    //                                if (isnan(Kp)!=0)//���� NaN
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
  
  
  TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
}




}

 
}

 /*--------------------------������ BMI160 driver-----------------------------*/
/*
* delay - �������� � ��
* ������������ �������� 0xffff
*/
void TimerBmi160(uint32_t delay)
 {
   TIM7->ARR = delay;
   TIM7->CNT = 0;
   TIM7->SR = (uint16_t)~TIM_FLAG_Update;
   while ((TIM7->SR & TIM_FLAG_Update) != TIM_FLAG_Update);
 }

 /*---------------------------�������� ������ � BLDC �� SBUS--------------------*/
 void TransmitPRM_BLDC(uint16_t g)
 {
   S_BUSTypeDef data;
   char buff[25];
   GPIO_SetBits(GPIOB, GPIO_Pin_0);
   memset(buff,0,25);
   data.analog_ch[0] = g;//������� ����� ������������, ��������  �� 0 �� 2047
   for (int i=1;i<16;i++)
     data.analog_ch[i] = 0;
   
   CreateSbusFrame( &data, buff);
   
   if (DMA_GetCmdStatus(DMA2_Stream6) == ENABLE)//if  ((DMA2_Stream6->CR & DMA_SxCR_EN) == DMA_SxCR_EN)//(DMA_GetFlagStatus(DMA1_FLAG_TC7) == SET)
   {       
     if (DMA_GetFlagStatus(DMA2_Stream6, DMA_FLAG_TCIF6) == SET)//if ((DMA2->HISR & DMA_FLAG_TCIF6) == DMA_FLAG_TCIF6)
     {
       /* Disable the selected DMAy Streamx by clearing EN bit */
       DMA_Cmd(DMA2_Stream6, DISABLE);//DMA2_Stream6->CR &= ~(uint32_t)DMA_SxCR_EN;//DMA_Cmd(DMA1_Channel7, DISABLE);
       /*Clears the DMAy Streamx's pending flags.*/
       DMA_ClearFlag(DMA2_Stream6, DMA_FLAG_TCIF6);//DMA2->HIFCR = (uint32_t)(DMA_FLAG_TCIF6);//DMA1->IFCR = DMA1_FLAG_TC7;
       DMA_SetCurrDataCounter(DMA2_Stream6, 25);//DMA2_Stream6->NDTR = 25;//DMA1_Channel7->CNDTR = 25;
       DMA2_Stream6->M0AR = (uint32_t)&buff;//DMA1_Channel7->CMAR = (uint32_t)&buff;
       DMA_Cmd(DMA2_Stream6, ENABLE);//DMA2_Stream6->CR |= (uint32_t)DMA_SxCR_EN;//DMA_Cmd(DMA1_Channel7, ENABLE);
     }
   }
   else
   {
    /* Disable the selected DMAy Streamx by clearing EN bit */
       DMA_Cmd(DMA2_Stream6, DISABLE);//DMA2_Stream6->CR &= ~(uint32_t)DMA_SxCR_EN;//DMA_Cmd(DMA1_Channel7, DISABLE);
       /*Clears the DMAy Streamx's pending flags.*/
       DMA_ClearFlag(DMA2_Stream6, DMA_FLAG_TCIF6);//DMA2->HIFCR = (uint32_t)(DMA_FLAG_TCIF6);//DMA1->IFCR = DMA1_FLAG_TC7;
       DMA_SetCurrDataCounter(DMA2_Stream6, 25);//DMA2_Stream6->NDTR = 25;//DMA1_Channel7->CNDTR = 25;
       DMA2_Stream6->M0AR = (uint32_t)&buff;//DMA1_Channel7->CMAR = (uint32_t)&buff;
       DMA_Cmd(DMA2_Stream6, ENABLE);//DMA2_Stream6->CR |= (uint32_t)DMA_SxCR_EN;//DMA_Cmd(DMA1_Channel7, ENABLE);
   }
    GPIO_ResetBits(GPIOB, GPIO_Pin_0);
   
 }
 

