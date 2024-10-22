/******************************************************************************
 *
 * @file    userland.h
 *
 * @brief   Memor addresses for userland code and sample data.
 *
 * @author  Luca Pugliese                           <lpugliese@itba.edu.ar>
 * @author  Felipes Venturino                        <fventurino@itba.edu.ar>
 * @author  Uriel Sosa Vazquez                      <usosavazquez@itba.edu.ar>
 *
 ******************************************************************************/
#ifndef USERLAND_H
#define USERLAND_H

// TODO: Refactor file name
static void *const userlandCodeModuleAddress = (void*)0x400000;
static void *const sampleDataModuleAddress = (void*)0x500000;

static void *const heapStartAddress = (void*)0x1000000; 
static void *const heapEndAddress = (void *)0x2000000;

typedef int (*EntryPoint)();

#endif