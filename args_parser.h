/***** args_parser.h *****
* Author : flekystyley
* Date   : 31.1.2019
* Brief  : parsing command line arguments for slowC
**************************/
#include <stdbool.h>
#ifndef ARGS_PARSER
#define ARGS_PARSER

/**
* @struct
* help           -h flag indicator, print help
* contentLength  -c contentLength, Maximum length of POST body (bytes)
* rampUpInterval -r rampUpInterval, Interval between new connections' acquisitions for a single dial worker (see dialWorkersCount)
* sleepInterval  -s sleepInterval, Sleep interval between subsequent packet transmissions
* testDuration   -t testDuration, Test duration
* victimURL      -v victimURL, VictimURL
* hostHeader     -h hostHeader, Host header value in case it is different than the hostname in victimURL
*/
typedef struct
{
    bool help;
    int content_length;
    int dial_worker_count;
    int ramp_up_interval;
    int sleep_interval;
    int test_duration;
    const char *victim_url;
    const char *host_header;
} Arguments;

/**
* @brief print program usage
* @param program name of the program (argv[0])
*/
void print_usage(const char *program);

/**
* @brief print detail usage with all command line arguments explained
* @param program name of the program (argv[0])
*/
void print_help(const char* program);

/**
* @brief initialize Arguments structure
* @param args the Arguments structure to initialize
*/
void init_args(Arguments *args);

/**
* @brief parse command line arguments
* @param argc   program's argc (number of argumets passed)
* @param argv   program's argv (vector containing all arguments passed)
* @param args   store here parsed arguments
*/
void parse(int argc, char const *argv[], Arguments *args);

#endif // ARGS_PARSER