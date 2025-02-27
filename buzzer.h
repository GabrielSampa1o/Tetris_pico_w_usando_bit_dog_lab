#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>
#include <stdbool.h>

/**
 * Inicializa o Buzzer-A (por exemplo, no GPIO21)
 */
void buzzer_a_init(void);

/**
 * Toca um tom no Buzzer-A com a frequência (Hz) por duration_ms milissegundos.
 */
void buzzer_a_play_tone(uint16_t freq, uint16_t duration_ms);

/**
 * Emite um beep curto no Buzzer-A.
 */
void buzzer_a_beep(void);

/**
 * Inicializa o Buzzer-B (por exemplo, no GPIO10)
 */
void buzzer_b_init(void);

/**
 * Toca um tom no Buzzer-B com a frequência (Hz) por duration_ms milissegundos.
 */
void buzzer_b_play_tone(uint16_t freq, uint16_t duration_ms);

/**
 * Emite um beep curto no Buzzer-B.
 */
void buzzer_b_beep(void);

#endif
