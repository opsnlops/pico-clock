
#include <stdio.h>
#include <time.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>
#include <timers.h>
#include <pico/lwip_freertos.h>

#include "pico/stdlib.h"
#include "hardware/rtc.h"
#include "lwip/apps/sntp.h"

#include "utz/utz.h"


#include "logging/logging.h"
#include "sntp.h"
#include "time_utils.h"
#include "types.h"
#include "wifi.h"


volatile size_t xFreeHeapSpace;

bool is_wifi_connected = false;


TaskHandle_t wifi_connect_task_handle;
TaskHandle_t sntp_setup_task_handle;
TimerHandle_t time_print_timer_handle = NULL;


portTASK_FUNCTION_PROTO(wifi_connect_task, pvParameters);
portTASK_FUNCTION_PROTO(sntp_setup_task, pvParameters);
void time_print_timer(TimerHandle_t xTimer);

void acw_post_logging_hook(char *message, uint8_t message_length) {

    (void) message_length;

    // Send the message to the console
    printf("%s\n", message);

}




int main() {

    stdio_init_all();

    logger_init();

    info("Creating tasks");
    xTaskCreate(wifi_connect_task,
                "wifi_connect_task",
                2048,
                NULL,
                1,
                &wifi_connect_task_handle);
    debug("Created wifi_connect_task");

    xTaskCreate(sntp_setup_task,
                "sntp_setup_task",
                2048,
                NULL,
                1,
                &sntp_setup_task_handle);
    debug("Created sntp_setup_task");


    time_print_timer_handle = xTimerCreate(
            "Print Time Timer",                    // Timer name
            pdMS_TO_TICKS(1000),    // Fire every SENSOR_TIMER_TIME_MS
            pdTRUE,                                     // Auto-reload
            (void *) 0,                                 // Timer ID (not used here)
            time_print_timer        // Callback function
    );
    xTimerStart(time_print_timer_handle, 1000);



    info("Starting scheduler");


    vTaskStartScheduler();

    return 0;
}


portTASK_FUNCTION(wifi_connect_task, pvParameters) {
    (void) pvParameters;

    wifi_connect();
    info("Connected to WiFi");
    is_wifi_connected = true;

    vTaskDelete(NULL);
}

portTASK_FUNCTION(sntp_setup_task, pvParameters) {
    (void) pvParameters;

    // Wait for wifi (this should be a task signal)
    while(!is_wifi_connected) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    info("Setting up SNTP");
    ip_addr_t sntp_server;
    IP4_ADDR(&sntp_server, 10, 3, 2, 6);

    rtc_init();

    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setserver(0, &sntp_server);
    sntp_init();


    vTaskDelete(NULL);
}


void time_print_timer(TimerHandle_t xTimer) {

    // Get our local time zone
    uzone_t active_zone;
    get_zone_by_name("San Francisco", &active_zone);

    struct tm utc_time, local_time;

    if (is_wifi_connected) {

        char buffer[26];

        if (get_date_now(&utc_time)) {

            // Convert this to a local time
            local_time= utc_tm_to_local_tm(utc_time, active_zone);

            memset(&buffer, '\0', 26);
            strftime(buffer, 26, "%Y-%m-%d %l:%M:%S %p", &local_time);
            debug("Time: %s", buffer);

        }
    }
}