
/*
 *
 * https://gist.github.com/JamesDunne/9b7fbedb74c22ccc833059623f47beb7
 *
 * 15-Apr-2025
 *
 */

#include "i2c_utils.h"

const char *i2c_fname = "/dev/i2c-1";
const u8 i2c_slave_address = 0x2D;

const u8 I2C_ADDR_ID = 0x00;
const u8 I2C_ADDR_CHARGE_STATUS = 0x02;
const u8 I2C_ADDR_COMM = 0x03;
const u8 I2C_ADDR_VBUS_VOLTAGE = 0x10;
const u8 I2C_ADDR_VBUS_CURRENT = 0x12;
const u8 I2C_ADDR_VBUS_POWER = 0x14;
const u8 I2C_ADDR_BATTERY_VOLTAGE = 0x20;
const u8 I2C_ADDR_BATTERY_CURRENT = 0x22;
const u8 I2C_ADDR_BATTERY_PERCENT = 0x24;
const u8 I2C_ADDR_BATTERY_REMAINING_CAPACITY = 0x26;
const u8 I2C_ADDR_BATTERY_REMAINING_DISCHARGE_TIME = 0x28;
const u8 I2C_ADDR_BATTERY_REMAINING_CHARGE_TIME = 0x2a;
const u8 I2C_ADDR_BATTERY_CELL1_VOLTAGE = 0x30;
const u8 I2C_ADDR_BATTERY_CELL2_VOLTAGE = 0x32;
const u8 I2C_ADDR_BATTERY_CELL3_VOLTAGE = 0x34;
const u8 I2C_ADDR_BATTERY_CELL4_VOLTAGE = 0x36;
//
const u8 I2C_MASK_CHARGE = 1 << 7;
const u8 I2C_MASK_FAST_CHARGE = 1 << 6;
const u8 I2C_MASK_VBUS_POWERED = 1 << 5;
//
const u8 I2C_MASK_STATE = 0b00000111;
//
const u8 I2C_MASK_STANDBY = 0b00000000;
const u8 I2C_MASK_TRICKLE_CHARGE = 0b00000001;
const u8 I2C_MASK_CONSTANT_CURRENT = 0b00000010;
const u8 I2C_MASK_CONSTANT_VOLTAGE = 0b00000011;
const u8 I2C_MASK_CHARGE_PENDING = 0b00000100;
const u8 I2C_MASK_CHARGE_FULL = 0b00000101;
const u8 I2C_MASK_CHARGE_TIMEOUT = 0b00000110;

int i2c_init(int *ptr_fd)
{
	*ptr_fd = -1;
	if ((*ptr_fd = open(i2c_fname, O_RDWR)) < 0)
	{
		sd_journal_perror("Error in i2c_init");
		return -1;
	}

	return 0;
}

int i2c_close(int *ptr_fd)
{
	close(*ptr_fd);
	return 0;
}

int i2c_write(int i2c_fd, u8 slave_addr, u8 reg, u8 data)
{
	int retval;
	u8 outbuf[2];

	struct i2c_msg msgs[1];
	struct i2c_rdwr_ioctl_data msgset[1];

	outbuf[0] = reg;
	outbuf[1] = data;

	msgs[0].addr = slave_addr;
	msgs[0].flags = 0;
	msgs[0].len = 2;
	msgs[0].buf = outbuf;

	msgset[0].msgs = msgs;
	msgset[0].nmsgs = 1;

	if (ioctl(i2c_fd, I2C_RDWR, &msgset) < 0)
	{
		sd_journal_perror("ioctl(I2C_RDWR) in i2c_write");
		return -1;
	}

	return 0;
}

int i2c_read(int i2c_fd, u8 slave_addr, u8 reg, u8 *result)
{
	int retval;
	u8 outbuf[1], inbuf[1];
	struct i2c_msg msgs[2];
	struct i2c_rdwr_ioctl_data msgset[1];

	msgs[0].addr = slave_addr;
	msgs[0].flags = 0;
	msgs[0].len = 1;
	msgs[0].buf = outbuf;

	msgs[1].addr = slave_addr;
	msgs[1].flags = I2C_M_RD | I2C_M_NOSTART;
	msgs[1].len = 1;
	msgs[1].buf = inbuf;

	msgset[0].msgs = msgs;
	msgset[0].nmsgs = 2;

	outbuf[0] = reg;

	inbuf[0] = 0;

	*result = 0;
	if (ioctl(i2c_fd, I2C_RDWR, &msgset) < 0)
	{
		sd_journal_perror("ioctl(I2C_RDWR) in i2c_read");
		return -1;
	}

	*result = inbuf[0];
	return 0;
}

int i2c_read_reg(u8 read_addr)
{
	int i2c_fd = -1;
	u8 res1, res2;

	i2c_init(&i2c_fd);
	i2c_read(i2c_fd, i2c_slave_address, read_addr, &res1);
	i2c_read(i2c_fd, i2c_slave_address, read_addr + 1, &res2);
	i2c_close(&i2c_fd);

	return res1 | res2 << 8;
}
