// SPDX-License-Identifier: GPL-2.0+
/*
 * s2mpg15-regulator.c
 *
 * Copyright (c) 2022 Samsung Electronics Co., Ltd
 *              http://www.samsung.com
 */

#include <linux/async.h>
#include <linux/bug.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <../drivers/pinctrl/samsung/pinctrl-samsung.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/regmap.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/of_regulator.h>
#include <linux/mfd/samsung/s2mpg15.h>
#include <linux/mfd/samsung/s2mpg15-register.h>
#include <linux/io.h>
#include <linux/mutex.h>
#include <linux/debugfs.h>
#include <linux/interrupt.h>
#include <linux/regulator/pmic_class.h>
#include <dt-bindings/regulator/samsung,s2mpg-regulator.h>

static struct regulator_desc regulators[S2MPG15_REGULATOR_MAX];

static unsigned int s2mpg15_of_map_mode(unsigned int val)
{
	switch (val) {
	case SEC_OPMODE_SUSPEND: /* ON in Standby Mode */
		return 0x1;
	case SEC_OPMODE_MIF: /* ON in PWREN_MIF mode */
		return 0x2;
	case SEC_OPMODE_ON: /* ON in Normal Mode */
		return 0x3;
	default:
		return 0x3;
	}
}

static int s2m_set_mode(struct regulator_dev *rdev, unsigned int mode)
{
	struct s2mpg15_pmic *s2mpg15 = rdev_get_drvdata(rdev);
	unsigned int val;
	int id = rdev_get_id(rdev);
	int enable_mask = rdev->desc->enable_mask;
	int enable_shift = 0;

	while (1) {
		if (enable_mask & 0x1)
			break;

		enable_shift++;
		enable_mask = enable_mask >> 1;

		if (enable_shift > 7) {
			dev_err(s2mpg15->iodev->dev, "[%d]: error calculating enable_shift!\n", id);
			return -EINVAL;
		}
	};

	val = (mode << enable_shift) & rdev->desc->enable_mask;

	s2mpg15->opmode[id] = val;
	return 0;
}

static int s2m_enable(struct regulator_dev *rdev)
{
	struct s2mpg15_pmic *s2mpg15 = rdev_get_drvdata(rdev);

	return s2mpg15_update_reg(s2mpg15->i2c, rdev->desc->enable_reg,
				  s2mpg15->opmode[rdev_get_id(rdev)],
				  rdev->desc->enable_mask);
}

static int s2m_disable(struct regulator_dev *rdev)
{
	struct s2mpg15_pmic *s2mpg15 = rdev_get_drvdata(rdev);
	unsigned int val;

	if (rdev->desc->enable_is_inverted)
		val = rdev->desc->enable_mask;
	else
		val = 0;

	return s2mpg15_update_reg(s2mpg15->i2c, rdev->desc->enable_reg, val,
				  rdev->desc->enable_mask);
}

static int s2m_is_enabled(struct regulator_dev *rdev)
{
	struct s2mpg15_pmic *s2mpg15 = rdev_get_drvdata(rdev);
	int ret;
	u8 val;

	ret = s2mpg15_read_reg(s2mpg15->i2c, rdev->desc->enable_reg, &val);
	if (ret)
		return ret;

	if (rdev->desc->enable_is_inverted)
		return (val & rdev->desc->enable_mask) == 0;
	return (val & rdev->desc->enable_mask) != 0;
}

static int s2m_get_voltage_sel(struct regulator_dev *rdev)
{
	struct s2mpg15_pmic *s2mpg15 = rdev_get_drvdata(rdev);
	int ret;
	u8 val;

	ret = s2mpg15_read_reg(s2mpg15->i2c, rdev->desc->vsel_reg, &val);
	if (ret)
		return ret;

	val &= rdev->desc->vsel_mask;

	return val;
}

static int s2m_set_voltage_sel(struct regulator_dev *rdev, unsigned int sel)
{
	struct s2mpg15_pmic *s2mpg15 = rdev_get_drvdata(rdev);
	int ret;

	ret = s2mpg15_update_reg(s2mpg15->i2c, rdev->desc->vsel_reg, sel,
				 rdev->desc->vsel_mask);
	if (ret < 0)
		goto out;

	if (rdev->desc->apply_bit)
		ret = s2mpg15_update_reg(s2mpg15->i2c, rdev->desc->apply_reg,
					 rdev->desc->apply_bit,
					 rdev->desc->apply_bit);
	return ret;
out:
	dev_warn(s2mpg15->iodev->dev, "%s: failed to set regulator voltage\n",
		 rdev->desc->name);
	ret = -EINVAL;
	return ret;
}

static int s2m_set_voltage_time_sel(struct regulator_dev *rdev,
				    unsigned int old_selector,
				    unsigned int new_selector)
{
	struct s2mpg15_pmic *s2mpg15 = rdev_get_drvdata(rdev);
	unsigned int ramp_delay = 0;
	int old_volt, new_volt;

	if (rdev->constraints->ramp_delay)
		ramp_delay = rdev->constraints->ramp_delay;
	else if (rdev->desc->ramp_delay)
		ramp_delay = rdev->desc->ramp_delay;

	if (ramp_delay == 0) {
		dev_warn(s2mpg15->iodev->dev, "%s: ramp_delay not set\n",
			 rdev->desc->name);
		return -EINVAL;
	}

	/* validity check */
	if (!rdev->desc->ops->list_voltage)
		return -EINVAL;

	old_volt = rdev->desc->ops->list_voltage(rdev, old_selector);
	new_volt = rdev->desc->ops->list_voltage(rdev, new_selector);

	if (old_selector < new_selector)
		return DIV_ROUND_UP(new_volt - old_volt, ramp_delay);
	return DIV_ROUND_UP(old_volt - new_volt, ramp_delay);
}

static struct regulator_ops s2mpg15_regulator_ops = {
	.list_voltage = regulator_list_voltage_linear,
	.map_voltage = regulator_map_voltage_linear,
	.is_enabled = s2m_is_enabled,
	.enable = s2m_enable,
	.disable = s2m_disable,
	.get_voltage_sel = s2m_get_voltage_sel,
	.set_voltage_sel = s2m_set_voltage_sel,
	.set_voltage_time_sel = s2m_set_voltage_time_sel,
	.set_mode = s2m_set_mode,
};

#define _BUCK(macro) S2MPG15_BUCK##macro
#define _LDO(macro) S2MPG15_LDO##macro
#define _REG(ctrl) S2MPG15_PM##ctrl
#define _TIME(macro) S2MPG15_ENABLE_TIME##macro
#define _MIN(group) S2MPG15_REG_MIN##group
#define _STEP(group) S2MPG15_REG_STEP##group
#define _N_VOLTAGES(num) S2MPG15_REG_N_VOLTAGES_##num
#define _MASK(num) S2MPG15_REG_ENABLE_MASK##num

#define REG_DESC(_name, _id, g, v, n, e, em, t)                           \
	{                                                                 \
		.name = _name, .id = _id, .ops = &s2mpg15_regulator_ops,  \
		.type = REGULATOR_VOLTAGE, .owner = THIS_MODULE,          \
		.min_uV = _MIN(g), .uV_step = _STEP(g), .n_voltages = n,  \
		.vsel_reg = v, .vsel_mask = n - 1, .enable_reg = e,       \
		.enable_mask = em, .enable_time = t,                      \
		.of_map_mode = s2mpg15_of_map_mode                        \
	}

static struct regulator_desc regulators[S2MPG15_REGULATOR_MAX] = {
	/* name, id, voltage_group, vsel_reg, n_voltages, */
	/* enable_reg, enable_mask, enable_time */
	REG_DESC("LDO1S", _LDO(1), 4, _REG(_L1S_CTRL), _N_VOLTAGES(128),
		 _REG(_LDO_CTRL1), _MASK(_1_0), _TIME(_LDO)),
	REG_DESC("LDO2S", _LDO(2), 4, _REG(_L2S_CTRL), _N_VOLTAGES(64),
		 _REG(_LDO_CTRL1), _MASK(_3_2), _TIME(_LDO)),
	REG_DESC("LDO3S", _LDO(3), 5, _REG(_L3S_CTRL), _N_VOLTAGES(64),
		 _REG(_L3S_CTRL), _MASK(_7_6), _TIME(_LDO)),
	REG_DESC("LDO4S", _LDO(4), 6, _REG(_L4S_CTRL), _N_VOLTAGES(64),
		 _REG(_L4S_CTRL), _MASK(_7), _TIME(_LDO)),
	REG_DESC("LDO5S", _LDO(5), 7, _REG(_L5S_CTRL), _N_VOLTAGES(64),
		 _REG(_L5S_CTRL), _MASK(_7), _TIME(_LDO)),
	REG_DESC("LDO6S", _LDO(6), 7, _REG(_L6S_CTRL), _N_VOLTAGES(64),
		 _REG(_L6S_CTRL), _MASK(_7), _TIME(_LDO)),
	REG_DESC("LDO7S", _LDO(7), 6, _REG(_L7S_CTRL), _N_VOLTAGES(64),
		 _REG(_L7S_CTRL), _MASK(_7), _TIME(_LDO)),
	REG_DESC("LDO8S", _LDO(8), 7, _REG(_L8S_CTRL), _N_VOLTAGES(64),
		 _REG(_L8S_CTRL), _MASK(_7), _TIME(_LDO)),
	REG_DESC("LDO9S", _LDO(9), 5, _REG(_L9S_CTRL), _N_VOLTAGES(64),
		 _REG(_L9S_CTRL), _MASK(_7), _TIME(_LDO)),
	REG_DESC("LDO10S", _LDO(10), 6, _REG(_L10S_CTRL), _N_VOLTAGES(64),
		 _REG(_L10S_CTRL), _MASK(_7), _TIME(_LDO)),
	REG_DESC("LDO11S", _LDO(11), 6, _REG(_L11S_CTRL), _N_VOLTAGES(64),
		 _REG(_L11S_CTRL), _MASK(_7), _TIME(_LDO)),
	REG_DESC("LDO12S", _LDO(12), 6, _REG(_L12S_CTRL), _N_VOLTAGES(64),
		 _REG(_L12S_CTRL), _MASK(_7), _TIME(_LDO)),
	REG_DESC("LDO13S", _LDO(13), 6, _REG(_L13S_CTRL), _N_VOLTAGES(64),
		 _REG(_L13S_CTRL), _MASK(_7_6), _TIME(_LDO)),
	REG_DESC("LDO14S", _LDO(14), 6, _REG(_L14S_CTRL), _N_VOLTAGES(64),
		 _REG(_L14S_CTRL), _MASK(_7), _TIME(_LDO)),
	REG_DESC("LDO15S", _LDO(15), 7, _REG(_L15S_CTRL), _N_VOLTAGES(64),
		 _REG(_L15S_CTRL), _MASK(_7), _TIME(_LDO)),
	REG_DESC("LDO16S", _LDO(16), 7, _REG(_L16S_CTRL), _N_VOLTAGES(64),
		 _REG(_L16S_CTRL), _MASK(_7), _TIME(_LDO)),
	REG_DESC("LDO17S", _LDO(17), 6, _REG(_L17S_CTRL), _N_VOLTAGES(64),
		 _REG(_L17S_CTRL), _MASK(_7), _TIME(_LDO)),
	REG_DESC("LDO18S", _LDO(18), 7, _REG(_L18S_CTRL), _N_VOLTAGES(64),
		 _REG(_L18S_CTRL), _MASK(_7_6), _TIME(_LDO)),
	REG_DESC("LDO19S", _LDO(19), 6, _REG(_L19S_CTRL), _N_VOLTAGES(64),
		 _REG(_L19S_CTRL), _MASK(_7_6), _TIME(_LDO)),
	REG_DESC("LDO20S", _LDO(20), 6, _REG(_L20S_CTRL), _N_VOLTAGES(64),
		 _REG(_L20S_CTRL), _MASK(_7_6), _TIME(_LDO)),
	REG_DESC("LDO21S", _LDO(21), 5, _REG(_L21S_CTRL), _N_VOLTAGES(64),
		 _REG(_L21S_CTRL), _MASK(_7), _TIME(_LDO)),
	REG_DESC("LDO22S", _LDO(22), 6, _REG(_L22S_CTRL), _N_VOLTAGES(64),
		 _REG(_L22S_CTRL), _MASK(_7), _TIME(_LDO)),
	REG_DESC("LDO23S", _LDO(23), 4, _REG(_L23S_CTRL), _N_VOLTAGES(128),
		 _REG(_LDO_CTRL1), _MASK(_5_4), _TIME(_LDO)),
	REG_DESC("LDO24S", _LDO(24), 5, _REG(_L24S_CTRL), _N_VOLTAGES(64),
		 _REG(_L24S_CTRL), _MASK(_7_6), _TIME(_LDO)),
	REG_DESC("LDO25S", _LDO(25), 7, _REG(_L25S_CTRL), _N_VOLTAGES(64),
		 _REG(_L25S_CTRL), _MASK(_7), _TIME(_LDO)),
	REG_DESC("LDO26S", _LDO(26), 7, _REG(_L26S_CTRL), _N_VOLTAGES(64),
		 _REG(_L26S_CTRL), _MASK(_7), _TIME(_LDO)),
	REG_DESC("LDO27S", _LDO(27), 6, _REG(_L27S_CTRL), _N_VOLTAGES(64),
		 _REG(_L27S_CTRL), _MASK(_7_6), _TIME(_LDO)),
	REG_DESC("LDO28S", _LDO(28), 7, _REG(_L28S_CTRL), _N_VOLTAGES(64),
		 _REG(_L28S_CTRL), _MASK(_7), _TIME(_LDO)),
	REG_DESC("LDO29S", _LDO(29), 6, _REG(_L29S_CTRL), _N_VOLTAGES(64),
		 _REG(_L29S_CTRL), _MASK(_7), _TIME(_LDO)),
	REG_DESC("BUCK1S", _BUCK(1), 1, _REG(_B1S_OUT1), _N_VOLTAGES(256),
		 _REG(_B1S_CTRL), _MASK(_7_6), _TIME(_BUCK)),
	REG_DESC("BUCK2S", _BUCK(2), 1, _REG(_B2S_OUT1), _N_VOLTAGES(256),
		 _REG(_B2S_CTRL), _MASK(_7_6), _TIME(_BUCK)),
	REG_DESC("BUCK3S", _BUCK(3), 1, _REG(_B3S_OUT1), _N_VOLTAGES(256),
		 _REG(_B3S_CTRL), _MASK(_7), _TIME(_BUCK)),
	REG_DESC("BUCK4S", _BUCK(4), 1, _REG(_B4S_OUT1), _N_VOLTAGES(256),
		 _REG(_B4S_CTRL), _MASK(_7), _TIME(_BUCK)),
	REG_DESC("BUCK5S", _BUCK(5), 1, _REG(_B5S_OUT1), _N_VOLTAGES(256),
		 _REG(_B5S_CTRL), _MASK(_7_6), _TIME(_BUCK)),
	REG_DESC("BUCK6S", _BUCK(6), 1, _REG(_B6S_OUT1), _N_VOLTAGES(256),
		 _REG(_B6S_CTRL), _MASK(_7_6), _TIME(_BUCK)),
	REG_DESC("BUCK7S", _BUCK(7), 2, _REG(_B7S_OUT1), _N_VOLTAGES(256),
		 _REG(_B7S_CTRL), _MASK(_7), _TIME(_BUCK)),
	REG_DESC("BUCK8S", _BUCK(8), 1, _REG(_B8S_OUT1), _N_VOLTAGES(256),
		 _REG(_B8S_CTRL), _MASK(_7_6), _TIME(_BUCK)),
	REG_DESC("BUCK9S", _BUCK(9), 1, _REG(_B9S_OUT0), _N_VOLTAGES(256),
		 _REG(_B9S_CTRL), _MASK(_7), _TIME(_BUCK)),
	REG_DESC("BUCK10S", _BUCK(10), 1, _REG(_B10S_OUT1), _N_VOLTAGES(256),
		 _REG(_B10S_CTRL), _MASK(_7_6), _TIME(_BUCK)),
	REG_DESC("BUCK11S", _BUCK(11), 1, _REG(_B11S_OUT1), _N_VOLTAGES(256),
		 _REG(_B11S_CTRL), _MASK(_7_6), _TIME(_BUCK)),
	REG_DESC("BUCK12S", _BUCK(12), 1, _REG(_B12S_OUT1), _N_VOLTAGES(256),
		 _REG(_B12S_CTRL), _MASK(_7_6), _TIME(_BUCK)),
	REG_DESC("BUCKD", _BUCK(D), 2, _REG(_BUCKD_OUT), _N_VOLTAGES(256),
		 _REG(_BUCKD_CTRL), _MASK(_7_6), _TIME(_BUCK)),
	REG_DESC("BUCKA", _BUCK(A), 2, _REG(_BUCKA_OUT), _N_VOLTAGES(256),
		 _REG(_BUCKA_CTRL), _MASK(_7_6), _TIME(_BUCK)),
	REG_DESC("BUCKC", _BUCK(C), 1, _REG(_BUCKC_OUT), _N_VOLTAGES(256),
		 _REG(_BUCKC_CTRL), _MASK(_7_6), _TIME(_BUCK)),
	REG_DESC("BUCKBOOST", _BUCK(BOOST), 3, _REG(_BB_OUT), _N_VOLTAGES(128),
		 _REG(_BB_CTRL), _MASK(_7_6), _TIME(_BUCK)),
};

#if IS_ENABLED(CONFIG_OF)
static int s2mpg15_pmic_dt_parse_pdata(struct s2mpg15_dev *iodev,
				       struct s2mpg15_platform_data *pdata)
{
	struct device_node *pmic_np, *regulators_np, *reg_np;
	struct s2mpg15_regulator_data *rdata;
	unsigned int i;
	int ret, len;
	u32 val = 0;
	const u32 *p;

	pmic_np = iodev->dev->of_node;
	if (!pmic_np) {
		dev_err(iodev->dev, "could not find pmic sub-node\n");
		return -ENODEV;
	}

	/* wtsr_en */
	pdata->wtsr_en = 0;
	ret = of_property_read_u32(pmic_np, "wtsr_en", &val);
	if (ret < 0)
		dev_err(iodev->dev, "fail to read wtsr_en\n");
	pdata->wtsr_en = val;

	regulators_np = of_find_node_by_name(pmic_np, "regulators");
	if (!regulators_np) {
		dev_err(iodev->dev, "could not find regulators sub-node\n");
		return -EINVAL;
	}

	/* count the number of regulators to be supported in pmic */
	pdata->num_regulators = 0;
	for_each_child_of_node(regulators_np, reg_np) {
		pdata->num_regulators++;
	}

	rdata = devm_kzalloc(iodev->dev, sizeof(*rdata) * pdata->num_regulators,
			     GFP_KERNEL);
	if (!rdata)
		return -ENOMEM;

	pdata->regulators = rdata;
	for_each_child_of_node(regulators_np, reg_np) {
		for (i = 0; i < ARRAY_SIZE(regulators); i++)
			if (!of_node_cmp(reg_np->name, regulators[i].name))
				break;

		if (i == ARRAY_SIZE(regulators)) {
			dev_warn(iodev->dev,
				 "don't know how to configure regulator %s\n",
				 reg_np->name);
			continue;
		}

		rdata->id = i;
		rdata->initdata = of_get_regulator_init_data(iodev->dev, reg_np,
							     &regulators[i]);
		rdata->reg_node = reg_np;
		rdata++;
	}

	/* parse BUCK OCP Detection information */
	of_property_read_u32(pmic_np, "buck_ocp_ctrl1", &pdata->buck_ocp_ctrl1);

	of_property_read_u32(pmic_np, "buck_ocp_ctrl2", &pdata->buck_ocp_ctrl2);

	of_property_read_u32(pmic_np, "buck_ocp_ctrl3", &pdata->buck_ocp_ctrl3);

	of_property_read_u32(pmic_np, "buck_ocp_ctrl4", &pdata->buck_ocp_ctrl4);

	of_property_read_u32(pmic_np, "buck_ocp_ctrl5", &pdata->buck_ocp_ctrl5);

	of_property_read_u32(pmic_np, "buck_ocp_ctrl6", &pdata->buck_ocp_ctrl6);

	of_property_read_u32(pmic_np, "buck_ocp_ctrl7", &pdata->buck_ocp_ctrl7);

	/* parse OCP_WARN information */
	pdata->b2_ocp_warn_pin = of_get_gpio(pmic_np, 0);
	if (pdata->b2_ocp_warn_pin < 0)
		dev_err(iodev->dev, "b2_ocp_warn_pin < 0: %d\n",
			pdata->b2_ocp_warn_pin);

	ret = of_property_read_u32(pmic_np, "b2_ocp_warn_en", &val);
	pdata->b2_ocp_warn_en = ret ? 0 : val;

	ret = of_property_read_u32(pmic_np, "b2_ocp_warn_cnt", &val);
	pdata->b2_ocp_warn_cnt = ret ? 0 : val;

	ret = of_property_read_u32(pmic_np, "b2_ocp_warn_dvs_mask", &val);
	pdata->b2_ocp_warn_dvs_mask = ret ? 0 : val;

	ret = of_property_read_u32(pmic_np, "b2_ocp_warn_lvl", &val);
	pdata->b2_ocp_warn_lvl = ret ? 0 : val;

	ret = of_property_read_u32(pmic_np, "b2_ocp_warn_debounce_clk", &val);
	pdata->b2_ocp_warn_debounce_clk = ret ? 0 : val;

	/* parse SOFT_OCP_WARN information */
	pdata->b2_soft_ocp_warn_pin = of_get_gpio(pmic_np, 1);
	if (pdata->b2_soft_ocp_warn_pin < 0)
		dev_err(iodev->dev, "b2_soft_ocp_warn_pin < 0: %d\n",
			pdata->b2_soft_ocp_warn_pin);

	ret = of_property_read_u32(pmic_np, "b2_soft_ocp_warn_en", &val);
	pdata->b2_soft_ocp_warn_en = ret ? 0 : val;

	ret = of_property_read_u32(pmic_np, "b2_soft_ocp_warn_cnt", &val);
	pdata->b2_soft_ocp_warn_cnt = ret ? 0 : val;

	ret = of_property_read_u32(pmic_np, "b2_soft_ocp_warn_dvs_mask", &val);
	pdata->b2_soft_ocp_warn_dvs_mask = ret ? 0 : val;

	ret = of_property_read_u32(pmic_np, "b2_soft_ocp_warn_lvl", &val);
	pdata->b2_soft_ocp_warn_lvl = ret ? 0 : val;

	ret = of_property_read_u32(pmic_np, "b2_soft_ocp_warn_debounce_clk", &val);
	pdata->b2_soft_ocp_warn_debounce_clk = ret ? 0 : val;

	/* Set SEL_VGPIO (control_sel) */
	p = of_get_property(pmic_np, "sel_vgpio", &len);
	if (!p) {
		dev_err(iodev->dev, "(ERROR) sel_vgpio isn't parsing\n");
		return -EINVAL;
	}

	len = len / sizeof(u32);
	if (len != S2MPG15_VGPIO_NUM) {
		dev_err(iodev->dev, "(ERROR) sel_vgpio num isn't not equal\n");
		return -EINVAL;
	}

	pdata->sel_vgpio = devm_kzalloc(iodev->dev, sizeof(u32) * len, GFP_KERNEL);
	if (!(pdata->sel_vgpio))
		return -ENOMEM;

	for (i = 0; i < len; i++) {
		ret = of_property_read_u32_index(pmic_np, "sel_vgpio", i, &pdata->sel_vgpio[i]);
		if (ret) {
			dev_err(iodev->dev, "(ERROR) sel_vgpio%d is empty\n", i + 1);
			pdata->sel_vgpio[i] = 0x1FF;
		}
	}

	return 0;
}
#else
static int s2mpg15_pmic_dt_parse_pdata(struct s2mpg15_dev *iodev,
				       struct s2mpg15_platform_data *pdata)
{
	return 0;
}
#endif /* CONFIG_OF */

static void s2mpg15_wtsr_enable(struct s2mpg15_pmic *s2mpg15,
				struct s2mpg15_platform_data *pdata)
{
	int ret;

	dev_dbg(s2mpg15->iodev->dev, "[PMIC] WTSR %s\n",
		pdata->wtsr_en ? "enable" : "not enable");

	ret = s2mpg15_update_reg(s2mpg15->i2c, S2MPG15_PM_CTRL1,
				 S2MPG15_WTSREN_MASK, S2MPG15_WTSREN_MASK);

	s2mpg15->wtsr_en = pdata->wtsr_en;
}

static void s2mpg15_wtsr_disable(struct s2mpg15_pmic *s2mpg15)
{
	int ret;

	dev_dbg(s2mpg15->iodev->dev, "[PMIC] WTSR disable\n");

	ret = s2mpg15_update_reg(s2mpg15->i2c, S2MPG15_PM_CTRL1,
				 0x00, S2MPG15_WTSREN_MASK);
}

#if IS_ENABLED(CONFIG_DRV_SAMSUNG_PMIC)
static struct i2c_client *s2mpg15_get_i2c_client(struct s2mpg15_pmic *s2mpg15,
						 u16 reg)
{
	switch (reg >> 8) {
	case I2C_ADDR_TOP:	return s2mpg15->iodev->i2c;
	case I2C_ADDR_PMIC:	return s2mpg15->iodev->pmic;
	case I2C_ADDR_METER:	return s2mpg15->iodev->meter;
	case I2C_ADDR_WLWP:	return s2mpg15->iodev->wlwp;
	case I2C_ADDR_GPIO:	return s2mpg15->iodev->gpio;
	case I2C_ADDR_MT_TRIM:	return s2mpg15->iodev->mt_trim;
	case I2C_ADDR_PM_TRIM2:	return s2mpg15->iodev->pm_trim2;
	case I2C_ADDR_PM_TRIM1:	return s2mpg15->iodev->pm_trim1;
	default:		return NULL;
	}
}

static ssize_t s2mpg15_pmic_read_store(struct device *dev,
				       struct device_attribute *attr,
				       const char *buf, size_t size)
{
	struct s2mpg15_pmic *s2mpg15 = dev_get_drvdata(dev);
	int ret;
	u16 reg_addr;

	if (!buf)
		return -1;

	ret = kstrtou16(buf, 0, &reg_addr);
	if (ret < 0) {
		dev_err(s2mpg15->dev, "fail to transform i2c address\n");
		return ret;
	}

	s2mpg15->read_addr = reg_addr;

	return size;
}

static ssize_t s2mpg15_pmic_read_show(struct device *dev,
				      struct device_attribute *attr, char *buf)
{
	struct s2mpg15_pmic *s2mpg15 = dev_get_drvdata(dev);
	struct i2c_client *client = NULL;
	u16 reg_addr = s2mpg15->read_addr;
	u8 val;
	int ret;

	client = s2mpg15_get_i2c_client(s2mpg15, reg_addr);
	if (!client) {
		dev_err(dev, "reg_addr is wrong: %2x\n", reg_addr);
		return -1;
	}

	ret = s2mpg15_read_reg(client, reg_addr, &val);
	if (ret < 0) {
		dev_err(dev, "fail to read i2c address\n");
		return ret;
	}

	dev_dbg(dev, "reg(0x%04X) data(0x%02X)\n", reg_addr, val);

	return scnprintf(buf, PAGE_SIZE, "0x%04X: 0x%02X\n", reg_addr, val);
}

static ssize_t s2mpg15_pmic_write_store(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t size)
{
	struct s2mpg15_pmic *s2mpg15 = dev_get_drvdata(dev);
	struct i2c_client *client = NULL;
	int ret;
	u16 reg;
	u8 data;

	if (!buf) {
		dev_err(s2mpg15->dev, "empty buffer\n");
		return size;
	}

	ret = sscanf(buf, "%hx %hhx", &reg, &data);
	if (ret != 2) {
		dev_err(s2mpg15->dev, "input error\n");
		return size;
	}

	dev_dbg(s2mpg15->dev, "reg(0x%04X) data(0x%02X)\n", reg, data);

	client = s2mpg15_get_i2c_client(s2mpg15, reg);
	if (!client) {
		dev_err(s2mpg15->dev, "reg is wrong: %2x\n", reg);
		return size;
	}

	ret = s2mpg15_write_reg(client, reg, data);
	if (ret < 0)
		dev_err(s2mpg15->dev, "fail to write i2c addr/data\n");

	return size;
}

static ssize_t s2mpg15_pmic_write_show(struct device *dev,
				       struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "echo (register addr.) (data) > s2mpg15_write\n");
}

static DEVICE_ATTR_RW(s2mpg15_pmic_write);
static DEVICE_ATTR_RW(s2mpg15_pmic_read);

int create_s2mpg15_pmic_sysfs(struct s2mpg15_pmic *s2mpg15)
{
	struct device *s2mpg15_pmic = s2mpg15->dev;
	int err = -ENODEV;

	s2mpg15->read_addr = 0;

	s2mpg15_pmic = pmic_device_create(s2mpg15, "s2mpg15-pmic");

	err = device_create_file(s2mpg15_pmic, &dev_attr_s2mpg15_pmic_write);
	if (err) {
		dev_err(s2mpg15->dev,
			"s2mpg15_sysfs: failed to create device file, %s\n",
				 dev_attr_s2mpg15_pmic_write.attr.name);
	}

	err = device_create_file(s2mpg15_pmic, &dev_attr_s2mpg15_pmic_read);
	if (err) {
		dev_err(s2mpg15->dev,
			"s2mpg15_sysfs: failed to create device file, %s\n",
				 dev_attr_s2mpg15_pmic_read.attr.name);
	}

	return 0;
}
#endif

void s2mpg15_ocp_detection_config(struct s2mpg15_pmic *s2mpg15,
				  struct s2mpg15_platform_data *pdata)
{
	int ret;

	dev_info(s2mpg15->iodev->dev, "OCP_CTRL1: 0x%x\n", pdata->buck_ocp_ctrl1);
	ret = s2mpg15_write_reg(s2mpg15->i2c, S2MPG15_PM_BUCK_OCP_CTRL1,
				pdata->buck_ocp_ctrl1);
	if (ret)
		dev_err(s2mpg15->iodev->dev,
			"i2c write error. BUCK_OCP_CTRL1: %d\n", ret);

	dev_info(s2mpg15->iodev->dev, "OCP_CTRL2: 0x%x\n", pdata->buck_ocp_ctrl2);
	ret = s2mpg15_write_reg(s2mpg15->i2c, S2MPG15_PM_BUCK_OCP_CTRL2,
				pdata->buck_ocp_ctrl2);
	if (ret)
		dev_err(s2mpg15->iodev->dev,
			"i2c write error. BUCK_OCP_CTRL2: %d\n", ret);

	dev_info(s2mpg15->iodev->dev, "OCP_CTRL3: 0x%x\n", pdata->buck_ocp_ctrl3);
	ret = s2mpg15_write_reg(s2mpg15->i2c, S2MPG15_PM_BUCK_OCP_CTRL3,
				pdata->buck_ocp_ctrl3);
	if (ret)
		dev_err(s2mpg15->iodev->dev,
			"i2c write error. BUCK_OCP_CTRL3: %d\n", ret);

	dev_info(s2mpg15->iodev->dev, "OCP_CTRL4: 0x%x\n", pdata->buck_ocp_ctrl4);
	ret = s2mpg15_write_reg(s2mpg15->i2c, S2MPG15_PM_BUCK_OCP_CTRL4,
				pdata->buck_ocp_ctrl4);
	if (ret)
		dev_err(s2mpg15->iodev->dev,
			"i2c write error. BUCK_OCP_CTRL4: %d\n", ret);

	dev_info(s2mpg15->iodev->dev, "OCP_CTRL5: 0x%x\n", pdata->buck_ocp_ctrl5);
	ret = s2mpg15_write_reg(s2mpg15->i2c, S2MPG15_PM_BUCK_OCP_CTRL5,
				pdata->buck_ocp_ctrl5);
	if (ret)
		dev_err(s2mpg15->iodev->dev,
			"i2c write error. BUCK_OCP_CTRL5: %d\n", ret);

	dev_info(s2mpg15->iodev->dev, "OCP_CTRL6: 0x%x\n", pdata->buck_ocp_ctrl6);
	ret = s2mpg15_write_reg(s2mpg15->i2c, S2MPG15_PM_BUCK_OCP_CTRL6,
				pdata->buck_ocp_ctrl6);
	if (ret)
		dev_err(s2mpg15->iodev->dev,
			"i2c write error. BUCK_OCP_CTRL6: %d\n", ret);

	dev_info(s2mpg15->iodev->dev, "OCP_CTRL7: 0x%x\n", pdata->buck_ocp_ctrl7);
	ret = s2mpg15_write_reg(s2mpg15->i2c, S2MPG15_PM_BUCK_OCP_CTRL7,
				pdata->buck_ocp_ctrl7);
	if (ret)
		dev_err(s2mpg15->iodev->dev,
			"i2c write error. BUCK_OCP_CTRL7: %d\n", ret);
	dev_info(s2mpg15->iodev->dev, "OCP_CTRL8: 0x%x\n", pdata->buck_ocp_ctrl8);
	ret = s2mpg15_write_reg(s2mpg15->i2c, S2MPG15_PM_BUCK_OCP_CTRL8,
				pdata->buck_ocp_ctrl8);
	if (ret)
		dev_err(s2mpg15->iodev->dev,
			"i2c write error. BUCK_OCP_CTRL8: %d\n", ret);
}

int s2mpg15_ocp_warn(struct s2mpg15_pmic *s2mpg15,
		     struct s2mpg15_platform_data *pdata)
{
	u8 val;
	int ret;

	val = (pdata->b2_ocp_warn_en << S2MPG15_OCP_WARN_EN_SHIFT) |
	      (pdata->b2_ocp_warn_cnt << S2MPG15_OCP_WARN_CNT_SHIFT) |
	      (pdata->b2_ocp_warn_dvs_mask << S2MPG15_OCP_WARN_DVS_MASK_SHIFT) |
	      (pdata->b2_ocp_warn_lvl << S2MPG15_OCP_WARN_LVL_SHIFT);

	dev_info(s2mpg15->iodev->dev, "B2S_OCP_WARN : 0x%x\n", val);
	ret = s2mpg15_write_reg(s2mpg15->i2c, S2MPG15_PM_B2S_OCP_WARN, val);
	if (ret)
		dev_err(s2mpg15->iodev->dev,
			"i2c write error setting b2s_ocp_warn\n");

	val = (pdata->b2_soft_ocp_warn_en << S2MPG15_OCP_WARN_EN_SHIFT) |
	      (pdata->b2_soft_ocp_warn_cnt << S2MPG15_OCP_WARN_CNT_SHIFT) |
	      (pdata->b2_soft_ocp_warn_dvs_mask
	       << S2MPG15_OCP_WARN_DVS_MASK_SHIFT) |
	      (pdata->b2_soft_ocp_warn_lvl << S2MPG15_OCP_WARN_LVL_SHIFT);

	dev_info(s2mpg15->iodev->dev, "B2S_SOFT_OCP_WARN : 0x%x\n", val);
	ret = s2mpg15_write_reg(s2mpg15->i2c, S2MPG15_PM_B2S_SOFT_OCP_WARN,
				val);
	if (ret)
		dev_err(s2mpg15->iodev->dev,
			"i2c write error setting b2s_soft_ocp_warn\n");

	val = (pdata->b2_ocp_warn_debounce_clk << 2) |
		(pdata->b2_soft_ocp_warn_debounce_clk);

	ret = s2mpg15_write_reg(s2mpg15->i2c, S2MPG15_PM_B2S_OCP_WARN_DEBOUNCE,
				val);

	if (ret)
		dev_err(s2mpg15->iodev->dev,
			"i2c write error setting ocp_warn debounce\n");

	return ret;
}

static int s2mpg15_set_sel_vgpio(struct s2mpg15_pmic *s2mpg15,
				 struct s2mpg15_platform_data *pdata)
{
	int ret, i;
	u8 reg, val;

	for (i = 0; i < S2MPG15_VGPIO_NUM; i++) {
		reg = S2MPG15_PM_PCTRLSEL1 + i;
		val = pdata->sel_vgpio[i];

		if (val > S2MPG15_VGPIO_MAX_VAL) {
			dev_err(s2mpg15->iodev->dev,
				"sel_vgpio%d exceed the value\n", i + 1);
			return -1;
		}

		ret = s2mpg15_write_reg(s2mpg15->i2c, reg, val);
		if (ret) {
			dev_err(s2mpg15->iodev->dev,
				"sel_vgpio%d write error\n", i + 1);
			return -1;
		}

		dev_dbg(s2mpg15->iodev->dev, "0x%02hhx[0x%02hhx]\n", reg, val);
	}

	return 0;
}

int s2mpg15_oi_function(struct s2mpg15_pmic *s2mpg15)
{
	int ret = 0;
	/* add OI configuration code if necessary */

	/* OI function enable */

	/* OI power down disable */

	/* OI detection time window : 500us, OI comp. output count : 50 times */

	return ret;
}

static int s2mpg15_pmic_probe(struct platform_device *pdev)
{
	struct s2mpg15_dev *iodev = dev_get_drvdata(pdev->dev.parent);
	struct s2mpg15_platform_data *pdata = iodev->pdata;
	struct regulator_config config = {};
	struct s2mpg15_pmic *s2mpg15;
	int i, ret;

	if (iodev->dev->of_node) {
		ret = s2mpg15_pmic_dt_parse_pdata(iodev, pdata);
		if (ret)
			return ret;
	}

	if (!pdata)
		return -ENODEV;

	s2mpg15 = devm_kzalloc(&pdev->dev, sizeof(struct s2mpg15_pmic),
			       GFP_KERNEL);
	if (!s2mpg15)
		return -ENOMEM;

	s2mpg15->rdev = devm_kzalloc(&pdev->dev,
				     sizeof(struct regulator_dev *) *
					     S2MPG15_REGULATOR_MAX,
				     GFP_KERNEL);
	if (!s2mpg15->rdev)
		return -ENOMEM;

	s2mpg15->opmode =
		devm_kzalloc(&pdev->dev,
			     sizeof(unsigned int) * S2MPG15_REGULATOR_MAX,
			     GFP_KERNEL);
	if (!s2mpg15->opmode)
		return -ENOMEM;

	s2mpg15->iodev = iodev;
	s2mpg15->i2c = iodev->pmic;
#if IS_ENABLED(CONFIG_DRV_SAMSUNG_PMIC)
	s2mpg15->dev = &pdev->dev;
#endif

	mutex_init(&s2mpg15->lock);
	platform_set_drvdata(pdev, s2mpg15);

	/* setting for LDOs with exceptional register structure */

	for (i = 0; i < pdata->num_regulators; i++) {
		int id = pdata->regulators[i].id;

		config.dev = &pdev->dev;
		config.init_data = pdata->regulators[i].initdata;
		config.driver_data = s2mpg15;
		config.of_node = pdata->regulators[i].reg_node;
		s2mpg15->opmode[id] = regulators[id].enable_mask;

		s2mpg15->rdev[i] = devm_regulator_register(&pdev->dev,
							   &regulators[id],
							   &config);
		if (IS_ERR(s2mpg15->rdev[i])) {
			ret = PTR_ERR(s2mpg15->rdev[i]);
			dev_err(&pdev->dev, "regulator init failed for %d\n", i);
			s2mpg15->rdev[i] = NULL;
			return ret;
		}
	}

	s2mpg15->num_regulators = pdata->num_regulators;

	s2mpg15_ocp_detection_config(s2mpg15, pdata);
	ret = s2mpg15_ocp_warn(s2mpg15, pdata);
	if (ret < 0) {
		dev_err(&pdev->dev, "s2mpg15_ocp_warn fail\n");
		return ret;
	}

	ret = s2mpg15_set_sel_vgpio(s2mpg15, pdata);
	if (ret < 0) {
		dev_err(&pdev->dev, "s2mpg15_set_sel_vgpio fail\n");
		return ret;
	}

	if (pdata->wtsr_en)
		s2mpg15_wtsr_enable(s2mpg15, pdata);

	ret = s2mpg15_oi_function(s2mpg15);
	if (ret < 0) {
		dev_err(&pdev->dev, "s2mpg15_oi_function fail\n");
		return ret;
	}

#if IS_ENABLED(CONFIG_DRV_SAMSUNG_PMIC)
	/* create sysfs */
	ret = create_s2mpg15_pmic_sysfs(s2mpg15);
	if (ret < 0)
		return ret;
#endif

	return 0;
}

static int s2mpg15_pmic_remove(struct platform_device *pdev)
{
#if IS_ENABLED(CONFIG_DRV_SAMSUNG_PMIC)
	struct s2mpg15_pmic *s2mpg15 = platform_get_drvdata(pdev);

	pmic_device_destroy(s2mpg15->dev->devt);
#endif
	return 0;
}

static void s2mpg15_pmic_shutdown(struct platform_device *pdev)
{
	struct s2mpg15_pmic *s2mpg15 = platform_get_drvdata(pdev);

	if (s2mpg15->wtsr_en)
		s2mpg15_wtsr_disable(s2mpg15);
}

#if IS_ENABLED(CONFIG_PM)
static int s2mpg15_pmic_suspend(struct device *dev)
{
	int ret = 0;

	return ret;
}

static int s2mpg15_pmic_resume(struct device *dev)
{
	int ret = 0;

	return ret;
}
#else
#define s2mpg15_pmic_suspend NULL
#define s2mpg15_pmic_resume NULL
#endif /* CONFIG_PM */

const struct dev_pm_ops s2mpg15_pmic_pm = {
	.suspend = s2mpg15_pmic_suspend,
	.resume = s2mpg15_pmic_resume,
};

static const struct platform_device_id s2mpg15_pmic_id[] = {
	{ "s2mpg15-regulator", 0 },
	{},
};

MODULE_DEVICE_TABLE(platform, s2mpg15_pmic_id);

static struct platform_driver s2mpg15_pmic_driver = {
	.driver = {
		   .name = "s2mpg15-regulator",
		   .owner = THIS_MODULE,
#if IS_ENABLED(CONFIG_PM)
		   .pm = &s2mpg15_pmic_pm,
#endif
		   .suppress_bind_attrs = true,
		    },
	.probe = s2mpg15_pmic_probe,
	.remove = s2mpg15_pmic_remove,
	.shutdown = s2mpg15_pmic_shutdown,
	.id_table = s2mpg15_pmic_id,
};

static int __init s2mpg15_pmic_init(void)
{
	return platform_driver_register(&s2mpg15_pmic_driver);
}
subsys_initcall(s2mpg15_pmic_init);

static void __exit s2mpg15_pmic_exit(void)
{
	platform_driver_unregister(&s2mpg15_pmic_driver);
}
module_exit(s2mpg15_pmic_exit);

/* Module information */
MODULE_AUTHOR("Hanam Hwang <hanam.hwang@samsung.com>");
MODULE_DESCRIPTION("SAMSUNG S2MPG15 Regulator Driver");
MODULE_LICENSE("GPL");
