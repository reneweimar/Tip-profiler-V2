//-----------------------------------------------------------------------------
//! \file       fonts.c
//! \author     R. Weimar
//! \brief      Contains definitions of the fonts for the display controller
//! \details    
//! \Attention  
//-----------------------------------------------------------------------------
#include "main.h"
#include "fonts.h"
//-----------------------------------------------------------------------------
//! \Font6x7 definition
static const uint8_t Font6x7 [] = { 
  0x00,0x00,0x00,0x00,0x00,0x00,0x00, //sp
  0x20,0x20,0x20,0x20,0x20,0x00,0x20, //!
  0x50,0x50,0x50,0x00,0x00,0x00,0x00, //"
  0x10,0x30,0x70,0xF0,0x70,0x30,0x10, //# ARROW LEFT
  0x00,0x20,0x20,0x70,0x70,0xF8,0xF8, //$ ARROW UP
  0x48,0xA8,0x50,0x20,0x50,0xA8,0x90, //%
  0x00,0xF8,0xF8,0x70,0x70,0x20,0x20, //& ARROW DOWN
  0x20,0x20,0x20,0x00,0x00,0x00,0x00, //'
  0x10,0x20,0x40,0x40,0x40,0x20,0x10, //(
  0x40,0x20,0x10,0x10,0x10,0x20,0x40, //)
  0x00,0xF8,0x00,0xF8,0x00,0xF8,0x00, //* MENU
  0x00,0x00,0x20,0x20,0xF8,0x20,0x20, //+
  0x80,0xC0,0xE0,0xF0,0xE0,0xC0,0x80, //, ARROW RIGHT
  0x00,0x00,0x00,0xF8,0x00,0x00,0x00, //-
  0x00,0x00,0x00,0x00,0x00,0x00,0x20, //.
  0x50,0x50,0x50,0xF8,0x88,0x88,0x70, /// PLUGGED IN
  0x70,0x88,0x88,0xA8,0x88,0x88,0x70, //0
  0x20,0x60,0x20,0x20,0x20,0x20,0x70, //1
  0x70,0x88,0x08,0x70,0x80,0x80,0xF8, //2
  0x70,0x88,0x08,0x70,0x08,0x88,0x70, //3
  0x88,0x88,0x88,0xF8,0x08,0x08,0x08, //4
  0xF8,0x80,0x80,0xF0,0x08,0x88,0x70, //5
  0x70,0x88,0x80,0xF0,0x88,0x88,0x70, //6
  0xF8,0x08,0x10,0x20,0x40,0x40,0x40, //7
  0x70,0x88,0x88,0x70,0x88,0x88,0x70, //8
  0x70,0x88,0x88,0x78,0x08,0x88,0x70, //9
  0x00,0x00,0x20,0x00,0x00,0x00,0x20, //:
  0x00,0x00,0x20,0x00,0x20,0x20,0x40, //;
  0x10,0x20,0x40,0x80,0x40,0x20,0x10, //<
  0x00,0x00,0xF8,0x00,0xF8,0x00,0x00, //=
  0x80,0x40,0x20,0x10,0x20,0x40,0x80, //>
  0x30,0x48,0x08,0x30,0x20,0x00,0x20, //?
  0x00,0x00,0x00,0x00,0x00,0x00,0x00, //@
  0x70,0x88,0x88,0xF8,0x88,0x88,0x88, //A
  0xF0,0x88,0x88,0xF8,0x88,0x88,0xF0, //B
  0x70,0x88,0x80,0x80,0x80,0x88,0x70, //C
  0xF0,0x88,0x88,0x88,0x88,0x88,0xF0, //D
  0xF8,0x80,0x80,0xF0,0x80,0x80,0xF8, //E
  0xF8,0x80,0x80,0xF0,0x80,0x80,0x80, //F
  0x70,0x88,0x80,0xB8,0x88,0x88,0x78, //G
  0x88,0x88,0x88,0xF8,0x88,0x88,0x88, //H
  0x70,0x20,0x20,0x20,0x20,0x20,0x70, //I
  0x38,0x08,0x08,0x08,0x08,0x88,0x70, //J
  0x88,0x90,0xA0,0xC0,0xA0,0x90,0x88, //K
  0x80,0x80,0x80,0x80,0x80,0x80,0xF8, //L
  0x88,0xD8,0xA8,0x88,0x88,0x88,0x88, //M
  0x88,0x88,0xC8,0xA8,0x98,0x88,0x88, //N
  0x70,0x88,0x88,0x88,0x88,0x88,0x70, //O
  0xF0,0x88,0x88,0xF0,0x80,0x80,0x80, //P
  0x70,0x88,0x88,0x88,0x88,0x98,0x78, //Q
  0xF0,0x88,0x88,0xF0,0x90,0x88,0x88, //R
  0x70,0x88,0x80,0x70,0x08,0x88,0x70, //S
  0xF8,0x20,0x20,0x20,0x20,0x20,0x20, //T
  0x88,0x88,0x88,0x88,0x88,0x88,0x70, //U
  0x88,0x88,0x88,0x88,0x88,0x50,0x20, //V
  0xA8,0xA8,0xA8,0xA8,0xA8,0xA8,0x50, //W
  0x88,0x88,0x50,0x20,0x50,0x88,0x88, //X
  0x88,0x88,0x50,0x20,0x20,0x20,0x20, //Y
  0xF8,0x08,0x10,0x20,0x40,0x80,0xF8, //Z    
};

/* Standard ASCII 6x10 font */
 static const uint8_t Font6x10 []=
 {
   
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //sp   
   0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x01,0x00,0x00, //!
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //"
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //#
   0x04,0x04,0x0E,0x0E,0x0E,0x1F,0x1F,0x1F,0x00,0x00, //$
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //%
   0x1F,0x1F,0x1F,0x0E,0x0E,0x0E,0x04,0x04,0x00,0x00, //&
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //'
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //(
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //)
   0x1F,0x1F,0x00,0x1F,0x1F,0x00,0x1F,0x1F,0x00,0x00, //*
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //+
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //,
   0x00,0x00,0x00,0x00,0x0F,0x00,0x00,0x00,0x00,0x00, //-
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00, //.
   0x01,0x01,0x02,0x02,0x04,0x04,0x08,0x08,0x00,0x00, ///
   0x0E,0x11,0x11,0x15,0x15,0x11,0x11,0x0E,0x00,0x00, //0
   0x02,0x06,0x02,0x02,0x02,0x02,0x02,0x07,0x00,0x00, //1
   0x0E,0x11,0x01,0x02,0x04,0x08,0x10,0x1F,0x00,0x00, //2
   0x0E,0x11,0x01,0x0E,0x01,0x01,0x11,0x0E,0x00,0x00, //3
   0x12,0x12,0x12,0x1F,0x02,0x02,0x02,0x02,0x00,0x00, //4
   0x1F,0x10,0x10,0x1E,0x01,0x01,0x11,0x0E,0x00,0x00, //5
   0x0E,0x11,0x10,0x1E,0x11,0x11,0x11,0x0E,0x00,0x00, //6
   0x1F,0x01,0x01,0x02,0x04,0x08,0x10,0x10,0x00,0x00, //7
   0x0E,0x11,0x11,0x0E,0x11,0x11,0x11,0x0E,0x00,0x00, //8
   0x0E,0x11,0x11,0x0F,0x01,0x01,0x11,0x0E,0x00,0x00, //9
   0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00, //:
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //;
   0x01,0x03,0x07,0x0F,0x0F,0x07,0x03,0x01,0x00,0x00, //<
   0x00,0x00,0x00,0x00,0x1F,0x00,0x00,0x1F,0x00,0x00, //=
   0x08,0x0C,0x0E,0x0F,0x0F,0x0E,0x0C,0x08,0x00,0x00, //>
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //?
   0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x00, //@
   0x0E,0x11,0x11,0x1F,0x11,0x11,0x11,0x11,0x00,0x00, //A
   0x0E,0x11,0x11,0x1E,0x11,0x11,0x11,0x1E,0x00,0x00, //B
   0x0E,0x11,0x10,0x10,0x10,0x10,0x11,0x0E,0x00,0x00, //C
   0x1E,0x11,0x11,0x11,0x11,0x11,0x11,0x1E,0x00,0x00, //D
   0x1F,0x10,0x10,0x1E,0x10,0x10,0x10,0x1F,0x00,0x00, //E
   0x1F,0x10,0x10,0x1E,0x10,0x10,0x10,0x10,0x00,0x00, //F
   0x0E,0x11,0x10,0x17,0x11,0x11,0x11,0x0F,0x00,0x00, //G
   0x11,0x11,0x11,0x1F,0x11,0x11,0x11,0x11,0x00,0x00, //H
   0x07,0x02,0x02,0x02,0x02,0x02,0x02,0x07,0x00,0x00, //I
   0x1F,0x01,0x01,0x01,0x01,0x11,0x11,0x0E,0x00,0x00, //J
   0x11,0x12,0x14,0x18,0x18,0x14,0x12,0x11,0x00,0x00, //K
   0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x1F,0x00,0x00, //L
   0x11,0x1B,0x15,0x15,0x15,0x15,0x15,0x15,0x00,0x00, //M
   0x11,0x11,0x19,0x15,0x13,0x11,0x11,0x11,0x00,0x00, //N
   0x0E,0x11,0x11,0x11,0x11,0x11,0x11,0x0E,0x00,0x00, //O
   0x1E,0x11,0x11,0x1E,0x10,0x10,0x10,0x10,0x00,0x00, //P
   0x0E,0x11,0x11,0x11,0x11,0x11,0x12,0x0E,0x02,0x00, //Q
   0x1E,0x11,0x11,0x1E,0x14,0x12,0x11,0x11,0x00,0x00, //R
   0x0E,0x11,0x10,0x0E,0x01,0x01,0x11,0x0E,0x00,0x00, //S
   0x1F,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x00,0x00, //T
   0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x0E,0x00,0x00, //U
   0x11,0x11,0x11,0x11,0x0A,0x0A,0x04,0x04,0x00,0x00, //V
   0x11,0x11,0x15,0x15,0x15,0x15,0x0A,0x0A,0x00,0x00, //W
   0x11,0x11,0x0A,0x04,0x04,0x0A,0x11,0x11,0x00,0x00, //X
   0x11,0x11,0x0A,0x0A,0x04,0x04,0x04,0x04,0x00,0x00, //Y
   0x0F,0x01,0x02,0x02,0x04,0x04,0x08,0x0F,0x00,0x00, //Z
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //[
   0x0A,0x0A,0x0A,0x0A,0x1F,0x11,0x11,0x11,0x11,0x0E, // backslash
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //]
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //^
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //_
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //`
   0x00,0x00,0x00,0x0E,0x12,0x12,0x12,0x0F,0x00,0x00, //a
   0x00,0x08,0x08,0x0E,0x09,0x09,0x09,0x0E,0x00,0x00, //b
   0x00,0x00,0x00,0x06,0x09,0x08,0x09,0x06,0x00,0x00, //c
   0x00,0x01,0x01,0x07,0x09,0x09,0x09,0x07,0x00,0x00, //d
   0x00,0x00,0x00,0x06,0x09,0x0E,0x08,0x06,0x00,0x00, //e
   0x00,0x03,0x02,0x07,0x02,0x02,0x02,0x02,0x00,0x00, //f
   0x00,0x00,0x00,0x06,0x09,0x09,0x09,0x07,0x01,0x07, //g
   0x00,0x08,0x08,0x0B,0x0D,0x09,0x09,0x09,0x00,0x00, //h
   0x00,0x01,0x00,0x01,0x01,0x01,0x01,0x01,0x00,0x00, //i
   0x00,0x01,0x00,0x01,0x01,0x01,0x01,0x01,0x09,0x06, //j
   0x00,0x08,0x08,0x09,0x0A,0x0C,0x0A,0x09,0x00,0x00, //k
   0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00, //l
   0x00,0x00,0x00,0x11,0x1B,0x15,0x15,0x15,0x00,0x00, //m
   0x00,0x00,0x00,0x0B,0x0D,0x09,0x09,0x09,0x00,0x00, //n
   0x00,0x00,0x00,0x06,0x09,0x09,0x09,0x06,0x00,0x00, //o
   0x00,0x00,0x00,0x0E,0x09,0x09,0x09,0x0E,0x08,0x08, //p
   0x00,0x00,0x00,0x06,0x09,0x09,0x09,0x07,0x01,0x01, //q
   0x00,0x00,0x00,0x05,0x06,0x04,0x04,0x04,0x00,0x00, //r
   0x00,0x00,0x00,0x07,0x08,0x06,0x01,0x0E,0x00,0x00, //s
   0x00,0x02,0x02,0x07,0x02,0x02,0x02,0x03,0x00,0x00, //t
   0x00,0x00,0x00,0x09,0x09,0x09,0x09,0x06,0x00,0x00, //u
   0x00,0x00,0x00,0x11,0x11,0x0A,0x0A,0x04,0x00,0x00, //v
   0x00,0x00,0x00,0x11,0x11,0x15,0x15,0x0A,0x00,0x00, //w
   0x00,0x00,0x00,0x11,0x0A,0x04,0x0A,0x11,0x00,0x00, //x
   0x00,0x00,0x00,0x11,0x11,0x0A,0x04,0x04,0x08,0x10, //y
   0x00,0x00,0x00,0x07,0x01,0x02,0x04,0x07,0x00,0x00, //z   
 };

//! \Font7x10 definition
static const uint16_t Font7x10 [] = {
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, //sp
0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x0000,0x1000,0x0000,0x0000, //!
0x2800,0x2800,0x2800,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, //"
0x2400,0x2400,0x7C00,0x2400,0x4800,0x7C00,0x4800,0x4800,0x0000,0x0000, //#
0x3800,0x5400,0x5000,0x3800,0x1400,0x5400,0x5400,0x3800,0x1000,0x0000, //$
0x2000,0x5400,0x5800,0x3000,0x2800,0x5400,0x1400,0x0800,0x0000,0x0000, //%
0x1000,0x2800,0x2800,0x1000,0x3400,0x4800,0x4800,0x3400,0x0000,0x0000, //&
0x1000,0x1000,0x1000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, //'
0x0800,0x1000,0x2000,0x2000,0x2000,0x2000,0x2000,0x2000,0x1000,0x0800, //(
0x2000,0x1000,0x0800,0x0800,0x0800,0x0800,0x0800,0x0800,0x1000,0x2000, //)
0x1000,0x3800,0x1000,0x2800,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, //*
0x0000,0x0000,0x1000,0x1000,0x7C00,0x1000,0x1000,0x0000,0x0000,0x0000, //+
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x1000,0x1000,0x1000, //,
0x0000,0x0000,0x0000,0x0000,0x0000,0x3800,0x0000,0x0000,0x0000,0x0000, //-
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x1000,0x0000,0x0000, //.
0x0800,0x0800,0x1000,0x1000,0x1000,0x1000,0x2000,0x2000,0x0000,0x0000, ///
0x3800,0x4400,0x4400,0x5400,0x4400,0x4400,0x4400,0x3800,0x0000,0x0000, //0
0x1000,0x3000,0x5000,0x1000,0x1000,0x1000,0x1000,0x1000,0x0000,0x0000, //1
0x3800,0x4400,0x4400,0x0400,0x0800,0x1000,0x2000,0x7C00,0x0000,0x0000, //2
0x3800,0x4400,0x0400,0x1800,0x0400,0x0400,0x4400,0x3800,0x0000,0x0000, //3
0x0800,0x1800,0x2800,0x2800,0x4800,0x7C00,0x0800,0x0800,0x0000,0x0000, //4
0x7C00,0x4000,0x4000,0x7800,0x0400,0x0400,0x4400,0x3800,0x0000,0x0000, //5
0x3800,0x4400,0x4000,0x7800,0x4400,0x4400,0x4400,0x3800,0x0000,0x0000, //6
0x7C00,0x0400,0x0800,0x1000,0x1000,0x2000,0x2000,0x2000,0x0000,0x0000, //7
0x3800,0x4400,0x4400,0x3800,0x4400,0x4400,0x4400,0x3800,0x0000,0x0000, //8
0x3800,0x4400,0x4400,0x4400,0x3C00,0x0400,0x4400,0x3800,0x0000,0x0000, //9
0x0000,0x0000,0x1000,0x0000,0x0000,0x0000,0x0000,0x1000,0x0000,0x0000, //:
0x0000,0x0000,0x0000,0x1000,0x0000,0x0000,0x0000,0x1000,0x1000,0x1000, //;
0x0000,0x0000,0x0C00,0x3000,0x4000,0x3000,0x0C00,0x0000,0x0000,0x0000, //<
0x0000,0x0000,0x0000,0x7C00,0x0000,0x7C00,0x0000,0x0000,0x0000,0x0000, //=
0x0000,0x0000,0x6000,0x1800,0x0400,0x1800,0x6000,0x0000,0x0000,0x0000, //>
0x3800,0x4400,0x0400,0x0800,0x1000,0x1000,0x0000,0x1000,0x0000,0x0000, //?
0x3800,0x4400,0x4C00,0x5400,0x5C00,0x4000,0x4000,0x3800,0x0000,0x0000, //@
0x1000,0x2800,0x2800,0x2800,0x2800,0x7C00,0x4400,0x4400,0x0000,0x0000, //A
0x7800,0x4400,0x4400,0x7800,0x4400,0x4400,0x4400,0x7800,0x0000,0x0000, //B
0x3800,0x4400,0x4000,0x4000,0x4000,0x4000,0x4400,0x3800,0x0000,0x0000, //C
0x7000,0x4800,0x4400,0x4400,0x4400,0x4400,0x4800,0x7000,0x0000,0x0000, //D
0x7C00,0x4000,0x4000,0x7C00,0x4000,0x4000,0x4000,0x7C00,0x0000,0x0000, //E
0x7C00,0x4000,0x4000,0x7800,0x4000,0x4000,0x4000,0x4000,0x0000,0x0000, //F
0x3800,0x4400,0x4000,0x4000,0x5C00,0x4400,0x4400,0x3800,0x0000,0x0000, //G
0x4400,0x4400,0x4400,0x7C00,0x4400,0x4400,0x4400,0x4400,0x0000,0x0000, //H
0x3800,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x3800,0x0000,0x0000, //I
0x0400,0x0400,0x0400,0x0400,0x0400,0x0400,0x4400,0x3800,0x0000,0x0000, //J
0x4400,0x4800,0x5000,0x6000,0x5000,0x4800,0x4800,0x4400,0x0000,0x0000, //K
0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x7C00,0x0000,0x0000, //L
0x4400,0x6C00,0x6C00,0x5400,0x4400,0x4400,0x4400,0x4400,0x0000,0x0000, //M
0x4400,0x6400,0x6400,0x5400,0x5400,0x4C00,0x4C00,0x4400,0x0000,0x0000, //N
0x3800,0x4400,0x4400,0x4400,0x4400,0x4400,0x4400,0x3800,0x0000,0x0000, //O
0x7800,0x4400,0x4400,0x4400,0x7800,0x4000,0x4000,0x4000,0x0000,0x0000, //P
0x3800,0x4400,0x4400,0x4400,0x4400,0x4400,0x5400,0x3800,0x0400,0x0000, //Q
0x7800,0x4400,0x4400,0x4400,0x7800,0x4800,0x4800,0x4400,0x0000,0x0000, //R
0x3800,0x4400,0x4000,0x3000,0x0800,0x0400,0x4400,0x3800,0x0000,0x0000, //S
0x7C00,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x0000,0x0000, //T
0x4400,0x4400,0x4400,0x4400,0x4400,0x4400,0x4400,0x3800,0x0000,0x0000, //U
0x4400,0x4400,0x4400,0x2800,0x2800,0x2800,0x1000,0x1000,0x0000,0x0000, //V
0x4400,0x4400,0x5400,0x5400,0x5400,0x6C00,0x2800,0x2800,0x0000,0x0000, //W
0x4400,0x2800,0x2800,0x1000,0x1000,0x2800,0x2800,0x4400,0x0000,0x0000, //X
0x4400,0x4400,0x2800,0x2800,0x1000,0x1000,0x1000,0x1000,0x0000,0x0000, //Y
0x7C00,0x0400,0x0800,0x1000,0x1000,0x2000,0x4000,0x7C00,0x0000,0x0000, //Z
0x1800,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1800, //[
0x2000,0x2000,0x1000,0x1000,0x1000,0x1000,0x0800,0x0800,0x0000,0x0000, /*\*/
0x3000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x3000, //]
0x1000,0x2800,0x2800,0x4400,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, //^
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xFE00, //_
0x2000,0x1000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, //`
0x0000,0x0000,0x3800,0x4400,0x3C00,0x4400,0x4C00,0x3400,0x0000,0x0000, //a
0x4000,0x4000,0x5800,0x6400,0x4400,0x4400,0x6400,0x5800,0x0000,0x0000, //b
0x0000,0x0000,0x3800,0x4400,0x4000,0x4000,0x4400,0x3800,0x0000,0x0000, //c
0x0400,0x0400,0x3400,0x4C00,0x4400,0x4400,0x4C00,0x3400,0x0000,0x0000, //d
0x0000,0x0000,0x3800,0x4400,0x7C00,0x4000,0x4400,0x3800,0x0000,0x0000, //e
0x0C00,0x1000,0x7C00,0x1000,0x1000,0x1000,0x1000,0x1000,0x0000,0x0000, //f
0x0000,0x0000,0x3400,0x4C00,0x4400,0x4400,0x4C00,0x3400,0x0400,0x7800, //g
0x4000,0x4000,0x5800,0x6400,0x4400,0x4400,0x4400,0x4400,0x0000,0x0000, //h
0x1000,0x0000,0x7000,0x1000,0x1000,0x1000,0x1000,0x1000,0x0000,0x0000, //i
0x1000,0x0000,0x7000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0xE000, //j
0x4000,0x4000,0x4800,0x5000,0x6000,0x5000,0x4800,0x4400,0x0000,0x0000, //k
0x7000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x0000,0x0000, //l
0x0000,0x0000,0x7800,0x5400,0x5400,0x5400,0x5400,0x5400,0x0000,0x0000, //m
0x0000,0x0000,0x5800,0x6400,0x4400,0x4400,0x4400,0x4400,0x0000,0x0000, //n
0x0000,0x0000,0x3800,0x4400,0x4400,0x4400,0x4400,0x3800,0x0000,0x0000, //o
0x0000,0x0000,0x5800,0x6400,0x4400,0x4400,0x6400,0x5800,0x4000,0x4000, //p
0x0000,0x0000,0x3400,0x4C00,0x4400,0x4400,0x4C00,0x3400,0x0400,0x0400, //q
0x0000,0x0000,0x5800,0x6400,0x4000,0x4000,0x4000,0x4000,0x0000,0x0000, //r
0x0000,0x0000,0x3800,0x4400,0x3000,0x0800,0x4400,0x3800,0x0000,0x0000, //s
0x2000,0x2000,0x7800,0x2000,0x2000,0x2000,0x2000,0x1800,0x0000,0x0000, //t
0x0000,0x0000,0x4400,0x4400,0x4400,0x4400,0x4C00,0x3400,0x0000,0x0000, //u
0x0000,0x0000,0x4400,0x4400,0x2800,0x2800,0x2800,0x1000,0x0000,0x0000, //v
0x0000,0x0000,0x5400,0x5400,0x5400,0x6C00,0x2800,0x2800,0x0000,0x0000, //w
0x0000,0x0000,0x4400,0x2800,0x1000,0x1000,0x2800,0x4400,0x0000,0x0000, //x
0x0000,0x0000,0x4400,0x4400,0x2800,0x2800,0x1000,0x1000,0x1000,0x6000, //y
0x0000,0x0000,0x7C00,0x0800,0x1000,0x2000,0x4000,0x7C00,0x0000,0x0000, //z
0x1800,0x1000,0x1000,0x1000,0x2000,0x2000,0x1000,0x1000,0x1000,0x1800, //{
0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000, //|
0x3000,0x1000,0x1000,0x1000,0x0800,0x0800,0x1000,0x1000,0x1000,0x3000, //}
0x0000,0x0000,0x0000,0x7400,0x4C00,0x0000,0x0000,0x0000,0x0000,0x0000, //~
};
//! \Font11x18 definition
static const uint16_t Font11x18 [] = {
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, //sp
0x0000,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0000,0x0C00,0x0C00,0x0000,0x0000,0x0000, //!
0x0000,0x1B00,0x1B00,0x1B00,0x1B00,0x1B00,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, //"
0x0000,0x1980,0x1980,0x1980,0x1980,0x7FC0,0x7FC0,0x1980,0x3300,0x7FC0,0x7FC0,0x3300,0x3300,0x3300,0x3300,0x0000,0x0000,0x0000, //#
0x0000,0x1E00,0x3F00,0x7580,0x6580,0x7400,0x3C00,0x1E00,0x0700,0x0580,0x6580,0x6580,0x7580,0x3F00,0x1E00,0x0400,0x0400,0x0000, //$
0x0000,0x7000,0xD800,0xD840,0xD8C0,0xD980,0x7300,0x0600,0x0C00,0x1B80,0x36C0,0x66C0,0x46C0,0x06C0,0x0380,0x0000,0x0000,0x0000, //%
0x0000,0x1E00,0x3F00,0x3300,0x3300,0x3300,0x1E00,0x0C00,0x3CC0,0x66C0,0x6380,0x6180,0x6380,0x3EC0,0x1C80,0x0000,0x0000,0x0000, //&
0x0000,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, //'
0x0080,0x0100,0x0300,0x0600,0x0600,0x0400,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0400,0x0600,0x0600,0x0300,0x0100,0x0080, //(
0x2000,0x1000,0x1800,0x0C00,0x0C00,0x0400,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0400,0x0C00,0x0C00,0x1800,0x1000,0x2000, //)
0x0000,0x0C00,0x2D00,0x3F00,0x1E00,0x3300,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, //*
0x0000,0x0000,0x0000,0x0C00,0x0C00,0x0C00,0x0C00,0xFFC0,0xFFC0,0x0C00,0x0C00,0x0C00,0x0C00,0x0000,0x0000,0x0000,0x0000,0x0000, //+
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0C00,0x0C00,0x0400,0x0400,0x0800, //,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x1E00,0x1E00,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, //-
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0C00,0x0C00,0x0000,0x0000,0x0000, //.
0x0000,0x0300,0x0300,0x0300,0x0600,0x0600,0x0600,0x0600,0x0C00,0x0C00,0x0C00,0x0C00,0x1800,0x1800,0x1800,0x0000,0x0000,0x0000, ///
0x0000,0x1E00,0x3F00,0x3300,0x6180,0x6180,0x6180,0x6D80,0x6D80,0x6180,0x6180,0x6180,0x3300,0x3F00,0x1E00,0x0000,0x0000,0x0000, //0
0x0000,0x0600,0x0E00,0x1E00,0x3600,0x2600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0000,0x0000,0x0000, //1
0x0000,0x1E00,0x3F00,0x7380,0x6180,0x6180,0x0180,0x0300,0x0600,0x0C00,0x1800,0x3000,0x6000,0x7F80,0x7F80,0x0000,0x0000,0x0000, //2
0x0000,0x1C00,0x3E00,0x6300,0x6300,0x0300,0x0E00,0x0E00,0x0300,0x0180,0x0180,0x6180,0x7380,0x3F00,0x1E00,0x0000,0x0000,0x0000, //3
0x0000,0x0600,0x0E00,0x0E00,0x1E00,0x1E00,0x1600,0x3600,0x3600,0x6600,0x7F80,0x7F80,0x0600,0x0600,0x0600,0x0000,0x0000,0x0000, //4
0x0000,0x7F00,0x7F00,0x6000,0x6000,0x6000,0x6E00,0x7F00,0x6380,0x0180,0x0180,0x6180,0x7380,0x3F00,0x1E00,0x0000,0x0000,0x0000, //5
0x0000,0x1E00,0x3F00,0x3380,0x6180,0x6000,0x6E00,0x7F00,0x7380,0x6180,0x6180,0x6180,0x3380,0x3F00,0x1E00,0x0000,0x0000,0x0000, //6
0x0000,0x7F80,0x7F80,0x0180,0x0300,0x0300,0x0600,0x0600,0x0C00,0x0C00,0x0C00,0x0800,0x1800,0x1800,0x1800,0x0000,0x0000,0x0000, //7
0x0000,0x1E00,0x3F00,0x6380,0x6180,0x6180,0x2100,0x1E00,0x3F00,0x6180,0x6180,0x6180,0x6180,0x3F00,0x1E00,0x0000,0x0000,0x0000, //8
0x0000,0x1E00,0x3F00,0x7300,0x6180,0x6180,0x6180,0x7380,0x3F80,0x1D80,0x0180,0x6180,0x7300,0x3F00,0x1E00,0x0000,0x0000,0x0000, //9
0x0000,0x0000,0x0000,0x0000,0x0000,0x0C00,0x0C00,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0C00,0x0C00,0x0000,0x0000,0x0000, //:
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0C00,0x0C00,0x0000,0x0000,0x0000,0x0000,0x0000,0x0C00,0x0C00,0x0400,0x0400,0x0800, //;
0x0000,0x0000,0x0000,0x0000,0x0080,0x0380,0x0E00,0x3800,0x6000,0x3800,0x0E00,0x0380,0x0080,0x0000,0x0000,0x0000,0x0000,0x0000, //<
0x0000,0x0000,0x0000,0x0000,0x0000,0x7F80,0x7F80,0x0000,0x0000,0x7F80,0x7F80,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, //=
0x0000,0x0000,0x0000,0x0000,0x4000,0x7000,0x1C00,0x0700,0x0180,0x0700,0x1C00,0x7000,0x4000,0x0000,0x0000,0x0000,0x0000,0x0000, //>
0x0000,0x1F00,0x3F80,0x71C0,0x60C0,0x00C0,0x01C0,0x0380,0x0700,0x0E00,0x0C00,0x0C00,0x0000,0x0C00,0x0C00,0x0000,0x0000,0x0000, //?
0x0000,0x1E00,0x3F00,0x3180,0x7180,0x6380,0x6F80,0x6D80,0x6D80,0x6F80,0x6780,0x6000,0x3200,0x3E00,0x1C00,0x0000,0x0000,0x0000, //@
0x0000,0x3F00,0x7F80,0x6180,0x6180,0x6180,0x6180,0x6180,0x7F80,0x7F80,0x6180,0x6180,0x6180,0x6180,0x6180,0x0000,0x0000,0x0000, //A
0x0000,0x7C00,0x7E00,0x6300,0x6300,0x6300,0x6300,0x7E00,0x7E00,0x6300,0x6180,0x6180,0x6380,0x7F00,0x7E00,0x0000,0x0000,0x0000, //B
0x0000,0x3F00,0x7F80,0x6180,0x6180,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6180,0x6180,0x7F80,0x3F00,0x0000,0x0000,0x0000, //C
0x0000,0x7C00,0x7F00,0x6300,0x6380,0x6180,0x6180,0x6180,0x6180,0x6180,0x6180,0x6300,0x6300,0x7E00,0x7C00,0x0000,0x0000,0x0000, //D
0x0000,0x7F80,0x7F80,0x6000,0x6000,0x6000,0x6000,0x7F00,0x7F00,0x6000,0x6000,0x6000,0x6000,0x7F80,0x7F80,0x0000,0x0000,0x0000, //E
0x0000,0x7F80,0x7F80,0x6000,0x6000,0x6000,0x6000,0x7F00,0x7F00,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x0000,0x0000,0x0000, //F
0x0000,0x1E00,0x3F00,0x3180,0x6180,0x6000,0x6000,0x6000,0x6380,0x6380,0x6180,0x6180,0x3180,0x3F80,0x1E00,0x0000,0x0000,0x0000, //G
0x0000,0x6180,0x6180,0x6180,0x6180,0x6180,0x6180,0x7F80,0x7F80,0x6180,0x6180,0x6180,0x6180,0x6180,0x6180,0x0000,0x0000,0x0000, //H
0x0000,0x3F00,0x3F00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x3F00,0x3F00,0x0000,0x0000,0x0000, //I
0x0000,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x6180,0x6180,0x7380,0x3F00,0x1E00,0x0000,0x0000,0x0000, //J
0x0000,0x60C0,0x6180,0x6300,0x6600,0x6600,0x6C00,0x7800,0x7C00,0x6600,0x6600,0x6300,0x6180,0x6180,0x60C0,0x0000,0x0000,0x0000, //K
0x0000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x7F80,0x7F80,0x0000,0x0000,0x0000, //L
0x0000,0x6180,0x6180,0x7380,0x7380,0x7F80,0x7F80,0x6D80,0x6D80,0x6180,0x6180,0x6180,0x6180,0x6180,0x6180,0x0000,0x0000,0x0000, //M
0x0000,0x7180,0x7180,0x7980,0x7980,0x7980,0x6D80,0x6D80,0x6D80,0x6580,0x6780,0x6780,0x6780,0x6380,0x6380,0x0000,0x0000,0x0000, //N
0x0000,0x3F00,0x7F80,0x6180,0x6180,0x6180,0x6180,0x6180,0x6180,0x6180,0x6180,0x6180,0x6180,0x7F80,0x3F00,0x0000,0x0000,0x0000, //O
0x0000,0x7F00,0x7F80,0x6180,0x6180,0x6180,0x6180,0x6180,0x7F80,0x7F00,0x6000,0x6000,0x6000,0x6000,0x6000,0x0000,0x0000,0x0000, //P
0x0000,0x1E00,0x3F00,0x3300,0x6180,0x6180,0x6180,0x6180,0x6180,0x6180,0x6580,0x6780,0x3300,0x3F80,0x1E40,0x0000,0x0000,0x0000, //Q
0x0000,0x7F00,0x7F80,0x6180,0x6180,0x6180,0x6180,0x6180,0x7F80,0x7F00,0x6300,0x6380,0x6180,0x6180,0x6180,0x0000,0x0000,0x0000, //R
0x0000,0x3F00,0x7F80,0x6180,0x6180,0x6000,0x6000,0x7F00,0x3F80,0x0180,0x0180,0x6180,0x6180,0x7F80,0x3F00,0x0000,0x0000,0x0000, //S
0x0000,0x7F80,0x7F80,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0000,0x0000,0x0000, //T
0x0000,0x6180,0x6180,0x6180,0x6180,0x6180,0x6180,0x6180,0x6180,0x6180,0x6180,0x6180,0x7380,0x3F00,0x1E00,0x0000,0x0000,0x0000, //U
0x0000,0x60C0,0x60C0,0x60C0,0x3180,0x3180,0x3180,0x1B00,0x1B00,0x1B00,0x1B00,0x0E00,0x0E00,0x0E00,0x0400,0x0000,0x0000,0x0000, //V
0x0000,0xC0C0,0xC0C0,0xC0C0,0xC0C0,0xC0C0,0xCCC0,0x4C80,0x4C80,0x5E80,0x5280,0x5280,0x7380,0x6180,0x6180,0x0000,0x0000,0x0000, //W
0x0000,0xC0C0,0x6080,0x6180,0x3300,0x3B00,0x1E00,0x0C00,0x0C00,0x1E00,0x1F00,0x3B00,0x7180,0x6180,0xC0C0,0x0000,0x0000,0x0000, //X
0x0000,0xC0C0,0x6180,0x6180,0x3300,0x3300,0x1E00,0x1E00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0000,0x0000,0x0000, //Y
0x0000,0x3F80,0x3F80,0x0180,0x0300,0x0300,0x0600,0x0C00,0x0C00,0x1800,0x1800,0x3000,0x6000,0x7F80,0x7F80,0x0000,0x0000,0x0000, //Z
0x0F00,0x0F00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0F00,0x0F00, //[
0x0000,0x1800,0x1800,0x1800,0x0C00,0x0C00,0x0C00,0x0C00,0x0600,0x0600,0x0600,0x0600,0x0300,0x0300,0x0300,0x0000,0x0000,0x0000, /*\*/
0x1E00,0x1E00,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x1E00,0x1E00, //]
0x0000,0x0C00,0x0C00,0x1E00,0x1200,0x3300,0x3300,0x6180,0x6180,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, //^
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xFFE0,0x0000, //_
0x0000,0x3800,0x1800,0x0C00,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, //`
0x0000,0x0000,0x0000,0x0000,0x0000,0x1F00,0x3F80,0x6180,0x0180,0x1F80,0x3F80,0x6180,0x6380,0x7F80,0x38C0,0x0000,0x0000,0x0000, //a
0x0000,0x6000,0x6000,0x6000,0x6000,0x6E00,0x7F00,0x7380,0x6180,0x6180,0x6180,0x6180,0x7380,0x7F00,0x6E00,0x0000,0x0000,0x0000, //b
0x0000,0x0000,0x0000,0x0000,0x0000,0x1E00,0x3F00,0x7380,0x6180,0x6000,0x6000,0x6180,0x7380,0x3F00,0x1E00,0x0000,0x0000,0x0000, //c
0x0000,0x0180,0x0180,0x0180,0x0180,0x1D80,0x3F80,0x7380,0x6180,0x6180,0x6180,0x6180,0x7380,0x3F80,0x1D80,0x0000,0x0000,0x0000, //d
0x0000,0x0000,0x0000,0x0000,0x0000,0x1E00,0x3F00,0x7300,0x6180,0x7F80,0x7F80,0x6000,0x7180,0x3F00,0x1E00,0x0000,0x0000,0x0000, //e
0x0000,0x07C0,0x0FC0,0x0C00,0x0C00,0x7F80,0x7F80,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0000,0x0000,0x0000, //f
0x0000,0x0000,0x0000,0x0000,0x1D80,0x3F80,0x7380,0x6180,0x6180,0x6180,0x6180,0x7380,0x3F80,0x1D80,0x0180,0x6380,0x7F00,0x3E00, //g
0x0000,0x6000,0x6000,0x6000,0x6000,0x6F00,0x7F80,0x7180,0x6180,0x6180,0x6180,0x6180,0x6180,0x6180,0x6180,0x0000,0x0000,0x0000, //h
0x0000,0x0600,0x0600,0x0000,0x0000,0x3E00,0x3E00,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0000,0x0000,0x0000, //i
0x0600,0x0600,0x0000,0x0000,0x3E00,0x3E00,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x4600,0x7E00,0x3C00, //j
0x0000,0x6000,0x6000,0x6000,0x6000,0x6180,0x6300,0x6600,0x6C00,0x7C00,0x7600,0x6300,0x6300,0x6180,0x60C0,0x0000,0x0000,0x0000, //k
0x0000,0x3E00,0x3E00,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0000,0x0000,0x0000, //l
0x0000,0x0000,0x0000,0x0000,0x0000,0xDD80,0xFFC0,0xCEC0,0xCCC0,0xCCC0,0xCCC0,0xCCC0,0xCCC0,0xCCC0,0xCCC0,0x0000,0x0000,0x0000, //m
0x0000,0x0000,0x0000,0x0000,0x0000,0x6F00,0x7F80,0x7180,0x6180,0x6180,0x6180,0x6180,0x6180,0x6180,0x6180,0x0000,0x0000,0x0000, //n
0x0000,0x0000,0x0000,0x0000,0x0000,0x1E00,0x3F00,0x7380,0x6180,0x6180,0x6180,0x6180,0x7380,0x3F00,0x1E00,0x0000,0x0000,0x0000, //o
0x0000,0x0000,0x0000,0x0000,0x6E00,0x7F00,0x7380,0x6180,0x6180,0x6180,0x6180,0x7380,0x7F00,0x6E00,0x6000,0x6000,0x6000,0x6000, //p
0x0000,0x0000,0x0000,0x0000,0x1D80,0x3F80,0x7380,0x6180,0x6180,0x6180,0x6180,0x7380,0x3F80,0x1D80,0x0180,0x0180,0x0180,0x0180, //q
0x0000,0x0000,0x0000,0x0000,0x0000,0x6700,0x3F80,0x3900,0x3000,0x3000,0x3000,0x3000,0x3000,0x3000,0x3000,0x0000,0x0000,0x0000, //r
0x0000,0x0000,0x0000,0x0000,0x0000,0x1E00,0x3F80,0x6180,0x6000,0x7F00,0x3F80,0x0180,0x6180,0x7F00,0x1E00,0x0000,0x0000,0x0000, //s
0x0000,0x0000,0x0800,0x1800,0x1800,0x7F00,0x7F00,0x1800,0x1800,0x1800,0x1800,0x1800,0x1800,0x1F80,0x0F80,0x0000,0x0000,0x0000, //t
0x0000,0x0000,0x0000,0x0000,0x0000,0x6180,0x6180,0x6180,0x6180,0x6180,0x6180,0x6180,0x6380,0x7F80,0x3D80,0x0000,0x0000,0x0000, //u
0x0000,0x0000,0x0000,0x0000,0x0000,0x60C0,0x3180,0x3180,0x3180,0x1B00,0x1B00,0x1B00,0x0E00,0x0E00,0x0600,0x0000,0x0000,0x0000, //v
0x0000,0x0000,0x0000,0x0000,0x0000,0xDD80,0xDD80,0xDD80,0x5500,0x5500,0x5500,0x7700,0x7700,0x2200,0x2200,0x0000,0x0000,0x0000, //w
0x0000,0x0000,0x0000,0x0000,0x0000,0x6180,0x3300,0x3300,0x1E00,0x0C00,0x0C00,0x1E00,0x3300,0x3300,0x6180,0x0000,0x0000,0x0000, //x
0x0000,0x0000,0x0000,0x0000,0x6180,0x6180,0x3180,0x3300,0x3300,0x1B00,0x1B00,0x1B00,0x0E00,0x0E00,0x0E00,0x1C00,0x7C00,0x7000, //y
0x0000,0x0000,0x0000,0x0000,0x0000,0x7FC0,0x7FC0,0x0180,0x0300,0x0600,0x0C00,0x1800,0x3000,0x7FC0,0x7FC0,0x0000,0x0000,0x0000, //z
0x0380,0x0780,0x0600,0x0600,0x0600,0x0600,0x0600,0x0E00,0x1C00,0x1C00,0x0E00,0x0600,0x0600,0x0600,0x0600,0x0600,0x0780,0x0380, //{
0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600, //|
0x3800,0x3C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x0E00,0x0700,0x0700,0x0E00,0x0C00,0x0C00,0x0C00,0x0C00,0x0C00,0x3C00,0x3800, //}
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x3880,0x7F80,0x4700,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, //~
};
//! \Battery image definition
/*uint16_t Battery [] = 
{
    0x3FF8,0x4008,0x8008,0x8008,0x8008,0x4008,0x3FF8, //Empty
    0x3FF8,0x4008,0x8068,0x8068,0x8068,0x4008,0x3FF8, //33 %
    0x3FF8,0x4008,0x8368,0x8368,0x8368,0x4008,0x3FF8, //66 %
    0x3FF8,0x4008,0x9B68,0x9B68,0x9B68,0x4008,0x3FF8, //100%
    0x3FF8,0x4008,0x8488,0x8308,0x8488,0x4008,0x3FF8, //No battery
};
*/
//! \Battery image definition
uint16_t Battery [] = 
{
    0x7FFF  ,   0xF800  ,
    0x4000  ,   0x0800  ,
    0x4000  ,   0x0800  ,
    0xC000  ,   0x0800  ,
    0xC000  ,   0x0800  ,
    0xC000  ,   0x0800  ,
    0xC000  ,   0x0800  ,
    0x4000  ,   0x0800  ,
    0x4000  ,   0x0800  ,
    0x7FFF  ,   0xF800  ,
    0x7FFF  ,   0xF800  ,
    0x4000  ,   0x0800  ,
    0x4000  ,   0xC800  ,
    0xC000  ,   0xC800  ,
    0xC000  ,   0xC800  ,
    0xC000  ,   0xC800  ,
    0xC000  ,   0xC800  ,
    0x4000  ,   0xC800  ,
    0x4000  ,   0x0800  ,
    0x7FFF  ,   0xF800  ,
    0x7FFF  ,   0xF800  ,
    0x4000  ,   0x0800  ,
    0x400C  ,   0xC800  ,
    0xC00C  ,   0xC800  ,
    0xC00C  ,   0xC800  ,
    0xC00C  ,   0xC800  ,
    0xC00C  ,   0xC800  ,
    0x400C  ,   0xC800  ,
    0x4000  ,   0x0800  ,
    0x7FFF  ,   0xF800  ,
    0x7FFF  ,   0xF800  ,
    0x4000  ,   0x0800  ,
    0x40CC  ,   0xC800  ,
    0xC0CC  ,   0xC800  ,
    0xC0CC  ,   0xC800  ,
    0xC0CC  ,   0xC800  ,
    0xC0CC  ,   0xC800  ,
    0x40CC  ,   0xC800  ,
    0x4000  ,   0x0800  ,
    0x7FFF  ,   0xF800  ,
    0x7FFF  ,   0xF800  ,
    0x4000  ,   0x0800  ,
    0x4CCC  ,   0xC800  ,
    0xCCCC  ,   0xC800  ,
    0xCCCC  ,   0xC800  ,
    0xCCCC  ,   0xC800  ,
    0xCCCC  ,   0xC800  ,
    0x4CCC  ,   0xC800  ,
    0x4000  ,   0x0800  ,
    0x7FFF  ,   0xF800  ,
    0x7FFF  ,   0xF800  ,
    0x4000  ,   0x0800  ,
    0x4303  ,   0x0800  ,
    0xC0CC  ,   0x0800  ,
    0xC030  ,   0x0800  ,
    0xC030  ,   0x0800  ,
    0xC0CC  ,   0x0800  ,
    0x4303  ,   0x0800  ,
    0x4000  ,   0x0800  ,
    0x7FFF  ,   0xF800  ,
};

//! \Reed machines logo definition
uint16_t Logo37x43[] =
{
0x003F,0xFFE0,0x0000,0x0000,
0x003F,0xFFE0,0x0000,0x0000,
0x003F,0xFFE0,0x0000,0x0000,
0x003F,0x83E0,0x0000,0x0000,
0x003F,0xBBE0,0x0000,0x0000,
0x003F,0xBBE0,0x0000,0x0000,
0xFFBF,0xBBE0,0x0000,0x0000,
0xAABF,0xBBE0,0x0000,0x0000,
0xDFFF,0xBBE0,0x0000,0x0000,
0xA803,0xBBE0,0x0000,0x0000,
0xDBFB,0x93E0,0x0000,0x0000,
0xABFB,0xD7E0,0x0000,0x0000,
0xDBFB,0xD400,0x0000,0x0000,
0xABFB,0xD7FF,0xF800,0x0000,
0xDBFB,0xD400,0x0800,0x0000,
0xABFB,0xD600,0x0800,0x0000,
0xD9F3,0x927E,0x0800,0x0000,
0xADF7,0xBA7E,0x0800,0x0000,
0xD5F7,0xBA7E,0x0800,0x0000,
0xADF7,0xBA7E,0x0800,0x0000,
0xD5F7,0xBA7E,0x0800,0x0000,
0xACE7,0xBA7E,0x0800,0x0000,
0xD6EF,0xBA3C,0x0800,0x0000,
0xAAEF,0xBA3C,0x0800,0x0000,
0xD6EF,0xBA3C,0x0800,0x0000,
0xAAEF,0xBA3C,0x0800,0x0000,
0xD6EF,0xBA3C,0x0800,0x0000,
0xACE7,0xBA3C,0x0800,0x0000,
0xD5F7,0xBA3C,0x0800,0x0000,
0xADF7,0x8218,0x0800,0x0000,
0xD5F7,0xDE18,0x0800,0x0000,
0xACE4,0x1018,0x0800,0x0000,
0xD6EF,0xD018,0x0800,0x0000,
0xAA0A,0xD018,0x0800,0x0000,
0xD7FD,0x5018,0x0800,0x0000,
0xAAAA,0xD018,0x0800,0x0000,
0xD555,0x5018,0x0800,0x0000,
0xAAAA,0xD018,0x0800,0x0000,
0xFFFF,0xD018,0x0800,0x0000,
0x0000,0x1000,0x0800,0x0000,
0x0000,0x1000,0x0800,0x0000,
0x0000,0x1000,0x0800,0x0000,
0x0000,0x1FFF,0xF800,0x0000,
};
//! \Font structures definition definition
FontDefEightBit Font_6x7 = {6,7,Font6x7};
FontDefEightBit Font_6x10 = {6,10,Font6x10};
FontDef Font_7x10 = {7,10,Font7x10};
FontDef Font_11x18 = {9,18,Font11x18};
//-----------------------------------------------------------------------------


