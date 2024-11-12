/******************************************************************************
 *
 * @file    colors.h
 *
 * @brief   colors!!
 *
 * @author  luca pugliese                           <lpugliese@itba.edu.ar>
 * @author  felipes venturino                        <fventurino@itba.edu.ar>
 * @author  uriel sosa vazquez                      <usosavazquez@itba.edu.ar>
 *
 ******************************************************************************/
#ifndef colors_h
#define colors_h

#include <stdint.h>

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb_t;

typedef uint32_t color_t;

#define color_black color(0, 0, 0)
#define color_white color(255, 255, 255)
#define color_red color(255, 0, 0)
#define color_green color(0, 255, 0)
#define color_blue color(0, 0, 255)
#define color_yellow color(255, 255, 0)
#define color_cyan color(0, 255, 255)
#define color_magenta color(255, 0, 255)

// pass from r,g,b value to a color_t (hex value)
uint32_t color(uint32_t r, uint32_t g, uint32_t b);

#endif