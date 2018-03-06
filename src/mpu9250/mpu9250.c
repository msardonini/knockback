/*
 * Copyright (C) 2015 Eistec AB
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lis3dh
 * @{
 *
 * @file
 * @brief       Implementation of LIS3DH SPI driver
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "mpu9250.h"
#include "spi.h"

// static inline int lis3dh_write_bits(const uint8_t reg,
//                                     const uint8_t mask,  const uint8_t values);
static int mpu9250_write_reg(const uint8_t reg, const uint8_t value);

static int lis3dh_read_regs(const uint8_t reg,
                            const uint8_t len, uint8_t *buf);


int16_t scale_factor = 8;


int mpu9250_init()
{
    uint8_t test = 0x00;

    /* test connection to the device */
    lis3dh_read_regs(WHO_AM_I_MPU9250, 1, &test);
    if (test != 0x73) {
        /* chip is not responding correctly */
        printf("[lis3dh] error reading the who am i reg [0x%02x]\n", (int)test);
        return -1;
    }


    mpu9250_write_reg(ACCEL_CONFIG_2, 0x01);
    mpu9250_write_reg(SMPLRT_DIV, 0x00);

    return 0;

}

int read_accel_data(int16_t data[3])
{
    uint8_t buf[6];
    lis3dh_read_regs(ACCEL_XOUT_H, 6, buf);

    data[0] = (int16_t)(((uint16_t)buf[0]<<8)|buf[1]);
    data[1] = (int16_t)(((uint16_t)buf[2]<<8)|buf[3]);
    data[2] = (int16_t)(((uint16_t)buf[4]<<8)|buf[5]);

    return 0;
}

/**
 * @brief Read sequential registers from the LIS3DH.
 *
 * @param[in]  reg          The source register starting address
 * @param[in]  len          Number of bytes to read
 * @param[out] buf          The values of the source registers will be written
 *                          here
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
static int lis3dh_read_regs(const uint8_t reg, const uint8_t len, uint8_t *buf)
{
    /* Set READ MULTIPLE mode */
    uint8_t addr = reg | BIT_SPI_READ;

    /* Perform the transaction */
    spi_transfer_regs(addr, NULL, buf, (size_t)len);


    return 0;
}

/**
 * @brief Write a value to an 8 bit register in the mpu9250.
 *
 * @param[in]  reg          The target register.
 * @param[in]  value        The value to write.
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
static int mpu9250_write_reg(const uint8_t reg, const uint8_t value)
{
    /* Set WRITE SINGLE mode */
    uint8_t addr = (reg & 0x7F);


    /* Perform the transaction */
    spi_transfer_reg(addr, value);


    return 0;
}

// *
//  * @brief Write (both set and clear) bits of an 8-bit register on the LIS3DH.
//  *
//  * @param[in]  addr         Register address on the LIS3DH.
//  * @param[in]  mask         Bitmask for the bits to modify.
//  * @param[in]  values       The values to write to the masked bits.
//  *
//  * @return                  0 on success
//  * @return                  -1 on error
 
// static inline int lis3dh_write_bits(const uint8_t reg, const uint8_t mask, const uint8_t values)
// {
//     uint8_t tmp;

//     if (lis3dh_read_regs(reg, 1, &tmp) < 0) {
//         /* Communication error */
//         return -1;
//     }

//     tmp &= ~mask;
//     tmp |= (values & mask);

//     if (lis3dh_write_reg(reg, tmp) < 0) {
//         /* Communication error */
//         return -1;
//     }

//     return 0;
// }

// /** @} */
