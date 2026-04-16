#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
// Initialize VL53L1X ToF (call once at startup)
void tof_setup();
// Poll sensor (non-blocking). Call as often as you like (e.g., in rate/angle loops).
void tof_poll();
// Return filtered height [mm] via *z_mm when valid. Returns true if valid.
bool tof_read_valid(uint16_t* z_mm);
#ifdef __cplusplus
}
#endif
