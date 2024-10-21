
#pragma once

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "types.h"

void wifi_setup(u32 country, const char *ssid, const char *password, u32 auth,
               const char *hostname, ip_addr_t *ip, ip_addr_t *netmask, ip_addr_t *gateway);

int wifi_connect();
