#include "measurements.h"
#include "options.h"
#include "utils.h"

extern ProfilerConfig profiler_config;

typedef struct TimeRange {
    double from;
    double to;
} TimeRange;

TimeRange get_estimated_time_range()
{
    /*
    *  Estimation for a specific compute node.
    *  Perhaps bad estimation in general.
    */

    struct TimeRange time_range;

    time_range.from =
        (double) profiler_config.delay_in_ms/1000 *
        profiler_config.number_of_iterations;
    time_range.to =
        profiler_config.number_of_devices *
        ((double) profiler_config.delay_in_ms/1000 + 666E-6) *
        profiler_config.number_of_iterations;

    return time_range;
}

void display_general_profiling_info()
{
    TimeRange estimated_time = get_estimated_time_range();

    printf("Run plugin profiling with\n\n");
    printf(
        "Max devices:          %lu\n",
        profiler_config.number_of_devices
    );
    printf(
        "Iterations:           %u\n",
        profiler_config.number_of_iterations
    );
    printf(
        "Delay                 %.2f ms %s\n",
        profiler_config.delay_in_ms,
        profiler_config.include_iteration_runtime_in_delay
            ? "( Measurements loop runtime included )"
            : ""
    );
    if( profiler_config.external_delay_in_ms )
        printf(
            "External sleep:       %.2f ms\n",
            profiler_config.external_delay_in_ms
        );
    if( profiler_config.device_name_pattern )
        printf(
            "Device names with pattern:  %s\n",
            profiler_config.device_name_pattern
        );

    printf(
        "Estimated runtime:    %.2fs to %.2fs\n",
        estimated_time.from,
        estimated_time.to
    );
    printf("\n");
}

void print_device_name_header(const char* device_name)
{
    printf("\n");
    printf("===== Device profiling =================================\n\n");
    printf("Device: %s\n", device_name);
    printf("\n");
}

void print_time_measurements(const ProfilerMeasurements* measurements)
{
    printf("-- Time Metrics\n\n");
    printf("Time inner:                %.6fs\n", measurements->time.inner);
    printf("Time outer:                %.6fs\n", measurements->time.outer);
    printf(
        "Time per iteration:        %lldus\n",
        measurements->time.per_iteration
    );
    printf("Time diff i/o loop:        %lluus\n", measurements->time.diff);
    printf("\n");
}

void print_energy_measurements(const ProfilerMeasurements* measurements)
{
    printf("-- Energy Metrics\n\n");
    printf(
        "Energy inner:              %.6fJ\n",
        measurements->energy.inner
    );
    printf(
        "Energy outer:              %.6fJ\n",
        measurements->energy.outer
    );
    printf(
        "Energy per iteration*:     %.6fJ\n",
        measurements->energy.per_iteration
    );
    printf("Energy delta i/o loop:     %.6fJ\n", measurements->energy.delta);
}

void display_measurements_for_device(
  const ProfilerMeasurements* measurements
)
{
    printf("=====Metrics============================================\n\n");
    printf(
        "Iterations:                %d\n",
        profiler_config.number_of_iterations
    );
    if( check_metric(&profiler_config.metrics->power_average) )
    {
        printf(
            "Power avg. inner:          %.6fW\n",
            measurements->power_average_inner
        );
        printf(
            "Power avg. outer:          %.6fW\n",
            measurements->power_average_outer
        );
    }
    printf("\n");

    if( check_metric(&profiler_config.metrics->update_frequency) )
        printf(
            "Update frequency*:         %.6fs\n",
            measurements->frequencies.update_frequency
        );
    if( check_metric(&profiler_config.metrics->change_frequency) )
        printf(
            "Change frequency*:         %.6fs\n",
            measurements->frequencies.change_frequency
        );
    printf("\n");

    if( check_metric(&profiler_config.metrics->time) )
        print_time_measurements(measurements);
    if( check_metric(&profiler_config.metrics->energy) )
        print_energy_measurements(measurements);

    printf("\n========================================================\n");
}
