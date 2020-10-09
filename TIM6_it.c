#include "stm32f4xx.h"
#include "Init.h"
#include "bmi160.h"
extern PIDTypeDef PID;
extern PRMTypedef PRM;
extern struct bmi160_dev sensor; 
extern struct bmi160_sensor_data accel;
extern struct bmi160_sensor_data gyro;
extern int8_t rslt;
extern void  TransmitPRM_BLDC(uint16_t g);


void TIM6_DAC_IRQHandler(void)
{
  GPIO_SetBits(GPIOB, GPIO_Pin_1);
/*------------  ���������� ������ ������������� � ��������� ---------------*/
rslt = bmi160_get_sensor_data(BMI160_GYRO_SEL, NULL, &gyro, &sensor);
rslt = bmi160_get_sensor_data(BMI160_ACCEL_SEL, &accel, NULL, &sensor);

/*���������� ���� �� ������������� � ��������*/
PID.alfa_acc = PI/2 - acos((float)accel.x/-17039);
if (isnan(PID.alfa_acc)!=0)//���� NaN
  PID.alfa_acc = -2*PI; 

/*���������� ���� �� ��������� � ��������*/
PID.alfa_gyro = (PID.alfa_gyro_old + ((float)(gyro.y)/16.9) * delta_t); 
PID.alfa_gyro_old = PID.alfa_gyro;

/* ��������������� ������*/
PID.alfa = (1-K)*(PID.alfa_old + ((float)(gyro.y)/16.9) * delta_t) + K*PID.alfa_acc*180/PI;
PID.alfa_old = PID.alfa;    
 //���������� ��������� ���������
 // if ((PRM.GasValue >= 0) && (PRM.GasValue < 20))
     if (PRM.GasValue >= 0) 
   // if (PRM.GasValue >= 0)
	//TIM2->CCR2 = 4500 + (15*PRM.GasValue);//(1500*PRM.GasValue/100)	
        TransmitPRM_BLDC(PRM.GasValue*2047/100);
    //  else
//	TIM2->CCR2 = 4500 - (15*PRM.GasValue);//(1500*PRM.GasValue/100)	
  
 
 /*���������� ������*/
 PID.e = fi - PID.alfa;
 /*��� ���������*/
 if (0)//(PRM.GasValue >= 20)//� %
 {
  uint16_t value; 
  float tm;
  PID.P = PID.Kp*PID.e;
  PID.I = PID.I + PID.Ki*PID.e; 
  PID.D = PID.Kd*(PID.e - PID.e_old);
  PID.u = PID.P + PID.I + PID.D;
  PID.e_old = PID.e;
  
 
//  if (u >= 1500) 
//    value = 1500;
//  else if (u < 0)    
//    value = 300;
//  else
//    value = (uint16_t)(300+u);
  tm = 300+PID.u;
  
   if (tm >= 1500)
      value = 1500;
   else if (tm <= 300)
     value = 300;
   else
     value = (uint16_t)tm;
  
    TransmitPRM_BLDC(value);
 }
 else
 {
 PID.P = 0;
 PID.I = 0;
 PID.D = 0;
 }
  
 
SendMesPC();

  
 /* Clear the IT pending Bit */
TIM_ClearFlag(TIM6, TIM_FLAG_Update);

 GPIO_ResetBits(GPIOB, GPIO_Pin_1);
}