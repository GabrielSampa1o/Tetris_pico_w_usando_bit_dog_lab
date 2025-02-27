#ifndef AUTO_REPEAT_H
#define AUTO_REPEAT_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t repeat_time;
    uint32_t hold_time;
    bool     pressed;
    bool     last_state;
    uint32_t pressed_time;
    uint32_t last_time;
} AutoRepeat;

void auto_repeat_init(AutoRepeat *ar,
                      uint32_t repeat_time,
                      uint32_t hold_time);

bool auto_repeat_next(AutoRepeat *ar,
                      uint32_t time_ms,
                      bool current_state);

#ifdef __cplusplus
}
#endif

#endif
