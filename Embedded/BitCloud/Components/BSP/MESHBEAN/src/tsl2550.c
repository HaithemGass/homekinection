/**************************************************************************//**
\file  tsl2550.c

\brief Implementation of access to tsl2550 the sensor, light sensor.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    29/06/07 E. Ivanov - Created
******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <tsl2550.h>
#include <i2cPacket.h>
#include <bspTaskManager.h>
#include <appTimer.h>
#include <bspDbg.h>

/******************************************************************************
                   Definitions section
******************************************************************************/
// device address on i2c bus
#define TSL_DEVICE_ADDRESS 0x39

// device registers internal address
#define TSL_READ_CHANNEL0_COMMAND 0x43
#define TSL_READ_CHANNEL1_COMMAND 0x83
#define TSL_POWERDOWN_COMMAND      0x00
#define TSL_POWERUP_COMMAND        0x03
#define TSL_EXTENDED_RANGE_COMMAND 0x1D
#define TSL_STANDARD_RANGE_COMMAND 0x18

// standard mode max
#define TSL_MAX_LUX_VALUE 1846

// Conversion time (400ms typ by datasheet). Choose a little more to be on a safe side.
#define TSL_CONVERSION_TIME 500

/******************************************************************************
                   Types section
******************************************************************************/
// states
typedef enum
{
  OFF,  
  IDLE,
  POWERUP,
  POWERUP_ERR,
  DATA,  
  DATA_ERR,
} Tsl2550States_t;

typedef struct
{  
  Tsl2550States_t state;
  uint8_t ch0;
  uint8_t ch1;
  void(* callback)(bool result, int16_t data); // data callback pointer
} Tsl2550Control_t;

/******************************************************************************
                   Constants section
******************************************************************************/
PROGMEM_DECLARE(const uint8_t tsl2550Ratio[129]) =
{
100,100,100,100,100,100,100,100,
100,100,100,100,100,100, 99, 99,
 99, 99, 99, 99, 99, 99, 99, 99,
 99, 99, 99, 98, 98, 98, 98, 98,
 98, 98, 97, 97, 97, 97, 97, 96,
 96, 96, 96, 95, 95, 95, 94, 94,
 93, 93, 93, 92, 92, 91, 91, 90,
 89, 89, 88, 87, 87, 86, 85, 84,
 83, 82, 81, 80, 79, 78, 77, 75,
 74, 73, 71, 69, 68, 66, 64, 62,
 60, 58, 56, 54, 52, 49, 47, 44,
 42, 41, 40, 40, 39, 39, 38, 38,
 37, 37, 37, 36, 36, 36, 35, 35,
 35, 35, 34, 34, 34, 34, 33, 33,
 33, 33, 32, 32, 32, 32, 32, 31,
 31, 31, 31, 31, 30, 30, 30, 30,
 30
};

PROGMEM_DECLARE(const uint16_t tsl2550Count[128]) =
{
   0,   1,   2,   3,   4,   5,   6,   7,
   8,   9,  10,  11,  12,  13,  14,  15,
  16,  18,  20,  22,  24,  26,  28,  30,
  32,  34,  36,  38,  40,  42,  44,  46,
  49,  53,  57,  61,  65,  69,  73,  77,
  81,  85,  89,  93,  97, 101, 105, 109,
 115, 123, 131, 139, 147, 155, 163, 171,
 179, 187, 195, 203, 211, 219, 227, 235,
 247, 263, 279, 295, 311, 327, 343, 359,
 375, 391, 407, 423, 439, 455, 471, 487,
 511, 543, 575, 607, 639, 671, 703, 735,
 767, 799, 831, 863, 895, 927, 959, 991,
1039,1103,1167,1231,1295,1359,1423,1487,
1551,1615,1679,1743,1807,1871,1935,1999,
2095,2223,2351,2479,2607,2735,2863,2991,
3119,3247,3375,3503,3631,3759,3887,4015
};

/******************************************************************************
                   Static function prototypes section
******************************************************************************/
static bool tsl2550StartReading(void);
static void tsl2550StartFirstReading(void);
static void tsl2550I2cPacketReadDoneCh1(bool result);
static void tsl2550I2cPacketWriteDoneCh1(bool result);
static void tsl2550I2cPacketReadDoneCh0(bool result);
static void tsl2550I2cPacketWriteDoneCh0(bool result);
static void tsl2550I2cPowerupDone(bool result);
static bool tsl2550StartPowerup(void);

/******************************************************************************
                   Implementations section
******************************************************************************/

static Tsl2550Control_t tsl2550Control = {.state = OFF};

/**************************************************************************//**
\brief  Opens the component to use.

\return
  BC_SUCCESS - the component is ready to be used. \n
  BC_FAIL - otherwise.
******************************************************************************/
result_t openTsl2550(void)
{
  if (IDLE == tsl2550Control.state || OFF == tsl2550Control.state)
    return BC_SUCCESS;

  return BC_FAIL;
}

/**************************************************************************//**
\brief  Performs the test if the component have completed request.

\return
  BC_FAIL - the previous request is not completed. \n
  BC_SUCCES - otherwise.
******************************************************************************/
result_t closeTsl2550(void)
{
  if (IDLE == tsl2550Control.state || OFF == tsl2550Control.state)
    return BC_SUCCESS;

  return BC_FAIL;
}

/**************************************************************************//**
\brief  Reads data from tsl2550 sensor.

\param[in]
    f - callback method
\param[in]
    result - the result of the requested operation.
             true - operation finished successfully, false - some error has
             occured.
\param[in]
    data - sensor data.

\return
    BC_FAIL - the previous request was not completed,
           the address of callback is 0, i2c interface is busy,
           there is error on i2c interface. \n
    BC_SUCCESS - in other case.
******************************************************************************/
result_t readTsl2550Data(void (*f)(bool result, int16_t data))
{
  HAL_i2cMode_t i2cMode = {.clockrate = I2C_CLOCK_RATE_62};

  if (NULL == f)
    return BC_FAIL;

  if (OFF == tsl2550Control.state)    // Sensor is in powerdown mode, powerup it before reading
  {
    if (-1 == HAL_OpenI2cPacket(&i2cMode))
      return BC_FAIL;

    if (false == tsl2550StartPowerup())
    {    
      HAL_CloseI2cPacket();
      return BC_FAIL;
    }
    tsl2550Control.state = POWERUP;
  }
  else if (IDLE == tsl2550Control.state)
  {      
    if (-1 == HAL_OpenI2cPacket(&i2cMode))
      return BC_FAIL;

    if (false == tsl2550StartReading())
    {    
      HAL_CloseI2cPacket();
      return BC_FAIL;
    }
    tsl2550Control.state = DATA;
  }
  else 
  {
    return BC_FAIL;
  }

  tsl2550Control.callback = f;

  return BC_SUCCESS;
}

/**************************************************************************//**
\brief BSP tsl2550 handler.
******************************************************************************/
void bspTsl2550Handler(void)
{
  uint8_t result = false;
  uint16_t lux = 0;

  HAL_CloseI2cPacket(); // free

  switch (tsl2550Control.state)
  {
    case POWERUP_ERR:      
      tsl2550Control.state = OFF;
      break;
      
    case DATA_ERR:    
      tsl2550Control.state = IDLE;      
      break;
  
    case DATA:
      {        
        uint32_t count0 = 0, count1 = 0;
        uint8_t ratio = 128;    // default scaling factor        
        uint8_t R;

        tsl2550Control.state = IDLE;

        if (tsl2550Control.ch0 & tsl2550Control.ch1 & 0x80)
        {
          memcpy_P(&count0, &tsl2550Count[tsl2550Control.ch0 & 0x7F], sizeof(uint16_t));
          memcpy_P(&count1, &tsl2550Count[tsl2550Control.ch1 & 0x7F], sizeof(uint16_t));

          if (!count0 || (count0 <= count1))  // count1 cannot be greater than count0
            break;          

          ratio = ((uint32_t)(count1 * 128ul) / count0);
          // calculate lux
          // the "256" is a scaling factor
          memcpy_P(&R, &tsl2550Ratio[ratio], sizeof(uint8_t));
          lux = ((count0 - count1) * R) / 256;
          // range check lux
          if (lux > TSL_MAX_LUX_VALUE)
            lux = TSL_MAX_LUX_VALUE;          

          result = true;
        }
      }
      break;
            
    default:
      ASSERT(false, TSL2550_UNEXPECTED_STATE);      
      tsl2550Control.state = IDLE;
      break;
  }
  
  tsl2550Control.callback(result, lux); 
}

/**************************************************************************//**
\brief Callback that reading from tsl2550 was completed.

\param[in]
  result - contains result of operation.
           if result is false there was problem on i2c interface.
******************************************************************************/
static void tsl2550I2cPacketReadDoneCh1(bool result)
{
  if (false == result)  
    tsl2550Control.state = DATA_ERR;

  bspPostTask(BSP_LIGHT);
}

/**************************************************************************//**
\brief Callback that writing command to tsl2550 was completed.

\param[in]
  result - contains result of operation.
           if result is false there was problem on i2c interface.
******************************************************************************/
static void tsl2550I2cPacketWriteDoneCh1(bool result)
{
  HAL_I2cParams_t i2cParam = 
  {
    .data = &tsl2550Control.ch1,
    .f = tsl2550I2cPacketReadDoneCh1,
    .id = TSL_DEVICE_ADDRESS,
    .length = 1,
    .lengthAddr = HAL_NO_INTERNAL_ADDRESS,
  };

  if ((false == result) || (-1 == HAL_ReadI2cPacket(&i2cParam)))
  {
    tsl2550Control.state = DATA_ERR;
    bspPostTask(BSP_LIGHT);
  }
}

/**************************************************************************//**
\brief Callback that reading from tsl2550 was completed.

\param[in]
  result - contains result of operation.
           if result is false there was problem on i2c interface.
******************************************************************************/
static void tsl2550I2cPacketReadDoneCh0(bool result)
{
  HAL_I2cParams_t i2cParam = 
  {
    .data = &tsl2550Control.ch1,
    .f = tsl2550I2cPacketWriteDoneCh1,
    .id = TSL_DEVICE_ADDRESS,
    .length = 1,
    .lengthAddr = HAL_NO_INTERNAL_ADDRESS,
  };

  tsl2550Control.ch1 = TSL_READ_CHANNEL1_COMMAND;

  if ((false == result) || (-1 == HAL_WriteI2cPacket(&i2cParam)))
  {
    tsl2550Control.state = DATA_ERR;
    bspPostTask(BSP_LIGHT);
  }
}

/**************************************************************************//**
\brief Callback that writing command to tsl2550 was completed.

\param[in]
  result - contains result of operation.
           if result is false there was problem on i2c interface.
******************************************************************************/
static void tsl2550I2cPacketWriteDoneCh0(bool result)
{
  HAL_I2cParams_t i2cParam = 
  {
    .data = &tsl2550Control.ch0,
    .f = tsl2550I2cPacketReadDoneCh0,
    .id = TSL_DEVICE_ADDRESS,
    .length = 1,
    .lengthAddr = HAL_NO_INTERNAL_ADDRESS,
  };

  if ((false == result) || (-1 == HAL_ReadI2cPacket(&i2cParam)))
  {
    tsl2550Control.state = DATA_ERR;
    bspPostTask(BSP_LIGHT);
  }
}

/**************************************************************************//**
\brief Start tsl2550 read sequence.

\return
    false - i2c packet wasn't sent
    true - in other case.
******************************************************************************/
static bool tsl2550StartReading(void)
{
  HAL_I2cParams_t i2cParam = 
  {
    .data = &tsl2550Control.ch0,
    .f = tsl2550I2cPacketWriteDoneCh0,
    .id = TSL_DEVICE_ADDRESS,
    .length = 1,
    .lengthAddr = HAL_NO_INTERNAL_ADDRESS,
  };

  tsl2550Control.ch0 = TSL_READ_CHANNEL0_COMMAND;

  return (-1 != HAL_WriteI2cPacket(&i2cParam)) ? true : false;
}

/**************************************************************************//**
\brief Callback on completion of tsl2550's delay after powerup.
******************************************************************************/
static void tsl2550StartFirstReading(void)
{
  if (false == tsl2550StartReading())
  {
    tsl2550Control.state = DATA_ERR;
    bspPostTask(BSP_LIGHT);    
    return;
  }
    
  tsl2550Control.state = DATA;
}

/**************************************************************************//**
\brief Callback on completion of tsl2550 powerup.

\param[in]
  result - contains result of operation.
           if result is false there was problem on i2c interface.
******************************************************************************/
static void tsl2550I2cPowerupDone(bool result)
{  
  static HAL_AppTimer_t tsl2550PowerupTimer =
  {
    .interval = TSL_CONVERSION_TIME * 2,    // Wait for conversions on two channels one-after-one
    .mode = TIMER_ONE_SHOT_MODE,
    .callback = tsl2550StartFirstReading,
  };

  if (false == result)
  {
    tsl2550Control.state = POWERUP_ERR;
    bspPostTask(BSP_LIGHT);
    return;
  }

  HAL_StartAppTimer(&tsl2550PowerupTimer);
}

/**************************************************************************//**
\brief Start tsl2550 powerup sequence.

\return
    false - i2c packet wasn't sent
    true - in other case.
******************************************************************************/
static bool tsl2550StartPowerup(void)
{
  HAL_I2cParams_t i2cParam = 
  {
    .data = &tsl2550Control.ch0,
    .f = tsl2550I2cPowerupDone,
    .id = TSL_DEVICE_ADDRESS,
    .length = 1,
    .lengthAddr = HAL_NO_INTERNAL_ADDRESS,
  };

  tsl2550Control.ch0 = TSL_POWERUP_COMMAND;

  return (-1 != HAL_WriteI2cPacket(&i2cParam)) ? true : false;
}

// eof tsl2550.c
