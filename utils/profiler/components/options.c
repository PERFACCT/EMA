#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "options.h"

extern ProfilerConfig profiler_config;

int parse_command_line_options(int argc, char **argv)
/*
 *  Parse command line options and set corresponding values
 *  in profiler config struct
 */
{
    static const struct option long_options[] =
    {
        {"iterations", required_argument, 0, 'i'},
        {"external-delay", required_argument, 0, 'E'},
        {"delay-runtime-included", required_argument, 0, 'd'},
        {"delay-runtime-excluded", required_argument, 0, 'D'},
        {"limit-runtime-included", required_argument, 0, 'l'},
        {"limit-runtime-excluded", required_argument, 0, 'L'},
        {"prefix", required_argument, 0, 'p'},
        {"verbose", no_argument, 0, 'v'},
        {"update-frequency", no_argument, 0, 'u'},
        {"change-frequency", no_argument, 0, 'c'},
        {"time", no_argument, 0, 't'},
        {"energy", no_argument, 0, 'e'},
        {"power-average", no_argument, 0, 'w'},
        {"help", no_argument, 0, 'h'},
        {0}
    };

    while( true )
    {
        int index = -1;
        int result = getopt_long(
            argc,
            argv,
            "i:E:d:D:l:L:p:vuctewh",
            long_options,
            &index
        );

        if( result == -1 )
            break; /* end of list */

        switch(result)
        {
            case 'i':
                profiler_config.number_of_iterations = atoi(optarg);
                break;
            case 'E':
                profiler_config.external_delay_in_ms = atof(optarg);
                break;
            case 'd':
                profiler_config.delay_in_ms = atof(optarg);
                profiler_config.include_iteration_runtime_in_delay = true;
                break;
            case 'D':
                profiler_config.delay_in_ms = atof(optarg);
                profiler_config.include_iteration_runtime_in_delay = false;
                break;
            case 'l':
                profiler_config.time_limit = atof(optarg);
                profiler_config.include_iteration_runtime_in_delay = true;
                break;
            case 'L':
                profiler_config.time_limit = atof(optarg);
                profiler_config.include_iteration_runtime_in_delay = false;
                break;
            case 'p':
                profiler_config.device_name_pattern = strdup(optarg);
                break;
            case 'v':
                profiler_config.verbose = true;
                break;
            case 'u':
                profiler_config.metrics->all = false;
                profiler_config.metrics->update_frequency = true;
                break;
            case 'c':
                profiler_config.metrics->all = false;
                profiler_config.metrics->change_frequency = true;
                break;
            case 't':
                profiler_config.metrics->all = false;
                profiler_config.metrics->time = true;
                break;
            case 'e':
                profiler_config.metrics->all = false;
                profiler_config.metrics->energy = true;
                break;
            case 'w':
                profiler_config.metrics->all = false;
                profiler_config.metrics->power_average = true;
                break;
            case 'h':
                printf(HELP_MESSAGE);
                return 0;
            default:
                break;
        }
    }
    printf("\n");
    return 1;
}
