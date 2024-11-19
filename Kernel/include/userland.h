/******************************************************************************
 *
 * @file    userland.h
 *
 * @brief   memor addresses for userland code and sample data.
 *
 * @author  luca pugliese                           <lpugliese@itba.edu.ar>
 * @author  felipes venturino                        <fventurino@itba.edu.ar>
 * @author  uriel sosa vazquez                      <usosavazquez@itba.edu.ar>
 *
 ******************************************************************************/
#ifndef userland_h
#define userland_h

// todo: refactor file name
static void *const userland_code_module_address = (void*)(uintptr_t)0x400000;
static void *const sample_data_module_address = (void*)(uintptr_t)0x500000;

static void *const heap_start_address = (void*)(uintptr_t)0x1000000; 
static void *const heap_end_address = (void *)(uintptr_t)0x2000000;

typedef int (*entry_point)();

#endif