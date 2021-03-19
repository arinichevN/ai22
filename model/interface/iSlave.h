#ifndef MODEL_ISLAVE_H
#define MODEL_ISLAVE_H

#include "../../lib/common.h"

typedef struct islave_st iSlave;
struct islave_st{
	void *self;
	state_t *state;
	iSlave *other;
	void (*start)(void *);
	void (*stop)(void *);
	void (*setOutput)(void *, double, struct timespec);
	void (*outputFailed)(void *);
};

#endif 
