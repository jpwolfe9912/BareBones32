/** @file 		config.h
 *  @brief
 *  	This file configures the EEPROM settings of the flight controller.
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		03 MAR 2022
 */

///////////////////////////////////////////////////////////////////////////////

#ifndef __CONFIG_H__
#define __CONFIG_H__

/* Defines */
#define FLASH_WRITE_EEPROM_ADDR  0x08060000  // FLASH_Sector_7

/* Enumerations */
enum { eepromConfigNUMWORD =  sizeof(eepromConfig_t)/sizeof(uint32_t) };

/* Global Variables */
extern const char rcChannelLetters[12];

extern semaphore_t eepromChanged;

/* Function Prototypes */
void readEEPROM(void);
void writeEEPROM(void);
void checkFirstTime(bool eepromReset);

///////////////////////////////////////////////////////////////////////////////
#endif
