#ifndef PROFILE_OPTIONS_H
#define PROFILE_OPTIONS_H

#include <stdbool.h>

#define HELP_MESSAGE "\
Usage\n\
\tprofile [OPTIONS]\n\n\
Profiles devices and prints output metrics.\n\
By default if non of the metrics options passed all metrics are included in the output.\n\
You can chain the metrics' options like this: -uct (for update and change frequencies and time)\n\n\
Options [arguments requred]:\n\
    --iterations, -i: Number of iterations.\n\
    --external-delay, -E: Add an external delay after inner loop process.[3]\n\
    --delay-runtime-included, -d: Add a delay in ms per iteration. Energy value request runtime included.[1]\n\
    --delay-runtime-excluded, -D: Add a delay in ms per iteration. Energy value request runtime excluded.[1]\n\
    --limit-runtime-included, -l: Time limit in seconds for each device profiling run. Energy value request runtime included.[2]\n\
    --limit-runtime-excluded, -L: Time limit in seconds for each device profiling run. Energy value request runtime excluded.[2]\n\
    --pattern, -p: Just execute devices with given pattern in the names.\n\n\
Metrics options [no arguments, chaining possible]:\n\
    --verbose, -v: Print values of the inner loop iterations.\n\
    --update-frequency, -u: Show update frequency in the output.\n\
    --change-frequency, -c: Show change frequency in the output.\n\
    --time, -t: Show time metrics in the output.\n\
    --energy, -e: Show energy metrics in the output.\n\
    --power-average, -w: Show power average in the output.\n\n\
    --help, -h: Display this help message.\n\n\
[1] When energy value request runtime is included in the delay (-d) the actual execution suspension time will be calculated as 'delay - runtime'\n\
    In iterations where runtime will be greater then user-specified delay the execution suspension will be skipped.\n\
    When specifying the delay with -D flag the execution will always be suspended for the provided ammount of time\n\
    and the energy value request runtime will be the overhead.\n\n\
[2] When either -l or -L options are provided the delay value will be calculated automatically based on iterations number.\n\
    Delay = time limit / number of iterations.\n\
    When specifying the limit with -L flag the execution will always be suspended for the provided ammount of time\n\
    and the energy value request runtime will be the overhead, in other case the energy request runtime will be included in the delay.\n\n\
[3] This is needed to model a scenario where we have a Region that we want to measure in out inner loop\n\
    and some extra code that is whithin our inner loop but does not have to be measured.\n\n"

typedef struct Metrics {
  bool all;
  bool energy;
  bool time;
  bool power_average;

  bool update_frequency;
  bool change_frequency;
} Metrics;

typedef struct ProfilerConfig {
  double delay_in_ms;
  double external_delay_in_ms;
  double time_limit; /* In seconds */
  unsigned int number_of_iterations;
  unsigned long int number_of_devices;
  const char* device_name_pattern;
  bool verbose;
  bool include_iteration_runtime_in_delay;
  Metrics* metrics;
} ProfilerConfig;

int parse_command_line_options(int argc, char **argv);

#endif
