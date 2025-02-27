#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"

/** Envia 1 comando */
void ssd1306_command(ssd1306_t *ssd, uint8_t cmd) {
    ssd->port_buffer[0] = 0x80;   // Co=1, D/C#=0 => comando
    ssd->port_buffer[1] = cmd;
    i2c_write_blocking(ssd->i2c_port,
                       ssd->address,
                       ssd->port_buffer,
                       2,
                       false);
}

void ssd1306_init(ssd1306_t *ssd,
                  uint8_t width,
                  uint8_t height,
                  bool external_vcc,
                  uint8_t address,
                  i2c_inst_t *i2c)
{
    ssd->width        = width;
    ssd->height       = height;
    ssd->pages        = height / 8;
    ssd->address      = address;
    ssd->i2c_port     = i2c;
    ssd->external_vcc = external_vcc;

    ssd->bufsize = ssd->width * ssd->pages + 1; // 1 + width*pages
    ssd->ram_buffer = (uint8_t *)calloc(ssd->bufsize, sizeof(uint8_t));

    // Primeiro byte (índice 0) = 0x40 => data
    ssd->ram_buffer[0] = 0x40;

    // Para enviar comandos, iremos usar port_buffer
    ssd->port_buffer[0] = 0x80;
}

void ssd1306_config(ssd1306_t *ssd) {
    // Display OFF
    ssd1306_command(ssd, 0xAE);

    // Memory addressing mode => 0x00 (horizontal)
    ssd1306_command(ssd, SET_MEM_ADDR);
    ssd1306_command(ssd, 0x00);

    // Start line = 0
    ssd1306_command(ssd, SET_DISP_START_LINE | 0x00);
    // Segment remap col127->SEG0
    ssd1306_command(ssd, SET_SEG_REMAP | 0x00);

    ssd1306_command(ssd, SET_MUX_RATIO);
    ssd1306_command(ssd, ssd->height -1);

    // COM out direction
    ssd1306_command(ssd, SET_COM_OUT_DIR | 0x08);

    ssd1306_command(ssd, SET_DISP_OFFSET);
    ssd1306_command(ssd, 0x00);

    ssd1306_command(ssd, SET_COM_PIN_CFG);
    ssd1306_command(ssd, (ssd->height==64) ? 0x12 : 0x02);

    ssd1306_command(ssd, SET_DISP_CLK_DIV);
    ssd1306_command(ssd, 0x80);

    ssd1306_command(ssd, SET_PRECHARGE);
    ssd1306_command(ssd, ssd->external_vcc ? 0x22 : 0xF1);

    ssd1306_command(ssd, SET_VCOM_DESEL);
    ssd1306_command(ssd, 0x30);

    ssd1306_command(ssd, SET_CONTRAST);
    ssd1306_command(ssd, 0xFF);

    ssd1306_command(ssd, SET_ENTIRE_ON); // normal
    ssd1306_command(ssd, SET_NORM_INV);  // normal

    ssd1306_command(ssd, SET_CHARGE_PUMP);
    ssd1306_command(ssd, 0x14);

    // Display ON
    ssd1306_command(ssd, 0xAF);
}

/** Envia todo o buffer (ram_buffer) */
void ssd1306_send_data(ssd1306_t *ssd) {
    // define col range
    ssd1306_command(ssd, SET_COL_ADDR);
    ssd1306_command(ssd, 0x00);
    ssd1306_command(ssd, ssd->width -1);

    // define page range
    ssd1306_command(ssd, SET_PAGE_ADDR);
    ssd1306_command(ssd, 0x00);
    ssd1306_command(ssd, ssd->pages -1);

    i2c_write_blocking(ssd->i2c_port,
                       ssd->address,
                       ssd->ram_buffer,
                       ssd->bufsize,
                       false);
}

// Desenha ou apaga pixel
void ssd1306_pixel(ssd1306_t *ssd, uint8_t lx, uint8_t ly, bool value) {
    // 'lx' e 'ly' são as coordenadas LÓGICAS
    // Vamos convertê-las para coordenadas físicas no display
    //  px = ly
    //  py = (ssd->height - 1) - lx

    uint8_t px = ly;
    uint8_t py = (ssd->height -1) - lx;

    // Checar se está dentro da área
    if(px >= ssd->width || py >= ssd->height) return;

    // Agora armazena no buffer
    uint16_t index = 1 + px + (py >> 3) * ssd->width;
    uint8_t  bit   = 1 << (py & 7);

    if(value) {
        ssd->ram_buffer[index] |=  bit;
    } else {
        ssd->ram_buffer[index] &= ~bit;
    }
}


// Preenche a tela com value
void ssd1306_fill(ssd1306_t *ssd, bool value) {
    uint8_t byte_val= value? 0xFF: 0x00;
    for(size_t i=1; i<ssd->bufsize; i++){
        ssd->ram_buffer[i]= byte_val;
    }
}

/** Retângulo com borda e opcional fill. 
 *  x,y => canto sup-esq; w,h => dimensões.
 */
void ssd1306_rect(ssd1306_t *ssd,
                  uint8_t x, uint8_t y,
                  uint8_t w, uint8_t h,
                  bool value, bool fill)
{
    if(w==0 || h==0) return;
    uint8_t x2 = x+w-1;
    uint8_t y2 = y+h-1;

    // linha superior
    for(uint8_t xx=x; xx<=x2; xx++){
        ssd1306_pixel(ssd, xx, y, value);
    }
    // linha inferior
    for(uint8_t xx=x; xx<=x2; xx++){
        ssd1306_pixel(ssd, xx, y2, value);
    }
    // linha esquerda
    for(uint8_t yy=y; yy<=y2; yy++){
        ssd1306_pixel(ssd, x, yy, value);
    }
    // linha direita
    for(uint8_t yy=y; yy<=y2; yy++){
        ssd1306_pixel(ssd, x2, yy, value);
    }

    if(fill && w>2 && h>2) {
        for(uint8_t fill_y=y+1; fill_y<y2; fill_y++){
            for(uint8_t fill_x=x+1; fill_x<x2; fill_x++){
                ssd1306_pixel(ssd, fill_x, fill_y, value);
            }
        }
    }
}

/** Bresenham line */
void ssd1306_line(ssd1306_t *ssd,
                  uint8_t x0, uint8_t y0,
                  uint8_t x1, uint8_t y1,
                  bool value)
{
    int dx= abs(x1-x0), sx= (x0<x1)?1:-1;
    int dy= -abs(y1-y0), sy= (y0<y1)?1:-1;
    int err= dx+dy;

    while(true){
        ssd1306_pixel(ssd,x0,y0,value);
        if(x0==x1 && y0==y1) break;
        int e2= 2*err;
        if(e2>= dy){
            err+= dy; x0+= sx;
        }
        if(e2<= dx){
            err+= dx; y0+= sy;
        }
    }
}

void ssd1306_hline(ssd1306_t *ssd,
                   uint8_t x0, uint8_t x1,
                   uint8_t y, bool value)
{
    if(x1<x0){
        uint8_t tmp= x0; x0=x1; x1=tmp;
    }
    for(uint8_t x=x0; x<=x1; x++){
        ssd1306_pixel(ssd, x,y,value);
    }
}

void ssd1306_vline(ssd1306_t *ssd,
                   uint8_t x, uint8_t y0,
                   uint8_t y1, bool value)
{
    if(y1<y0){
        uint8_t tmp= y0; y0=y1; y1=tmp;
    }
    for(uint8_t y=y0; y<=y1;y++){
        ssd1306_pixel(ssd, x,y,value);
    }
}

/** Desenha caractere 8x8 da fonte */
void ssd1306_draw_char(ssd1306_t *ssd, char c, uint8_t x, uint8_t y)
{
    uint16_t index=0;
    if(c>='0'&& c<='9'){
        index= (c-'0'+1)*8; // 1=> '0'
    } else if(c>='A'&& c<='Z'){
        index= (c-'A'+11)*8; // 11=> 'A'
    } else {
        index=0; // se fora do range
    }

    for(uint8_t col=0; col<8; col++){
        uint8_t line= font[index+ col];
        for(uint8_t row=0; row<8; row++){
            bool pixel_on= (line &(1<<row)) !=0;
            ssd1306_pixel(ssd, x+col, y+row, pixel_on);
        }
    }
}

/** Desenha string, cada char=8x8 */
void ssd1306_draw_string(ssd1306_t *ssd, const char *str,
                         uint8_t x, uint8_t y)
{
    while(*str){
        ssd1306_draw_char(ssd, *str, x,y);
        x+=8;
        if(x+8>= ssd->width){
            x=0;
            y+=8;
        }
        if(y+8>= ssd->height) break;
        str++;
    }
}

// -------------------------------------------------------------------
// *NOVAS* Funções equivalentes às que tetris.c chama
// -------------------------------------------------------------------

void ssd1306_clear(ssd1306_t *ssd){
    // apaga tudo
    ssd1306_fill(ssd,false);
}

void ssd1306_fill_rect(ssd1306_t *ssd, 
                       uint8_t x, uint8_t y,
                       uint8_t w, uint8_t h,
                       bool value)
{
    // retângulo preenchido
    ssd1306_rect(ssd, x,y, w,h, value,true);
}

void ssd1306_show(ssd1306_t *ssd){
    // envia buffer ao display
    ssd1306_send_data(ssd);
}
