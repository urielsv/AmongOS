// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "snake.h"
#include <amongus.h>
#include <colors.h>
#include <draw.h>
#include <random.h>
#include <stdio.h>
#include <stdlib.h>
#include <syscalls.h>

#define pixel 16
#define board_size 32
#define fruit -1

#define draw_start_x 10 * (pixel)
#define draw_start_y 5 * (pixel)
#define snake_length 5

struct controller {
    uint8_t up;
    uint8_t down;
    uint8_t left;
    uint8_t right;
};

struct snake_body {
    uint64_t x;
    uint64_t y;
    uint8_t dir;
};

struct snake {
    struct snake_body head;
    uint64_t len;
};

struct food {
    uint64_t x;
    uint64_t y;
};

struct player {
    int uid;
    color_t color;
    snake_t snake;
    controller_t controller;
    uint8_t alive;
};

struct block {
    int uid;
    int count;
};

static block_t collision_board[board_size][board_size];
static uint8_t player_count;

int snake() {

    player_count = 0;
    main_menu();
    return 0;
}


void main_menu() {
    uint64_t width, height;
    screen_info(&width, &height);
    player_t player1 = {0}; 
    player_t player2 = {0};

    clear(color_black);
    amongus(0, height / 2 + 160);
    font_size(2);
    writing_pos(width / 2 - 12 * pixel, 90);
    printf_color("snake: ", color_red, color_black);
    printf_color("the last impostor", color_white, color_black);
    font_size(1);
    writing_pos(width / 2 - 5 * pixel, height / 2 - 8 * pixel);
    int plcount = 0;
    scanf("player count (1-2): %d", &plcount);

    // initialize collision board
    for (int i = 0; i < board_size; i++) {
        for (int j = 0; j < board_size; j++) {
            collision_board[i][j].uid = 0;
            collision_board[i][j].count = 0;
        }
    }

    if (plcount == 1) {
        init_player(player1, color_red, (controller_t){'w', 's', 'a', 'd'});
    } else if (plcount == 2) {
        init_player(player1, color_red, (controller_t){'w', 's', 'a', 'd'});
        init_player(player2, color_green, (controller_t){'i', 'k', 'j', 'l'});
    } else {
        clear(color_black);
        printf_color("invalid player count, aborting...\n", color_white, color_black);
        sleep(500);
        clear(color_black);
        return;
    }

    for (int i = 3; i > 0; i--) {
        writing_pos(width / 2 - 5 * pixel, height / 2 - 6 * pixel);
        printf("game starting in: %d", i);
        sleep(500);
    }

    clear(color_black);

    game(player1, player2);
}

void game(player_t player1, player_t player2) {

    draw_board(color_white, board_size * pixel, draw_start_x, draw_start_y);
    hud(player1, player2);

    food();

    do {

        draw_game(player1, player2);
        move_player(player1, player2);

        sleep(200);
    } while ((player_count == 1 && player1->alive) ||
             (player_count == 2 && (player1->alive || player2->alive)));

    gameover_menu();
}

void gameover_menu() {

    uint64_t width, height;
    screen_info(&width, &height);

    clear(color_black);

    font_size(2);
    writing_pos(width / 2 - 12 * pixel, 90);
    printf_color("snake: ", color_red, color_black);
    printf_color("the last impostor", color_white, color_black);
    font_size(1);
    writing_pos(width / 2 - 9 * pixel, height / 2 - 9 * pixel);
    printf("the game is over, good luck next time!");
    writing_pos(width / 2 - 5 * pixel + 8, height / 2 - 6 * pixel);
    printf("press 'q' to quit");
    writing_pos(width / 2 - 5 * pixel + 8, height / 2 - 4 * pixel);
    printf("press 'r' to restart");
    uint8_t ch;
    do {
        ch = getchar();
    } while (ch != 'q' && ch != 'r');

    if (ch == 'q') {
        clear(color_black);
        writing_pos(width / 2 - 5 * pixel, height / 2 - pixel);
        printf_color("thanks for playing! <3\n", color_green, color_black);
         for(int i = 1000; i > 850; i-=10){
            sound(i, 100);
    }
        sleep(500);
        clear(color_black);
        return;
    } else //if (ch == 'r') 
    {
        clear(color_black);
        snake();
    }
    clear(color_black);
}

void init_player(player_t player, color_t color, controller_t controller) {
    player->color = color;
    player->controller = controller;
    player->snake.len = snake_length;

    player->alive = 1;

    player->snake.head.x = 4 + player_count * 15;
    player->snake.head.y = 6;

    player->uid = ++player_count;
    player->snake.head.dir = player_count == 1 ? controller.right : controller.left;

    collision_board[player->snake.head.x][player->snake.head.y].uid = player->uid;
    collision_board[player->snake.head.x][player->snake.head.y].count = player->snake.len;
}

void draw_board(color_t color, uint16_t size, uint64_t x, uint64_t y) {
    draw_rectangle(color, size + pixel, x - pixel, y - pixel);
}

// change name to update board
void draw_game(player_t player1, player_t player2) {
    for (int i = 0; i < board_size; i++) {
        for (int j = 0; j < board_size; j++) {
            switch (collision_board[i][j].uid) {
            case fruit:
                draw_pixel(color_magenta, pixel / 2, draw_start_x + i * pixel, draw_start_y + j * pixel);
                break;
            case 0:
                draw_pixel(color_black, pixel, draw_start_x + i * pixel, draw_start_y + j * pixel);
                break;
            case 1:
                if (player1->alive)
                    draw_pixel(player1->color, pixel, draw_start_x + i * pixel, draw_start_y + j * pixel);
                break;
            case 2:
                if (player2->alive)
                    draw_pixel(player2->color, pixel, draw_start_x + i * pixel, draw_start_y + j * pixel);
                break;
            }

            if (collision_board[i][j].count > 0) {
                collision_board[i][j].count--;
            } else if (collision_board[i][j].uid != fruit) {
                collision_board[i][j].uid = 0;
            }
        }
    }
}

static int test_input_dir(uint8_t ch, player_t player) {
    return ch == player->controller.up || ch == player->controller.down ||
           ch == player->controller.left || ch == player->controller.right;
}

void move_player(player_t player1, player_t player2) {

    char ch = getchar();

    if (test_input_dir(ch, player1)) {
        update_position(player1, ch);
    } else {
        update_position(player1, 0);
    }

    if (player_count == 2) {
        if (test_input_dir(ch, player2)) {
            update_position(player2, ch);
        } else {
            update_position(player2, 0);
        }
    }
}

void update_position(player_t player, uint8_t dir) {

    if ((dir == 0) || (dir == player->controller.up && player->snake.head.dir == player->controller.down) ||
        (dir == player->controller.down && player->snake.head.dir == player->controller.up) ||
        (dir == player->controller.left && player->snake.head.dir == player->controller.right) ||
        (dir == player->controller.right && player->snake.head.dir == player->controller.left)) {
        dir = player->snake.head.dir;
    } else {
        player->snake.head.dir = dir;
    }

    if (dir == player->controller.up) {
        player->snake.head = (snake_body_t){player->snake.head.x, player->snake.head.y - 1,
            player->snake.head.dir};
    } else if (dir == player->controller.right) {
        player->snake.head = (snake_body_t){player->snake.head.x + 1, player->snake.head.y,
            player->snake.head.dir};
    } else if (dir == player->controller.down) {
        player->snake.head = (snake_body_t){player->snake.head.x, player->snake.head.y + 1,
            player->snake.head.dir};
    } else if (dir == player->controller.left) {
        player->snake.head = (snake_body_t){player->snake.head.x - 1, player->snake.head.y,
            player->snake.head.dir};
    }

    check_collision(player);
    collision_board[player->snake.head.x][player->snake.head.y].count = player->snake.len;
    collision_board[player->snake.head.x][player->snake.head.y].uid = player->uid;
}

void food() {

    int x, y;
    do {
        x = rand(21304, board_size);
        y = rand(21304, board_size);
    } while (collision_board[x][y].uid != 0);

    collision_board[x][y].uid = fruit;
}

void check_collision(player_t player) {

    // check player in bound of playing area
    if ( player->snake.head.x >= board_size|| player->snake.head.y >= board_size) {
        player->alive = 0;
        delete_snake(player);
        kill_snake(player);
        return;
    }

    switch (collision_board[player->snake.head.x][player->snake.head.y].uid) {
    // empty space, do nothing
    case 0:
        break;

    case fruit:
        collision_board[player->snake.head.x][player->snake.head.y].uid = player->uid;
        increment_length(player);
        food();
        update_points(player);
        sound(1000, 100);
        break;

    // collided with a player or themselves
    default:
        player->alive = 0;
        delete_snake(player);
        kill_snake(player);
        break;
    }
}

void delete_snake(player_t player) {
    // iterate through the colliosion matrix and replace all the values of the player id with 0
    for (int i = 0; i < board_size; i++) {
        for (int j = 0; j < board_size; j++) {
            if (collision_board[i][j].uid == player->uid) {
                collision_board[i][j].uid = 0;
                collision_board[i][j].count = 0;
                player->snake.len = 0;
                player->snake.head.x = -1;
                player->snake.head.y = -1;
            }
        }
    }
}

void increment_length(player_t player) {
    player->snake.len++;
}

void update_points(player_t player) {
    if (player->uid == 1) {
        writing_pos(46 * pixel, 7 * pixel);
    } else if (player->uid == 2) {
        writing_pos(46 * pixel, 17 * pixel);
    }
    printf("points: %d\n", player->snake.len - snake_length);
}

void hud(player_t player1, player_t player2) {
    switch (player_count) {
    case 2:
        draw_rectangle(color_white, 8 * pixel, 45 * pixel, 15 * pixel);
        writing_pos(46 * pixel, 16 * pixel);
        printf("player 2");
        update_points(player2);
    case 1:
        draw_rectangle(color_white, 8 * pixel, 45 * pixel, 5 * pixel);
        writing_pos(46 * pixel, 6 * pixel);
        printf("player 1");
        update_points(player1);
        break;
    }
}

void kill_snake(player_t player) {
    if (player->uid == 1) {
        writing_pos(47 * pixel, 8 * pixel);
    } else if (player->uid == 2) {
        writing_pos(47 * pixel, 18 * pixel);
    }
    printf_color("executed", color_red, color_black);
}