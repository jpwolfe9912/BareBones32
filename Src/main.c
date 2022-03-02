#include "board.h"


__attribute__((__section__(".eeprom"), used)) const int8_t eepromArray[16384];

eepromConfig_t	eepromConfig;

uint8_t			execUpCount = 0;

sensors_t		sensors;

heading_t		heading;

gps_t			gps;

homeData_t		homeData;

uint16_t		timerValue;

void			(*openLogPortPrintF)(const char *fmt, ...);

float roll, pitch, yaw;

int main(void)
{
	uint32_t currentTime;

	arm_matrix_instance_f32 a;
	arm_matrix_instance_f32 b;
	arm_matrix_instance_f32 x;

	systemReady = false;

	systemInit();

	systemReady = true;

	uint16_t frameCounter50Hz = 0;

	while (1)
	{
		if(frame_1000Hz)
		{
			frame_1000Hz = false;

			currentTime = micros();
			deltaTime1000Hz = currentTime - previous1000HzTime;
			previous1000HzTime = currentTime;
			readMPU6000();
			ibusProcess();

			motor_value[0] = 100;//ibusChannels[2] * 2 - 1952;
			motor_value[1] = 100;//ibusChannels[2] * 2 - 1952;
			motor_value[2] = 100;//ibusChannels[2] * 2 - 1952;
			motor_value[3] = 100;//ibusChannels[2] * 2 - 1952;
			dshotWrite(motor_value);
			printf("Motor Value is: %u\n", motor_value[1]);

//			printf("ROLL:  %f\n", getRoll());
//			printf("PITCH: %f\n", getPitch());
//			printf("YAW:   %f\n\n", getYaw());
		}

		if (frame_500Hz)
		{

			frame_500Hz = false;

			currentTime       = micros();
			deltaTime500Hz    = currentTime - previous500HzTime;
			previous500HzTime = currentTime;

//			TIM_Cmd(TIM10, DISABLE);
//			timerValue = TIM_GetCounter(TIM10);
//			TIM_SetCounter(TIM10, 0);
//			TIM_Cmd(TIM10, ENABLE);

			dt500Hz = (float)timerValue * 0.0000005f;  // For integrations in 500 Hz loop

			computeMPU6000TCBias();

			nonRotatedAccelData[XAXIS] = ((float)accelSummedSamples500Hz[XAXIS] * 0.5f - accelTCBias[XAXIS]) * ACCEL_SCALE_FACTOR;
			nonRotatedAccelData[YAXIS] = ((float)accelSummedSamples500Hz[YAXIS] * 0.5f - accelTCBias[YAXIS]) * ACCEL_SCALE_FACTOR;
			nonRotatedAccelData[ZAXIS] = ((float)accelSummedSamples500Hz[ZAXIS] * 0.5f - accelTCBias[ZAXIS]) * ACCEL_SCALE_FACTOR;

			arm_mat_init_f32(&a, 3, 3, (float *)mpuOrientationMatrix);

			arm_mat_init_f32(&b, 3, 1, (float *)nonRotatedAccelData);

			arm_mat_init_f32(&x, 3, 1,          sensors.accel500Hz);

			arm_mat_mult_f32(&a, &b, &x);

			nonRotatedGyroData[ROLL ] = ((float)gyroSummedSamples500Hz[ROLL]  * 0.5f - gyroRTBias[ROLL ] - gyroTCBias[ROLL ]) * GYRO_SCALE_FACTOR;
			nonRotatedGyroData[PITCH] = ((float)gyroSummedSamples500Hz[PITCH] * 0.5f - gyroRTBias[PITCH] - gyroTCBias[PITCH]) * GYRO_SCALE_FACTOR;
			nonRotatedGyroData[YAW  ] = ((float)gyroSummedSamples500Hz[YAW]   * 0.5f - gyroRTBias[YAW  ] - gyroTCBias[YAW  ]) * GYRO_SCALE_FACTOR;

			arm_mat_init_f32(&a, 3, 3, (float *)mpuOrientationMatrix);

			arm_mat_init_f32(&b, 3, 1, (float *)nonRotatedGyroData);

			arm_mat_init_f32(&x, 3, 1,          sensors.gyro500Hz);

			arm_mat_mult_f32(&a, &b, &x);

			updateIMU(nonRotatedGyroData[ROLL ], nonRotatedGyroData[PITCH], nonRotatedGyroData[YAW],
					nonRotatedAccelData[XAXIS], nonRotatedAccelData[YAXIS], nonRotatedAccelData[ZAXIS]);
			roll = getRoll();
			pitch = getPitch();
			yaw = getYaw();

//			MargAHRSupdate(sensors.gyro500Hz[ROLL],   sensors.gyro500Hz[PITCH],  sensors.gyro500Hz[YAW],
//					sensors.accel500Hz[XAXIS], sensors.accel500Hz[YAXIS], sensors.accel500Hz[ZAXIS],
//					sensors.mag10Hz[XAXIS],    sensors.mag10Hz[YAXIS],    sensors.mag10Hz[ZAXIS],
//					eepromConfig.accelCutoff,
//					magDataUpdate,
//					dt500Hz);
//
//			magDataUpdate = false;
//
//			computeAxisCommands(dt500Hz);
//			mixTable();
//			writeServos();
//			dshot_write(motor_value);

			executionTime500Hz = micros() - currentTime;

		}

		///////////////////////////////

		if (frame_100Hz)
		{

			frame_100Hz = false;

			currentTime       = micros();
			deltaTime100Hz    = currentTime - previous100HzTime;
			previous100HzTime = currentTime;

//			TIM_Cmd(TIM11, DISABLE);
//			timerValue = TIM_GetCounter(TIM11);
//			TIM_SetCounter(TIM11, 0);
//			TIM_Cmd(TIM11, ENABLE);

			dt100Hz = (float)timerValue * 0.0000005f;  // For integrations in 100 Hz loop

			nonRotatedAccelData[XAXIS] = ((float)accelSummedSamples100Hz[XAXIS] * 0.1f - accelTCBias[XAXIS]) * ACCEL_SCALE_FACTOR;
			nonRotatedAccelData[YAXIS] = ((float)accelSummedSamples100Hz[YAXIS] * 0.1f - accelTCBias[YAXIS]) * ACCEL_SCALE_FACTOR;
			nonRotatedAccelData[ZAXIS] = ((float)accelSummedSamples100Hz[ZAXIS] * 0.1f - accelTCBias[ZAXIS]) * ACCEL_SCALE_FACTOR;

			arm_mat_init_f32(&a, 3, 3, (float *)mpuOrientationMatrix);

			arm_mat_init_f32(&b, 3, 1, (float *)nonRotatedAccelData);

			arm_mat_init_f32(&x, 3, 1,          sensors.accel100Hz);

			arm_mat_mult_f32(&a, &b, &x);

//			createRotationMatrix();
//			bodyAccelToEarthAccel();
//			vertCompFilter(dt100Hz);
//
//			if (armed == true)
//			{
//				if ( eepromConfig.activeTelemetry == 1 )
//				{
//					// Roll Loop
//					openLogPortPrintF("1,%1d,%9.4f,%9.4f,%9.4f,%9.4f,%9.4f,%9.4f\n", flightMode,
//							rateCmd[ROLL],
//							sensors.gyro500Hz[ROLL],
//							ratePID[ROLL],
//							attCmd[ROLL],
//							sensors.attitude500Hz[ROLL],
//							attPID[ROLL]);
//				}
//
//				if ( eepromConfig.activeTelemetry == 2 )
//				{
//					// Pitch Loop
//					openLogPortPrintF("2,%1d,%9.4f,%9.4f,%9.4f,%9.4f,%9.4f,%9.4f\n", flightMode,
//							rateCmd[PITCH],
//							sensors.gyro500Hz[PITCH],
//							ratePID[PITCH],
//							attCmd[PITCH],
//							sensors.attitude500Hz[PITCH],
//							attPID[PITCH]);
//				}
//
//				if ( eepromConfig.activeTelemetry == 4 )
//				{
//					// Sensors
//					openLogPortPrintF("3,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,\n", sensors.accel500Hz[XAXIS],
//							sensors.accel500Hz[YAXIS],
//							sensors.accel500Hz[ZAXIS],
//							sensors.gyro500Hz[ROLL],
//							sensors.gyro500Hz[PITCH],
//							sensors.gyro500Hz[YAW],
//							sensors.mag10Hz[XAXIS],
//							sensors.mag10Hz[YAXIS],
//							sensors.mag10Hz[ZAXIS],
//							sensors.attitude500Hz[ROLL],
//							sensors.attitude500Hz[PITCH],
//							sensors.attitude500Hz[YAW]);
//
//				}
//
//				if ( eepromConfig.activeTelemetry == 8 )
//				{
//
//				}
//
//				if ( eepromConfig.activeTelemetry == 16)
//				{
//					// Vertical Variables
//					openLogPortPrintF("%9.4f, %9.4f, %9.4f, %4ld, %1d, %9.4f\n", verticalVelocityCmd,
//							hDotEstimate,
//							hEstimate,
//							ms5611Temperature,
//							verticalModeState,
//							throttleCmd);
//				}
//			}

			executionTime100Hz = micros() - currentTime;

		}
		if (frame_50Hz)
		{
			frameCounter50Hz++;
			if (frameCounter50Hz > FRAME_COUNT)
				frameCounter50Hz = 1;

			frame_50Hz = false;
			currentTime      = micros();
			deltaTime50Hz    = currentTime - previous50HzTime;
			previous50HzTime = currentTime;


//			processFlightCommands();

//			if (newTemperatureReading && newPressureReading)
//			{
//				d1Value = d1.value;
//				d2Value = d2.value;
//
//				calculateTemperature();
//				calculatePressureAltitude();
//
//				newTemperatureReading = false;
//				newPressureReading    = false;
//			}

//			sensors.pressureAlt50Hz = firstOrderFilter(sensors.pressureAlt50Hz, &firstOrderFilters[PRESSURE_ALT_LOWPASS]);

//			rssiMeasure();

//			updateMax7456(currentTime, 0);

			executionTime50Hz = micros() - currentTime;

		}

		///////////////////////////////

		if (frame_10Hz)
		{

			frame_10Hz = false;

			currentTime      = micros();
			deltaTime10Hz    = currentTime - previous10HzTime;
			previous10HzTime = currentTime;

//			if (newMagData == true)
//			{
//				nonRotatedMagData[XAXIS] = (rawMag[XAXIS].value * magScaleFactor[XAXIS]) - eepromConfig.magBias[XAXIS + eepromConfig.externalHMC5883];
//				nonRotatedMagData[YAXIS] = (rawMag[YAXIS].value * magScaleFactor[YAXIS]) - eepromConfig.magBias[YAXIS + eepromConfig.externalHMC5883];
//				nonRotatedMagData[ZAXIS] = (rawMag[ZAXIS].value * magScaleFactor[ZAXIS]) - eepromConfig.magBias[ZAXIS + eepromConfig.externalHMC5883];
//
//				arm_mat_init_f32(&a, 3, 3, (float *)hmcOrientationMatrix);
//
//				arm_mat_init_f32(&b, 3, 1, (float *)nonRotatedMagData);
//
//				arm_mat_init_f32(&x, 3, 1,          sensors.mag10Hz);
//
//				arm_mat_mult_f32(&a, &b, &x);
//
//				newMagData = false;
//				magDataUpdate = true;
//			}
//
//			decodeUbloxMsg();
//
//			batMonTick();
//
//			cliCom();
//
//			if (eepromConfig.mavlinkEnabled == true)
//			{
//				mavlinkSendAttitude();
//				mavlinkSendVfrHud();
//			}

			executionTime10Hz = micros() - currentTime;

		}

		///////////////////////////////



		///////////////////////////////

		if (frame_5Hz)
		{
			frame_5Hz = false;

			currentTime     = micros();
			deltaTime5Hz    = currentTime - previous5HzTime;
			previous5HzTime = currentTime;

//			if (gpsValid() == true)
//			{
//
//			}
//
//			if (eepromConfig.mavlinkEnabled == true)
//			{
//				mavlinkSendGpsRaw();
//			}
//
//			if (batMonVeryLowWarning > 0)
//			{
//				LED1_TOGGLE;
//				batMonVeryLowWarning--;
//			}
//
//			if (execUp == true)
//				BLUE_LED_TOGGLE;

			executionTime5Hz = micros() - currentTime;
		}

		///////////////////////////////

		if (frame_1Hz)
		{
			frame_1Hz = false;

			currentTime     = micros();
			deltaTime1Hz    = currentTime - previous1HzTime;
			previous1HzTime = currentTime;

//			if (execUp == true)
//				GREEN_LED_TOGGLE;
//
//			if (execUp == false)
//				execUpCount++;
//
//			if ((execUpCount == 5) && (execUp == false))
//			{
//				execUp = true;
//
//				pwmEscInit();
//
//				homeData.magHeading = sensors.attitude500Hz[YAW];
//			}
//
//			if (batMonLowWarning > 0)
//			{
//				LED1_TOGGLE;
//				batMonLowWarning--;
//			}
//
//			if (eepromConfig.mavlinkEnabled == true)
//			{
//				mavlinkSendHeartbeat();
//				mavlinkSendSysStatus();
//			}

			executionTime1Hz = micros() - currentTime;
		}
	}
}

/**
 * @brief System Clock Configuration
 * @retval None
 */


/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/