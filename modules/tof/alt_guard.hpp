#pragma once
#include <stdint.h>
static inline float altitude_guard_command(uint32_t target_mm, uint32_t tol_mm, uint32_t z_mm, float user_cmd, float pid_corr){
    int32_t err = (int32_t)target_mm - (int32_t)z_mm;
    if (err <= (int32_t)tol_mm && err >= -(int32_t)tol_mm){
        if (user_cmd>1.0f) user_cmd=1.0f; if (user_cmd<-1.0f) user_cmd=-1.0f;
        return user_cmd;
    }
    float cmd = user_cmd + pid_corr;
    if (cmd>1.0f) cmd=1.0f; if (cmd<-1.0f) cmd=-1.0f;
    uint32_t upper=target_mm+tol_mm, lower=(target_mm>tol_mm)?(target_mm-tol_mm):0;
    if (z_mm>upper){ if (cmd>0.0f) cmd=0.0f; }
    else if (z_mm<lower){ if (cmd<0.0f) cmd=0.0f; }
    return cmd;
}