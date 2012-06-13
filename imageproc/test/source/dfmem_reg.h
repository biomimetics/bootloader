/********************************************
* Name: dfmem_reg.h
* Desc: Header for Atmel DataFlash Registers
* Date: 2008-8-25
* Author: fgb
********************************************/

// DataFlash Status Register Definition
volatile unsigned int  DFSTATUS;
__extension__ typedef struct tagDFSTATUSBITS {
  union {
    struct {
      unsigned PAGESIZE:1;
      unsigned PROTECT:1;
      unsigned DENSITY:4;
      unsigned COMP:1;
      unsigned READY:1;
    };
    struct {
      unsigned :2;
      unsigned DENSITY0:1;
      unsigned DENSITY1:1;
      unsigned DENSITY2:1;
      unsigned DENSITY3:1;      
      unsigned :2;
    };
  };
} DFSTATUSBITS;
volatile DFSTATUSBITS DFSTATUSbits;

/* DFSTATUS */
#define _PAGESIZE DFSTATUSbits.PAGESIZE
#define _PROTECT DFSTATUSbits.PROTECT
#define _DENSITY DFSTATUSbits.DENSITY
#define _DENSITY0 DFSTATUSbits.DENSITY0
#define _DENSITY1 DFSTATUSbits.DENSITY1
#define _DENSITY2 DFSTATUSbits.DENSITY2
#define _DENSITY3 DFSTATUSbits.DENSITY3
#define _COMP DFSTATUSbits.COMP
#define _READY DFSTATUSbits.READY
