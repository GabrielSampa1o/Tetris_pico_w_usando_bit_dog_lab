#ifndef SSD1306_H
#define SSD1306_H

#include <stdbool.h>
#include <stdint.h>
#include "hardware/i2c.h"

// Definições de comando
typedef enum {
  SET_CONTRAST       = 0x81,
  SET_ENTIRE_ON      = 0xA4,
  SET_NORM_INV       = 0xA6,
  SET_DISP           = 0xAE,
  SET_MEM_ADDR       = 0x20,
  SET_COL_ADDR       = 0x21,
  SET_PAGE_ADDR      = 0x22,
  SET_DISP_START_LINE= 0x40,
  SET_SEG_REMAP      = 0xA0,
  SET_MUX_RATIO      = 0xA8,
  SET_COM_OUT_DIR    = 0xC0,
  SET_DISP_OFFSET    = 0xD3,
  SET_COM_PIN_CFG    = 0xDA,
  SET_DISP_CLK_DIV   = 0xD5,
  SET_PRECHARGE      = 0xD9,
  SET_VCOM_DESEL     = 0xDB,
  SET_CHARGE_PUMP    = 0x8D
} ssd1306_command_t;

typedef struct {
  uint8_t width;
  uint8_t height;
  uint8_t pages;        // height/8
  uint8_t address;
  bool    external_vcc;
  i2c_inst_t *i2c_port;

  uint8_t *ram_buffer;
  size_t   bufsize;

  uint8_t  port_buffer[2];
} ssd1306_t;

/** Inicializa a estrutura ssd e aloca buffer. */
void ssd1306_init(ssd1306_t *ssd,
                  uint8_t width,
                  uint8_t height,
                  bool external_vcc,
                  uint8_t address,
                  i2c_inst_t *i2c);

/** Envia sequência padrão de config e liga o display. */
void ssd1306_config(ssd1306_t *ssd);

/** Envia 1 byte de comando. */
void ssd1306_command(ssd1306_t *ssd, uint8_t cmd);

/** Envia todo o buffer ram_buffer ao display. */
void ssd1306_send_data(ssd1306_t *ssd);

/** Desenha ou apaga 1 pixel. */
void ssd1306_pixel(ssd1306_t *ssd, uint8_t x, uint8_t y, bool value);

/** Preenche completamente a tela com true(1) ou false(0). */
void ssd1306_fill(ssd1306_t *ssd, bool value);

/** Desenha um retângulo (com ou sem preenchimento). */
void ssd1306_rect(ssd1306_t *ssd, uint8_t x, uint8_t y,
                  uint8_t w, uint8_t h,
                  bool value, bool fill);

/** Desenha uma linha usando Bresenham. */
void ssd1306_line(ssd1306_t *ssd,
                  uint8_t x0, uint8_t y0,
                  uint8_t x1, uint8_t y1,
                  bool value);

void ssd1306_hline(ssd1306_t *ssd,
                   uint8_t x0, uint8_t x1,
                   uint8_t y, bool value);

void ssd1306_vline(ssd1306_t *ssd,
                   uint8_t x, uint8_t y0,
                   uint8_t y1, bool value);

/** Desenha caractere 8x8. */
void ssd1306_draw_char(ssd1306_t *ssd, char c, uint8_t x, uint8_t y);
/** Desenha string a partir de (x,y). */
void ssd1306_draw_string(ssd1306_t *ssd, const char *str,
                         uint8_t x, uint8_t y);

/** 
 *  *NOVAS* Funções equivalentes às chamadas
 *   que você faz em 'tetris.c'.
 */
void ssd1306_clear(ssd1306_t *ssd);
void ssd1306_fill_rect(ssd1306_t *ssd, 
                       uint8_t x, uint8_t y,
                       uint8_t w, uint8_t h,
                       bool value);
void ssd1306_show(ssd1306_t *ssd);

#endif // SSD1306_H
