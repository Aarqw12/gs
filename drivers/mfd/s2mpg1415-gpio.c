// SPDX-License-Identifier: GPL-2.0
/*
 * s2mpg1415-gpio.c
 *
 * Copyright 2022 Google LLC
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/gpio/driver.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/pinctrl/pinconf-generic.h>
#include <linux/pinctrl/pinconf.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/mfd/samsung/s2mpg14.h>
#include <linux/mfd/samsung/s2mpg15.h>
#include <linux/mfd/samsung/s2mpg1415.h>

const struct pinctrl_pin_desc s2mpg14_pins[] = {
	PINCTRL_PIN(0, "gpio0"), PINCTRL_PIN(1, "gpio1"),
	PINCTRL_PIN(2, "gpio2"), PINCTRL_PIN(3, "gpio3"),
	PINCTRL_PIN(4, "gpio4"), PINCTRL_PIN(5, "gpio5"),
};

const struct pinctrl_pin_desc s2mpg15_pins[] = {
	PINCTRL_PIN(0, "gpio6"),  PINCTRL_PIN(1, "gpio7"),
	PINCTRL_PIN(2, "gpio8"),  PINCTRL_PIN(3, "gpio9"),
	PINCTRL_PIN(4, "gpio10"), PINCTRL_PIN(5, "gpio11"),
	PINCTRL_PIN(6, "gpio12"), PINCTRL_PIN(7, "gpio13"),
	PINCTRL_PIN(8, "gpio14"), PINCTRL_PIN(9, "gpio15"),
};

struct s2mpg1415_gpio {
	struct gpio_chip gc;
	struct pinctrl_ops pctrl_ops;
	struct pinmux_ops pmux_ops;
	struct pinconf_ops pconf_ops;
	struct pinctrl_desc pctrl;
	enum s2mpg1415_id id;
	struct i2c_client *i2c;
	struct device *dev;
};

static const int GPIO_STATUS[] = { S2MPG14_GPIO_STATUS, S2MPG15_GPIO_STATUS1 };
static const int GPIO_CTRL_BASE[] = { S2MPG14_GPIO0_SET, S2MPG15_GPIO0_SET };

#define GPIO_MODE_SHIFT		0	/* Remote GPIO */
#define GPIO_DRV_STR_SHIFT	2	/* Drive strength selection(x2 when 1) */
#define GPIO_PD_SHIFT		3	/* Pull-Down enable */
#define GPIO_PU_SHIFT		4	/* Pull-Up enable */
#define GPIO_OUT_SHIFT		5	/* output data */
#define GPIO_OEN_SHIFT		6	/* output enabled */

#define GPIO_MODE_MASK		(3 << GPIO_MODE_SHIFT)
#define GPIO_DRV_STR_MASK	BIT(GPIO_DRV_STR_SHIFT)
#define GPIO_PD_MASK		BIT(GPIO_PD_SHIFT)
#define GPIO_PU_MASK		BIT(GPIO_PU_SHIFT)
#define GPIO_OUT_MASK		BIT(GPIO_OUT_SHIFT)
#define GPIO_OEN_MASK		BIT(GPIO_OEN_SHIFT)
#define GPIO_RSV_MASK		BIT(GPIO_RSV_SHIFT)

static int s2mpg1415_read_gpio_reg(struct s2mpg1415_gpio *gc,
				   unsigned int offset)
{
	int ret;
	u8 val;

	ret = s2mpg1415_read_reg(gc->id, gc->i2c,
				 GPIO_CTRL_BASE[gc->id] + offset, &val);
	if (ret < 0) {
		dev_err(gc->dev, "Error: %s %d", gc->gc.label, offset);
		return ret;
	}
	dev_dbg(gc->dev, "offset: %#x, val: %#x\n",
		GPIO_CTRL_BASE[gc->id] + offset, val);

	return val;
}

static int s2mpg1415_read_gpio_bit(struct s2mpg1415_gpio *gc,
				   unsigned int offset, unsigned int bit_mask)
{
	return s2mpg1415_read_gpio_reg(gc, offset) & bit_mask;
}

static int s2mpg1415_read_gpio_status_reg(struct s2mpg1415_gpio *gc,
					  unsigned int over)
{
	int ret;
	u8 val;

	ret = s2mpg1415_read_reg(gc->id, gc->i2c,
				 GPIO_STATUS[gc->id] + over, &val);

	if (ret < 0) {
		dev_err(gc->dev, "Error: %s", gc->gc.label);
		return ret;
	}
	dev_dbg(gc->dev, "offset: %#x, val: %#x\n",
		GPIO_CTRL_BASE[gc->id] + over, val);

	return val;
}

static int s2mpg1415_read_gpio_status_bit(struct s2mpg1415_gpio *gc,
					  unsigned int bit_mask,
					  unsigned int over)
{
	return (s2mpg1415_read_gpio_status_reg(gc, over) >> bit_mask) & 0x1;
}

static int s2mpg1415_update_gpio_reg(struct s2mpg1415_gpio *gc,
				     unsigned int offset, u8 val, u8 mask)
{
	int ret;

	ret = s2mpg1415_update_reg(gc->id, gc->i2c,
				   GPIO_CTRL_BASE[gc->id] + offset, val, mask);
	if (ret < 0)
		dev_err(gc->dev, "Error: %s %d", gc->gc.label, offset);
	dev_dbg(gc->dev, "offset: %#x val: %#x mask: %#x\n",
		GPIO_CTRL_BASE[gc->id] + offset, val, mask);

	return ret;
}

static int s2mpg1415_gpio_get_direction(struct gpio_chip *chip,
					unsigned int offset)
{
	struct s2mpg1415_gpio *data = gpiochip_get_data(chip);

	return !s2mpg1415_read_gpio_bit(data, offset, GPIO_OEN_MASK);
}

static int s2mpg1415_gpio_get(struct gpio_chip *chip, unsigned int offset)
{
	struct s2mpg1415_gpio *data = gpiochip_get_data(chip);
	unsigned int over = 0;

	if (offset >= 8)
		over = 1;

	if (s2mpg1415_gpio_get_direction(chip, offset))
		return s2mpg1415_read_gpio_status_bit(data, offset, over);
	return s2mpg1415_read_gpio_bit(data, offset, GPIO_OUT_MASK);
}

static void s2mpg1415_gpio_set(struct gpio_chip *chip, unsigned int offset,
			       int value)
{
	struct s2mpg1415_gpio *data = gpiochip_get_data(chip);

	if (!s2mpg1415_gpio_get_direction(chip, offset))
		s2mpg1415_update_gpio_reg(data, offset,
					  value << GPIO_OUT_SHIFT,
					  GPIO_OUT_MASK);
}

static int s2mpg1415_gpio_direction_input(struct gpio_chip *chip,
					  unsigned int offset)
{
	struct s2mpg1415_gpio *data = gpiochip_get_data(chip);

	return s2mpg1415_update_gpio_reg(data, offset, 0, GPIO_OEN_MASK);
}

static int s2mpg1415_gpio_direction_output(struct gpio_chip *chip,
					   unsigned int offset, int value)
{
	struct s2mpg1415_gpio *data = gpiochip_get_data(chip);
	int ret;

	ret = s2mpg1415_update_gpio_reg(data, offset,
					value << GPIO_OUT_SHIFT,
					GPIO_OUT_MASK);
	if (ret == 0)
		ret = s2mpg1415_update_gpio_reg(data, offset,
						0x1 << GPIO_OEN_SHIFT,
						GPIO_OEN_MASK);

	return ret;
}

static int s2mpg1415_pinconf_get(struct pinctrl_dev *pctldev, unsigned int pin,
				 unsigned long *config)
{
	struct s2mpg1415_gpio *gc = pinctrl_dev_get_drvdata(pctldev);
	enum pin_config_param param = pinconf_to_config_param(*config);
	u8 data;
	u32 argument = 0;
	int ret;

	switch (param) {
	case PIN_CONFIG_BIAS_DISABLE:
		ret = s2mpg1415_read_reg(gc->id, gc->i2c,
					 GPIO_CTRL_BASE[gc->id] + pin, &data);
		if (ret)
			return ret;
		if (data & GPIO_PD_MASK)
			return -EINVAL;
		ret = s2mpg1415_read_reg(gc->id, gc->i2c,
					 GPIO_CTRL_BASE[gc->id] + pin, &data);
		if (ret)
			return ret;
		if (data & GPIO_PU_MASK)
			return -EINVAL;
		break;
	case PIN_CONFIG_BIAS_PULL_DOWN:
		ret = s2mpg1415_read_reg(gc->id, gc->i2c,
					 GPIO_CTRL_BASE[gc->id] + pin, &data);
		if (ret)
			return ret;
		if (!(data & GPIO_PD_MASK))
			return -EINVAL;
		break;
	case PIN_CONFIG_BIAS_PULL_UP:
		ret = s2mpg1415_read_reg(gc->id, gc->i2c,
					 GPIO_CTRL_BASE[gc->id] + pin, &data);
		if (ret)
			return ret;
		if (!(data & GPIO_PU_MASK))
			return -EINVAL;
		break;
	case PIN_CONFIG_DRIVE_PUSH_PULL:
		/* do nothing */
		break;
	case PIN_CONFIG_DRIVE_STRENGTH:
		ret = s2mpg1415_read_reg(gc->id, gc->i2c,
					 GPIO_CTRL_BASE[gc->id] + pin, &data);
		if (ret)
			return ret;
		argument = (data & GPIO_DRV_STR_MASK) ? 4 : 2;
		break;
	case PIN_CONFIG_INPUT_ENABLE:
		/* do nothing */
		break;
	case PIN_CONFIG_OUTPUT:
		ret = s2mpg1415_read_reg(gc->id, gc->i2c,
					 GPIO_CTRL_BASE[gc->id] + pin, &data);
		if (ret)
			return ret;
		argument = !!(data & GPIO_OUT_MASK);
		break;
	case PIN_CONFIG_OUTPUT_ENABLE:
		ret = s2mpg1415_read_reg(gc->id, gc->i2c,
					 GPIO_CTRL_BASE[gc->id] + pin, &data);
		if (ret)
			return ret;
		if (!(data & GPIO_OEN_MASK))
			return -EINVAL;
		break;
	default:
		return -ENOTSUPP;
	}

	*config = pinconf_to_config_packed(param, argument);

	return 0;
}

static int s2mpg1415_pinconf_set(struct pinctrl_dev *pctldev, unsigned int pin,
				 unsigned long *configs,
				 unsigned int num_configs)
{
	struct s2mpg1415_gpio *gc = pinctrl_dev_get_drvdata(pctldev);
	u8 data;
	unsigned int i;
	int ret;

	for (i = 0; i < num_configs; i++) {
		enum pin_config_param param =
			pinconf_to_config_param(configs[i]);

		u32 argument = pinconf_to_config_argument(configs[i]);

		switch (param) {
		case PIN_CONFIG_BIAS_DISABLE:
			ret = s2mpg1415_update_reg(gc->id, gc->i2c,
						   GPIO_CTRL_BASE[gc->id] + pin,
						   0, GPIO_PD_MASK);
			if (ret)
				return ret;
			ret = s2mpg1415_update_reg(gc->id, gc->i2c,
						   GPIO_CTRL_BASE[gc->id] + pin,
						   0, GPIO_PU_MASK);
			if (ret)
				return ret;
			break;
		case PIN_CONFIG_BIAS_PULL_DOWN:
			ret = s2mpg1415_update_reg(gc->id, gc->i2c,
						   GPIO_CTRL_BASE[gc->id] + pin,
						   GPIO_PD_MASK, GPIO_PD_MASK);
			if (ret)
				return ret;
			break;
		case PIN_CONFIG_BIAS_PULL_UP:
			ret = s2mpg1415_update_reg(gc->id, gc->i2c,
						   GPIO_CTRL_BASE[gc->id] + pin,
						   GPIO_PU_MASK, GPIO_PU_MASK);
			if (ret)
				return ret;
			break;
		case PIN_CONFIG_DRIVE_PUSH_PULL:
			/* do nothing */
			break;
		case PIN_CONFIG_DRIVE_STRENGTH:
			switch (argument) {
			case 2:
				data = 0;
				break;
			case 4:
				data = GPIO_DRV_STR_MASK;
				break;
			default:
				pr_err("Drive-strength %umA not supported\n",
				       argument);
				return -ENOTSUPP;
			}
			ret = s2mpg1415_update_reg(gc->id, gc->i2c,
						   GPIO_CTRL_BASE[gc->id] + pin,
						   data, GPIO_DRV_STR_MASK);
			if (ret)
				return ret;
			break;
		case PIN_CONFIG_INPUT_ENABLE:
			if (!argument)
				return -EINVAL;
			break;
		case PIN_CONFIG_OUTPUT:
			data = argument ? GPIO_OUT_MASK : 0;
			ret = s2mpg1415_update_reg(gc->id, gc->i2c,
						   GPIO_CTRL_BASE[gc->id] + pin,
						   data, GPIO_OUT_MASK);
			if (ret)
				return ret;
			argument = 1;
			fallthrough;
		case PIN_CONFIG_OUTPUT_ENABLE:
			data = argument ? GPIO_OEN_MASK : 0;
			ret = s2mpg1415_update_reg(gc->id, gc->i2c,
						   GPIO_CTRL_BASE[gc->id] + pin,
						   data, GPIO_OEN_MASK);
			if (ret)
				return ret;
			break;
		default:
			return -ENOTSUPP;
		}
	}

	return 0;
}

static int pinmux_get_funcs_count(struct pinctrl_dev *pctldev)
{
	return 0;
}

static const char *pinmux_get_func_name(struct pinctrl_dev *pctldev,
					unsigned int function)
{
	return NULL;
}

static int pinmux_get_func_groups(struct pinctrl_dev *pctldev,
				  unsigned int function,
				  const char *const **groups,
				  unsigned int *const num_groups)
{
	return -ENOTSUPP;
}

static int pinmux_set_mux(struct pinctrl_dev *pctldev, unsigned int function,
			  unsigned int group)
{
	return -ENOTSUPP;
}

static int pinmux_gpio_set_direction(struct pinctrl_dev *pctldev,
				     struct pinctrl_gpio_range *range,
				     unsigned int offset, bool input)
{
	unsigned long config =
		pinconf_to_config_packed(PIN_CONFIG_OUTPUT_ENABLE, !input);
	return s2mpg1415_pinconf_set(pctldev, offset, &config, 1);
}

static int pinctrl_get_groups_count(struct pinctrl_dev *pctldev)
{
	return 0;
}

static const char *pinctrl_get_group_name(struct pinctrl_dev *pctldev,
					  unsigned int group)
{
	return NULL;
}

static int s2mpg1415_gpio_probe(struct platform_device *pdev)
{
	int ret;
	struct s2mpg1415_gpio *s2mpg1415_gpio =
		devm_kzalloc(&pdev->dev,
			     sizeof(struct s2mpg1415_gpio), GFP_KERNEL);
	struct pinctrl_dev *pctl;
	u32 ngpios;
	const char *pinctrl_of_name = NULL;
	struct kbase_device *kbdev_parent = dev_get_drvdata(pdev->dev.parent);

	if (!s2mpg1415_gpio)
		return -ENOMEM;

	s2mpg1415_gpio->dev = &pdev->dev;
	s2mpg1415_gpio->id = pdev->id_entry->driver_data;
	switch (s2mpg1415_gpio->id) {
	case ID_S2MPG14:
		s2mpg1415_gpio->i2c = ((struct s2mpg14_dev *)kbdev_parent)->gpio;
		pinctrl_of_name = "s2mpg14_pinctrl";
		s2mpg1415_gpio->pctrl.pins = s2mpg14_pins;
		s2mpg1415_gpio->pctrl.npins = ARRAY_SIZE(s2mpg14_pins);
		break;
	case ID_S2MPG15:
		s2mpg1415_gpio->i2c = ((struct s2mpg15_dev *)kbdev_parent)->gpio;
		pinctrl_of_name = "s2mpg15_pinctrl";
		s2mpg1415_gpio->pctrl.pins = s2mpg15_pins;
		s2mpg1415_gpio->pctrl.npins = ARRAY_SIZE(s2mpg15_pins);
		break;
	default:
		return -EINVAL;
	}

	/* GPIO config */
	s2mpg1415_gpio->gc.label = pdev->name;
	s2mpg1415_gpio->gc.parent = &pdev->dev;
	s2mpg1415_gpio->gc.get_direction = s2mpg1415_gpio_get_direction;
	s2mpg1415_gpio->gc.get = s2mpg1415_gpio_get;
	s2mpg1415_gpio->gc.set = s2mpg1415_gpio_set;
	s2mpg1415_gpio->gc.direction_input = s2mpg1415_gpio_direction_input;
	s2mpg1415_gpio->gc.direction_output = s2mpg1415_gpio_direction_output;
	s2mpg1415_gpio->gc.base = -1;
	s2mpg1415_gpio->gc.can_sleep = true;
	s2mpg1415_gpio->gc.of_node =
		of_find_node_by_name(pdev->dev.parent->of_node, pdev->name);
	s2mpg1415_gpio->gc.set_config = gpiochip_generic_config;
	s2mpg1415_gpio->gc.request = gpiochip_generic_request;
	s2mpg1415_gpio->gc.free = gpiochip_generic_free;

	if (!s2mpg1415_gpio->gc.of_node) {
		dev_err(&pdev->dev, "Failed to find %s DT node\n", pdev->name);
		return -EINVAL;
	}
	if (of_property_read_u32(s2mpg1415_gpio->gc.of_node, "ngpios", &ngpios)) {
		dev_err(&pdev->dev, "Failed to get ngpios from %s DT node\n",
			pdev->name);
		return -EINVAL;
	}
	s2mpg1415_gpio->gc.ngpio = ngpios;

	/* pinctrl config */
	s2mpg1415_gpio->pctrl_ops.get_groups_count = pinctrl_get_groups_count;
	s2mpg1415_gpio->pctrl_ops.get_group_name = pinctrl_get_group_name;
	s2mpg1415_gpio->pctrl_ops.dt_node_to_map =
		pinconf_generic_dt_node_to_map_pin;
	s2mpg1415_gpio->pctrl_ops.dt_free_map = pinconf_generic_dt_free_map;

	s2mpg1415_gpio->pmux_ops.get_functions_count = pinmux_get_funcs_count;
	s2mpg1415_gpio->pmux_ops.get_function_name = pinmux_get_func_name;
	s2mpg1415_gpio->pmux_ops.get_function_groups = pinmux_get_func_groups;
	s2mpg1415_gpio->pmux_ops.set_mux = pinmux_set_mux;
	s2mpg1415_gpio->pmux_ops.gpio_set_direction = pinmux_gpio_set_direction;

	s2mpg1415_gpio->pconf_ops.is_generic = true;
	s2mpg1415_gpio->pconf_ops.pin_config_get = s2mpg1415_pinconf_get;
	s2mpg1415_gpio->pconf_ops.pin_config_set = s2mpg1415_pinconf_set;

	/* pins defined in chip id switch statement */
	s2mpg1415_gpio->pctrl.pctlops = &s2mpg1415_gpio->pctrl_ops;
	s2mpg1415_gpio->pctrl.pmxops = &s2mpg1415_gpio->pmux_ops;
	s2mpg1415_gpio->pctrl.confops = &s2mpg1415_gpio->pconf_ops;
	s2mpg1415_gpio->pctrl.owner = THIS_MODULE;
	s2mpg1415_gpio->pctrl.name = dev_name(&pdev->dev);

	pdev->dev.of_node = of_find_node_by_name(pdev->dev.parent->of_node,
						 pinctrl_of_name);
	if (!pdev->dev.of_node) {
		dev_err(&pdev->dev, "Failed to find %s DT node\n",
			pinctrl_of_name);
		return -EINVAL;
	}

	ret = devm_pinctrl_register_and_init(&pdev->dev, &s2mpg1415_gpio->pctrl,
					     s2mpg1415_gpio, &pctl);
	if (ret < 0) {
		dev_err(&pdev->dev, "Failed to register pin_ctrl: %d\n", ret);
		return ret;
	}

	ret = pinctrl_enable(pctl);
	if (ret < 0) {
		dev_err(&pdev->dev, "Failed to enable pin_ctrl: %d\n", ret);
		return ret;
	}

	ret = devm_gpiochip_add_data(&pdev->dev, &s2mpg1415_gpio->gc,
				     s2mpg1415_gpio);
	if (ret < 0) {
		dev_err(&pdev->dev, "Failed to register gpio_chip: %d\n", ret);
		return ret;
	}

	platform_set_drvdata(pdev, s2mpg1415_gpio);

	return 0;
}

static int s2mpg1415_gpio_remove(struct platform_device *pdev)
{
	return 0;
}

static const struct platform_device_id s2mpg1415_gpio_id[] = {
	{ "s2mpg14_gpio", ID_S2MPG14 },
	{ "s2mpg15_gpio", ID_S2MPG15 },
	{},
};
MODULE_DEVICE_TABLE(platform, s2mpg1415_gpio_id);

static struct platform_driver s2mpg1415_gpio_driver = {
	.driver = {
		   .name = "s2mpg1415_gpio",
		   .owner = THIS_MODULE,
		   },
	.probe = s2mpg1415_gpio_probe,
	.remove = s2mpg1415_gpio_remove,
	.id_table = s2mpg1415_gpio_id,
};

static int __init s2mpg1415_gpio_init(void)
{
	return platform_driver_register(&s2mpg1415_gpio_driver);
}
subsys_initcall(s2mpg1415_gpio_init);

static void __exit s2mpg1415_gpio_exit(void)
{
	platform_driver_unregister(&s2mpg1415_gpio_driver);
}
module_exit(s2mpg1415_gpio_exit);

MODULE_DESCRIPTION("S2MPG14 S2MPG15 GPIO Driver");
MODULE_AUTHOR("Thierry Strudel <tstrudel@google.com>");
MODULE_LICENSE("GPL");
