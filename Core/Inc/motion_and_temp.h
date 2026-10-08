/*
 * motion_and_temp.h
 *
 *  Created on: Oct 3, 2026
 *      Author: asus
 */

#ifndef INC_MOTION_AND_TEMP_H_
#define INC_MOTION_AND_TEMP_H_

//extern float processed_MPU_data[7];

extern volatile float Ax, Ay, Az, Gx, Gy, Gz, Tempr;

void process_mpu6500_motion_and_temp(void);
void uart_send_data(void);


#endif /* INC_MOTION_AND_TEMP_H_ */
