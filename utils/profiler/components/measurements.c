#include <EMA.h>

#include "measurements.h"
#include "options.h"
#include "utils.h"

extern ProfilerConfig profiler_config;

double to_seconds(double value) /* In micro seconds */
{
  return value / 1e6;
}

double get_power_average(
  unsigned long long time_outer,
  unsigned long long energy_outer
)
{
  return (double) energy_outer / (double) time_outer;
}

TimeMeasurements get_time_measurements(
  unsigned long long time_outer,
  unsigned long long time_inner
)
{
  struct TimeMeasurements measurements;

  measurements.inner = to_seconds((double) time_inner);
  measurements.outer = to_seconds((double) time_outer);
  measurements.per_iteration =
    time_outer / profiler_config.number_of_iterations;
  measurements.diff = time_outer - time_inner;

  return measurements;
}

EnergyMeasurements get_energy_measurements(
  unsigned long long energy_start,
  unsigned long long energy_outer,
  unsigned long long energy_inner,
  unsigned long long time_outer
)
{
  struct EnergyMeasurements measurements;

  measurements.outer = (double) energy_outer / 1e6;
  measurements.inner = (double) energy_inner / 1e6;
  measurements.per_iteration = 
    energy_outer / (double) profiler_config.number_of_iterations / 1e6;
  measurements.delta =
    (energy_outer - energy_inner) / 1e6;

  return measurements;
}

Frequencies get_frequencies(
  unsigned long long time_inner, /* In microseconds */
  unsigned long long time_outer, /* In microseconds */
  unsigned long long number_of_energy_value_changes
)
{
  /* Returns frequencies in seconds (display ready) */
  struct Frequencies freq;

  if( check_metric(&profiler_config.metrics->change_frequency) )
  {
    double change_frequency_us =
      time_inner / (double) number_of_energy_value_changes;
    freq.change_frequency = to_seconds(change_frequency_us);
  }

  if( check_metric(&profiler_config.metrics->update_frequency) )
  {
    double update_frequency_us =
      time_outer / (double) profiler_config.number_of_iterations;
    freq.update_frequency = to_seconds(update_frequency_us);
  }

  return freq;
}

ProfilerMeasurements get_profiler_measurements(
  MeasurementsSourcePayload* payload
)
{
  struct ProfilerMeasurements measurements;

  if( check_metric(&profiler_config.metrics->power_average) )
  {
    measurements.power_average_outer =
      get_power_average(payload->time_outer, payload->energy_outer);
    measurements.power_average_inner =
      get_power_average(payload->time_inner, payload->energy_inner);
  }

  if( check_metric(&profiler_config.metrics->time) )
  {
    measurements.time = get_time_measurements(
      payload->time_outer,
      payload->time_inner
    );
  }

  if( check_metric(&profiler_config.metrics->energy) )
  {
    measurements.energy = get_energy_measurements(
      payload->energy_start,
      payload->energy_outer,
      payload->energy_inner,
      payload->time_outer
    );
  }

  measurements.frequencies = get_frequencies(
    payload->time_inner,
    payload->time_outer,
    payload->number_of_energy_value_changes
  );

  return measurements;
}
