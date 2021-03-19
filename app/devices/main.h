#ifndef APP_DEVICES_H
#define APP_DEVICES_H

#include "../../lib/dstructure.h"
#include "../../lib/serial.h"
#include "../../lib/common.h"
#include "../../lib/acp/command/main.h"
#ifdef USE_AOIDS
#include "../../lib/aoid.h"
#endif
#ifdef USE_NOIDS
#include "../../lib/noid.h"
#endif
#include "../../model/Device/main.h"
#include "../../model/Device/llist.h"
#include "../AppSerial/main.h"

#define FOREACH_DEVICE FOREACH_LLIST_N(device, devices, Device)

extern int devices_activeExists();

extern int devices_getIdFirst(int *out);

extern void devices_begin(int default_btn);

#ifdef USE_AOIDS
extern void devices_buildAoids(Aoid *next_oid, Aoid *parent_oid, size_t *id);
#endif

#ifdef USE_NOIDS
extern Noid *devices_buildNoids(Noid *prev);
#endif

#endif 
