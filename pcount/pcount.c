/*
 * Jayke Samson Nguyen 
 * 2022-02-28
 *
 * Modified SPS30 measurement script using a Sensirion SPS30 sensor
 * Measures the mass density, particle count, and particle size
 * The first ~16 seconds of data on startup are not reliable since algorithm relies on statistics
 * Data is guaranteed reliable after 30 seconds
 * This measurements script was designed to be run on the startup of the computer
 *
 * General timeline: startup -> measure for 30 seconds and toss initial data -> record data indefinitely -> shutdown
 *
 * Logs the data to a binary data format
 * By default logs data every second, change "interval" to change this
 *
 * "marking" a file writes -1 for all entries and notes the script start point
 * If the script crashes, the data file is marked at the time
 * The first 30 seconds after the marked time is not reliable 
 *
 * Change "output_filename_format" to wherever you want to output and name format
 * default output filename format -> folder/YYYY-MM-DD_sps30.bin
 *
 * Binary format is:
 * time    ,  pm1.0,  pm2.5,  pm4.0, pm10.0,  nc0.5,  nc1.0,  nc2.5,  nc4.0, nc10.0, particle size
 * long int,  float,  float,  float,  float,  float,  float,  float,  float,  float,  float
 * 24 bytes total per measurement
 *
 * Usage: ./pcount
 */

#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "string.h"

#include "sps/sensirion_uart.h"
#include "sps/sps30.h"

// microseconds to seconds
#define USEC2SEC 1000000

// prints the current time with no newline
void curr_time(){
    printf("%ld ", time(NULL));
    return;
}

// print the measurement
void print_measurement(struct sps30_measurement *m){
    curr_time();
    printf("%0.3f %0.3f %0.3f %0.3f %0.3f %0.3f %0.3f %0.3f %0.3f %0.3f\n",
           m->mc_1p0, m->mc_2p5, m->mc_4p0, m->mc_10p0,
           m->nc_0p5, m->nc_1p0, m->nc_2p5, m->nc_4p0, m->nc_10p0,
           m->typical_particle_size);
}

// marks file with the time all -1 for the measurements
// preserves binary format
void mark_file(char* filename, time_t t){
    FILE *file_ptr;
    file_ptr = fopen(filename, "ab");
    fwrite(&t, 1, sizeof(t), file_ptr);
    float fnull = -1.0;
    for(int i=0; i<10; i++){
        fwrite(&fnull, 1, sizeof(float), file_ptr);
    }
    fclose(file_ptr);
}

// append to the output binary file, does not overwrite data
void append_to_file(char *filename, time_t t, struct sps30_measurement m){
    FILE *file_ptr;
    file_ptr = fopen(filename, "ab");

    fwrite(&t, 1, sizeof(t), file_ptr);
    fwrite(&m.mc_1p0, 1, sizeof(m.mc_1p0), file_ptr);
    fwrite(&m.mc_2p5, 1, sizeof(m.mc_2p5), file_ptr);
    fwrite(&m.mc_4p0, 1, sizeof(m.mc_4p0), file_ptr);
    fwrite(&m.mc_10p0, 1, sizeof(m.mc_10p0), file_ptr);
    fwrite(&m.nc_0p5, 1, sizeof(m.nc_0p5), file_ptr);
    fwrite(&m.nc_1p0, 1, sizeof(m.nc_1p0), file_ptr);
    fwrite(&m.nc_2p5, 1, sizeof(m.nc_2p5), file_ptr);
    fwrite(&m.nc_4p0, 1, sizeof(m.nc_4p0), file_ptr);
    fwrite(&m.nc_10p0, 1, sizeof(m.nc_10p0), file_ptr);
    fwrite(&m.typical_particle_size, 1, sizeof(m.typical_particle_size), file_ptr);

    fclose(file_ptr);
}

int main(int argc, char ** argv) {

    int interval = 1; // measurement interval in seconds
    const uint8_t AUTO_CLEAN_DAYS = 4;
    char *output_filename_format = "/home/espresso/sps/pcount/data/%d-%02d-%02d_sps30.bin"; // change this if want different output

    struct sps30_measurement m;
    int16_t res;

    char old_output_filename[128];
    char output_filename[128];
    time_t t;
    struct tm datetime;

    // initialize the output filename var
    t = time(NULL);
    datetime = *localtime(&t);
    sprintf(output_filename, output_filename_format, datetime.tm_year + 1900, datetime.tm_mon + 1, datetime.tm_mday);

    // mark file on initialization in case of crash
    mark_file(output_filename, t);

    // uart connection
    while (sensirion_uart_open() != 0) {
        curr_time();
        printf("[ERROR]: UART init failed\n");
        sensirion_sleep_usec(USEC2SEC);
    }

    // check for sensor
    while (sps30_probe() != 0) {
        curr_time();
        printf("[ERROR]: SPS30 sensor probing failed\n");
        sensirion_sleep_usec(USEC2SEC);
    }

    // automatically clean the fan every x days
    res = sps30_set_fan_auto_cleaning_interval_days(AUTO_CLEAN_DAYS);
    if (res) {
        curr_time();
        printf("[ERROR]: setting the auto-clean interval %d\n", res);
    }

    // start measurement mode
    res = sps30_start_measurement();
    if (res < 0) {
        curr_time();
        printf("[ERROR]: Cannot start measurement!\n");
    }

    // read measurements for the first 30 seconds, but don't do anything with them
    // measurements in the first 30 seconds after startup are unreliable since
    // there aren't good statistics in the beginning
    printf("[LOG]: MEASUREMENT STARTUP, DATA NOT BEING RECORDED\n");
    for(int i=0; i<30; i++){
        res = sps30_read_measurement(&m);
        sensirion_sleep_usec((uint32_t) USEC2SEC);
        print_measurement(&m);
    }

    // start main measurement loop
    printf("[LOG]: STARTING MEASUREMENTS AND RECORDING DATA\n");
    while(1) {

        res = sps30_read_measurement(&m);
        if (res < 0) {
            // measurement reading error, don't record and sleep
            curr_time();
            printf("[WARN]: MEASUREMENT ERROR\n");
        } else {

            // may be in error state
            if (SPS30_IS_ERR_STATE(res)) {
                curr_time();
                printf("[ERROR]: STATE %u\n", SPS30_GET_ERR_STATE(res));
            } else {

                // not in error state, log data
                // print_measurement(&m);

                t = time(NULL);
                datetime = *localtime(&t);
                sprintf(output_filename, output_filename_format, datetime.tm_year + 1900, datetime.tm_mon + 1, datetime.tm_mday);

                if (strcmp(old_output_filename, output_filename)){
                    printf("[LOG]: NEW OUTPUT FILENAME -> %s\n", output_filename);
                }

                // opens appends and closes file
                // good if we need to restart!
                append_to_file(output_filename, time(NULL), m);
                strcpy(old_output_filename, output_filename);

            }
        }
        sensirion_sleep_usec((uint32_t) interval*USEC2SEC);
    }

    // stop measurement
    res = sps30_stop_measurement();
    if (res) {
        curr_time();
        printf("[ERROR]: Stopping measurement failed!\n");
    }

    // close uart connection
    res = sensirion_uart_close();
    if (res != 0){
        curr_time();
        printf("[ERROR]: Failed to close UART!\n");
    }

    return 0;
}
