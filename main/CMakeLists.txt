idf_component_register(SRCS "main.c" "wifi_sta.c" "udp_client.c"
                    PRIV_REQUIRES esp_driver_i2s fatfs i2s_examples_common esp_wifi nvs_flash esp_netif esp_event
                    INCLUDE_DIRS)
