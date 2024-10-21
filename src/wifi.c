

#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwipopts.h"

#include "logging/logging.h"

#include "types.h"
#include "wifi.h"
#include "wifi_creds.h"



void wifi_setup(u32 country, const char *ssid, const char *password, u32 auth,
               const char *hostname, ip_addr_t *ip, ip_addr_t *netmask, ip_addr_t *gateway) {

    if(cyw43_arch_init_with_country(country)) {
        error("Failed to initialize CYW43");
        vTaskDelete(NULL);
    }
    debug("CYW43 initialized");

    cyw43_arch_enable_sta_mode();
    debug("STA mode enabled");

    if(cyw43_arch_wifi_connect_timeout_ms(ssid, password, auth, 30000)) {
        error("Failed to connect to wifi");
        vTaskDelete(NULL);
    }

    int flashrate = 1000;
    int status = CYW43_LINK_UP + 1;

    while( status >=0 && status != CYW43_LINK_UP) {
        int new_status = cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA);

        if(new_status != status) {
            status = new_status;
            flashrate = flashrate / (status + 1);
            debug("connection status: %d %d\n", status, flashrate);
        }
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(flashrate));
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(flashrate));
    }

    if(status < 0) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    }
    else {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        if( ip != NULL ) {
            netif_set_ipaddr(netif_default, ip);
        }
        if(netmask != NULL ) {
            netif_set_netmask(netif_default, netmask);
        }
        if(gateway != NULL ) {
            netif_set_gw(netif_default, gateway);
        }

        info("IP Address: %s", ip4addr_ntoa(netif_ip_addr4(netif_default)));
        info("   Netmask: %s", ip4addr_ntoa(netif_ip_netmask4(netif_default)));
        info("   Gateway: %s", ip4addr_ntoa(netif_ip_gw4(netif_default)));
        info("  Hostname: %s", netif_get_hostname(netif_default));

    }

}


int wifi_connect() {
    char ssid[] = WIFI_NETWORK;
    char password[] = WIFI_PASSWORD;
    u32 country = CYW43_COUNTRY_USA;
    u32 auth = CYW43_AUTH_WPA2_MIXED_PSK;

    wifi_setup(country, ssid, password, auth, "PicoW", NULL, NULL, NULL);

    return 1;
}
