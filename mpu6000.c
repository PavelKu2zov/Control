#include "stm32f4xx.h"
#include "Init.h"
#include "mpu6000.h"
#include "SpiMPU6000Driver.h"


void mpu6000_config_sensor_data(TypeDefACCEL *accel, TypeDefGYRO *gyro );

//void mpu6000_get_sensor_data(TypeDefACCEL *accel, TypeDefGYRO *gyro )
//{
//  //чтение данных акселерометров
//  uint8_t tm[6];
//  ReadDataSPI1(MPUREG_ACCEL_XOUT_H, tm, 6);
//  
//}
//


