# Firebird Gateway V0.1
Bench-only ESP-IDF 5.5.x starter for ESP32-S3-N8R2. Not vehicle-ready. No CAN transmission or real odometer updates. Simulation uses separate FRAM slots at 0x1000 and 0x1080. Never migrate simulated totals into vehicle records.

## Build
Open an ESP-IDF terminal in this folder:
```
idf.py set-target esp32s3
idf.py build
idf.py -p COM7 flash monitor
```
Replace COM7 with your gateway port. No external component downloads required. ESP-IDF build has not been run in the generation environment.

## Wiring (provisional, verify board labels)
3.3 V logic, common ground, external I2C pullups to 3.3 V. FRAM A0/A1/A2/WP grounded, address 0x50; ADS1115 ADDR grounded, 0x48. SDA GPIO8, SCL GPIO9. GPS TX -> GPIO18; GPS RX -> GPIO17 (verify breakout levels). SPI MOSI11 MISO13 SCK12; reserved MCP CS10 INT14. Reserved TWAI TX4 RX5. Do not attach 5 V logic or raw automotive power. Do not connect sender wires directly to ADC.

## Implemented
- Example fuel-flow calculation with differential pressure correction and dead-time subtraction; assumes eight injectors, one injection per cycle, linear flow. Example injector calibration only.
- Simulated distance, fuel and engine time counters; FRAM alternating records with CRC, sequence and readback. Missing FRAM permits RAM-only bench operation. Blank or invalid bench slots start at zero.
- I2C initialization and ADC probe, SPI bus initialization, UART byte reception.

## Not implemented yet
ADS1115 conversions and sender conditioning; MCP2515 driver; TWAI initialization; verified MS3/TurboLamik decoding; GPS NMEA parser; actual odometer initialization and independent validity tracking; trip counters; NVS backups; display network; real vehicle fault handling. No production odometer reset/recovery policy yet. The current bench accumulator deliberately couples distance and fuel validity; production must track these independently.

## Bench checks
Boot without peripherals; expect FRAM unavailable and simulation logs. Connect FRAM with power off, reboot, verify load and save. Power cycle and confirm simulated miles resume. Connect ADS1115 and verify probe succeeds. GPS byte count should appear when receiver transmits. Example expected MPG about 18.32 at 65 mph (3.55 gal/hr). Do not connect to vehicle CAN yet.
