#include <amongus.h>
#include <stdint.h>

#define PIXEL 8
void amongus(uint64_t startx, uint64_t starty) {

    uint64_t x = startx;
    uint64_t y = starty;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            switch(silence_art[i][j]) {
                case '/':
                    draw(0xFFFFED, x, y);
                    break;
                case ',':
                case '*':
                    draw(COLOR_YELLOW, x, y);
                    break;
                case '@':
                    draw(COLOR_WHITE, x, y);
                    break;
                case '&':
                    draw(0xC24641, x, y);
                    break;
                case '%':
                    draw(COLOR_RED, x, y);
                    break;
                case '#':
                case '(':
                    draw(0x808080, x, y);
                    break;
                case '.':
                    draw(0xd3d3d3, x, y);
                    break;
                default:
                    break;
            }
            x += PIXEL;
        }
        x = startx;
        y += PIXEL;
    }
}
