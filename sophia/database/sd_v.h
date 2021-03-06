#ifndef SD_V_H_
#define SD_V_H_

/*
 * sophia database
 * sphia.org
 *
 * Copyright (c) Dmitry Simonenko
 * BSD License
*/

typedef struct sdv sdv;

struct sdv {
	uint32_t offset;
	uint8_t  flags;
} sspacked;

extern svif sd_vif;
extern svif sd_vrawif;

#endif
