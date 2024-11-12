/******************************************************************************
 *
 * @file    string.h
 *
 * @brief   string related functions.
 *
 * @author  luca pugliese                           <lpugliese@itba.edu.ar>
 * @author  felipes venturino                        <fventurino@itba.edu.ar>
 * @author  uriel sosa vazquez                      <usosavazquez@itba.edu.ar>
 *
 ******************************************************************************/

#ifndef string_h
#define string_h
#include <stdint.h>

/*
 * @brief compares the src to tgt.
 * @param src pointer to the source of data.
 * @param tgt pointer to the destination array.
 */
int8_t strcmp(const char* src, const char* tgt);


/*
 * @brief size of the string.
 * @returns length of the string.
 */
uint64_t strlen(char* buff);


/*
 * @brief copy the string from src to dest.
 * @param dest pointer to the destination array where the content is to be copied.
 * @param src string to be copied.
 */
void strcpy(char* dest, char* src);

char *strchr(char* str, char c);

char to_lower(char c);
#endif
