#include "stm32f4xx.h"
#include "bmi160.h"
#include "Init.h"
#include "Sbus.h"
#include "mpu6000.h"
#include "math.h"
#include "SpiMPU6000Driver.h"
#include "stdio.h"

extern int GetMesPC(uint8_t *buff);


PRMTypedef PRM;
PIDTypeDef PID;

struct bmi160_dev sensor; 
struct bmi160_sensor_data accel;
struct bmi160_sensor_data gyro;

TypeDefACCEL_MPU6000    accel_MPU6000;
TypeDefGYRO_MPU6000     gyro_MPU6000;

int8_t rslt;
uint8_t tempp[10];
void main()
{
 Init();
GPIO_SetBits(GPIOA, GPIO_Pin_4);//CS MPU6000
  PID.alfa = PID.alfa_acc = PID.alfa_g = PID.alfa_gyro = PID.alfa_gyro_old = PID.alfa_old =0;
  PID.Kp = 11.5;
  PID.Ki = 0.1;
  PID.Kd = 280;
  PID.P = 0;
  PID.I = 0;
  PID.D = 0;
  PID.e_old = 0;
  
 ReadDataSPI1(MPUREG_WHOAMI, tempp, 1); 
 //отключаем sleep режим
 WriteDataSPI1(MPUREG_PWR_MGMT_1, 0);
  TimerBmi160(100);
 //отключаем шину i2c
 WriteDataSPI1(MPUREG_USER_CTRL, BIT_I2C_IF_DIS);  
   TimerBmi160(100);
 //настраиваем Sample Rate 8 kHz/(1+smplrt) smplrt = 79
 WriteDataSPI1(MPUREG_SMPLRT_DIV, 79);
   TimerBmi160(100);
 //настраиваем config
 WriteDataSPI1(MPUREG_CONFIG, 0);
    TimerBmi160(100);
 //настраиваем gyro
 ReadDataSPI1(MPUREG_GYRO_CONFIG, tempp, 1);
 WriteDataSPI1(MPUREG_GYRO_CONFIG, 0x18);//FS = 3 -> 2000 deg/s
   TimerBmi160(100);
 ReadDataSPI1(MPUREG_GYRO_CONFIG, tempp, 1);
  
 //настраиваем accel
 ReadDataSPI1(MPUREG_ACCEL_CONFIG, tempp, 1);
 WriteDataSPI1(MPUREG_ACCEL_CONFIG, 0);//AFS = 0 -> 2 g
   TimerBmi160(100);
 ReadDataSPI1(MPUREG_ACCEL_CONFIG, tempp, 1);
  
 //повышение частоты CLK SPI1 
 SPI_SetBaudRatePrescaler(SPI1, SPI_BaudRatePrescaler_8);

//  while(1)
//  {
//    mpu6000_get_sensor_data(&accel_MPU6000, &gyro_MPU6000 );
//    TimerBmi160(500);
//  }
//    
  
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
  
  //rslt = bmi160_get_sensor_data(BMI160_ACCEL_SEL, &accel, NULL, &sensor);
  mpu6000_get_sensor_data(&accel_MPU6000, &gyro_MPU6000 );
  //PID.alfa_acc = PI/2 - acos((float)accel.x/-17039);
  PID.alfa_acc = PI/2 - acos((float)accel_MPU6000.x/-17039);
  PID.alfa_gyro_old = PID.alfa_acc*180/PI;
  PID.alfa_old = PID.alfa_gyro_old;
 
 TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
 uint8_t buff[64];
 
 
  for(;;)
    {
      //разбираем принятые данные с ПК
      if (0)//(GetMesPC(buff)>0)
        {
           float Kp_tm_float;
           float Ki_tm_float;
           float Kd_tm_float;
           uint32_t *Kp_tm  = ((uint32_t*)&Kp_tm_float);
           uint32_t *Ki_tm  = ((uint32_t*)&Ki_tm_float);
           uint32_t *Kd_tm  = ((uint32_t*)&Kd_tm_float);
          
          TIM_ITConfig(TIM6, TIM_IT_Update, DISABLE);
          if (buff[0] > 100)
            PRM.GasValue = 100;
          else
            PRM.GasValue = buff[0] ;
          
          *Kp_tm = (buff[1]<<24)|(buff[2]<<16)|(buff[3]<<8)|buff[4];
           PID.Kp = Kp_tm_float;
          *Ki_tm = (buff[5]<<24)|(buff[6]<<16)|(buff[7]<<8)|buff[8];
           PID.Ki = Ki_tm_float;
          *Kd_tm = (buff[9]<<24)|(buff[10]<<16)|(buff[11]<<8)|buff[12];  
          
          TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
        }

    }

 
}

 /*--------------------------Таймер BMI160 driver-----------------------------*/
/*
* delay - задержка в мс
* максимальное значение 0xffff
*/
void TimerBmi160(uint32_t delay)
 {
   TIM7->ARR = delay;
   TIM7->CNT = 0;
   TIM7->SR = (uint16_t)~TIM_FLAG_Update;
   while ((TIM7->SR & TIM_FLAG_Update) != TIM_FLAG_Update);
 }

 /*---------------------------Передача данных в BLDC по SBUS--------------------*/
 void TransmitPRM_BLDC(uint16_t g)
 {
   S_BUSTypeDef data;
   char buff[25];
   GPIO_SetBits(GPIOB, GPIO_Pin_0);
   memset(buff,0,25);
   data.analog_ch[0] = g;//уровень ручки акселератора, значения  от 0 до 2047
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
 

