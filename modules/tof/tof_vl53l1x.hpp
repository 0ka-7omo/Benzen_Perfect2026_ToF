#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "vl53l1_platform.h"
#include "VL53L1X_api.h"

class ToF_VL53L1X {
public:
    explicit ToF_VL53L1X(uint16_t dev_addr = 0x29);
    bool init();
    bool configure(uint16_t distance_mode = 2, uint16_t timing_budget_ms = 50, uint16_t inter_measurement_ms = 60);
    bool start();
    bool stop();
    bool poll();
    uint16_t raw_mm() const { return last_raw_mm_; }
    uint16_t filtered_mm() const { return filtered_mm_; }
    uint8_t range_status() const { return range_status_; }
    bool valid() const { return valid_; }
private:
    uint16_t dev_;
    uint8_t is_ready_ = 0;
    uint8_t range_status_ = 255;
    uint16_t last_raw_mm_ = 0;
    uint16_t filtered_mm_ = 0xFFFF;
    bool valid_ = false;
    static constexpr int K = 5;
    uint16_t ring_[K] = {};
    int idx_ = 0;
    int filled_ = 0;
    float ema_ = -1.0f;
    float alpha_ = 0.3f;
};