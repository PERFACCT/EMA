#ifndef PROFILE_MEASUREMENTS_H
#define PROFILE_MEASUREMENTS_H

#include <EMA.h>

#include "profile.h"

typedef struct MeasurementsSourcePayload {
  unsigned long long number_of_energy_value_changes;
  /* In microseconds */
  unsigned long long time_start;
  unsigned long long time_inner;
  unsigned long long time_outer;
  /* In micro-Joule */
  unsigned long long energy_start;
  unsigned long long energy_inner;
  unsigned long long energy_inner_extra;
  unsigned long long energy_outer;
} MeasurementsSourcePayload;

typedef struct Frequencies {
  /* In seconds */
  double change_frequency;
  double update_frequency;
} Frequencies;

typedef struct EnergyMeasurements {
  /* In Joule */
  double inner;
  double outer;
  double per_iteration;
  double delta; /* Inner/outer loop */
} EnergyMeasurements;

typedef struct TimeMeasurements {
  /* In seconds */
  double inner;
  double outer;
  /* In microseconds */
  unsigned long long per_iteration;
  unsigned long long diff; /* Inner/outer loop */
} TimeMeasurements;

typedef struct ProfilerMeasurements {
  double power_average_inner;
  double power_average_outer;
  TimeMeasurements time;
  EnergyMeasurements energy;
  Frequencies frequencies;
} ProfilerMeasurements;

Frequencies get_frequencies(
  /* In microseconds */
  unsigned long long time_inner,
  unsigned long long time_outer,
  unsigned long long number_of_energy_value_changes
);

ProfilerMeasurements get_profiler_measurements(
  MeasurementsSourcePayload* payload
);

#endif
