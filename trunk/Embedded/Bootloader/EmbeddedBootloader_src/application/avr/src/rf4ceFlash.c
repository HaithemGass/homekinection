/****************************************************************************//**
  \file rf4ceFlash.c

  \brief Implementation function for RF4CE OTAU.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    12/01/11 A. Khromykh - Created
*******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <types.h>
#include <bootStructure.h>
#include <sp_driver.h>
#include <flashLoader.h>
#include <rf4ceFlash.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
/** This macro saves the global interrupt status. */
#define ENTER_CRITICAL_REGION()         {uint8_t sreg = SREG; cli()
/** This macro restores the global interrupt status. */
#define LEAVE_CRITICAL_REGION()         SREG = sreg;}
#define EEPROM_BUSY_WAIT()              do{}while(EECR & (1 << EEPE))

/******************************************************************************
                   Prototypes section
******************************************************************************/
#if defined(__ICCAVR__)
#elif defined(__GNUC__)
static void flash_fill_page_buffer(fill_page_buffer_req_t *param) __attribute__((section(".rf4ce")));
static void flash_program_page(program_page_req_t *param) __attribute__((section(".rf4ce")));
static void flash_swap(swap_images_req_t *param) __attribute__((section(".rf4ce"))) __attribute__((__noreturn__));
static void flash_boot_request_handler(void) __attribute__((section(".rf4ce")));
static void flash_erase_page(erase_page_req_t *param) __attribute__((section(".rf4ce")));
static void flash_write_page(write_page_req_t *param) __attribute__((section(".rf4ce")));
#else
  #error 'Unsupported compiler.'
#endif

/******************************************************************************
                   Global variables section
******************************************************************************/
#if defined(__ICCAVR__)
#elif defined(__GNUC__)
__attribute__((section(".access_sect"))) void (* access_point)(void) = flash_boot_request_handler;
#endif

/******************************************************************************
                    Implementation section
******************************************************************************/
/**************************************************************************//**
\brief Enter point to bootloader function
******************************************************************************/
static void flash_boot_request_handler(void)
{
  rf4ce_boot_req_t *request_address;
  uint16_t temp_value;

  temp_value = GPIOR1;
  temp_value <<= 8;
  temp_value |= GPIOR0;
  request_address = (rf4ce_boot_req_t *)temp_value;

  switch (request_address->request_type)
  {
    case FILL_PAGE_BUFFER:
      flash_fill_page_buffer(&request_address->common_boot_req.fill_page_buffer_req);
      break;
    case PROGRAM_PAGE:
      flash_program_page(&request_address->common_boot_req.program_page_req);
      break;
    case SWAP_IMAGE:
      flash_swap(&request_address->common_boot_req.swap_images_req);
      break;
    case ERASE_PAGE:
      flash_erase_page(&request_address->common_boot_req.erase_page_req);
      break;
    case WRITE_PAGE:
      flash_write_page(&request_address->common_boot_req.write_page_req);
      break;
    default:
      break;
  }
}

/**************************************************************************//**
\brief Fill flash buffer

\param[in] param - pointer to request parameters for page buffer filling.
******************************************************************************/
static void flash_fill_page_buffer(fill_page_buffer_req_t *param)
{
  uint16_t addr;

  for (addr = param->start_offset; addr < param->length + param->start_offset; addr += sizeof(uint16_t), param->data += sizeof(uint16_t))
  {
    // Perform word load.
    SP_LoadFlashWord(addr, *(uint16_t *)param->data);
    // Wait for SPM to finish.
    flashWaitForSPM();
  }
}

/**************************************************************************//**
\brief Store data from flash buffer to flash page

\param[in] param - pointer to request parameters for page programing.
******************************************************************************/
static void flash_program_page(program_page_req_t *param)
{
  ENTER_CRITICAL_REGION();

  EEPROM_BUSY_WAIT();
  // Perform page erase.
  SP_EraseApplicationPage(param->page_start_addr);
  // Wait for SPM to finish.
  flashWaitForSPM();
  // Perform page write.
  SP_WriteApplicationPage(param->page_start_addr);
  // Wait for SPM to finish.
  flashWaitForSPM();
  // enable rww section
  SP_RWWSectionEnable();
  // Wait for SPM to finish.
  flashWaitForSPM();

  LEAVE_CRITICAL_REGION();
}

/**************************************************************************//**
\brief Swap flash data from additional area to general area.

\param[in] param - pointer to request parameters for image swapping.
******************************************************************************/
static void flash_swap(swap_images_req_t *param)
{
  static uint8_t flash_page[FLASH_PAGE_SIZE];
  uint16_t total_num_pages;
  uint16_t page_number;
  uint16_t byte_addr;
  common_boot_req_t common_boot_req;
  uint32_t page_start_addr;

  cli();  // Disable global interrupts

  EEPROM_BUSY_WAIT();

  /* Move image to current application code flash pages */
  total_num_pages = (param->image_size + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;
  for (page_number = 0; page_number < total_num_pages; page_number++)
  {
    /* Read image data to flash_page variable */
    page_start_addr = param->image_start_addr + ((uint32_t)FLASH_PAGE_SIZE * page_number);
    for (byte_addr = 0; byte_addr < FLASH_PAGE_SIZE; byte_addr++)
      flash_page[byte_addr] = SP_ReadInternalFlash(page_start_addr++);

    /* Fill temporary page buffer */
    common_boot_req.fill_page_buffer_req.start_offset = 0;
    common_boot_req.fill_page_buffer_req.length = FLASH_PAGE_SIZE;
    common_boot_req.fill_page_buffer_req.data = flash_page;
    flash_fill_page_buffer(&common_boot_req.fill_page_buffer_req);

    /* Store data from flash buffer to flash page */
    common_boot_req.program_page_req.page_start_addr = (uint32_t)FLASH_PAGE_SIZE * page_number;
    flash_program_page(&common_boot_req.program_page_req);
  }

  /* Start wdt on 16 ms and reset mcu */
  wdt_reset();
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  WDTCSR = (1<<WDE);
  while(1);
}

static void flash_erase_page(erase_page_req_t *param)
{
    ENTER_CRITICAL_REGION();

    EEPROM_BUSY_WAIT();

    SP_EraseApplicationPage(param->page_start_addr);
    flashWaitForSPM();  // Wait until the memory is erased.

    // Reenable RWW-section again. We need this if we want to jump back
    // to the application after bootloading.
    SP_RWWSectionEnable();
    flashWaitForSPM();

    LEAVE_CRITICAL_REGION();
}

static void flash_write_page(write_page_req_t *param)
{
    ENTER_CRITICAL_REGION();

    EEPROM_BUSY_WAIT();

    SP_WriteApplicationPage(param->page_start_addr);  // Store buffer in flash page.
    flashWaitForSPM();  // Wait until the memory is written.

    // Reenable RWW-section again. We need this if we want to jump back
    // to the application after bootloading.
    SP_RWWSectionEnable();
    flashWaitForSPM();

    LEAVE_CRITICAL_REGION();
}

// eof rf4ceFlash.c
