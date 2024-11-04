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

#define HEAP_START 0x1000000
#define HEAP_SIZE (512ULL * 1024ULL)  // 512KB = 524,288 bytes
static void *const heapStartAddress = (void*)HEAP_START;
static void *const heapEndAddress = (void*)(HEAP_START + HEAP_SIZE); // Will be 0x1080000

typedef int (*EntryPoint)();

#endif