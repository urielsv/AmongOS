/******************************************************************************
 *
 * @file    sound.h
 *
 * @brief   sound functions to play sounds.
 *
 * @author  luca pugliese                           <lpugliese@itba.edu.ar>
 * @author  felipes venturino                        <fventurino@itba.edu.ar>
 * @author  uriel sosa vazquez                      <usosavazquez@itba.edu.ar>
 *
 ******************************************************************************/

#ifndef sound_h
#define sound_h
#include <stdint.h>

// setup the sound with frequency.
extern void asm_start_sound(uint32_t freq);

// stop the sound.
extern void asm_end_sound();


/*
 * @name sound
 *
 * @brief plays a sound with the frequency and duration passed as parameters.
 * 
 * @param freq frequency of the sound.
 * @param duration duration of the sound.
 */
void sound(uint8_t freq, uint64_t duration);

// play a beep sound.
void beep();
#endif