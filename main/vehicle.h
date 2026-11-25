#pragma once
#include <stdbool.h>
#include <stdint.h>
typedef struct { double rpm,pw_ms,dead_ms,rail_gauge_psi,map_abs_psi,baro_abs_psi,mph; bool valid; } vehicle_t;
typedef struct { double injector_lb_hr,rated_psi,density_lb_gal; } fuel_config_t;
typedef struct { uint64_t distance_mm, fuel_ul, engine_ms; } totals_t;
bool fuel_gph(const vehicle_t *v,const fuel_config_t *c,double *out);
void accumulate(totals_t *t,const vehicle_t *v,double gph,double seconds);
