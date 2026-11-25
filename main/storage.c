#include "storage.h"
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#define MAGIC 0x46425244u
#define SLOT_SIZE 128
// Separate bench namespace, never use these slots for real vehicle mileage.
#define BASE 0x1000
typedef struct {uint32_t magic,version; uint64_t seq; totals_t totals; uint32_t crc;} record_t;
static i2c_master_dev_handle_t dev;
static uint64_t seq;
static bool ready;
static uint32_t crc32(const void *p,size_t n){const uint8_t *b=p;uint32_t c=~0u;while(n--){c^=*b++;for(int i=0;i<8;i++)c=(c>>1)^((0u-(c&1u))&0xedb88320u);}return ~c;}
static esp_err_t read_slot(int slot,record_t *r){uint16_t a=BASE+slot*SLOT_SIZE;uint8_t addr[]={a>>8,a&255};return i2c_master_transmit_receive(dev,addr,2,(uint8_t*)r,sizeof(*r),100);}
static bool valid(const record_t *r){return r->magic==MAGIC && r->version==1 && r->crc==crc32(r,offsetof(record_t,crc));}
esp_err_t storage_init(i2c_master_bus_handle_t bus){i2c_device_config_t cfg={.dev_addr_length=I2C_ADDR_BIT_LEN_7,.device_address=0x50,.scl_speed_hz=100000};return i2c_master_bus_add_device(bus,&cfg,&dev);}
esp_err_t storage_load(totals_t *t){record_t a,b;esp_err_t e=read_slot(0,&a);if(e!=ESP_OK)return e;e=read_slot(1,&b);if(e!=ESP_OK)return e;bool av=valid(&a),bv=valid(&b);if(!av&&!bv){memset(t,0,sizeof(*t));seq=0;ready=true;return ESP_ERR_NOT_FOUND;}record_t *r=av&&(!bv||a.seq>=b.seq)?&a:&b;*t=r->totals;seq=r->seq;ready=true;return ESP_OK;}
esp_err_t storage_save(const totals_t *t){if(!ready)return ESP_ERR_INVALID_STATE;record_t r;memset(&r,0,sizeof(r));r.magic=MAGIC;r.version=1;r.seq=seq+1;r.totals=*t;r.crc=crc32(&r,offsetof(record_t,crc));uint16_t a=BASE+(r.seq&1)*SLOT_SIZE;uint8_t buf[2+sizeof(r)];buf[0]=a>>8;buf[1]=a&255;memcpy(buf+2,&r,sizeof(r));esp_err_t e=i2c_master_transmit(dev,buf,sizeof(buf),100);if(e!=ESP_OK)return e;record_t check;e=read_slot(r.seq&1,&check);if(e!=ESP_OK)return e;if(!valid(&check)||memcmp(&r,&check,offsetof(record_t,crc))!=0)return ESP_FAIL;seq=r.seq;return ESP_OK;}
