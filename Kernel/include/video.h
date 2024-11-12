/******************************************************************************
 *
 * @file    video.h
 *
 * @brief   video functions to draw on screen while in video mode.
 *
 * @author  luca pugliese                           <lpugliese@itba.edu.ar>
 * @author  felipes venturino                        <fventurino@itba.edu.ar>
 * @author  uriel sosa vazquez                      <usosavazquez@itba.edu.ar>
 *
 ******************************************************************************/

#ifndef video_h__
#define video_h__

#include <stdint.h>


// write a character to the screen
void put_char_at(unsigned char c, uint64_t *x, uint64_t *y, uint64_t fgcolor, uint64_t bgcolor);

// remove last character from the screen
void delete_char(uint64_t *x, uint64_t *y, uint64_t fgcolor, uint64_t bgcolor);

// fill the whole screen with a color
void clear_screen(uint32_t hex_color);

// put a pixel in position (pixel size is 1x1)
void put_pixel(uint32_t hex_color, uint64_t x, uint64_t y);

// set de font size for the draweable characters
void video_fontsize(int newsize);

// screen information (width and height)
int get_width();
int get_height();
void new_line(uint64_t *x, uint64_t *y);
#endif