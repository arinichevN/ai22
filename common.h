#ifndef COMMON_H
#define COMMON_H

#include <SoftwareSerial.h>

enum StateE {
	INIT,
	OFF,
	FAILURE,
	DISABLE,
	DONE,
	RUN,

    READ_COMMAND,
  READ_INT,
  READ_DOUBLE
};

struct FTS {
	double value;
	unsigned long tm;
	int state;
};

#endif 
