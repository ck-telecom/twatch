/* Copyright (c) 2022 Qingsong Gou <gouqs@hotmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PINETIME_INCLUDE_AXP202_H
#define PINETIME_INCLUDE_AXP202_H

#include <device.h>

#ifdef __cplusplus
extern "C" {
#endif

enum hrs3300_hwt {
	HRS3300_HWT_800MS = 0x0,
	HRS3300_HWT_400MS = 0x1,
	HRS3300_HWT_200MS = 0x2,
	HRS3300_HWT_100MS = 0x3,
	HRS3300_HWT_75MS = 0x4,
	HRS3300_HWT_50MS = 0x5,
	HRS3300_HWT_12_5MS = 0x6,
	HRS3300_HWT_0MS = 0x7
};

enum hrs3300_pdrive {
	HRS3300_PDRIVE_12_5 = 0,
	HRS3300_PDRIVE_20 = 1,
	HRS3300_PDRIVE_30 = 2,
	HRS3300_PDRIVE_40 = 3
};

enum hrs3300_hgain {
	HRS3300_HGAIN_1 = 0,
	HRS3300_HGAIN_2 = 1,
	HRS3300_HGAIN_4 = 2,
	HRS3300_HGAIN_8 = 3,
	HRS3300_HGAIN_64 = 4,
};

#define AXP202_CHIP_ID 0x41

//! REG MAP
#define AXP202_STATUS (0x00)
#define AXP202_MODE_CHGSTATUS (0x01)
#define AXP202_OTG_STATUS (0x02)
#define AXP202_IC_TYPE (0x03)
#define AXP202_DATA_BUFFER1 (0x04)
#define AXP202_DATA_BUFFER2 (0x05)
#define AXP202_DATA_BUFFER3 (0x06)
#define AXP202_DATA_BUFFER4 (0x07)
#define AXP202_DATA_BUFFER5 (0x08)
#define AXP202_DATA_BUFFER6 (0x09)
#define AXP202_DATA_BUFFER7 (0x0A)
#define AXP202_DATA_BUFFER8 (0x0B)
#define AXP202_DATA_BUFFER9 (0x0C)
#define AXP202_DATA_BUFFERA (0x0D)
#define AXP202_DATA_BUFFERB (0x0E)
#define AXP202_DATA_BUFFERC (0x0F)
#define AXP202_LDO234_DC23_CTL (0x12)
#define AXP202_DC2OUT_VOL (0x23)
#define AXP202_LDO3_DC2_DVM (0x25)
#define AXP202_DC3OUT_VOL (0x27)
#define AXP202_LDO24OUT_VOL (0x28)
#define AXP202_LDO3OUT_VOL (0x29)
#define AXP202_IPS_SET (0x30)
#define AXP202_VOFF_SET (0x31)
#define AXP202_OFF_CTL (0x32)
#define AXP202_CHARGE1 (0x33)
#define AXP202_CHARGE2 (0x34)
#define AXP202_BACKUP_CHG (0x35)
#define AXP202_POK_SET (0x36)
#define AXP202_DCDC_FREQSET (0x37)
#define AXP202_VLTF_CHGSET (0x38)
#define AXP202_VHTF_CHGSET (0x39)
#define AXP202_APS_WARNING1 (0x3A)
#define AXP202_APS_WARNING2 (0x3B)
#define AXP202_TLTF_DISCHGSET (0x3C)
#define AXP202_THTF_DISCHGSET (0x3D)
#define AXP202_DCDC_MODESET (0x80)
#define AXP202_ADC_EN1 (0x82)
#define AXP202_ADC_EN2 (0x83)
#define AXP202_ADC_SPEED (0x84)
#define AXP202_ADC_INPUTRANGE (0x85)
#define AXP202_ADC_IRQ_RETFSET (0x86)
#define AXP202_ADC_IRQ_FETFSET (0x87)
#define AXP202_TIMER_CTL (0x8A)
#define AXP202_VBUS_DET_SRP (0x8B)
#define AXP202_HOTOVER_CTL (0x8F)
#define AXP202_GPIO0_CTL (0x90)
#define AXP202_GPIO0_VOL (0x91)
#define AXP202_GPIO1_CTL (0x92)
#define AXP202_GPIO2_CTL (0x93)
#define AXP202_GPIO012_SIGNAL (0x94)
#define AXP202_GPIO3_CTL (0x95)
#define AXP202_INTEN1 (0x40)
#define AXP202_INTEN2 (0x41)
#define AXP202_INTEN3 (0x42)
#define AXP202_INTEN4 (0x43)
#define AXP202_INTEN5 (0x44)
#define AXP202_INTSTS1 (0x48)
#define AXP202_INTSTS2 (0x49)
#define AXP202_INTSTS3 (0x4A)
#define AXP202_INTSTS4 (0x4B)
#define AXP202_INTSTS5 (0x4C)

//Irq control register
#define AXP192_INTEN1 (0x40)
#define AXP192_INTEN2 (0x41)
#define AXP192_INTEN3 (0x42)
#define AXP192_INTEN4 (0x43)
#define AXP192_INTEN5 (0x4A)
//Irq status register
#define AXP192_INTSTS1 (0x44)
#define AXP192_INTSTS2 (0x45)
#define AXP192_INTSTS3 (0x46)
#define AXP192_INTSTS4 (0x47)
#define AXP192_INTSTS5 (0x4D)

#define AXP192_DC1_VLOTAGE (0x26)
#define AXP192_LDO23OUT_VOL (0x28)
#define AXP192_GPIO0_CTL (0x90)
#define AXP192_GPIO0_VOL (0x91)
#define AXP192_GPIO1_CTL (0X92)
#define AXP192_GPIO2_CTL (0x93)
#define AXP192_GPIO012_SIGNAL (0x94)
#define AXP192_GPIO34_CTL (0x95)



/* axp 192/202 adc data register */
#define AXP202_BAT_AVERVOL_H8 (0x78)
#define AXP202_BAT_AVERVOL_L4 (0x79)
#define AXP202_BAT_AVERCHGCUR_H8 (0x7A)
#define AXP202_BAT_AVERCHGCUR_L4 (0x7B)
#define AXP202_BAT_AVERCHGCUR_L5 (0x7B)
#define AXP202_ACIN_VOL_H8 (0x56)
#define AXP202_ACIN_VOL_L4 (0x57)
#define AXP202_ACIN_CUR_H8 (0x58)
#define AXP202_ACIN_CUR_L4 (0x59)
#define AXP202_VBUS_VOL_H8 (0x5A)
#define AXP202_VBUS_VOL_L4 (0x5B)
#define AXP202_VBUS_CUR_H8 (0x5C)
#define AXP202_VBUS_CUR_L4 (0x5D)
#define AXP202_INTERNAL_TEMP_H8 (0x5E)
#define AXP202_INTERNAL_TEMP_L4 (0x5F)
#define AXP202_TS_IN_H8 (0x62)
#define AXP202_TS_IN_L4 (0x63)
#define AXP202_GPIO0_VOL_ADC_H8 (0x64)
#define AXP202_GPIO0_VOL_ADC_L4 (0x65)
#define AXP202_GPIO1_VOL_ADC_H8 (0x66)
#define AXP202_GPIO1_VOL_ADC_L4 (0x67)

#define AXP202_BAT_AVERDISCHGCUR_H8 (0x7C)
#define AXP202_BAT_AVERDISCHGCUR_L5 (0x7D)
#define AXP202_APS_AVERVOL_H8 (0x7E)
#define AXP202_APS_AVERVOL_L4 (0x7F)
#define AXP202_INT_BAT_CHGCUR_H8 (0xA0)
#define AXP202_INT_BAT_CHGCUR_L4 (0xA1)
#define AXP202_EXT_BAT_CHGCUR_H8 (0xA2)
#define AXP202_EXT_BAT_CHGCUR_L4 (0xA3)
#define AXP202_INT_BAT_DISCHGCUR_H8 (0xA4)
#define AXP202_INT_BAT_DISCHGCUR_L4 (0xA5)
#define AXP202_EXT_BAT_DISCHGCUR_H8 (0xA6)
#define AXP202_EXT_BAT_DISCHGCUR_L4 (0xA7)
#define AXP202_BAT_CHGCOULOMB3 (0xB0)
#define AXP202_BAT_CHGCOULOMB2 (0xB1)
#define AXP202_BAT_CHGCOULOMB1 (0xB2)
#define AXP202_BAT_CHGCOULOMB0 (0xB3)
#define AXP202_BAT_DISCHGCOULOMB3 (0xB4)
#define AXP202_BAT_DISCHGCOULOMB2 (0xB5)
#define AXP202_BAT_DISCHGCOULOMB1 (0xB6)
#define AXP202_BAT_DISCHGCOULOMB0 (0xB7)
#define AXP202_COULOMB_CTL (0xB8)
#define AXP202_BAT_POWERH8 (0x70)
#define AXP202_BAT_POWERM8 (0x71)
#define AXP202_BAT_POWERL8 (0x72)

#define AXP202_VREF_TEM_CTRL (0xF3)
#define AXP202_BATT_PERCENTAGE (0xB9)

/* bit definitions for AXP events, irq event */
/*  AXP202  */
#define AXP202_IRQ_USBLO (1)
#define AXP202_IRQ_USBRE (2)
#define AXP202_IRQ_USBIN (3)
#define AXP202_IRQ_USBOV (4)
#define AXP202_IRQ_ACRE (5)
#define AXP202_IRQ_ACIN (6)
#define AXP202_IRQ_ACOV (7)

#define AXP202_IRQ_TEMLO (8)
#define AXP202_IRQ_TEMOV (9)
#define AXP202_IRQ_CHAOV (10)
#define AXP202_IRQ_CHAST (11)
#define AXP202_IRQ_BATATOU (12)
#define AXP202_IRQ_BATATIN (13)
#define AXP202_IRQ_BATRE (14)
#define AXP202_IRQ_BATIN (15)

#define AXP202_IRQ_POKLO (16)
#define AXP202_IRQ_POKSH (17)
#define AXP202_IRQ_LDO3LO (18)
#define AXP202_IRQ_DCDC3LO (19)
#define AXP202_IRQ_DCDC2LO (20)
#define AXP202_IRQ_CHACURLO (22)
#define AXP202_IRQ_ICTEMOV (23)

#define AXP202_IRQ_EXTLOWARN2 (24)
#define AXP202_IRQ_EXTLOWARN1 (25)
#define AXP202_IRQ_SESSION_END (26)
#define AXP202_IRQ_SESS_AB_VALID (27)
#define AXP202_IRQ_VBUS_UN_VALID (28)
#define AXP202_IRQ_VBUS_VALID (29)
#define AXP202_IRQ_PDOWN_BY_NOE (30)
#define AXP202_IRQ_PUP_BY_NOE (31)

#define AXP202_IRQ_GPIO0TG (32)
#define AXP202_IRQ_GPIO1TG (33)
#define AXP202_IRQ_GPIO2TG (34)
#define AXP202_IRQ_GPIO3TG (35)
#define AXP202_IRQ_PEKFE (37)
#define AXP202_IRQ_PEKRE (38)
#define AXP202_IRQ_TIMER (39)

//Signal Capture
#define AXP202_BATT_VOLTAGE_STEP (1.1F)
#define AXP202_BATT_DISCHARGE_CUR_STEP (0.5F)
#define AXP202_BATT_CHARGE_CUR_STEP (0.5F)
#define AXP202_ACIN_VOLTAGE_STEP (1.7F)
#define AXP202_ACIN_CUR_STEP (0.625F)
#define AXP202_VBUS_VOLTAGE_STEP (1.7F)
#define AXP202_VBUS_CUR_STEP (0.375F)
#define AXP202_INTERNAL_TEMP_STEP (0.1F)
#define AXP202_INTERNAL_TEMP_MIN (-144.7F)
#define AXP202_APS_VOLTAGE_STEP (1.4F)
#define AXP202_TS_PIN_OUT_STEP (0.8F)
#define AXP202_GPIO0_STEP (0.5F)
#define AXP202_GPIO1_STEP (0.5F)
/** @brief Enable measurement.
 *
 * @param dev Device.
 *
 * @return 0 on suucess, negative error code otherwise.
 */
int hrs3300_enable(struct device *dev);

/** @brief Disable measurement.
 *
 * @param dev Device.
 *
 * @return 0 on suucess, negative error code otherwise.
 */
int hrs3300_disable(struct device *dev);


/** @brief Set hgain.
 *
 * @param dev Device.
 * @param val Hgain.
 *
 * @return 0 on suucess, negative error code otherwise.
 */
int hrs3300_hgain_set(struct device *dev, enum hrs3300_hgain val);


/** @brief Get hgain.
 *
 * @param dev Device.
 * @param val Location to store hgain.
 *
 * @return 0 on suucess, negative error code otherwise.
 */
int hrs3300_hgain_get(struct device *dev, enum hrs3300_hgain *val);

/** @brief Set HWT.
 *
 * @param dev Device.
 * @param val Gain.
 *
 * @return 0 on suucess, negative error code otherwise.
 */
int hrs3300_hwt_set(struct device *dev, enum hrs3300_hwt val);

/** @brief Get HWT.
 *
 * @param dev Device.
 * @param val Location to store HWT.
 *
 * @return 0 on suucess, negative error code otherwise.
 */
int hrs3300_hwt_get(struct device *dev, enum hrs3300_hwt *val);

/** @brief Set PDrive.
 *
 * @param dev Device.
 * @param val Gain.
 *
 * @return 0 on suucess, negative error code otherwise.
 */
int hrs3300_pdrive_set(struct device *dev, enum hrs3300_pdrive val);

/** @brief Get PDrive.
 *
 * @param dev Device.
 * @param val Location to store PDrive.
 *
 * @return 0 on suucess, negative error code otherwise.
 */
int hrs3300_pdrive_get(struct device *dev, enum hrs3300_pdrive *val);

struct hrs3300_data {

	u32_t raw[2];
	struct hrs3300_addr_reg config[4];
};

struct axp202_config {
	struct i2c_dt_spec i2c;	
};

#ifdef __cplusplus
}
#endif

#endif /* PINETIME_INCLUDE_HRS3300_H */
