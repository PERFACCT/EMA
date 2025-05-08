#define _GNU_SOURCE
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <EMA.h>

#define TS_BUF_SIZE 64
#define OUT_FILENAME_SIZE 64

#define printl(MSG) do { printf(MSG "\n"); } while (0)

#define HANDLE_ERROR(ERR) do { \
    if (ERR) { \
        printf("Error: %d\n", ERR); \
        status = EXIT_FAILURE; \
        goto exit; \
    } \
} while (0)

void get_iso_time(char* buffer, size_t buff_size) {
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, buff_size, "%Y-%m-%dT%H:%M:%S", timeinfo);
    snprintf(
        buffer + strlen(buffer),
        buff_size - strlen(buffer),
        "%c%02d:%02d",
        (timeinfo->tm_gmtoff >= 0 ? '+' : '-'),
        abs(timeinfo->tm_gmtoff) / 3600,
        (abs(timeinfo->tm_gmtoff) % 3600) / 60
    );
}

void append_quoted(char *dest, const char *src) {
    strcat(dest, "\"");

    for (; *src; ++src) {
        if (*src == '"') {
            strcat(dest, "\\\"");
        } else {
            strncat(dest, src, 1);
        }
    }
    strcat(dest, "\" ");
}

int main(int argc, char** argv) {
    int status = EXIT_SUCCESS;
    FILE* f = NULL;
    char* cmd = NULL;
    char ts_start[TS_BUF_SIZE];
    char ts_end[TS_BUF_SIZE];

    if (argc < 2) {
        printl("USAGE: ema_measure CMD [args...]");
        status =  EXIT_FAILURE;
        goto exit;
    }

    long arg_max = sysconf(_SC_ARG_MAX);
    if (arg_max == -1) {
        perror("sysconf");
        status = EXIT_FAILURE;
        goto exit;
    }

    cmd = calloc(0, sizeof(char) * arg_max);
    if (!cmd) {
        perror("calloc");
        status = EXIT_FAILURE;
        goto exit;
    }

    for (int i = 1; i < argc; i++) {
        append_quoted(cmd, argv[i]);
    }
    printf("CMD: %s\n", cmd);

    printl("Initiallizing EMA...");
    int err = EMA_init(NULL);
    HANDLE_ERROR(err);

    get_iso_time(ts_start, TS_BUF_SIZE);

    EMA_REGION_DECLARE(region);
    EMA_REGION_DEFINE(&region, "region");

    EMA_REGION_BEGIN(region);

    system(cmd);

    EMA_REGION_END(region);
    
    get_iso_time(ts_end, TS_BUF_SIZE);

    printl("Finalizing EMA...");
    err = EMA_finalize();
    HANDLE_ERROR(err);

    pid_t pid = getpid();

    char filename[OUT_FILENAME_SIZE];
    snprintf(filename, OUT_FILENAME_SIZE, "timestamps.EMA.%u", pid);

    f = fopen(filename, "w");
    if (f == NULL) {
        perror("Failed to open timestamps file!");
        status = EXIT_FAILURE;
        goto exit;
    }

    fprintf(f, "ts_start,ts_end\n");
    fprintf(f, "%s,%s\n", ts_start, ts_end);

exit:
    
    if (cmd) free(cmd);
    if (f) fclose(f);

    return status;
}
