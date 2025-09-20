#ifndef PROFILER_PROFILE_H
#define PROFILER_PROFILE_H

#include <EMA.h>

typedef struct Energy {
  unsigned long long value;
  unsigned long long request_time;
} Energy;

int profile_device(const Device *device);

#endif
