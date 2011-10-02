/**************************************************************************//**
  \file  i2cPacket.c

  \brief Provides the functionality for the writing and the reading \n
         of packets through the TWI.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      5/12/07 A. Khromykh - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

/******************************************************************************
                   Includes section
******************************************************************************/
#include <types.h>
#include <i2cPacket.h>
#include <i2c.h>
#include <halTaskManager.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
/* states of the i2c transaction */
#define I2C_CLOSE                    0
#define I2C_IDLE                     1
#define I2C_WRITE_IADDR_WRITE_DATA   2
#define I2C_WRITE_IADDR_READ_DATA    3
#define I2C_WRITE_DATA               4
#define I2C_READ_DATA                5
#define I2C_TRANSAC_SUCCESS          6
#define I2C_TRANSAC_FAIL             7

/******************************************************************************
                   Types section
******************************************************************************/
typedef struct
{
  volatile uint8_t* data;             // bytes to write to the i2c bus
  volatile uint8_t length;            // length in bytes of the request
  volatile uint8_t index;             // current index of read/write byte
  volatile uint8_t addr;              // destination address
  volatile uint32_t intAddress;       // internal address inner i2c device
  void (*done)(bool result);          // callback
} HalI2cPacketControl_t;

/******************************************************************************
                   Global variables section
******************************************************************************/
// current state of the i2c request
volatile uint8_t halI2cPacketState = I2C_CLOSE;
HalI2cPacketControl_t halI2cPacketControl;

/******************************************************************************
                   Implementations section
******************************************************************************/
/******************************************************************************
Resets TWI bus and i2c HAL.
******************************************************************************/
void halI2cBusReset(void)
{
  halI2cPacketState = I2C_TRANSAC_FAIL;
  halResetI2c();
  halPostTask4(HAL_TWI);
}

/******************************************************************************
Opens resource.
Parameters:
  i2cMode - pointer to clock rate structure.
Returns:
  Returns:
  -1 - resource was opened or pointer is NULL
   0 - success.
******************************************************************************/
int HAL_OpenI2cPacket(HAL_i2cMode_t *i2cMode)
{
  if (NULL == i2cMode)
    return -1;
  if (I2C_CLOSE == halI2cPacketState)
  {
    halInitI2c(i2cMode);
    halI2cPacketState = I2C_IDLE;
    halI2cPacketControl.index = 0;
    return 0;
  }
  return -1;
}

/******************************************************************************
Closes resource.
Returns:
  -1 - resource was not opened.
  0 -  success.
******************************************************************************/
int HAL_CloseI2cPacket(void)
{
  if (I2C_CLOSE != halI2cPacketState)
  {
    halI2cPacketControl.done = NULL;
    halI2cPacketState = I2C_CLOSE;
    return 0;
  }
  return -1;
}

/******************************************************************************
Writes the series of bytes out to the TWI bus.
Parameters:
  param - pointer to HAL_I2cParams_t structure
Returns:
  0 - the bus is free and the request is accepted.
  -1 - other case.
******************************************************************************/
int HAL_WriteI2cPacket(HAL_I2cParams_t *param)
{
  if ((I2C_IDLE == halI2cPacketState) && param)
  {
    halI2cPacketControl.addr = param->id;
    halI2cPacketControl.data = param->data;
    halI2cPacketControl.index = param->lengthAddr;
    halI2cPacketControl.length = param->length;
    halI2cPacketControl.done = param->f;
    halI2cPacketControl.intAddress = param->internalAddr;
  }
  else
  {
    return -1;
  }

  if (HAL_NO_INTERNAL_ADDRESS == halI2cPacketControl.index)
    halI2cPacketState = I2C_WRITE_DATA;
  else
    halI2cPacketState = I2C_WRITE_IADDR_WRITE_DATA;
  halSendStartI2c();
  return 0;
}

/******************************************************************************
Reads the series of bytes out to the TWI bus.
Parameters:
  param - pointer to HAL_I2cParams_t structure
Returns:
  0 - the bus is free and the request is accepted.
  -1 - other case.
******************************************************************************/
int HAL_ReadI2cPacket(HAL_I2cParams_t *param)
{
  if ((I2C_IDLE == halI2cPacketState) && param)
  {
    halI2cPacketControl.addr = param->id;
    halI2cPacketControl.index = param->lengthAddr;
    halI2cPacketControl.length = param->length;
    halI2cPacketControl.data = param->data;
    halI2cPacketControl.done = param->f;
    halI2cPacketControl.intAddress = param->internalAddr;
  }
  else
  {
    return -1;
  }

  if (HAL_NO_INTERNAL_ADDRESS == halI2cPacketControl.index)
    halI2cPacketState = I2C_READ_DATA;
  else
    halI2cPacketState = I2C_WRITE_IADDR_READ_DATA;
  halSendStartI2c();
  return 0;
}

/******************************************************************************
Notification about the start condition was sent.
Parameters:
  none.
Returns:
  none.
******************************************************************************/
void halSendStartDoneI2c(void)
{
  if ((I2C_WRITE_IADDR_WRITE_DATA == halI2cPacketState) ||
      (I2C_WRITE_IADDR_READ_DATA == halI2cPacketState) ||
      (I2C_WRITE_DATA == halI2cPacketState))
  {
    halWriteI2c(((halI2cPacketControl.addr << 1) + 0));
  }
  else if (I2C_READ_DATA == halI2cPacketState)
  {
    halWriteI2c(((halI2cPacketControl.addr << 1) + 1));
  }
  else
  { // abnormal
    halI2cBusReset();
  }
}

/******************************************************************************
Sending data to i2c bus. If last byte then send stop condition and post task.
Parameters:
  none.
Returns:
  none.
******************************************************************************/
void halWriteData(void)
{
  if (halI2cPacketControl.index < halI2cPacketControl.length)
  {
    halWriteI2c(halI2cPacketControl.data[halI2cPacketControl.index++]);
  }
  else
  {
    halI2cPacketState = I2C_TRANSAC_SUCCESS;
    halSendStopI2c();
    halPostTask4(HAL_TWI);
  }
}

/******************************************************************************
Sending internal device address to i2c bus. If address is sent then switch i2c
hal state.
Parameters:
  none.
Returns:
  none.
******************************************************************************/
void halWriteInternalAddress(void)
{
  uint8_t data;

  data = (uint8_t)(halI2cPacketControl.intAddress >> --halI2cPacketControl.index * 8);
  halWriteI2c(data);

  if (0 == halI2cPacketControl.index)
  {
    if (I2C_WRITE_IADDR_WRITE_DATA == halI2cPacketState)
      halI2cPacketState = I2C_WRITE_DATA;
    else
      halI2cPacketState = I2C_READ_DATA;
  }
}

/******************************************************************************
Notification that byte was written to the TWI.
Parameters:
  result - contains result of previous operation.
Returns:
  none.
******************************************************************************/
void halWriteDoneI2c(void)
{
  if (I2C_WRITE_DATA == halI2cPacketState)
  {
    halWriteData();
  }
  else if ((I2C_WRITE_IADDR_WRITE_DATA == halI2cPacketState) || (I2C_WRITE_IADDR_READ_DATA == halI2cPacketState))
  {
    halWriteInternalAddress();
  }
  else if (I2C_READ_DATA == halI2cPacketState)
  {
    halSendStartI2c();
  }
  else
  { // abnormal
    halI2cBusReset();
  }
}

/******************************************************************************
Notification that address byte was written to the TWI and was read ACK.
Starts reading data.
Parameters:
  none.
Returns:
  none.
******************************************************************************/
void halMasterReadWriteAddressAckI2c(void)
{
  if (I2C_READ_DATA == halI2cPacketState)
  {
    if (1 == halI2cPacketControl.length)
      halReadI2c(false); // send nack
    else
      halReadI2c(true);  // send ack
  }
  else
  { // abnormal
    halI2cBusReset();
  }
}

/******************************************************************************
Notification that byte was read from the TWI.
Parameters:
  data - contains byte that was read.
Returns:
  none.
******************************************************************************/
void halReadDoneI2c(uint8_t data)
{
  if (I2C_READ_DATA == halI2cPacketState)
  {
    halI2cPacketControl.data[halI2cPacketControl.index++] = data;
    if (halI2cPacketControl.index < (halI2cPacketControl.length - 1))
      halReadI2c(true);  // send ACK
    else
      halReadI2c(false); // send NACK
  }
  else
  { // abnormal
    halI2cBusReset();
  }
}

/******************************************************************************
Notification that last byte was read from the TWI. Needs send STOP condition
on bus.
Parameters:
  data - contains byte that was read.
Returns:
  none.
******************************************************************************/
void halReadLastByteDoneI2c(uint8_t data)
{
  if (I2C_READ_DATA == halI2cPacketState)
  {
    halI2cPacketControl.data[halI2cPacketControl.index++] = data;
    halI2cPacketState = I2C_TRANSAC_SUCCESS;
    halSendStopI2c();
    halPostTask4(HAL_TWI);
  }
  else
  { // abnormal
    halI2cBusReset();
  }
}

/******************************************************************************
Waits for end of sending and calls user's callback
******************************************************************************/
void halSig2WireSerialHandler(void)
{
  if (halI2cPacketControl.done)
  {
    if (I2C_TRANSAC_SUCCESS == halI2cPacketState)
    {
      halWaitEndOfStopStation();
      halI2cPacketState = I2C_IDLE;
      halI2cPacketControl.done(true);
    }
    else
    {
      halI2cPacketState = I2C_IDLE;
      halI2cPacketControl.done(false);
    }
  }
  else
  {
    halI2cPacketState = I2C_IDLE;
  }
}
// eof i2cPacket.c
