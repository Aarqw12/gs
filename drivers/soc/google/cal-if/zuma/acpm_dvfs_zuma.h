/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
 */

/* Making sure margin_id and acpm_dvfs_id are sync'ed */
enum acpm_dvfs_id {
	MIF = ACPM_VCLK_TYPE,
	INT,
	CPUCL0,
	CPUCL1,
	CPUCL2,
	DSU,
	BCI,
	G3D,
	G3DL2,
	TPU,
	INTCAM,
	TNR,
	CAM,
	MFC,
	DISP,
	AOC,
	BW,
};

struct vclk acpm_vclk_list[] = {
	CMUCAL_ACPM_VCLK(MIF, NULL, NULL, NULL, NULL),
	CMUCAL_ACPM_VCLK(INT, NULL, NULL, NULL, NULL),
	CMUCAL_ACPM_VCLK(CPUCL0, NULL, NULL, NULL, NULL),
	CMUCAL_ACPM_VCLK(CPUCL1, NULL, NULL, NULL, NULL),
	CMUCAL_ACPM_VCLK(CPUCL2, NULL, NULL, NULL, NULL),
	CMUCAL_ACPM_VCLK(DSU, NULL, NULL, NULL, NULL),
	CMUCAL_ACPM_VCLK(BCI, NULL, NULL, NULL, NULL),
	CMUCAL_ACPM_VCLK(G3D, NULL, NULL, NULL, NULL),
	CMUCAL_ACPM_VCLK(G3DL2, NULL, NULL, NULL, NULL),
	CMUCAL_ACPM_VCLK(TPU, NULL, NULL, NULL, NULL),
	CMUCAL_ACPM_VCLK(INTCAM, NULL, NULL, NULL, NULL),
	CMUCAL_ACPM_VCLK(TNR, NULL, NULL, NULL, NULL),
	CMUCAL_ACPM_VCLK(CAM, NULL, NULL, NULL, NULL),
	CMUCAL_ACPM_VCLK(MFC, NULL, NULL, NULL, NULL),
	CMUCAL_ACPM_VCLK(DISP, NULL, NULL, NULL, NULL),
	CMUCAL_ACPM_VCLK(BW, NULL, NULL, NULL, NULL),
};

unsigned int acpm_vclk_size = ARRAY_SIZE(acpm_vclk_list);
