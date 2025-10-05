#ifndef __MAIN_H
#define __MAIN_H

#include <rgbdisplay.h>
#include <stddef.h>
#include <stdint.h>

extern RgbDisplay disp;
extern size_t *rgbDisplayTxIndex;
extern uint32_t *rgbDisplayBuffer;
extern size_t tmpIndex;

#endif /* __MAIN_H */
