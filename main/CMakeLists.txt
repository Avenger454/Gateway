idf_component_register(SRCS "main.c" "vehicle.c" "storage.c" INCLUDE_DIRS "." REQUIRES esp_driver_i2c esp_driver_spi esp_driver_uart esp_driver_gpio driver esp_timer nvs_flash)
