#include "buzzer.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"

// Defina os pinos para cada buzzer (conforme sua tabela):
#define BUZZER_A_PIN 21  // Buzzer-A (opcional)
#define BUZZER_B_PIN 10  // Buzzer-B

// Variáveis estáticas para armazenar o número da slice de PWM para cada buzzer
static uint buzzer_a_slice;
static uint buzzer_b_slice;

void buzzer_a_init(void) {
    // Inicializa o Buzzer-A no GPIO21
    gpio_set_function(BUZZER_A_PIN, GPIO_FUNC_PWM);
    buzzer_a_slice = pwm_gpio_to_slice_num(BUZZER_A_PIN);
    pwm_set_wrap(buzzer_a_slice, 4095); // 12 bits de resolução
    pwm_set_chan_level(buzzer_a_slice, pwm_gpio_to_channel(BUZZER_A_PIN), 0);
    pwm_set_enabled(buzzer_a_slice, true);
}

void buzzer_a_play_tone(uint16_t freq, uint16_t duration_ms) {
    uint32_t clock_hz = 125000000; // Frequência do sistema (ajuste conforme necessário)
    // Calcula divisor aproximado: clock_hz / (4096 * freq)
    float divisor = (float)clock_hz / (4096 * freq);
    pwm_set_clkdiv(buzzer_a_slice, divisor);
    // Define duty cycle de 50%
    pwm_set_chan_level(buzzer_a_slice, pwm_gpio_to_channel(BUZZER_A_PIN), 2048);
    sleep_ms(duration_ms);
    // Desliga o som
    pwm_set_chan_level(buzzer_a_slice, pwm_gpio_to_channel(BUZZER_A_PIN), 0);
}

void buzzer_a_beep(void) {
    buzzer_a_play_tone(1000, 100); // 1000Hz por 100ms
}

void buzzer_b_init(void) {
    // Inicializa o Buzzer-B no GPIO10
    gpio_set_function(BUZZER_B_PIN, GPIO_FUNC_PWM);
    buzzer_b_slice = pwm_gpio_to_slice_num(BUZZER_B_PIN);
    pwm_set_wrap(buzzer_b_slice, 4095); // 12 bits
    pwm_set_chan_level(buzzer_b_slice, pwm_gpio_to_channel(BUZZER_B_PIN), 0);
    pwm_set_enabled(buzzer_b_slice, true);
}

void buzzer_b_play_tone(uint16_t freq, uint16_t duration_ms) {
    uint32_t clock_hz = 125000000;
    float divisor = (float)clock_hz / (4096 * freq);
    pwm_set_clkdiv(buzzer_b_slice, divisor);
    pwm_set_chan_level(buzzer_b_slice, pwm_gpio_to_channel(BUZZER_B_PIN), 2048);
    sleep_ms(duration_ms);
    pwm_set_chan_level(buzzer_b_slice, pwm_gpio_to_channel(BUZZER_B_PIN), 0);
}

void buzzer_b_beep(void) {
    buzzer_b_play_tone(1000, 100);
}
