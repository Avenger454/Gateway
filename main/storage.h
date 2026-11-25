#pragma once
#include "vehicle.h"
#include "driver/i2c_master.h"
#include "esp_err.h"
esp_err_t storage_init(i2c_master_bus_handle_t bus);
esp_err_t storage_load(totals_t *t);
esp_err_t storage_save(const totals_t *t);
