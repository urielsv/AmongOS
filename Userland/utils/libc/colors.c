// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <colors.h>

static uint32_t color_to_hex(rgb_t color)
{
    return (color.r << 16) | (color.g << 8) | color.b;
}

uint32_t color(uint32_t r, uint32_t g, uint32_t b)
{
    rgb_t color;
    color.r = r;
    color.g = g;
    color.b = b;
    return color_to_hex(color);
}