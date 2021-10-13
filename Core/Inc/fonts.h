#ifndef _FONTS_H
#define _FONTS_H

#include <stdint.h>

//
//  Structure used to define fonts
//
typedef struct {
    const uint8_t FontWidth;    /* Font width in pixels */
    uint8_t FontHeight;         /* Font height in pixels */
    const uint16_t *data;       /* Pointer to data font data array */
} FontDef;
//
//  Structure used to define fonts 8 bit
//
typedef struct {
    const uint8_t FontWidth;    /* Font width in pixels */
    uint8_t FontHeight;         /* Font height in pixels */
    const uint8_t *data;       /* Pointer to data font data array */
} FontDefEightBit;

//
//  Export the 3 available fonts
//
extern FontDefEightBit Font_6x7;
extern FontDef Font_7x10;
extern FontDef Font_11x18;
extern FontDef Font_16x26;
extern uint16_t Logo37x43[];
extern uint16_t Battery [];

#endif  // _FONTS_H
