#include "auto_repeat.h"

void auto_repeat_init(AutoRepeat *ar,
                      uint32_t repeat_time,
                      uint32_t hold_time)
{
    if(!ar) return;
    ar->repeat_time = repeat_time;
    ar->hold_time   = hold_time;
    ar->pressed     = false;
    ar->last_state  = false;
    ar->pressed_time= 0;
    ar->last_time   = 0;
}

bool auto_repeat_next(AutoRepeat *ar,
                      uint32_t time_ms,
                      bool current_state)
{
    if(!ar) return false;

    bool changed = (current_state != ar->last_state);
    ar->last_state = current_state;

    if(changed) {
        if(current_state) {
            // pressionou agora
            ar->pressed_time = time_ms;
            ar->pressed      = true;
            ar->last_time    = time_ms;
            return true; // disparo inicial
        } else {
            // soltou
            ar->pressed_time = 0;
            ar->pressed      = false;
            ar->last_time    = 0;
        }
    }
    // se sem auto-repeat
    if(ar->repeat_time==0) return false;

    // se ainda pressionado
    if(ar->pressed) {
        uint32_t repeat_rate= ar->repeat_time;
        // se passou hold_time, repete mais rápido
        if(ar->hold_time>0 && (time_ms - ar->pressed_time)> ar->hold_time){
            repeat_rate/=3;
        }
        if((time_ms - ar->last_time)> repeat_rate){
            ar->last_time= time_ms;
            return true;
        }
    }

    return false;
}
