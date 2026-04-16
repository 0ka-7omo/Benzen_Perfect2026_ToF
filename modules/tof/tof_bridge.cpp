#include "pico/stdlib.h"
#include "tof_vl53l1x.hpp"
static ToF_VL53L1X g_tof(0x29);
static bool g_inited=false;

extern "C" void tof_setup(){
    if (g_inited) return;
    // Init and configure
    if (!g_tof.init()) return;
    g_tof.configure(2, 50, 60); // LONG, 50ms budget, 60ms inter
    g_tof.start();
    g_inited=true;
}

extern "C" void tof_poll(){
    if (!g_inited) return;
    // non-blocking; updates internal buffers only when data ready
    g_tof.poll();
}

extern "C" bool tof_read_valid(uint16_t* z_mm){
    if (!g_inited) return false;
    if (!g_tof.valid()) return false;
    if (z_mm) *z_mm = g_tof.filtered_mm();
    return true;
}
