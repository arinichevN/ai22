#ifndef MODEL_IMASTER_H
#define MODEL_IMASTER_H

#include "../../lib/common.h"
#include "iSlave.h"

typedef struct {
	void *self;
	int (*start)(void *, iSlave *);
	int (*stop)(void *, iSlave *);
	int (*getChannelParam)(void *, dk_t, void *);
	void (*saveChannelParam)(void *, dk_t, void *);
} iMaster;

#endif 
