
#ifndef __I2C_UTILS_H__
#define __I2C_UTILS_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <systemd/sd-journal.h>

typedef unsigned char u8;

extern const char *i2c_fname;
extern const u8 i2c_slave_address;

extern const u8 I2C_ADDR_ID;
extern const u8 I2C_ADDR_CHARGE_STATUS;
extern const u8 I2C_ADDR_COMM;
extern const u8 I2C_ADDR_VBUS_VOLTAGE;
extern const u8 I2C_ADDR_VBUS_CURRENT;
extern const u8 I2C_ADDR_VBUS_POWER;
extern const u8 I2C_ADDR_BATTERY_VOLTAGE;
extern const u8 I2C_ADDR_BATTERY_CURRENT;
extern const u8 I2C_ADDR_BATTERY_PERCENT;
extern const u8 I2C_ADDR_BATTERY_REMAINING_CAPACITY;
extern const u8 I2C_ADDR_BATTERY_REMAINING_DISCHARGE_TIME;
extern const u8 I2C_ADDR_BATTERY_REMAINING_CHARGE_TIME;
extern const u8 I2C_ADDR_BATTERY_CELL1_VOLTAGE;
extern const u8 I2C_ADDR_BATTERY_CELL2_VOLTAGE;
extern const u8 I2C_ADDR_BATTERY_CELL3_VOLTAGE;
extern const u8 I2C_ADDR_BATTERY_CELL4_VOLTAGE;
//
extern const u8 I2C_MASK_CHARGE;
extern const u8 I2C_MASK_FAST_CHARGE;
extern const u8 I2C_MASK_VBUS_POWERED;
//
extern const u8 I2C_MASK_STATE;
//
extern const u8 I2C_MASK_STANDBY;
extern const u8 I2C_MASK_TRICKLE_CHARGE;
extern const u8 I2C_MASK_CONSTANT_CURRENT;
extern const u8 I2C_MASK_CONSTANT_VOLTAGE;
extern const u8 I2C_MASK_CHARGE_PENDING;
extern const u8 I2C_MASK_CHARGE_FULL;
extern const u8 I2C_MASK_CHARGE_TIMEOUT;

int i2c_init(int *ptr_fd);
int i2c_close(int *ptr_fd);
int i2c_write(int i2c_fd, u8 slave_addr, u8 reg, u8 data);
int i2c_read(int i2c_fd, u8 slave_addr, u8 reg, u8 *result);
int i2c_read_reg(u8 slave_addr);

#endif
