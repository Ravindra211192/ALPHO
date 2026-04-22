// *****************************************************************************
// PIC_Func.h
//
//  Rüffler Computer Hard+Soft
//  Hans Rüffler
//  (c) Copyright 2016-2018 by Hans Rüffler
//
//  Global functions header file
//
// *****************************************************************************
#ifndef _PIC_FUNC_H                // Guard against multiple inclusion
#define _PIC_FUNC_H
// -----------------------------------------------------------------------------

void    CRCinit(uint16 *crcval);
void    CRC(uint16 *crcval, char c);
uint16  CRC16str(char *strp, uint16 usDataLen);

int     swapis(char * dp, int16 i);
int     swapls(char * dp, int32 ll);
int     swap2s(char * dp, char * sp);
int     swap4s(char * dp, char * sp);
int     swap8s(char * dp, char * sp);
int     cswap2s(char * dp, char * sp, int little);
int     cswap4s(char * dp, char * sp, int little);


// *****************************************************************************
#endif /* _PIC_FUNC_H */
// + + + End of File + + +
