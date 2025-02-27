#include "tetris.h"
#include <string.h>  // memset
#include <stdlib.h>  // rand
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "buzzer.h"

// Se você precisa usar o driver ssd1306, inclua:
#include "ssd1306.h"

#define buzzer_play_tone(freq, dur) do { \
    buzzer_a_play_tone((freq), (dur));  \
    buzzer_b_play_tone((freq), (dur));  \
} while(0)

#define buzzer_beep() do { \
    buzzer_a_beep();         \
    buzzer_b_beep();         \
} while(0)


// Precisamos de uma referência global ou 'extern' para o display:
extern ssd1306_t g_oled_dev; 

static int board[TETRIS_WIDTH][TETRIS_HEIGHT];
static bool game_over = false;
static uint32_t score = 0;

static uint32_t gravity_interval = 800;
static uint32_t gravity_timer    = 0;

typedef struct {
    const unsigned int *shape;
    int x, y;
    int rotation; // 0..3
    int color_id; // 1..7
} TetrisPiece;

static TetrisPiece current;
static TetrisPiece next;

// Shapes
static unsigned int I_SHAPE[] = {0x0F00,0x2222,0x00F0,0x4444};
static unsigned int O_SHAPE[] = {0xCC00,0xCC00,0xCC00,0xCC00};
static unsigned int T_SHAPE[] = {0x0E40,0x4C40,0x4E00,0x4640};
static unsigned int S_SHAPE[] = {0x06C0,0x8C40,0x6C00,0x4620};
static unsigned int Z_SHAPE[] = {0x0C60,0x4C80,0xC600,0x2640};
static unsigned int J_SHAPE[] = {0x44C0,0x8E00,0x6440,0x0E20};
static unsigned int L_SHAPE[] = {0x4460,0x0E80,0xC440,0x2E00};

static const unsigned int *ALL_SHAPES[] = {
    I_SHAPE, O_SHAPE, T_SHAPE, S_SHAPE, Z_SHAPE, J_SHAPE, L_SHAPE
};
static int ALL_COLORS[] = {1,2,3,4,5,6,7};

static void new_game(void);
static void spawn_piece(void);
static bool check_collision(const TetrisPiece *p, int nx, int ny, int nrot);
static void lock_piece(void);
static void remove_lines(void);

void tetris_init(void) {
    srand(1234); // ou algo mais aleatório
    new_game();
}

static void new_game(void) {
    memset(board, 0, sizeof(board));
    game_over = false;
    score     = 0;
    gravity_interval = 800;
    gravity_timer    = 0;

    // inicia 'next'
    int idx = rand() % 7;
    next.shape    = ALL_SHAPES[idx];
    next.x        = 3;
    next.y        = 0;
    next.rotation = 0;
    next.color_id = ALL_COLORS[idx];

    spawn_piece();
}

static void spawn_piece(void) {
    current = next;
    int idx = rand() % 7;
    next.shape    = ALL_SHAPES[idx];
    next.x        = 3;
    next.y        = 0;
    next.rotation = 0;
    next.color_id = ALL_COLORS[idx];

    if(check_collision(&current, current.x, current.y, current.rotation)) {
        game_over = true;
    }
}

static bool check_collision(const TetrisPiece *p, int nx, int ny, int nrot) {
    unsigned int blocks = p->shape[nrot];
    unsigned int bit = 0x8000;
    int row=0, col=0;

    for(; bit>0; bit>>=1) {
        if(blocks & bit) {
            int bx = nx + col;
            int by = ny + row;
            if(bx<0 || bx>=TETRIS_WIDTH || by<0|| by>=TETRIS_HEIGHT) {
                return true;
            }
            if(board[bx][by] != 0) {
                return true;
            }
        }
        col++;
        if(col==4) { col=0; row++; }
    }
    return false;
}

void tetris_update(uint32_t dt_ms) {
    if(game_over) return;

    gravity_timer += dt_ms;
    if(gravity_timer >= gravity_interval) {
        gravity_timer = 0;
        // move down
        int ny = current.y + 1;
        if(check_collision(&current, current.x, ny, current.rotation)) {
            lock_piece();
            remove_lines();
            spawn_piece();
        } else {
            current.y = ny;
        }
    }
}

static void lock_piece(void) {
    unsigned int blocks = current.shape[current.rotation];
    unsigned int bit=0x8000;
    int row=0,col=0;
    for(; bit>0; bit>>=1) {
        if(blocks & bit) {
            int bx = current.x + col;
            int by = current.y + row;
            board[bx][by] = current.color_id;
        }
        col++;
        if(col==4){ col=0; row++; }
    }
    buzzer_a_play_tone(300, 50);
    buzzer_b_play_tone(300, 50);

}

static void remove_lines(void) {
    int lines_cleared=0;
    for(int y=0; y<TETRIS_HEIGHT; y++){
        bool full=true;
        for(int x=0; x< TETRIS_WIDTH;x++){
            if(board[x][y]==0){
                full=false;
                break;
            }
        }
        if(full){
            lines_cleared++;
            // shift everything down
            for(int yy=y; yy>0;yy--){
                for(int xx=0;xx<TETRIS_WIDTH;xx++){
                    board[xx][yy]= board[xx][yy-1];
                }
            }
            for(int xx=0;xx<TETRIS_WIDTH;xx++){
                board[xx][0]=0;
            }
        }
    }
    if(lines_cleared>0){
        score += (100U * (int)pow(2,(lines_cleared-1)));
        if(gravity_interval>100){
            gravity_interval-= (20*lines_cleared);
        }
        // Toca um tom mais longo ao completar linha(s)
        buzzer_play_tone(600, 150);
    }
}

void tetris_move_left(void) {
    if(game_over)return;
    int nx= current.x-1;
    if(!check_collision(&current,nx,current.y,current.rotation)){
        current.x= nx;

        // Som ao mover para a esquerda
        buzzer_a_beep();
        buzzer_b_beep();

    }
}
void tetris_move_right(void){
    if(game_over)return;
    int nx= current.x+1;
    if(!check_collision(&current,nx,current.y,current.rotation)){
        current.x=nx;

        // Som ao mover para a esquerda
        buzzer_a_beep();
        buzzer_b_beep();

    }
}

void tetris_rotate_clockwise(void) {
    if(game_over)return;
    int nr= (current.rotation+1)%4;
    if(!check_collision(&current,current.x,current.y,nr)){
        current.rotation= nr;
        buzzer_beep(); // Som ao girar (horário)
    }
}

void tetris_rotate_counter(void){
    if(game_over)return;
    int nr= (current.rotation+3)%4;
    if(!check_collision(&current,current.x,current.y,nr)){
        current.rotation= nr;
        buzzer_beep(); // Som ao girar (horário)
    }
}

void tetris_soft_drop(void){
    if(game_over)return;
    int ny= current.y+1;
    if(check_collision(&current,current.x,ny,current.rotation)){
        lock_piece();
        remove_lines();
        spawn_piece();
    } else {
        current.y=ny;
    }
}

void tetris_hard_drop(void){
    if(game_over)return;
    while(!check_collision(&current,current.x,current.y+1,current.rotation)){
        current.y++;
    }
    lock_piece();
    remove_lines();
    spawn_piece();
}

bool tetris_is_game_over(void){
    return game_over;
}

uint32_t tetris_get_score(void){
    return score;
}

/**
 * tetris_draw: desenha o estado do Tetris no display SSD1306
 * Cada célula ~ 4x4 px (ou 6x6, etc.) 
 */
void tetris_draw(void) {
    // Apaga display
    ssd1306_clear(&g_oled_dev);

    const int cell_w = 6; 
    const int cell_h = 6;

    // Desenha o board
    for(int y=0; y< TETRIS_HEIGHT; y++){
        for(int x=0; x< TETRIS_WIDTH; x++){
            int c= board[x][y];
            if(c!=0){
                ssd1306_fill_rect(&g_oled_dev,
                    x*cell_w, y*cell_h,
                    cell_w, cell_h,
                    true);
            }
        }
    }

    // Desenha a peça atual
    unsigned int blocks= current.shape[current.rotation];
    unsigned int bit=0x8000;
    int row=0, col=0;
    for(;bit>0; bit>>=1){
        if(blocks & bit){
            int bx= current.x+col;
            int by= current.y+row;
            ssd1306_fill_rect(&g_oled_dev,
                bx*cell_w, by*cell_h,
                cell_w, cell_h,
                true);
        }
        col++;
        if(col==4){col=0; row++;}
    }

    // Conclui enviando ao display
    ssd1306_show(&g_oled_dev);
}
