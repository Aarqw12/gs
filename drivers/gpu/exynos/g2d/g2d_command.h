/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2017 Samsung Electronics Co., Ltd.
 */

#ifndef _G2D_COMMAND_H_
#define _G2D_COMMAND_H_

#include "g2d_format.h"

struct g2d_device;
struct g2d_task;
struct g2d_layer;
struct g2d_task_data;

struct g2d_fmt {
	const char	*name;
	u32		fmtvalue;
	u8		bpp[G2D_MAX_PLANES];
	u8		num_planes;
};

void g2d_init_commands(struct g2d_task *task);
void g2d_complete_commands(struct g2d_task *task);
const struct g2d_fmt *g2d_find_format(u32 fmtval, unsigned long devcaps);
int g2d_import_commands(struct g2d_device *g2d_dev, struct g2d_task *task,
			struct g2d_task_data *data, unsigned int num_sources,
			struct g2d_reg *cmdaddr);
bool g2d_validate_source_commands(struct g2d_device *g2d_dev,
				  struct g2d_task *task,
				  unsigned int i, struct g2d_layer *source,
				  struct g2d_layer *target);
bool g2d_validate_target_commands(struct g2d_device *g2d_dev,
				  struct g2d_task *task);
size_t g2d_get_payload(struct g2d_reg cmd[], const struct g2d_fmt *fmt,
		       u32 flags, unsigned long caps, bool dst);
unsigned int g2d_get_payload_index(struct g2d_reg cmd[], const struct g2d_fmt *fmt,
				   unsigned int idx, unsigned int buffer_count,
				   unsigned long caps, u32 flags, bool dst);
bool g2d_prepare_source(struct g2d_task *task,
			struct g2d_layer *layer, int index);
bool g2d_prepare_target(struct g2d_task *task);

#endif /* _G2D_COMMAND_H_ */
