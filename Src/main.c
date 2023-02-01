/** @file 		main.c
 *  @brief
 *  	This file runs all the other functions and carries out the scheduling of tasks.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 */

/* Includes */
#include "board.h"

#ifndef SANDBOX
/* Global Variables */
const uint8_t __attribute__((__section__(".eeprom"), used)) eepromArray[131072];

eepromConfig_t eepromConfig;

uint8_t execUpCount = 0;

sensors_t sensors;

uint16_t timerValue;

int main(void)
{
    systemReady = false;

    systemInit();

    /* Add tasks */
    Tasks *execTasks[8] = {NULL};

    append(&execTasks[FRAME_1000HZ], readMPU6000);

    append(&execTasks[FRAME_500HZ], computeRotations500Hz);
    append(&execTasks[FRAME_500HZ], updateIMU);
    append(&execTasks[FRAME_500HZ], updateAttitude);
    append(&execTasks[FRAME_500HZ], processCommands);
    append(&execTasks[FRAME_500HZ], computeAxisCommands);
    append(&execTasks[FRAME_500HZ], mixTable);
    append(&execTasks[FRAME_500HZ], dshotWrite);

    append(&execTasks[FRAME_200HZ], ibusProcess);

    append(&execTasks[FRAME_100HZ], printLog);

    append(&execTasks[FRAME_5HZ], battMonRead);

    append(&execTasks[FRAME_1HZ], ledsSet);

    systemReady = true;

    while (1)
    {
        run(execTasks);
        // if (frame_1000Hz)
        // {
        //     frame_1000Hz = false;

        //     currentTime = micros();
        //     deltaTime1000Hz = currentTime = previous1000HzTime;
        //     previous1000HzTime = currentTime;

        //     executionTime1000Hz = micros() - currentTime;
        // }

        // if (frame_500Hz)
        // {

        //     frame_500Hz = false;

        //     currentTime = micros();
        //     deltaTime500Hz = currentTime - previous500HzTime;
        //     previous500HzTime = currentTime;

        //     computeRotations500Hz();
        //     updateIMU();
        //     processCommands();
        //     computeAxisCommands();
        //     mixTable();
        //     dshotWrite();

        //     executionTime500Hz = micros() - currentTime;
        // }

        // if (frame_200Hz)
        // {
        //     frame_200Hz = false;

        //     currentTime = micros();
        //     deltaTime200Hz = currentTime - previous200HzTime;
        //     previous200HzTime = currentTime;

        //     ibusProcess();

        //     executionTime200Hz = micros() - currentTime;
        // }

        // if (frame_100Hz)
        // {

        //     frame_100Hz = false;

        //     currentTime = micros();
        //     deltaTime100Hz = currentTime - previous100HzTime;
        //     previous100HzTime = currentTime;

        //     if (armed == false)
        //     {
        //         printLog(PITCH);
        //     }

        //     executionTime100Hz = micros() - currentTime;
        // }
        
        // if (frame_50Hz)
        // {

        //     frame_50Hz = false;

        //     currentTime = micros();
        //     deltaTime50Hz = currentTime - previous50HzTime;
        //     previous50HzTime = currentTime;

        //     executionTime50Hz = micros() - currentTime;
        // }

        // if (frame_10Hz)
        // {

        //     frame_10Hz = false;

        //     currentTime = micros();
        //     deltaTime10Hz = currentTime - previous10HzTime;
        //     previous10HzTime = currentTime;
        //     executionTime10Hz = micros() - currentTime;
        // }

        // if (frame_5Hz)
        // {
        //     frame_5Hz = false;

        //     currentTime = micros();
        //     deltaTime5Hz = currentTime - previous5HzTime;
        //     previous5HzTime = currentTime;

        //     battMonRead();
        //     if (battEmpty)
        //         led4TOGGLE();
        //     else
        //         led4OFF();

        //     executionTime5Hz = micros() - currentTime;
        // }

        // if (frame_1Hz)
        // {
        //     frame_1Hz = false;

        //     currentTime = micros();
        //     deltaTime1Hz = currentTime - previous1HzTime;
        //     previous1HzTime = currentTime;

        //     if (armed) // 1 - Armed
        //         led1ON();
        //     else
        //         led1OFF();
        //     if (systemReady) // 2 - System Initialized
        //         led2ON();
        //     else
        //         led2OFF();
        //     if (rcActive) // 3 - Receiver Connected
        //         led3ON();
        //     else
        //         led3OFF();
        //     if (battLow) // 4 - Battery Low
        //         led4TOGGLE();
        //     else
        //         led4OFF();
        //     if (mode == FLIGHT) // 5(toggle) - Flight Mode
        //         led5TOGGLE();
        //     else if (mode == ROVER) // 5(on) - Rover Mode
        //         led5ON();
        //     else
        //         led5OFF();

        //     executionTime1Hz = micros() - currentTime;
        // }
    }
}

#else
const uint8_t __attribute__((__section__(".eeprom"), used)) eepromArray[131072];

eepromConfig_t eepromConfig;

uint8_t execUpCount = 0;

sensors_t sensors;

uint16_t timerValue;

int main(void)
{
    systemInit();
    systemReady = true;

    while (1)
    {
        if (frame_1000Hz)
            frame_1000Hz = false;
        if (frame_500Hz)
        {
            frame_500Hz = false;

            computeRotations500Hz();

            updateIMU(sensors.gyro500Hz[ROLL], sensors.gyro500Hz[PITCH], sensors.gyro500Hz[YAW],
                      sensors.accel500Hz[XAXIS], sensors.accel500Hz[YAXIS], sensors.accel500Hz[ZAXIS]);
            sensors.attitude500Hz[ROLL] = getRollRadians();
            sensors.attitude500Hz[PITCH] = getPitchRadians();
            sensors.attitude500Hz[YAW] = getYawRadians();

            sensors.attDeg500Hz[ROLL] = sensors.attitude500Hz[ROLL] * 57.29578f;
            sensors.attDeg500Hz[PITCH] = sensors.attitude500Hz[PITCH] * 57.29578f;
            sensors.attDeg500Hz[YAW] = sensors.attitude500Hz[YAW] * 57.29578f;

            printf("%f\n", sensors.attDeg500Hz[ROLL]);
        }
    }
}

#endif

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}

#ifdef USE_FULL_ASSERT
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
