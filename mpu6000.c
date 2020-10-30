#include "stm32f4xx.h"
#include "Init.h"
#include "mpu6000.h"
#include "SpiMPU6000Driver.h"


//void mpu6000_config_sensor_data(TypeDefACCEL *accel, TypeDefGYRO *gyro );

void mpu6000_get_sensor_data(TypeDefACCEL_MPU6000 *accel, TypeDefGYRO_MPU6000 *gyro )
{
  uint8_t temp[10];
  //чтение данных акселерометров
  ReadDataSPI1(MPUREG_ACCEL_XOUT_H, temp, 6);
  accel->x = temp[0]<<8;
  accel->x |= temp[1];
 
  accel->y = temp[2]<<8;
  accel->y |= temp[3];
       
  accel->z = temp[4]<<8;
  accel->z |= temp[5];
  
  //чтение данных гироскопов
  ReadDataSPI1(MPUREG_GYRO_XOUT_H, temp, 6);
  gyro->x = temp[0]<<8;
  gyro->x |= temp[1];

  gyro->y = temp[2]<<8;
  gyro->y |= temp[3];
  
  gyro->z = temp[4]<<8;
  gyro->z |= temp[5];
}



