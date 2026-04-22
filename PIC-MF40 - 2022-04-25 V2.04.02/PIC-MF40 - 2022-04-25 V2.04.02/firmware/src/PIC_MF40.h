// *****************************************************************************
// PIC_MF40.h
//
//  Rüffler Computer Hard+Soft
//  Hans Rüffler
//  (c) Copyright 2017 by Hans Rüffler
//
//  Header file for PIC_MF40.c
//  04.10.2017 HR
//
// *****************************************************************************

#ifndef _PIC_MF40_H                // Guard against multiple inclusion
#define _PIC_MF40_H

#include "PIC_VAR.h"


//#include "PIC32.h"
//#include "system_definitions.h"

// *****************************************************************************
// #defines
// *****************************************************************************
// 
#define Ana_P           0
#define Ana_I           1
#define Ana_cosPHI      2
#define Ana_Freq        3
#define Ana_Pyro_1      4
#define Ana_P_Reg_Ext   5

#define LEDholdTime     (100/16)                                // [16ms]
#define LEDholdTimeReg  (LED_300ms?(300/16):(100/16))           // [16ms]
#define LEDminTime      3                                       // [16ms]
//#define LEDholdTime     (MF40_Setup.LED_300ms?300:100)        // [ms]
//#define LEDminTime      20                                    // [ms]

#define UD_SAVE_ON_START        // MF40_Userdata
//#define UD_SAVE_ON_CHANGE     // MF40_Userdata

// *****************************************************************************
// hier enthaltene globale Funktionen
// *****************************************************************************
float   ANAval(int kan);
uint32  ADCvalMean(int kan);
uint32  ADCval32(int kan);
int     BCD2i(char bcd);
int     CheckSI(int year, int month);
char    getParityChar(int parity);
char *  getParityStr(int parity);
int     getRotarySwitch(void);
int     getRotaryUpDown(void);
uint16  getSCADA_Status0(void);
void    fMIMA(float *fp, float UW, float OW);
int     IncDecStepwise(int val, int step, int incdec, int uw, int ow);
void    IOExtenderOP(void);
void    IOExtenderSetup(void);
int     LED_val(int x);
uint    LED_status16(void);
void    MF40_MilliSekunden(void);
void    MF40_Init(void);
void    MF40_Eval(void);
int     MF40_Setup_Save(int onchange);
void    MF40_Setup_Read(void);
void    MF40_Setup_Restore(void);
int     MF40_Setup_Default(struct MF40_Setup_Struct * SU, int force);
int     MF40_Setup_MF_HF(struct MF40_Setup_Struct * SU);
int     MF40_Userdata_Save(int onchange);
void    MF40_Userdata_Read(void);
void    MF40_Userdata_Restore(void);
int     MF40_Userdata_Default(struct MF40_Userdata_Struct * UD, int force);
int     MF40_Lifedata_Save(void);
void    MF40_Lifedata_Read(void);
int     MF40_Lifedata_Default(struct MF40_Lifedata_Struct * SU, int force);
int     MF40_Interpreter(char * bp, int mode);
int     PID_Iactive(void);

void    RampShift(int up);

int     PSETmax10(void);
float   fPSETmax(void);
int     TEMPSETmax(void);


// *****************************************************************************
// #defines
// *****************************************************************************
#define MF40_TIMER_MAX      MF40_Userdata.TIMER_max     // 18 Stunden max.
#define MF40_TIMER_TS       ((MF40_TIMER_MAX<=600)?1:0) // 1/10s verwenden?
#define MF40_TIMER_TSlim    10                          // bis
#define MF40_PIDset_MAX     4                           // 0(B),1..4
#define MF40_IntRun         1
#define MF40_IntRunRamp     2
#define MF40_ExtRun         3
#define GENOFFopt           (MF40_Setup.GENOFF_Option)

// *****************************************************************************
// Variablen
// *****************************************************************************
extern char   MG_name[8][10];
extern char   MG_unit[8][10];
extern char   MG_val[8][12];
extern float  MG_fac[8];
extern char   MG_fix[8];
extern char   TabAna2MG[6];
extern char   TabAnaName[6][10];
extern char   LED_nameON[16][10];
extern char   LED_nameOFF[16][10];
extern char   LEDpos[16];


#endif /* _PIC_MF40_H */
// + + + End of File + + +
