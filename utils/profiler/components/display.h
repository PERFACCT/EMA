#ifndef PROFILE_DISPLAY_H
#define PROFILE_DISPLAY_H

#include "measurements.h"

void print_device_name_header(const char* device_name);

void display_measurements_for_device(
  const ProfilerMeasurements* measurements
);

//TODO: Come up with a better name
void display_general_profiling_info();

#endif
