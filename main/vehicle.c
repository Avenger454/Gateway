#include "vehicle.h"
#include <math.h>
bool fuel_gph(const vehicle_t *v,const fuel_config_t *c,double *out){
 if(!v->valid || !isfinite(v->rpm) || !isfinite(v->pw_ms) || !isfinite(v->dead_ms) || !isfinite(v->rail_gauge_psi) || !isfinite(v->map_abs_psi) || !isfinite(v->baro_abs_psi) || c->injector_lb_hr<=0 || c->rated_psi<=0 || c->density_lb_gal<=0 || v->rpm<0 || v->pw_ms<0 || v->dead_ms<0) return false;
 double dp=v->rail_gauge_psi+v->baro_abs_psi-v->map_abs_psi;
 if(dp<=0) return false;
 double duty=fmax(0,v->pw_ms-v->dead_ms)*v->rpm/120000.0;
 if(duty>1) return false;
 *out=8*c->injector_lb_hr*sqrt(dp/c->rated_psi)*duty/c->density_lb_gal;
 return isfinite(*out);
}
void accumulate(totals_t *t,const vehicle_t *v,double gph,double seconds){
 if(!v->valid || !isfinite(seconds) || seconds<=0 || seconds>2 || !isfinite(v->mph) || v->mph<0 || !isfinite(gph) || gph<0) return;
 t->distance_mm+=(uint64_t)llround(v->mph*447.04*seconds);
 t->fuel_ul+=(uint64_t)llround(gph*3785411.784*seconds/3600);
 if(v->rpm>0)t->engine_ms+=(uint64_t)llround(seconds*1000);
}
