/*
 * etx_ota_update.c
 *
 *  Created on: 26-Jul-2021
 *      Author: EmbeTronicX
 */

#include <stdio.h>
#include "etx_ota_update.hpp"
#include "main.h"
#include <string.h>
#include <stdbool.h>
#include "pal.hpp"
#include <cstdint>

/* Buffer to hold the received data */
static uint8_t Rx_Buffer[ ETX_OTA_PACKET_MAX_SIZE ];

/* OTA State */
static ETX_OTA_STATE_ ota_state = ETX_OTA_STATE_IDLE;

/* Firmware Total Size that we are going to receive */
static uint32_t ota_fw_total_size;
/* Firmware image's CRC32 */
static uint32_t ota_fw_crc;
/* Firmware Size that we have received */
static uint32_t ota_fw_received_size;

static uint16_t etx_receive_chunk( uint8_t *buf, uint16_t max_len );
static ETX_OTA_EX_ etx_process_data( uint8_t *buf, uint16_t len );
static void etx_ota_send_resp( uint8_t type );
static HAL_StatusTypeDef write_data_to_flash_app( uint8_t *data,
                                        uint16_t data_len, bool is_full_image );

/**
  * @brief Download the application from UART and flash it.
  * @param None
  * @retval ETX_OTA_EX_
  */
ETX_OTA_EX_ etx_ota_download_and_flash( void )
{
  ETX_OTA_EX_ ret  = ETX_OTA_EX_OK;
  uint16_t    len;

  uartPrintf("Waiting for the OTA data...\r\n");

  /* Reset the variables */
  ota_fw_total_size    = 0u;
  ota_fw_received_size = 0u;
  ota_fw_crc           = 0u;
  ota_state            = ETX_OTA_STATE_START;

  do
  {
    //clear the buffer
    memset( Rx_Buffer, 0, ETX_OTA_PACKET_MAX_SIZE );

    len = etx_receive_chunk( Rx_Buffer, ETX_OTA_PACKET_MAX_SIZE );
    uartPrintf("Received: lenght %d\r\n", len);

    if( len != 0u )
    {
      ret = etx_process_data( Rx_Buffer, len );
      uartPrintf("Received: ret %d\r\n", ret);
    }
    else
    {
      //didn't received data. break.
      ret = ETX_OTA_EX_ERR;
      uartPrintf("Received:not \r\n");
    }

    //Send ACK or NACK
    if( ret != ETX_OTA_EX_OK )
    {
      uartPrintf("Sending NACK\r\n");
      etx_ota_send_resp( ETX_OTA_NACK );
      break;
    }
    else
    {
      //uartPrintf("Sending ACK\r\n");
      etx_ota_send_resp( ETX_OTA_ACK );
    }

  }while( ota_state != ETX_OTA_STATE_IDLE );

  return ret;
}

/**
  * @brief Process the received data from UART4.
  * @param buf buffer to store the received data
  * @param max_len maximum length to receive
  * @retval ETX_OTA_EX_
  */
static ETX_OTA_EX_ etx_process_data( uint8_t *buf, uint16_t len )
{
  ETX_OTA_EX_ ret = ETX_OTA_EX_ERR;

  do
  {
    if( ( buf == NULL ) || ( len == 0u) )
    {
      break;
    }

    //Check we received OTA Abort command
    ETX_OTA_COMMAND_ *cmd = (ETX_OTA_COMMAND_*)buf;
    if( cmd->packet_type == ETX_OTA_PACKET_TYPE_CMD )
    {
      if( cmd->cmd == ETX_OTA_CMD_ABORT )
      {
        //received OTA Abort command. Stop the process
        break;
      }
    }

    switch( ota_state )
    {
      case ETX_OTA_STATE_IDLE:
      {
        uartPrintf("ETX_OTA_STATE_IDLE...\r\n");
        ret = ETX_OTA_EX_OK;
      }
      break;

      case ETX_OTA_STATE_START:
      {
        ETX_OTA_COMMAND_ *cmd = (ETX_OTA_COMMAND_*)buf;

        if( cmd->packet_type == ETX_OTA_PACKET_TYPE_CMD )
        {
          if( cmd->cmd == ETX_OTA_CMD_START )
          {
            uartPrintf("Received OTA START Command\r\n");
            ota_state = ETX_OTA_STATE_HEADER;
            ret = ETX_OTA_EX_OK;
          }
        }
      }
      break;

      case ETX_OTA_STATE_HEADER:
      {
        ETX_OTA_HEADER_ *header = (ETX_OTA_HEADER_*)buf;
        if( header->packet_type == ETX_OTA_PACKET_TYPE_HEADER )
        {
          ota_fw_total_size = header->meta_data.package_size;
          ota_fw_crc        = header->meta_data.package_crc;
          uartPrintf("Received OTA Header. FW Size = %ld\r\n", ota_fw_total_size);
          ota_state = ETX_OTA_STATE_DATA;
          ret = ETX_OTA_EX_OK;
        }
      }
      break;

      case ETX_OTA_STATE_DATA:
      {
        ETX_OTA_DATA_     *data     = (ETX_OTA_DATA_*)buf;
        uint16_t          data_len = data->data_len;
        HAL_StatusTypeDef ex;

        if( data->packet_type == ETX_OTA_PACKET_TYPE_DATA )
        {
          /* write the chunk to the Flash (App location) */
          ex = write_data_to_flash_app( buf+4, data_len, ( ota_fw_received_size == 0) );

          if( ex == HAL_OK )
          {
            uartPrintf("[%ld/%ld]\r\n", ota_fw_received_size/ETX_OTA_DATA_MAX_SIZE, ota_fw_total_size/ETX_OTA_DATA_MAX_SIZE);
            if( ota_fw_received_size >= ota_fw_total_size )
            {
              //received the full data. So, move to end
              ota_state = ETX_OTA_STATE_END;
            }
            ret = ETX_OTA_EX_OK;
          }
        }
      }
      break;

      case ETX_OTA_STATE_END:
      {

        ETX_OTA_COMMAND_ *cmd = (ETX_OTA_COMMAND_*)buf;

        if( cmd->packet_type == ETX_OTA_PACKET_TYPE_CMD )
        {
          if( cmd->cmd == ETX_OTA_CMD_END )
          {
            uartPrintf("Received OTA END Command\r\n");

            //TODO: Very full package CRC

            ota_state = ETX_OTA_STATE_IDLE;
            ret = ETX_OTA_EX_OK;
          }
        }
      }
      break;

      default:
      {
        /* Should not come here */
        ret = ETX_OTA_EX_ERR;
      }
      break;
    };
  }while( false );

  return ret;
}

/**
  * @brief Receive a one chunk of data.
  * @param buf buffer to store the received data
  * @param max_len maximum length to receive
  * @retval ETX_OTA_EX_
  */
static uint16_t etx_receive_chunk( uint8_t *buf, uint16_t max_len )
{
  int16_t  ret;
  uint16_t index     = 0u;
  uint16_t data_len;

  do
  {
    //receive SOF byte (1byte)
    // ret = HAL_UART_Receive( &huart2, &buf[index], 1, HAL_MAX_DELAY );
    ret =PAL::uartReceiveBlocking(UartInst::Uart2, &buf[index], 1);
    if( ret != HAL_OK )
    {
      uartPrintf("1 ret %d\r\n", ret);
      break;
    }

    if( buf[index++] != ETX_OTA_SOF )
    {
      //Not received start of frame
      ret = ETX_OTA_EX_ERR;
      break;
    }

    //Receive the packet type (1byte).
    // ret = HAL_UART_Receive( &huart2, &buf[index++], 1, HAL_MAX_DELAY );
    ret =PAL::uartReceiveBlocking(UartInst::Uart2,  &buf[index++], 1);
    if( ret != HAL_OK )
    {
      break;
    }

    //Get the data length (2bytes).
    // ret = HAL_UART_Receive( &huart2, &buf[index], 2, HAL_MAX_DELAY );
    ret =PAL::uartReceiveBlocking(UartInst::Uart2,  &buf[index], 2);
    if( ret != HAL_OK )
    {
      break;
    }
    data_len = *(uint16_t *)&buf[index];
    index += 2u;

    for( uint16_t i = 0u; i < data_len; i++ )
    {
      // ret = HAL_UART_Receive( &huart2, &buf[index++], 1, HAL_MAX_DELAY );
      ret =PAL::uartReceiveBlocking(UartInst::Uart2,  &buf[index++], 1);
      if( ret != HAL_OK )
      {
        break;
      }
    }

    //Get the CRC.
    // ret = HAL_UART_Receive( &huart2, &buf[index], 4, HAL_MAX_DELAY );
    ret =PAL::uartReceiveBlocking(UartInst::Uart2,  &buf[index], 4);
    if( ret != HAL_OK )
    {
      break;
    }
    index += 4u;

    //TODO: Add CRC verification

    //receive EOF byte (1byte)
    // ret = HAL_UART_Receive( &huart2, &buf[index], 1, HAL_MAX_DELAY );
    ret =PAL::uartReceiveBlocking(UartInst::Uart2,  &buf[index], 1);
    if( ret != HAL_OK )
    {
      break;
    }

    if( buf[index++] != ETX_OTA_EOF )
    {
      //Not received end of frame
      ret = ETX_OTA_EX_ERR;
      break;
    }
  }while( false );

  if( ret != HAL_OK )
  {
    //clear the index if error
    index = 0u;
  }

  if( max_len < index )
  {
    uartPrintf("Received more data than expected. Expected = %d, Received = %d\r\n",
                                                              max_len, index );
    index = 0u;
  }

  return index;
}

/**
  * @brief Send the response.
  * @param type ACK or NACK
  * @retval none
  */
static void etx_ota_send_resp( uint8_t type )
{
  ETX_OTA_RESP_ rsp =
  {
    .sof         = ETX_OTA_SOF,
    .packet_type = ETX_OTA_PACKET_TYPE_RESPONSE,
    .data_len    = 1u,
    .status      = type,
    .crc         = 0u,                //TODO: Add CRC
    .eof         = ETX_OTA_EOF
  };

  //send response
  // HAL_UART_Transmit(&huart2, (uint8_t *)&rsp, sizeof(ETX_OTA_RESP_), HAL_MAX_DELAY);
      PAL::uartSendBlocking(UartInst::Uart2,
                          (uint8_t *)&rsp,
                          sizeof(ETX_OTA_RESP_));
}

/**
  * @brief Write data to the Application's actual flash location.
  * @param data data to be written
  * @param data_len data length
  * @is_first_block true - if this is first block, false - not first block
  * @retval HAL_StatusTypeDef
  */

static HAL_StatusTypeDef write_data_to_flash_app(uint8_t *data,
                                                 uint16_t data_len,
                                                 bool is_first_block)
{
    HAL_StatusTypeDef ret = HAL_OK;

    do
    {
        if (is_first_block)
        {
            uartPrintf("Erasing the Flash memory...\r\n");

            FLASH_EraseInitTypeDef EraseInitStruct;
            uint32_t SectorError;

            ret = HAL_FLASH_Unlock();
            if (ret != HAL_OK) break;

            // Configure erase (adjust as needed for your app location)
            EraseInitStruct.TypeErase    = FLASH_TYPEERASE_SECTORS;
            EraseInitStruct.Sector       = FLASH_SECTOR_5;  // start of app
            EraseInitStruct.NbSectors    = 2;               // adjust if more needed
            EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;

            ret = HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);
            if (ret != HAL_OK) break;

            ota_fw_received_size = 0;
        }

        // Write data in 32-bit WORDs
        for (uint32_t i = 0; i < data_len; i += 4)
        {
            uint32_t word = 0xFFFFFFFF;

            // Copy up to 4 bytes safely
            memcpy(&word, &data[i], (data_len - i) >= 4 ? 4 : (data_len - i));

            ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                                    (ETX_APP_FLASH_ADDR + ota_fw_received_size),
                                    word);
            if (ret != HAL_OK)
            {
                uartPrintf("Flash Write Error at 0x%08lX\r\n",
                           (ETX_APP_FLASH_ADDR + ota_fw_received_size));
                break;
            }

            ota_fw_received_size += 4;
        }

        if (ret != HAL_OK) break;

        // ⚠️ Keep flash unlocked during the whole OTA
        // If you want to lock after each chunk (slower, safer):
        ret = HAL_FLASH_Lock();

    } while (false);

    return ret;
}


#if 0
static HAL_StatusTypeDef write_data_to_flash_app( uint8_t *data,
                                        uint16_t data_len, bool is_first_block )
{
  HAL_StatusTypeDef ret;

  do
  {
    ret = HAL_FLASH_Unlock();
    if( ret != HAL_OK )
    {
      break;
    }
#if 0
    //No need to erase every time. Erase only the first time.
    if( is_first_block )
    {

      uartPrintf("Erasing the Flash memory...\r\n");
      //Erase the Flash
      FLASH_EraseInitTypeDef EraseInitStruct;
      uint32_t SectorError;

      EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
      EraseInitStruct.Sector        = FLASH_SECTOR_5;
      EraseInitStruct.NbSectors     = 2;                    //erase 2 sectors(5,6)
      EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;

      ret = HAL_FLASHEx_Erase( &EraseInitStruct, &SectorError );
      if( ret != HAL_OK )
      {
        break;
      }
    }
#endif
    for(int i = 0; i < data_len; i++ )
    {
      ret = HAL_FLASH_Program( FLASH_TYPEPROGRAM_BYTE,
                               (ETX_APP_FLASH_ADDR + ota_fw_received_size),
                               data[i]
                             );
      if( ret == HAL_OK )
      {
        //update the data count
        ota_fw_received_size += 1;
      }
      else
      {
        uartPrintf("Flash Write Error\r\n");
        break;
      }
    }

    if( ret != HAL_OK )
    {
      break;
    }

    ret = HAL_FLASH_Lock();
    if( ret != HAL_OK )
    {
      break;
    }
  }while( false );

  return ret;
}
#endif

uint32_t ota_flag_read(void)
{
    return *(uint32_t*)OTA_FLAG_ADDR;
}

void ota_flag_write(uint32_t value)
{
    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SectorError = 0;

    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.Sector = FLASH_SECTOR_7;   // Last sector for STM32F446
    EraseInitStruct.NbSectors = 1;
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
    {
        HAL_FLASH_Lock();
        // Error_Handler();   // handle erase error
    }

    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, OTA_FLAG_ADDR, value) != HAL_OK)
    {
        HAL_FLASH_Lock();
        // Error_Handler();   // handle program error
    }

    HAL_FLASH_Lock();
}

