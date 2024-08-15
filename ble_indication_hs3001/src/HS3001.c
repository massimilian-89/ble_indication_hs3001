/**
 ****************************************************************************************
 *
 * @file HS3001.c
 *
 * @brief HS3001 module.
 *
 * Copyright (C) 2012-2023 Renesas Electronics Corporation and/or its affiliates
 * The MIT License (MIT)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */
#include "HS3001.h"

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

void HS3001_wakeup(void) {
    uint8_t df_command = DF_COMMAND;
    i2c_abort_t abort_code = I2C_ABORT_NONE;
    i2c_master_transmit_buffer_sync(&df_command, 1, &abort_code, I2C_F_ADD_STOP);

    for (uint32_t i = 0; i < 150000; i++) // Wait for about 45 ms
    {
        __NOP();
    }

    if (abort_code != I2C_ABORT_NONE) {
        ASSERT_ERROR(0);
    }
}

void Data_Fetch_all(uint8_t* buffer) {
    HS3001_wakeup();
    i2c_abort_t abort_code = I2C_ABORT_NONE;
    i2c_master_receive_buffer_sync(buffer, 4, &abort_code, I2C_F_ADD_STOP);

    if (abort_code != I2C_ABORT_NONE) {
        ASSERT_ERROR(0);
    }
}

int32_t HS3001_get_temperature(uint8_t* buffer) {
    int32_t temperature_raw;
    int32_t temperature;
    Data_Fetch_all(buffer);

    // Combine the bytes to form the raw temperature value
    temperature_raw = (int32_t)(((buffer[2] << 8) | (buffer[3] & 0xFC)) >> 2);

    // Convert the raw value to temperature in degree Celsius using integer arithmetic
    temperature = (16500 * temperature_raw) / 16383 - 4000;

    // Print the temperature (note: temperature is multiplied by 100 to avoid floating-point arithmetic)
    arch_printf("Temperature is :%d.%02d \n\r", temperature / 100, temperature % 100);

    return temperature;
}

int32_t HS3001_get_humidity(uint8_t* buffer) {
    int32_t humidity_raw;
    int32_t humidity;
    Data_Fetch_all(buffer);

    // Combine the bytes to form the raw humidity value
    humidity_raw = (int32_t)(((buffer[0] & 0x3F) << 8) | buffer[1]);

    // Convert the raw value to relative humidity in percentage using integer arithmetic
    humidity = (10000 * humidity_raw) / 16383;

    // Print the humidity (note: humidity is multiplied by 100 to avoid floating-point arithmetic)
    arch_printf("Humidity is :%d.%02d \n\r", humidity / 100, humidity % 100);

    return humidity;
}
