/*
 * Copyright (c) 2018, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "stdio.h"
#include "stdlib.h"
#include "time.h"

#include "sps/sensirion_uart.h"
#include "sps/sps30.h"

void curr_time(){
    printf("%ld ", time(NULL));
    return;
}

int main(int argc, char ** argv) {

    if ((argc != 1) && (argc != 3)){
        printf("USAGE: pcount <# measurements> <interval>\n");
    }

    // default take measurements for a minute
    // data is not reliable until ~30 seconds in
    int n_measurements = 60;
    int interval = 1;
    if (argc == 3){
        n_measurements = atoi(argv[1]);
        interval = atoi(argv[2]); // interval in seconds
    }

    struct sps30_measurement m;
    const uint8_t AUTO_CLEAN_DAYS = 4;
    int16_t res;

    // uart connection
    while (sensirion_uart_open() != 0) {
        curr_time();
        printf("[ERROR]: UART init failed\n");
        sensirion_sleep_usec(1000000); /* sleep for 1s */
    }

    // check for sensor
    while (sps30_probe() != 0) {
        curr_time();
        printf("[ERROR]: SPS30 sensor probing failed\n");
        sensirion_sleep_usec(1000000); /* sleep for 1s */
    }

    // automatically clean the fan every x days
    res = sps30_set_fan_auto_cleaning_interval_days(AUTO_CLEAN_DAYS);
    if (res) {
        curr_time();
        printf("[ERROR]: setting the auto-clean interval %d\n", res);
    }

    uint32_t second = 1000000; // 1 second in microseconds

    // start measurement loop

    res = sps30_start_measurement();
    if (res < 0) {
        curr_time();
        printf("[ERROR]: Cannot start measurement!\n");
    }

    for (int i=0; i<n_measurements; i++) {

        res = sps30_read_measurement(&m);
        if (res < 0) {
            // measurement reading error, sleep
            curr_time();
            printf("-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 \n");
        } else {
            if (SPS30_IS_ERR_STATE(res)) {
                curr_time();
                printf("[ERROR]: STATE %u\n", SPS30_GET_ERR_STATE(res));
            }

            // pm1.0 pm2.5 pm4.0 pm10.0 nc0.5 nc1.0 nc2.5 nc4.0 nc10.0 psize
            curr_time();
            printf("%0.3f %0.3f %0.3f %0.3f %0.3f %0.3f %0.3f %0.3f %0.3f %0.3f\n",
                   m.mc_1p0, m.mc_2p5, m.mc_4p0, m.mc_10p0,
                   m.nc_0p5, m.nc_1p0, m.nc_2p5, m.nc_4p0, m.nc_10p0,
                   m.typical_particle_size);
        }
        sensirion_sleep_usec((uint32_t) interval*second); /* sleep for 1s */
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
