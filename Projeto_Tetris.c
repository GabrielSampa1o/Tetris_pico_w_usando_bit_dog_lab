#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "auto_repeat.h"
#include "ssd1306.h"
#include "tetris.h"
#include "buzzer.h"


// Mapeamento
#define BUT_A_PIN 5    // rotação anti-horário
#define BUT_B_PIN 6    // rotação horário
#define JOY_BUT_PIN 22 // joystick push
#define JOY_VRX_PIN 27 // ADC1
#define JOY_VRY_PIN 26 // ADC0

#define LED_R_PIN  13
#define LED_G_PIN  11
#define LED_B_PIN  12

#define I2C_SDA    14
#define I2C_SCL    15
#define I2C_PORT   i2c1
#define OLED_ADDR  0x3C
#define OLED_W     128
#define OLED_H     64

// Variável global do display
ssd1306_t g_oled_dev;

// AutoRepeat para cada botão, se quiser
static AutoRepeat ar_butA, ar_butB;
static AutoRepeat ar_joyBut;
static uint32_t last_time=0;

// Funções de callback ou polling
// Exemplo: ler estado e chamar tetris_xxx

int main(void){
    stdio_init_all();
    sleep_ms(2000);

    // init GPIO (botões)
    gpio_init(BUT_A_PIN);
    gpio_set_dir(BUT_A_PIN, GPIO_IN);
    gpio_pull_up(BUT_A_PIN);

    gpio_init(BUT_B_PIN);
    gpio_set_dir(BUT_B_PIN, GPIO_IN);
    gpio_pull_up(BUT_B_PIN);

    // Joystick button
    gpio_init(JOY_BUT_PIN);
    gpio_set_dir(JOY_BUT_PIN, GPIO_IN);
    gpio_pull_up(JOY_BUT_PIN);

    // LED RGB (saída)
    gpio_init(LED_R_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_put(LED_R_PIN,false);

    gpio_init(LED_G_PIN);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_put(LED_G_PIN,false);

    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);
    gpio_put(LED_B_PIN,false);

    // ADC Joystick
    adc_init();
    adc_gpio_init(JOY_VRX_PIN); // ADC1
    adc_gpio_init(JOY_VRY_PIN); // ADC0

    // I2C + SSD1306
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // init display
    ssd1306_init(&g_oled_dev, OLED_W,OLED_H,
                 false, // external_vcc
                 OLED_ADDR, I2C_PORT);
    ssd1306_config(&g_oled_dev);

    // Inicializa os buzzers
    buzzer_a_init();
    buzzer_b_init();


    // init tetris
    tetris_init();

    // autoRepeat
    auto_repeat_init(&ar_butA, 200,1000);
    auto_repeat_init(&ar_butB, 200,1000);
    auto_repeat_init(&ar_joyBut, 200,1000);

    last_time= to_ms_since_boot(get_absolute_time());

    while(true){
        // tempo
        uint32_t now= to_ms_since_boot(get_absolute_time());
        uint32_t dt= now- last_time;
        last_time= now;

        // update tetris
        tetris_update(dt);

        // Leitura botões
        bool a_state= (gpio_get(BUT_A_PIN)==0);
        bool b_state= (gpio_get(BUT_B_PIN)==0);
        bool joy_but= (gpio_get(JOY_BUT_PIN)==0);

        // auto-repeat
        if(auto_repeat_next(&ar_butA, now, a_state)){
            // anti-horário
            tetris_rotate_counter();
        }
        if(auto_repeat_next(&ar_butB, now, b_state)){
            // horário
            tetris_rotate_clockwise();
        }
        if(auto_repeat_next(&ar_joyBut, now, joy_but)){
            // Podíamos usar para "hard_drop" ou togglar LED
            tetris_hard_drop();
        }

        // Ler joystick ADC
        // VRX => ADC1
        adc_select_input(1);
        uint16_t vx= adc_read();
        // VRY => ADC0
        adc_select_input(0);
        uint16_t vy= adc_read();

        // se vx<1000 => move left, >3000 => move right
        if(vy<1000){
            tetris_move_left();
        } else if(vy>3000){
            tetris_move_right();
        }

        // se vy>3000 => soft drop
        // se vy<1000 => rotate
        if(vx>3000){
            tetris_soft_drop();
        } else if(vx<1000){
            tetris_rotate_clockwise();
        }

        // draw
        tetris_draw(); // desenha no display

        // se game_over => reinit
        if(tetris_is_game_over()){
            // piscar LED vermelho, etc.
            for(int i=0;i<3;i++){
                gpio_put(LED_R_PIN, true);
                sleep_ms(200);
                gpio_put(LED_R_PIN, false);
                sleep_ms(200);
                
                // Toca um tom de 200Hz por 100ms em cada buzzer (A e B) – pode ser simultâneo ou em sequência
                buzzer_a_play_tone(200, 100);
                buzzer_b_play_tone(200, 100);
                sleep_ms(100);
            }
            printf("Game Over. Score=%u\n", tetris_get_score());
            tetris_init();
        }

        sleep_ms(50);
    }

    return 0;
}
