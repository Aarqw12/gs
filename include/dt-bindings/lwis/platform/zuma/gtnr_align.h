/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Google LWIS ZUMA GTNR_ALIGN Interrupt And Event Defines
 *
 * Copyright (c) 2022 Google, LLC
 */

#ifndef DT_BINDINGS_LWIS_PLATFORM_ZUMA_GTNR_ALIGN_H_
#define DT_BINDINGS_LWIS_PLATFORM_ZUMA_GTNR_ALIGN_H_

#include <dt-bindings/lwis/platform/common.h>

/* clang-format off */


#define GTNR_ALIGN_INT_BASE (HW_EVENT_MASK + 0)

#define GTNR_ALIGN_INT_MSA_FRAME_DONE 0
#define GTNR_ALIGN_INT_NM_FRAME_DONE 1
#define GTNR_ALIGN_INT_FRO_DONE 2



#define GTNR_ALIGN_SECU_INT_MUTE_BASE (HW_EVENT_MASK + 32)

#define GTNR_ALIGN_SECU_INT_MUTE_MUTE 0
#define GTNR_ALIGN_SECU_INT_MUTE_UNMUTE 1



#define GTNR_ALIGN_SECU_INT_SECU_FLG_BASE (HW_EVENT_MASK + 64)

#define GTNR_ALIGN_SECU_INT_SECU_FLG_SECURITY_FILTER_VIOLATION_WDMA 0
#define GTNR_ALIGN_SECU_INT_SECU_FLG_SECURITY_FILTER_VIOLATION_RDMA 1

/* clang-format on */

#define LWIS_PLATFORM_EVENT_ID_GTNR_ALIGN_INT_MSA_FRAME_DONE                                       \
	EVENT_ID(GTNR_ALIGN_INT_BASE, GTNR_ALIGN_INT_MSA_FRAME_DONE)
#define LWIS_PLATFORM_EVENT_ID_GTNR_ALIGN_INT_NM_FRAME_DONE                                        \
	EVENT_ID(GTNR_ALIGN_INT_BASE, GTNR_ALIGN_INT_NM_FRAME_DONE)
#define LWIS_PLATFORM_EVENT_ID_GTNR_ALIGN_INT_FRO_DONE                                             \
	EVENT_ID(GTNR_ALIGN_INT_BASE, GTNR_ALIGN_INT_FRO_DONE)

#define LWIS_PLATFORM_EVENT_ID_GTNR_ALIGN_SECU_INT_MUTE_MUTE                                       \
	EVENT_ID(GTNR_ALIGN_SECU_INT_MUTE_BASE, GTNR_ALIGN_SECU_INT_MUTE_MUTE)
#define LWIS_PLATFORM_EVENT_ID_GTNR_ALIGN_SECU_INT_MUTE_UNMUTE                                     \
	EVENT_ID(GTNR_ALIGN_SECU_INT_MUTE_BASE, GTNR_ALIGN_SECU_INT_MUTE_UNMUTE)

#define LWIS_PLATFORM_EVENT_ID_GTNR_ALIGN_SECU_INT_SECU_FLG_SECURITY_FILTER_VIOLATION_WDMA         \
	EVENT_ID(GTNR_ALIGN_SECU_INT_SECU_FLG_BASE,                                                \
		 GTNR_ALIGN_SECU_INT_SECU_FLG_SECURITY_FILTER_VIOLATION_WDMA)
#define LWIS_PLATFORM_EVENT_ID_GTNR_ALIGN_SECU_INT_SECU_FLG_SECURITY_FILTER_VIOLATION_RDMA         \
	EVENT_ID(GTNR_ALIGN_SECU_INT_SECU_FLG_BASE,                                                \
		 GTNR_ALIGN_SECU_INT_SECU_FLG_SECURITY_FILTER_VIOLATION_RDMA)

#endif /* DT_BINDINGS_LWIS_PLATFORM_ZUMA_GTNR_ALIGN_H_ */
