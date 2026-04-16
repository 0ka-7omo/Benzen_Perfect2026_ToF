#pragma once
struct AltHoldPID {
    float Kp = 0.0025f;
    float Ki = 0.0006f;
    float Kd = 0.0015f;
    float i_min = -0.3f, i_max = 0.3f;
    float out_min = -0.5f, out_max = 0.5f;
    float integ = 0.0f, prev_err = 0.0f; bool first=true;
    void reset(){ integ=0.0f; prev_err=0.0f; first=true; }
    float update(unsigned set_mm, unsigned meas_mm, float dt){
        float e = (float)set_mm - (float)meas_mm;
        float d = first ? 0.0f : (e - prev_err)/dt; first=false; prev_err=e;
        integ += Ki*e*dt; if (integ>i_max) integ=i_max; if (integ<i_min) integ=i_min;
        float out = Kp*e + integ + Kd*d;
        if (out>out_max) out=out_max; if (out<out_min) out=out_min;
        return out;
    }
};