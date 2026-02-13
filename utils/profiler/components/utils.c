#include <stdbool.h>

#include "options.h"

extern ProfilerConfig profiler_config;

bool check_metric(const bool *metric)
/*
 *  A utility function for metrics boolean flags checking.
 *  When metrics.all flag is true all checks should return true.
 */
{
  return profiler_config.metrics->all || *metric;
}
