#include "vehicle.h"
#include "storage.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/uart.h"
#include "driver/twai.h"
#define TAG "gateway"
// Bench firmware only. CAN listen-only, no vehicle commands.
void app_main(void){
 i2c_master_bus_handle_t bus;
 i2c_master_bus_config_t i2c={.i2c_port=I2C_NUM_0,.sda_io_num=8,.scl_io_num=9,.clk_source=I2C_CLK_SRC_DEFAULT,.glitch_ignore_cnt=7,.flags.enable_internal_pullup=true};
 ESP_ERROR_CHECK(i2c_new_master_bus(&i2c,&bus));
 bool fram=false; totals_t totals={0};
 if(storage_init(bus)==ESP_OK){esp_err_t e=storage_load(&totals);fram=e==ESP_OK||e==ESP_ERR_NOT_FOUND;ESP_LOGI(TAG,"Bench FRAM load: %s",esp_err_to_name(e));}
 ESP_LOGI(TAG,"ADS1115 probe: %s",esp_err_to_name(i2c_master_probe(bus,0x48,100)));
 spi_bus_config_t spi={.mosi_io_num=11,.miso_io_num=13,.sclk_io_num=12,.quadwp_io_num=-1,.quadhd_io_num=-1};
 ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST,&spi,SPI_DMA_DISABLED));
 // MCP2515 device driver intentionally deferred until module clock/voltage confirmed.
 uart_config_t uart={.baud_rate=9600,.data_bits=UART_DATA_8_BITS,.parity=UART_PARITY_DISABLE,.stop_bits=UART_STOP_BITS_1,.flow_ctrl=UART_HW_FLOWCTRL_DISABLE,.source_clk=UART_SCLK_DEFAULT};
 ESP_ERROR_CHECK(uart_param_config(UART_NUM_1,&uart));ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1,17,18,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE));ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1,2048,0,0,NULL,0));
 // TWAI intentionally not started: verify bitrate and transceiver first.
 ESP_LOGW(TAG,"SIMULATION ONLY: no real mileage or CAN decoding");
 const fuel_config_t cfg={60,43.5,6.2}; // example only
 const vehicle_t v={.rpm=2200,.pw_ms=3.5,.dead_ms=1,.rail_gauge_psi=43.5,.map_abs_psi=14.7,.baro_abs_psi=14.7,.mph=65,.valid=true};
 int64_t last=esp_timer_get_time();
 while(1){vTaskDelay(pdMS_TO_TICKS(1000));int64_t now=esp_timer_get_time();double dt=(now-last)/1e6;last=now;double gph;if(fuel_gph(&v,&cfg,&gph)){accumulate(&totals,&v,gph,dt);ESP_LOGI(TAG,"SIM mph=%.1f mpg=%.2f gph=%.3f miles=%.4f",v.mph,gph>0?v.mph/gph:0,gph,totals.distance_mm/1609344.0);if(fram){esp_err_t e=storage_save(&totals);if(e!=ESP_OK)ESP_LOGE(TAG,"FRAM save: %s",esp_err_to_name(e));}}uint8_t gps[256];int n=uart_read_bytes(UART_NUM_1,gps,sizeof(gps),0);if(n>0)ESP_LOGI(TAG,"GPS received %d bytes (not decoded)",n);}
}
