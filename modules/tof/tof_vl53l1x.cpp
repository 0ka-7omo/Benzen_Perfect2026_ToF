#include "tof_vl53l1x.hpp"
#include <algorithm>
#include <stdio.h>

ToF_VL53L1X::ToF_VL53L1X(uint16_t dev_addr) : dev_(dev_addr) {}

bool ToF_VL53L1X::init() {
    stdio_init_all();
    i2c_init(I2C_PORT, i2C_CLOCK);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_set_pulls(SDA_PIN, true, false);
    gpio_set_pulls(SCL_PIN, true, false);
    sleep_ms(3000);
    uint8_t state = 0;
    int8_t status = 0;
    for (int i = 0; i < 20 && (state & 1) == 0; ++i) {
        status = VL53L1X_BootState(dev_, &state);
        sleep_ms(10);
    }
    if ((state & 1) == 0) { printf("[ToF] Boot timeout\n"); return false; }
    status = VL53L1X_SensorInit(dev_);
    if (status != 0) { printf("[ToF] SensorInit failed: %d\n", status); return false; }
    return true;
}
bool ToF_VL53L1X::configure(uint16_t distance_mode, uint16_t timing_budget_ms, uint16_t inter_measurement_ms) {
    int8_t status = 0;
    status |= VL53L1X_SetDistanceMode(dev_, distance_mode);
    status |= VL53L1X_SetTimingBudgetInMs(dev_, timing_budget_ms);
    status |= VL53L1X_SetInterMeasurementInMs(dev_, inter_measurement_ms);
    status |= VL53L1X_SetROI(dev_, 16, 16);
    if (status != 0) { printf("[ToF] Configure failed: %d\n", status); return false; }
    return true;
}
bool ToF_VL53L1X::start() { int8_t s = VL53L1X_StartRanging(dev_); if (s) printf("[ToF] StartRanging failed: %d\n", s); return s==0; }
bool ToF_VL53L1X::stop()  { int8_t s = VL53L1X_StopRanging(dev_);  if (s) printf("[ToF] StopRanging failed: %d\n", s);  return s==0; }
bool ToF_VL53L1X::poll() {
    int8_t status = VL53L1X_CheckForDataReady(dev_, &is_ready_);
    if (status != 0 || is_ready_ == 0) return false;
    is_ready_ = 0;
    status |= VL53L1X_GetRangeStatus(dev_, &range_status_);
    status |= VL53L1X_GetDistance(dev_, &last_raw_mm_);
    status |= VL53L1X_ClearInterrupt(dev_);
    valid_ = (status == 0) && (range_status_ == 0);
    if (valid_) {
        if (filled_ < K) { ring_[filled_++] = last_raw_mm_; }
        else { ring_[idx_] = last_raw_mm_; }
        idx_ = (idx_ + 1) % K;
        uint16_t tmp[ K ]; for (int i=0;i<filled_;++i) tmp[i]=ring_[i];
        std::sort(tmp, tmp+filled_);
        uint16_t med = tmp[filled_/2];
        if (ema_ < 0.0f) ema_ = med;
        else ema_ = alpha_*med + (1.0f-alpha_)*ema_;
        filtered_mm_ = (uint16_t)(ema_ + 0.5f);
    }
    return true;
}