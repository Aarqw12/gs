/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * s2mpg14-register.h
 *
 * Copyright (C) 2022 Samsung Electronics
 *
 * header file including register information of s2mpg14
 */

#ifndef __LINUX_MFD_S2MPG14_REGISTER_H
#define __LINUX_MFD_S2MPG14_REGISTER_H

#include <linux/i2c.h>
#include "s2mpg1415-register.h"

#define S2MPG14_REG_INVALID	(0xFF)

enum S2MPG14_pmic_rev {
	S2MPG14_EVT0,
};

#define I2C_ADDR_TOP 0x00
#define I2C_ADDR_PMIC 0x01
#define I2C_ADDR_RTC 0x02
#define I2C_ADDR_METER 0x0A
#define I2C_ADDR_WLWP 0x0B
#define I2C_ADDR_GPIO 0x0C
#define I2C_ADDR_MT_TRIM 0x0D
#define I2C_ADDR_PM_TRIM2 0x0E
#define I2C_ADDR_PM_TRIM1 0x0F

/* Common(0x0) Registers */
enum S2MPG14_COMMON_REG {
	S2MPG14_COMMON_VGPIO0 = 0x0,
	S2MPG14_COMMON_VGPIO1 = 0x1,
	S2MPG14_COMMON_VGPIO2 = 0x2,
	S2MPG14_COMMON_VGPIO3 = 0x3,
	S2MPG14_COMMON_I3C_DAA = 0x4,
	S2MPG14_COMMON_IBI0 = 0x5,
	S2MPG14_COMMON_IBI1 = 0x6,
	S2MPG14_COMMON_IBI2 = 0x7,
	S2MPG14_COMMON_IBI3 = 0x8,
	S2MPG14_COMMON_CHIPID = 0xB,
	S2MPG14_COMMON_I3C_CFG1 = 0xC,
	S2MPG14_COMMON_I3C_CFG2 = 0xD,
	S2MPG14_COMMON_I3C_STA = 0xE,
	S2MPG14_COMMON_IBIM1 = 0xF,
	S2MPG14_COMMON_IBIM2 = 0x10,
};

/* PM(0x1) Registers */
enum S2MPG14_PM_REG {
	S2MPG14_PM_INT1 = 0x0,
	S2MPG14_PM_INT2 = 0x1,
	S2MPG14_PM_INT3 = 0x2,
	S2MPG14_PM_INT4 = 0x3,
	S2MPG14_PM_INT5 = 0x4,
	S2MPG14_PM_INT1M = 0x5,
	S2MPG14_PM_INT2M = 0x6,
	S2MPG14_PM_INT3M = 0x7,
	S2MPG14_PM_INT4M = 0x8,
	S2MPG14_PM_INT5M = 0x9,
	S2MPG14_PM_STATUS1 = 0xA,
	S2MPG14_PM_STATUS2 = 0xB,
	S2MPG14_PM_PWRONSRC = 0xC,
	S2MPG14_PM_OFFSRC1 = 0xD,
	S2MPG14_PM_OFFSRC2 = 0xE,
	S2MPG14_PM_BUCHG = 0xF,
	S2MPG14_PM_RTCBUF = 0x10,
	S2MPG14_PM_CTRL1 = 0x11,
	S2MPG14_PM_CTRL2 = 0x12,
	S2MPG14_PM_CTRL3 = 0x13,
	S2MPG14_PM_CTRL4 = 0x14,
	S2MPG14_PM_CTRL5 = 0x15,
	S2MPG14_PM_SMPL_WARN_CTRL = 0x16,
	S2MPG14_PM_B1M_CTRL = 0x17,
	S2MPG14_PM_B1M_OUT1 = 0x18,
	S2MPG14_PM_B2M_CTRL = 0x19,
	S2MPG14_PM_B2M_OUT1 = 0x1A,
	S2MPG14_PM_B3M_CTRL = 0x1B,
	S2MPG14_PM_B3M_OUT1 = 0x1C,
	S2MPG14_PM_B4M_CTRL = 0x1D,
	S2MPG14_PM_B4M_OUT1 = 0x1E,
	S2MPG14_PM_B5M_CTRL = 0x1F,
	S2MPG14_PM_B5M_OUT1 = 0x20,
	S2MPG14_PM_B6M_CTRL = 0x21,
	S2MPG14_PM_B6M_OUT1 = 0x22,
	S2MPG14_PM_B7M_CTRL = 0x23,
	S2MPG14_PM_B7M_OUT0 = 0x24,
	S2MPG14_PM_B7M_OUT3 = 0x25,
	S2MPG14_PM_B8M_CTRL = 0x26,
	S2MPG14_PM_B8M_OUT1 = 0x27,
	S2MPG14_PM_B9M_CTRL = 0x28,
	S2MPG14_PM_B9M_OUT1 = 0x29,
	S2MPG14_PM_L1M_CTRL = 0x2A,
	S2MPG14_PM_L2M_CTRL = 0x2B,
	S2MPG14_PM_L3M_CTRL1 = 0x2C,
	S2MPG14_PM_L3M_CTRL2 = 0x2D,
	S2MPG14_PM_L4M_CTRL = 0x2E,
	S2MPG14_PM_L5M_CTRL = 0x2F,
	S2MPG14_PM_L6M_CTRL = 0x30,
	S2MPG14_PM_L7M_CTRL = 0x31,
	S2MPG14_PM_L8M_CTRL = 0x32,
	S2MPG14_PM_L9M_CTRL = 0x33,
	S2MPG14_PM_L10M_CTRL = 0x34,
	S2MPG14_PM_L11M_CTRL1 = 0x35,
	S2MPG14_PM_L12M_CTRL1 = 0x36,
	S2MPG14_PM_L13M_CTRL1 = 0x37,
	S2MPG14_PM_L14M_CTRL = 0x38,
	S2MPG14_PM_L15M_CTRL1 = 0x39,
	S2MPG14_PM_L16M_CTRL = 0x3A,
	S2MPG14_PM_L17M_CTRL1 = 0x3B,
	S2MPG14_PM_L18M_CTRL = 0x3C,
	S2MPG14_PM_L19M_CTRL = 0x3D,
	S2MPG14_PM_L20M_CTRL = 0x3E,
	S2MPG14_PM_L21M_CTRL = 0x3F,
	S2MPG14_PM_L22M_CTRL = 0x40,
	S2MPG14_PM_L23M_CTRL = 0x41,
	S2MPG14_PM_L24M_CTRL = 0x42,
	S2MPG14_PM_L25M_CTRL = 0x43,
	S2MPG14_PM_LDO_CTRL1 = 0x44,
	S2MPG14_PM_LDO_CTRL2 = 0x45,
	S2MPG14_PM_LDO_DSCH1 = 0x46,
	S2MPG14_PM_LDO_DSCH2 = 0x47,
	S2MPG14_PM_LDO_DSCH3 = 0x48,
	S2MPG14_PM_LDO_DSCH4 = 0x49,
	S2MPG14_PM_B9M_HLIMIT = 0x4A,
	S2MPG14_PM_L11M_HLIMIT = 0x4B,
	S2MPG14_PM_L12M_HLIMIT = 0x4C,
	S2MPG14_PM_L17M_HLIMIT = 0x4D,
	S2MPG14_PM_L21M_HLIMIT = 0x4E,
	S2MPG14_PM_L21M_LLIMIT = 0x4F,
	S2MPG14_PM_DVS_RAMP1 = 0x50,
	S2MPG14_PM_DVS_RAMP2 = 0x51,
	S2MPG14_PM_DVS_RAMP3 = 0x52,
	S2MPG14_PM_DVS_RAMP4 = 0x53,
	S2MPG14_PM_DVS_RAMP5 = 0x54,
	S2MPG14_PM_DVS_RAMP6 = 0x55,
	S2MPG14_PM_DVS_RAMP7 = 0x56,
	S2MPG14_PM_DVS_SYNC_CTRL1 = 0x57,
	S2MPG14_PM_DVS_SYNC_CTRL2 = 0x58,
	S2MPG14_PM_DVS_SYNC_CTRL3 = 0x59,
	S2MPG14_PM_DVS_SYNC_CTRL4 = 0x5A,
	S2MPG14_PM_DVS_SYNC_CTRL5 = 0x5B,
	S2MPG14_PM_DVS_SYNC_CTRL6 = 0x5C,
	S2MPG14_PM_DVS_OPTION1 = 0x5D,
	S2MPG14_PM_OFF_CTRL1 = 0x5E,
	S2MPG14_PM_OFF_CTRL2 = 0x5F,
	S2MPG14_PM_OFF_CTRL3 = 0x60,
	S2MPG14_PM_OFF_CTRL4 = 0x61,
	S2MPG14_PM_OFF_CTRL5 = 0x62,
	S2MPG14_PM_OFF_CTRL6 = 0x63,
	S2MPG14_PM_OFF_CTRL7 = 0x64,
	S2MPG14_PM_OFF_CTRL8 = 0x65,
	S2MPG14_PM_OFF_CTRL9 = 0x66,
	S2MPG14_PM_OFF_CTRL10 = 0x67,
	S2MPG14_PM_OFF_CTRL11 = 0x68,
	S2MPG14_PM_OFF_CTRL12 = 0x69,
	/* ONSEQ1 ~ ONSEQ30 (0x6A ~ 0x87) */
	/* OFFSEQ1 ~ OFFSEQ15 (0x88 ~ 0x96) */
	S2MPG14_PM_PCTRLSEL1 = 0x97,
	S2MPG14_PM_PCTRLSEL2 = 0x98,
	S2MPG14_PM_PCTRLSEL3 = 0x99,
	S2MPG14_PM_PCTRLSEL4 = 0x9A,
	S2MPG14_PM_PCTRLSEL5 = 0x9B,
	S2MPG14_PM_PCTRLSEL6 = 0x9C,
	S2MPG14_PM_PCTRLSEL7 = 0x9D,
	S2MPG14_PM_PCTRLSEL8 = 0x9E,
	S2MPG14_PM_PCTRLSEL9 = 0x9F,
	S2MPG14_PM_PCTRLSEL10 = 0xA0,
	S2MPG14_PM_PCTRLSEL11 = 0xA1,
	S2MPG14_PM_DCTRLSEL1 = 0xA2,
	S2MPG14_PM_DCTRLSEL2 = 0xA3,
	S2MPG14_PM_DCTRLSEL3 = 0xA4,
	S2MPG14_PM_DCTRLSEL4 = 0xA5,
	S2MPG14_PM_DCTRLSEL5 = 0xA6,
	S2MPG14_PM_DCTRLSEL6 = 0xA7,
	S2MPG14_PM_DCTRLSEL7 = 0xA8,
	S2MPG14_PM_BUCK_OCP_EN1 = 0xA9,
	S2MPG14_PM_BUCK_OCP_EN2 = 0xAA,
	S2MPG14_PM_BUCK_OCP_PD_EN1 = 0xAB,
	S2MPG14_PM_BUCK_OCP_PD_EN2 = 0xAC,
	S2MPG14_PM_BUCK_OCP_CTRL1 = 0xAD,
	S2MPG14_PM_BUCK_OCP_CTRL2 = 0xAE,
	S2MPG14_PM_BUCK_OCP_CTRL3 = 0xAF,
	S2MPG14_PM_BUCK_OCP_CTRL4 = 0xB0,
	S2MPG14_PM_BUCK_OCP_CTRL5 = 0xB1,
	S2MPG14_PM_PSI_CTRL1 = 0xB2,
	S2MPG14_PM_PSI_CTRL2 = 0xB3,
	S2MPG14_PM_PSI_CTRL3 = 0xB4,
	S2MPG14_PM_PSI_CTRL4 = 0xB5,
	S2MPG14_PM_SEL_HW_VGPIO = 0xB6,
	S2MPG14_PM_AON_CTRL = 0xB7,
	S2MPG14_PM_B2M_OCP_WARN_X = 0xB8,
	S2MPG14_PM_B3M_OCP_WARN_X = 0xB9,
	S2MPG14_PM_B7M_OCP_WARN_X = 0xBA,
	S2MPG14_PM_B2M_OCP_WARN = 0xBB,
	S2MPG14_PM_B3M_OCP_WARN = 0xBC,
	S2MPG14_PM_B7M_OCP_WARN = 0xBD,
	S2MPG14_PM_B2M_SOFT_OCP_WARN = 0xBE,
	S2MPG14_PM_B3M_SOFT_OCP_WARN = 0xBF,
	S2MPG14_PM_B7M_SOFT_OCP_WARN = 0xC0,
	S2MPG14_PM_OCP_WARN_DEBOUNCE = 0xC1,
	S2MPG14_PM_SOFT_OCP_WARN_DEBOUNCE = 0xC2,
	S2MPG14_PM_B1M_OUT2 = 0xC3,
	S2MPG14_PM_B2M_OUT2 = 0xC4,
	S2MPG14_PM_B3M_OUT2 = 0xC5,
	S2MPG14_PM_B4M_OUT2 = 0xC6,
	S2MPG14_PM_B5M_OUT2 = 0xC7,
	S2MPG14_PM_B6M_OUT2 = 0xC8,
	S2MPG14_PM_B7M_OUT1 = 0xC9,
	S2MPG14_PM_B7M_OUT2 = 0xCA,
	S2MPG14_PM_B7M_OUT4 = 0xCB,
	S2MPG14_PM_B8M_OUT2 = 0xCC,
	S2MPG14_PM_B9M_OUT2 = 0xCD,
	S2MPG14_PM_L11M_CTRL2 = 0xCE,
	S2MPG14_PM_L12M_CTRL2 = 0xCF,
	S2MPG14_PM_L13M_CTRL2 = 0xD0,
	S2MPG14_PM_L15M_CTRL2 = 0xD1,
	S2MPG14_PM_L17M_CTRL2 = 0xD2,
	S2MPG14_PM_BUCK_HRMODE1 = 0xD3,
	S2MPG14_PM_BUCK_HRMODE2 = 0xD4,
	S2MPG14_PM_LDO_SENSE1 = 0xD5,
	S2MPG14_PM_LDO_SENSE2 = 0xD6,
	S2MPG14_PM_LDO_SENSE3 = 0xD7,
	S2MPG14_PM_LDO_SENSE4 = 0xD8,
	S2MPG14_PM_B1M_USONIC = 0xD9,
	S2MPG14_PM_B2M_USONIC = 0xDA,
	S2MPG14_PM_B3M_USONIC = 0xDB,
	S2MPG14_PM_B4M_USONIC = 0xDC,
	S2MPG14_PM_B5M_USONIC = 0xDD,
	S2MPG14_PM_B6M_USONIC = 0xDE,
	S2MPG14_PM_B7M_USONIC = 0xDF,
	S2MPG14_PM_B8M_USONIC = 0xE0,
	S2MPG14_PM_B9M_USONIC = 0xE1,
	S2MPG14_PM_FAULTOUT_CTRL = 0xE2,
	S2MPG14_PM_INT_REG_SEL = 0xE3,
	S2MPG14_PM_SW_RESET = 0xE4,
};

/* Meter(0xA) Registers */
enum S2MPG14_METER_REG {
	S2MPG14_METER_INT1 = 0x0,
	S2MPG14_METER_INT2 = 0x1,
	S2MPG14_METER_INT1M = 0x4,
	S2MPG14_METER_INT2M = 0x5,
	S2MPG14_METER_CTRL1 = 0x8,
	S2MPG14_METER_CTRL2 = 0x9,
	S2MPG14_METER_CTRL4 = 0xB,
	S2MPG14_METER_CTRL5 = 0xC,
	S2MPG14_METER_CTRL6 = 0xD,
	S2MPG14_METER_CTRL7 = 0xE,
	S2MPG14_METER_BUCKEN1 = 0xF,
	S2MPG14_METER_BUCKEN2 = 0x10,
	S2MPG14_METER_MUXSEL0 = 0x11,
	S2MPG14_METER_MUXSEL1 = 0x12,
	S2MPG14_METER_MUXSEL2 = 0x13,
	S2MPG14_METER_MUXSEL3 = 0x14,
	S2MPG14_METER_MUXSEL4 = 0x15,
	S2MPG14_METER_MUXSEL5 = 0x16,
	S2MPG14_METER_MUXSEL6 = 0x17,
	S2MPG14_METER_MUXSEL7 = 0x18,
	S2MPG14_METER_MUXSEL8 = 0x19,
	S2MPG14_METER_MUXSEL9 = 0x1A,
	S2MPG14_METER_MUXSEL10 = 0x1B,
	S2MPG14_METER_MUXSEL11 = 0x1C,
	S2MPG14_METER_LPF_C0_0 = 0x1D,
	S2MPG14_METER_LPF_C0_1 = 0x1E,
	S2MPG14_METER_LPF_C0_2 = 0x1F,
	S2MPG14_METER_LPF_C0_3 = 0x20,
	S2MPG14_METER_LPF_C0_4 = 0x21,
	S2MPG14_METER_LPF_C0_5 = 0x22,
	S2MPG14_METER_LPF_C0_6 = 0x23,
	S2MPG14_METER_LPF_C0_7 = 0x24,
	S2MPG14_METER_LPF_C0_8 = 0x25,
	S2MPG14_METER_LPF_C0_9 = 0x26,
	S2MPG14_METER_LPF_C0_10 = 0x27,
	S2MPG14_METER_LPF_C0_11 = 0x28,
	S2MPG14_METER_PWR_WARN0 = 0x31,
	S2MPG14_METER_PWR_WARN1 = 0x32,
	S2MPG14_METER_PWR_WARN2 = 0x33,
	S2MPG14_METER_PWR_WARN3 = 0x34,
	S2MPG14_METER_PWR_WARN4 = 0x35,
	S2MPG14_METER_PWR_WARN5 = 0x36,
	S2MPG14_METER_PWR_WARN6 = 0x37,
	S2MPG14_METER_PWR_WARN7 = 0x38,
	S2MPG14_METER_PWR_WARN8 = 0x39,
	S2MPG14_METER_PWR_WARN9 = 0x3A,
	S2MPG14_METER_PWR_WARN10 = 0x3B,
	S2MPG14_METER_PWR_WARN11 = 0x3C,
	S2MPG14_METER_PWR_HYS1 = 0x55,
	S2MPG14_METER_PWR_HYS2 = 0x56,
	S2MPG14_METER_PWR_HYS3 = 0x57,
	S2MPG14_METER_PWR_HYS4 = 0x58,
	S2MPG14_METER_PWR_HYS5 = 0x59,
	S2MPG14_METER_PWR_HYS6 = 0x5A,
	S2MPG14_METER_ACC_DATA_CH0_1 = 0x63,
	S2MPG14_METER_ACC_DATA_CH0_2 = 0x64,
	S2MPG14_METER_ACC_DATA_CH0_3 = 0x65,
	S2MPG14_METER_ACC_DATA_CH0_4 = 0x66,
	S2MPG14_METER_ACC_DATA_CH0_5 = 0x67,
	S2MPG14_METER_ACC_DATA_CH0_6 = 0x68,
	S2MPG14_METER_ACC_DATA_CH1_1 = 0x69,
	S2MPG14_METER_ACC_DATA_CH1_2 = 0x6A,
	S2MPG14_METER_ACC_DATA_CH1_3 = 0x6B,
	S2MPG14_METER_ACC_DATA_CH1_4 = 0x6C,
	S2MPG14_METER_ACC_DATA_CH1_5 = 0x6D,
	S2MPG14_METER_ACC_DATA_CH1_6 = 0x6E,
	S2MPG14_METER_ACC_DATA_CH2_1 = 0x6F,
	S2MPG14_METER_ACC_DATA_CH2_2 = 0x70,
	S2MPG14_METER_ACC_DATA_CH2_3 = 0x71,
	S2MPG14_METER_ACC_DATA_CH2_4 = 0x72,
	S2MPG14_METER_ACC_DATA_CH2_5 = 0x73,
	S2MPG14_METER_ACC_DATA_CH2_6 = 0x74,
	S2MPG14_METER_ACC_DATA_CH3_1 = 0x75,
	S2MPG14_METER_ACC_DATA_CH3_2 = 0x76,
	S2MPG14_METER_ACC_DATA_CH3_3 = 0x77,
	S2MPG14_METER_ACC_DATA_CH3_4 = 0x78,
	S2MPG14_METER_ACC_DATA_CH3_5 = 0x79,
	S2MPG14_METER_ACC_DATA_CH3_6 = 0x7A,
	S2MPG14_METER_ACC_DATA_CH4_1 = 0x7B,
	S2MPG14_METER_ACC_DATA_CH4_2 = 0x7C,
	S2MPG14_METER_ACC_DATA_CH4_3 = 0x7D,
	S2MPG14_METER_ACC_DATA_CH4_4 = 0x7E,
	S2MPG14_METER_ACC_DATA_CH4_5 = 0x7F,
	S2MPG14_METER_ACC_DATA_CH4_6 = 0x80,
	S2MPG14_METER_ACC_DATA_CH5_1 = 0x81,
	S2MPG14_METER_ACC_DATA_CH5_2 = 0x82,
	S2MPG14_METER_ACC_DATA_CH5_3 = 0x83,
	S2MPG14_METER_ACC_DATA_CH5_4 = 0x84,
	S2MPG14_METER_ACC_DATA_CH5_5 = 0x85,
	S2MPG14_METER_ACC_DATA_CH5_6 = 0x86,
	S2MPG14_METER_ACC_DATA_CH6_1 = 0x87,
	S2MPG14_METER_ACC_DATA_CH6_2 = 0x88,
	S2MPG14_METER_ACC_DATA_CH6_3 = 0x89,
	S2MPG14_METER_ACC_DATA_CH6_4 = 0x8A,
	S2MPG14_METER_ACC_DATA_CH6_5 = 0x8B,
	S2MPG14_METER_ACC_DATA_CH6_6 = 0x8C,
	S2MPG14_METER_ACC_DATA_CH7_1 = 0x8D,
	S2MPG14_METER_ACC_DATA_CH7_2 = 0x8E,
	S2MPG14_METER_ACC_DATA_CH7_3 = 0x8F,
	S2MPG14_METER_ACC_DATA_CH7_4 = 0x90,
	S2MPG14_METER_ACC_DATA_CH7_5 = 0x91,
	S2MPG14_METER_ACC_DATA_CH7_6 = 0x92,
	S2MPG14_METER_ACC_DATA_CH8_1 = 0x93,
	S2MPG14_METER_ACC_DATA_CH8_2 = 0x94,
	S2MPG14_METER_ACC_DATA_CH8_3 = 0x95,
	S2MPG14_METER_ACC_DATA_CH8_4 = 0x96,
	S2MPG14_METER_ACC_DATA_CH8_5 = 0x97,
	S2MPG14_METER_ACC_DATA_CH8_6 = 0x98,
	S2MPG14_METER_ACC_DATA_CH9_1 = 0x99,
	S2MPG14_METER_ACC_DATA_CH9_2 = 0x9A,
	S2MPG14_METER_ACC_DATA_CH9_3 = 0x9B,
	S2MPG14_METER_ACC_DATA_CH9_4 = 0x9C,
	S2MPG14_METER_ACC_DATA_CH9_5 = 0x9D,
	S2MPG14_METER_ACC_DATA_CH9_6 = 0x9E,
	S2MPG14_METER_ACC_DATA_CH10_1 = 0x9F,
	S2MPG14_METER_ACC_DATA_CH10_2 = 0xA0,
	S2MPG14_METER_ACC_DATA_CH10_3 = 0xA1,
	S2MPG14_METER_ACC_DATA_CH10_4 = 0xA2,
	S2MPG14_METER_ACC_DATA_CH10_5 = 0xA3,
	S2MPG14_METER_ACC_DATA_CH10_6 = 0xA4,
	S2MPG14_METER_ACC_DATA_CH11_1 = 0xA5,
	S2MPG14_METER_ACC_DATA_CH11_2 = 0xA6,
	S2MPG14_METER_ACC_DATA_CH11_3 = 0xA7,
	S2MPG14_METER_ACC_DATA_CH11_4 = 0xA8,
	S2MPG14_METER_ACC_DATA_CH11_5 = 0xA9,
	S2MPG14_METER_ACC_DATA_CH11_6 = 0xAA,
	S2MPG14_METER_ACC_COUNT_1 = 0xAB,
	S2MPG14_METER_ACC_COUNT_2 = 0xAC,
	S2MPG14_METER_ACC_COUNT_3 = 0xAD,
	S2MPG14_METER_LPF_DATA_CH0_1 = 0xAE,
	S2MPG14_METER_LPF_DATA_CH0_2 = 0xAF,
	S2MPG14_METER_LPF_DATA_CH0_3 = 0xB0,
	S2MPG14_METER_LPF_DATA_CH1_1 = 0xB1,
	S2MPG14_METER_LPF_DATA_CH1_2 = 0xB2,
	S2MPG14_METER_LPF_DATA_CH1_3 = 0xB3,
	S2MPG14_METER_LPF_DATA_CH2_1 = 0xB4,
	S2MPG14_METER_LPF_DATA_CH2_2 = 0xB5,
	S2MPG14_METER_LPF_DATA_CH2_3 = 0xB6,
	S2MPG14_METER_LPF_DATA_CH3_1 = 0xB7,
	S2MPG14_METER_LPF_DATA_CH3_2 = 0xB8,
	S2MPG14_METER_LPF_DATA_CH3_3 = 0xB9,
	S2MPG14_METER_LPF_DATA_CH4_1 = 0xBA,
	S2MPG14_METER_LPF_DATA_CH4_2 = 0xBB,
	S2MPG14_METER_LPF_DATA_CH4_3 = 0xBC,
	S2MPG14_METER_LPF_DATA_CH5_1 = 0xBD,
	S2MPG14_METER_LPF_DATA_CH5_2 = 0xBE,
	S2MPG14_METER_LPF_DATA_CH5_3 = 0xBF,
	S2MPG14_METER_LPF_DATA_CH6_1 = 0xC0,
	S2MPG14_METER_LPF_DATA_CH6_2 = 0xC1,
	S2MPG14_METER_LPF_DATA_CH6_3 = 0xC2,
	S2MPG14_METER_LPF_DATA_CH7_1 = 0xC3,
	S2MPG14_METER_LPF_DATA_CH7_2 = 0xC4,
	S2MPG14_METER_LPF_DATA_CH7_3 = 0xC5,
	S2MPG14_METER_LPF_DATA_CH8_1 = 0xC6,
	S2MPG14_METER_LPF_DATA_CH8_2 = 0xC7,
	S2MPG14_METER_LPF_DATA_CH8_3 = 0xC8,
	S2MPG14_METER_LPF_DATA_CH9_1 = 0xC9,
	S2MPG14_METER_LPF_DATA_CH9_2 = 0xCA,
	S2MPG14_METER_LPF_DATA_CH9_3 = 0xCB,
	S2MPG14_METER_LPF_DATA_CH10_1 = 0xCC,
	S2MPG14_METER_LPF_DATA_CH10_2 = 0xCD,
	S2MPG14_METER_LPF_DATA_CH10_3 = 0xCE,
	S2MPG14_METER_LPF_DATA_CH11_1 = 0xCF,
	S2MPG14_METER_LPF_DATA_CH11_2 = 0xD0,
	S2MPG14_METER_LPF_DATA_CH11_3 = 0xD1,
	S2MPG14_METER_VBAT_DATA1 = 0xD2,
	S2MPG14_METER_VBAT_DATA2 = 0xD3,
	S2MPG14_METER_EXT_SIGNED_DATA_1 = 0xE4,
	S2MPG14_METER_EXT_SIGNED_DATA_2 = 0xE5,
};

/* gpio(0xC) Registers */
enum S2MPG14_GPIO_REG {
	S2MPG14_GPIO_INT1 = 0x0,
	S2MPG14_GPIO_INT2 = 0x1,
	S2MPG14_GPIO_INT1M = 0x2,
	S2MPG14_GPIO_INT2M = 0x3,
	S2MPG14_GPIO_STATUS = 0x4,
	S2MPG14_GPIO0_SET = 0x5,
	S2MPG14_GPIO1_SET = 0x6,
	S2MPG14_GPIO2_SET = 0x7,
	S2MPG14_GPIO3_SET = 0x8,
	S2MPG14_GPIO4_SET = 0x9,
	S2MPG14_GPIO5_SET = 0xA,
	S2MPG14_GPIO0_MONSEL = 0xB,
	S2MPG14_GPIO1_MONSEL = 0xC,
	S2MPG14_GPIO2_MONSEL = 0xD,
	S2MPG14_GPIO3_MONSEL = 0xE,
	S2MPG14_GPIO4_MONSEL = 0xF,
	S2MPG14_GPIO5_MONSEL = 0x10,
};

/* PM_TRIM1(0xF) Registers */
enum S2MPG14_PM_TRIM_REG {
	S2MPG14_PM_TRIM1_BUCK_HR_MODE = 0xC6,
};

/* S2MPG14 regulator ids */
enum S2MPG14_REGULATOR {
	S2MPG14_LDO1,
	S2MPG14_LDO2,
	S2MPG14_LDO3,
	S2MPG14_LDO4,
	S2MPG14_LDO5,
	S2MPG14_LDO6,
	S2MPG14_LDO7,
	S2MPG14_LDO8,
	S2MPG14_LDO9,
	S2MPG14_LDO10,
	S2MPG14_LDO11,
	S2MPG14_LDO12,
	S2MPG14_LDO13,
	S2MPG14_LDO14,
	S2MPG14_LDO15,
	S2MPG14_LDO16,
	S2MPG14_LDO17,
	S2MPG14_LDO18,
	S2MPG14_LDO19,
	S2MPG14_LDO20,
	S2MPG14_LDO21,
	S2MPG14_LDO22,
	S2MPG14_LDO23,
	S2MPG14_LDO24,
	S2MPG14_LDO25,
	S2MPG14_BUCK1,
	S2MPG14_BUCK2,
	S2MPG14_BUCK3,
	S2MPG14_BUCK4,
	S2MPG14_BUCK5,
	S2MPG14_BUCK6,
	S2MPG14_BUCK7,
	S2MPG14_BUCK8,
	S2MPG14_BUCK9,
	S2MPG14_REGULATOR_MAX,
};

enum s2mpg14_irq {
	/* PMIC */
	S2MPG14_IRQ_PWRONF_INT1,
	S2MPG14_IRQ_PWRONR_INT1,
	S2MPG14_IRQ_JIGONBF_INT1,
	S2MPG14_IRQ_JIGONBR_INT1,
	S2MPG14_IRQ_ACOKBF_INT1,
	S2MPG14_IRQ_ACOKBR_INT1,
	S2MPG14_IRQ_PWRON1S_INT1,
	S2MPG14_IRQ_MRB_INT1,

	S2MPG14_IRQ_RTC60S_INT2,
	S2MPG14_IRQ_RTCA1_INT2,
	S2MPG14_IRQ_RTCA0_INT2,
	S2MPG14_IRQ_RTC1S_INT2,
	S2MPG14_IRQ_WTSR_COLDRST_INT2,
	S2MPG14_IRQ_WTSR_INT2,
	S2MPG14_IRQ_WRST_INT2,
	S2MPG14_IRQ_SMPL_INT2,

	S2MPG14_IRQ_120C_INT3,
	S2MPG14_IRQ_140C_INT3,
	S2MPG14_IRQ_TSD_INT3,
	S2MPG14_IRQ_SCL_SOFTRST_INT3,
	S2MPG14_IRQ_WLWP_ACC_INT3,

	S2MPG14_IRQ_OCP_B1M_INT4,
	S2MPG14_IRQ_OCP_B2M_INT4,
	S2MPG14_IRQ_OCP_B3M_INT4,
	S2MPG14_IRQ_OCP_B4M_INT4,
	S2MPG14_IRQ_OCP_B5M_INT4,
	S2MPG14_IRQ_OCP_B6M_INT4,
	S2MPG14_IRQ_OCP_B7M_INT4,
	S2MPG14_IRQ_OCP_B8M_INT4,

	S2MPG14_IRQ_OCP_B9M_INT5,
	S2MPG14_IRQ_SMPL_TIMEOUT_INT5,
	S2MPG14_IRQ_WTSR_TIMEOUT_INT5,

	S2MPG14_IRQ_PMETER_OVERF_INT6,
	S2MPG14_IRQ_PWR_WARN_CH0_INT6,
	S2MPG14_IRQ_PWR_WARN_CH1_INT6,
	S2MPG14_IRQ_PWR_WARN_CH2_INT6,
	S2MPG14_IRQ_PWR_WARN_CH3_INT6,
	S2MPG14_IRQ_PWR_WARN_CH4_INT6,
	S2MPG14_IRQ_PWR_WARN_CH5_INT6,

	S2MPG14_IRQ_PWR_WARN_CH6_INT7,
	S2MPG14_IRQ_PWR_WARN_CH7_INT7,
	S2MPG14_IRQ_PWR_WARN_CH8_INT7,
	S2MPG14_IRQ_PWR_WARN_CH9_INT7,
	S2MPG14_IRQ_PWR_WARN_CH10_INT7,
	S2MPG14_IRQ_PWR_WARN_CH11_INT7,

	S2MPG14_IRQ_NR,
};

/* Common(0x0) MASK */
/* S2MPG14_COMMON_IBI0 */
#define S2MPG14_PMIC_M_MASK		BIT(0)
#define S2MPG14_PMIC_S_MASK		BIT(1)

/* S2MPG14_COMMON_IBI1 */
#define S2MPG14_METER_IRQ_MASK		BIT(0)

/* S2MPG14_COMMON_IBIM1 */
#define S2MPG14_IRQSRC_MASK		BIT(0)
#define S2MPG14_METER_IRQSRC_MASK	BIT(1)

/* PM(0x1) MASK */
#define S2MPG14_BUCK_MAX		9
#define S2MPG14_LDO_MAX			25
#define S2MPG14_VGPIO_NUM		11
#define S2MPG14_VGPIO_MAX_VAL		(0xFF)

/*
 * _MIN(group) S2MPG14_REG_MIN##group
 * _STEP(group) S2MPG14_REG_STEP##group
 */

/* BUCK 1M~9M ## group: 1 */
#define S2MPG14_REG_MIN1	200000
#define S2MPG14_REG_STEP1	6250
/* LDO 1M,3M,8M,11M~13M,15M,17M ## group: 2 */
#define S2MPG14_REG_MIN2	300000
#define S2MPG14_REG_STEP2	12500
/* LDO 6M,9M,10M,16M,18M,22M ## group: 3 */
#define S2MPG14_REG_MIN3	725000
#define S2MPG14_REG_STEP3	12500
/* LDO 2M,5M,20M,23M,24M,25M ## group: 4 */
#define S2MPG14_REG_MIN4	700000
#define S2MPG14_REG_STEP4	25000
/* LDO 4M,7M,14M,19M,21M ## group: 5 */
#define S2MPG14_REG_MIN5	1800000
#define S2MPG14_REG_STEP5	25000

/* _N_VOLTAGES(num) S2MPG14_REG_N_VOLTAGES_##num */
#define S2MPG14_REG_N_VOLTAGES_64	0x40
#define S2MPG14_REG_N_VOLTAGES_128	0x80
#define S2MPG14_REG_N_VOLTAGES_256	0x100

/* _MASK(num) S2MPG14_REG_ENABLE_MASK##num */
#define S2MPG14_REG_ENABLE_WIDTH	2
#define S2MPG14_REG_ENABLE_MASK_1_0	MASK(S2MPG14_REG_ENABLE_WIDTH, 0)
#define S2MPG14_REG_ENABLE_MASK_3_2	MASK(S2MPG14_REG_ENABLE_WIDTH, 2)
#define S2MPG14_REG_ENABLE_MASK_4_3	MASK(S2MPG14_REG_ENABLE_WIDTH, 3)
#define S2MPG14_REG_ENABLE_MASK_5_4	MASK(S2MPG14_REG_ENABLE_WIDTH, 4)
#define S2MPG14_REG_ENABLE_MASK_7_6	MASK(S2MPG14_REG_ENABLE_WIDTH, 6)
#define S2MPG14_REG_ENABLE_MASK_7	BIT(7)

/* _TIME(macro) S2MPG14_ENABLE_TIME##macro */
#define S2MPG14_ENABLE_TIME_LDO		128
#define S2MPG14_ENABLE_TIME_BUCK	130

/* S2MPG14_PM_DCTRLSEL1 ~ 9 */
#define S2MPG14_DCTRLSEL_VOUT1			0x0
#define S2MPG14_DCTRLSEL_PWREN			0x1
#define S2MPG14_DCTRLSEL_PWREN_TRG		0x2
#define S2MPG14_DCTRLSEL_PWREN_MIF		0x3
#define S2MPG14_DCTRLSEL_PWREN_MIF_TRG		0x4
#define S2MPG14_DCTRLSEL_AP_ACTIVE_N		0x5
#define S2MPG14_DCTRLSEL_AP_ACTIVE_TRG		0x6
#define S2MPG14_DCTRLSEL_PWREN_MIF_ACTIVE_N	0x7
#define S2MPG14_DCTRLSEL_AOC_RET		0x8
#define S2MPG14_DCTRLSEL_TPU_EN			0x9
#define S2MPG14_DCTRLSEL_ACTIVE_PWREN_TRG	0xA
#define S2MPG14_DCTRLSEL_ACTIVE_TPU_EN		0xB
#define S2MPG14_DCTRLSEL_PWREN_MIF_RTG		0xC

/* VGPIO_RX_MONITOR ADDR. */
#define VGPIO_I3C_BASE			0x15500000
#define VGPIO_MONITOR_ADDR		0x1704

/* VGPIO_PENDING_CLEAR */
#define SYSREG_VGPIO2AP			0x15090000
#define INTC0_IPEND			0x0290

/* POWER-KEY MASK */
#define S2MPG14_FALLING_EDGE		BIT(1)	/* INT1 reg */
#define S2MPG14_RISING_EDGE		BIT(0)	/* INT1 reg */

/* S2MPG14_PM_STATUS1(0x10A) */
#define S2MPG14_STATUS1_PWRON		BIT(0)

/* S2MPG14_PM_SMPL_WARN_CTRL(0x116) */
#define S2MPG14_SMPL_WARN_LBDT_SHIFT	0
#define S2MPG14_SMPL_WARN_HYS_SHIFT	3
#define S2MPG14_SMPL_WARN_LVL_SHIFT	5

/* S2MPG14_PM_OCP_WARN(0x1BB~0x1C0) */
#define S2MPG14_OCP_WARN_EN_SHIFT	7
#define S2MPG14_OCP_WARN_CNT_SHIFT	6
#define S2MPG14_OCP_WARN_DVS_MASK_SHIFT	5
#define S2MPG14_OCP_WARN_LVL_SHIFT	0

/* METER(0xA) MASK */
/* S2MPG14_METER_CTRL1 */
#define S2MPG14_METER_EN_MASK		BIT(0)

#endif /* __LINUX_MFD_S2MPG14_REGISTER_H */
