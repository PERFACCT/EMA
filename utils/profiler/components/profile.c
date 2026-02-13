#include <unistd.h>

#include <EMA.h>

#include "display.h"
#include "measurements.h"
#include "options.h"
#include "profile.h"
#include "utils.h"

extern ProfilerConfig profiler_config;

static unsigned long long slow_energy_value_request_runtime = 0;

Energy _get_energy(const Device* device) {
    struct Energy energy;

    unsigned long long energy_value_request_start_ts = EMA_get_time_in_us();
    energy.value = EMA_plugin_get_energy_uj(device);
    unsigned long long energy_value_request_end_ts = EMA_get_time_in_us();

    energy.request_time =
        energy_value_request_end_ts - energy_value_request_start_ts;

    return energy;
}

unsigned long long _estimate_slow_energy_value_request_runtime(
    const Device* device,
    const unsigned int delay
)
{
    usleep(delay);
    Energy energy = _get_energy(device);
    return energy.request_time;
}

void _process_energy_measurement(
    const Device* device,
    unsigned long long* e0,
    unsigned long long* e1,
    MeasurementsSourcePayload* measurements_payload
)
{
    *e1 = EMA_plugin_get_energy_uj(device);
    if( *e1 != *e0 )
    {
        measurements_payload->energy_inner += *e1 - *e0;
        if(profiler_config.verbose)
            printf(
                "Energy measured:              %llu uj\n",
                *e1 - *e0
            );

        if( check_metric(&profiler_config.metrics->change_frequency) )
            ++measurements_payload->number_of_energy_value_changes;
    }
}

void _process_execution_suspension(
    const unsigned int delay,
    const long long delay_runtime_diff
)
{
    /* Suspend execution if needed */
    if( !delay || delay_runtime_diff <= 0 )
        return;

    if( profiler_config.include_iteration_runtime_in_delay )
    {
        usleep(delay_runtime_diff);
    }
    else
    {
        usleep(delay);
    }
}

void process_iteration(
    const Device* device,
    unsigned long long* e0,
    unsigned long long* e1,
    const unsigned int delay,
    MeasurementsSourcePayload* measurements_payload
)
{
    unsigned long long iteration_end_ts;
    unsigned long long iteration_start_ts = EMA_get_time_in_us();

    struct Energy energy = _get_energy(device);
    measurements_payload->energy_inner_extra += energy.value - *e0;
    *e0 = energy.value;

    if( profiler_config.verbose )
    {
        printf(
            "Energy value request runtime: %llu us\n",
            energy.request_time
        );
        printf("Energy value:                 %llu uj\n", *e0);
    }

    long long delay_runtime_diff =
        delay - (
        (EMA_get_time_in_us() - iteration_start_ts) +
        slow_energy_value_request_runtime
        );

    if( profiler_config.verbose )
    {
        if(delay)
            printf(
                "Delay/runtime diff:           %lli us\n",
                delay_runtime_diff
            );
    }

    _process_execution_suspension(
        delay,
        delay_runtime_diff
    );
    
    _process_energy_measurement(device, e0, e1, measurements_payload);
    iteration_end_ts = EMA_get_time_in_us();

    measurements_payload->time_inner +=
        iteration_end_ts - iteration_start_ts;
    if( profiler_config.verbose )
    {
        printf(
            "Iteration runtime:            %llu us\n",
            iteration_end_ts - iteration_start_ts
        );
        printf("\n");
    }
}

int profile_device(const Device *device)
{
    if( profiler_config.number_of_iterations == 0 )
        return 0;

    const char* device_name = EMA_get_device_name(device);

    print_device_name_header(device_name);

    struct MeasurementsSourcePayload measurements_payload;

    unsigned int delay_in_us = profiler_config.delay_in_ms * 1000;

    if( delay_in_us )
        slow_energy_value_request_runtime =
            _estimate_slow_energy_value_request_runtime(device, delay_in_us);

    measurements_payload.time_inner = 0;
    measurements_payload.energy_inner = 0;
    measurements_payload.number_of_energy_value_changes = 0;

    int k = profiler_config.number_of_iterations;

    unsigned long long e1 = 0;
    unsigned long long e0 = EMA_plugin_get_energy_uj(device);

    measurements_payload.time_start = EMA_get_time_in_us();
    measurements_payload.energy_start = e0;

    while( k-- )
    {
        process_iteration(
            device,
            &e0,
            &e1,
            delay_in_us,
            &measurements_payload
        );
        if ( profiler_config.external_delay_in_ms )
        {
            unsigned int external_delay =
                profiler_config.external_delay_in_ms * 1e3;
            usleep(external_delay);
        }
    }

    measurements_payload.time_outer =
        EMA_get_time_in_us() - measurements_payload.time_start;
    measurements_payload.energy_outer =
        EMA_plugin_get_energy_uj(device) - measurements_payload.energy_start;

    if( profiler_config.verbose) {
        printf(
            "Time inner:                   %lluus\n",
            measurements_payload.time_inner
        );
        printf(
            "Time outer:                   %lluus\n\n",
            measurements_payload.time_outer
        );
    }

    ProfilerMeasurements measurements =
        get_profiler_measurements(&measurements_payload);


    display_measurements_for_device(&measurements);

    return 0;
}
