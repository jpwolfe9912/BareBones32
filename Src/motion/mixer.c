/** @file 		mixer.c
 *  @brief
 *  	This file takes the output of the PID controller and assigns values to the motors.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 MAR 2022
 */

/* Includes */
#include "board.h"

/* Global Variables */
uint8_t numberMotor = 4;
uint16_t throttleCmd;
int16_t steerCmd, speedCmd;
int16_t motor_temp[4];

/** @brief Pulses the motors.
 *
 *  @return Void.
 */
void
pulseMotors(void)
{
	motor_value[0] = DSHOT_CMD_BEACON1;
	motor_value[1] = DSHOT_CMD_BEACON1;
	motor_value[2] = DSHOT_CMD_BEACON1;
	motor_value[3] = DSHOT_CMD_BEACON1;
	delay(10);
}

/** @brief Mixes the values from the PID controller and assigns values to the motors.
 *
 *  @return Void.
 */
void
mixTable(void)
{
	uint8_t i;

	if(armed == true)
	{
		motor_temp[0] = PIDMIXFLIGHT( -1.0f,  1.0f, -1.0f, 1.0f );      // Rear Right  CW
		motor_temp[1] = PIDMIXFLIGHT( -1.0f, -1.0f,  1.0f, 1.0f );      // Front Right CCW
		motor_temp[2] = PIDMIXFLIGHT(  1.0f,  1.0f,  1.0f, 1.0f );      // Rear Left   CCW
		motor_temp[3] = PIDMIXFLIGHT(  1.0f, -1.0f, -1.0f, 1.0f );      // Front Left  CW

		float maxDeltaThrottle;
		float minDeltaThrottle;
		float deltaThrottle;

		maxDeltaThrottle = MAXCOMMAND - rxCommands[THROTTLE];
		minDeltaThrottle = rxCommands[THROTTLE] - eepromConfig.minThrottle;
		deltaThrottle    = (minDeltaThrottle < maxDeltaThrottle) ? minDeltaThrottle : maxDeltaThrottle;

		for (i = 0; i < numberMotor; i++)
		{
			motor_temp[i] = constrain(motor_temp[i], rxCommands[THROTTLE] - deltaThrottle, rxCommands[THROTTLE] + deltaThrottle);

			motor_temp[i] = ((motor_temp[i] * THROTTLE_DEADBAND_SLOPE) + THROTTLE_DEADBAND) / 2 + 47;
			motor_value[i] = constrain16(motor_temp[i], 1237, 2047);
		}
	}
	else
	{
		motor_value[MOTOR1] = 0;
		motor_value[MOTOR2] = 0;
		motor_value[MOTOR3] = 0;
		motor_value[MOTOR4] = 0;
	}
}
