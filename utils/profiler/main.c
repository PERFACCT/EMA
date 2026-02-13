#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <EMA.h>

#include "components/constants.h"
#include "components/display.h"
#include "components/options.h"

struct ProfilerConfig profiler_config = {
  DEFAULT_DELAY_IN_MILLISECONDS,
  DEFAULT_DELAY_IN_MILLISECONDS,
  DEFAULT_LIMIT,
  DEFAULT_ITERATIONS,
  DEFAULT_NUMBER_OF_DEVICES,
  DEFAULT_PATTERN,
  DEFAULT_INCLUDE_ITERATION_RUNTIME_IN_DELAY,
  DEFAULT_METRICS
};

int profile_devices(
    DevicePtrArray* devices
)
{
    for( size_t i = 0; i < devices->size; ++i )
    {
        const Device* device = devices->array[i];
        const char* device_name = EMA_get_device_name(device);

        if(
            profiler_config.device_name_pattern
            && !strstr(device_name, profiler_config.device_name_pattern)
          )
            continue;

        profile_device(device);
    }

    return 0;
}

int main(int argc, char **argv)
{
    /* By default all metrics are expected to be calculated and displayed */
    Metrics metrics = {true, false, false, false, false, false};
    profiler_config.metrics = &metrics;

    if( !parse_command_line_options(argc, argv) )
    {
        return 0;
    }
    else
    {
        /* Initializing EMA. */
        int err = EMA_init(NULL);

        /* Checking error. */
        if( err )
            return 1;

        DevicePtrArray devices = EMA_get_devices();

        profiler_config.number_of_devices = devices.size;

        if( profiler_config.time_limit )
        {
            double delay_in_seconds =
                profiler_config.time_limit /
                profiler_config.number_of_iterations;

            profiler_config.delay_in_ms =
                floor(delay_in_seconds*1e3);
        }

        display_general_profiling_info();

        if( profile_devices(&devices) )
            printf("Error occured while profiling devices!\n");

        return 0;
    }
}
