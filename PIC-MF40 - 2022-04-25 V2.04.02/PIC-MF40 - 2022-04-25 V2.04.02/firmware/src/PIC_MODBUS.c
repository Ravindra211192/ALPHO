/* ************************************************************************** */
/** PIC_MODBUS.c

  @Company
    Rüffler Computer Hard+Soft
    Hans Rüffler

  @File Name
    PIC_MODBUS.c

  @Summary
    MODBUS Routines
    01.09.2021 HR

  @Description
    MODBUS relevant routines are located in this file
 */
/* ************************************************************************** */

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <stdio.h>

#include "PIC32.h"
#include "PIC_VAR.h"
#include "PIC_Func.h"

#include "system_config.h"
#include "system_definitions.h"

// *****************************************************************************
// hier enthaltene Funktionen
// *****************************************************************************

void    MBSL_Eval(void);
void    MBSL_DoEval(char * sp, int len);
int     MBSL_DoCmdEval(char * sp, int len);
void    MBSL_RX(char c);
int     MBSL_Map_Read(char * dp, uchar code, uint16 datadr, uint16 reqlen);
int     MBSL_Map_Write(char * dp, uchar code, uint16 datadr, uint16 reqlen, uint16 bytelen);
int     MBSL_CoilMap_Read(char * dp, uchar code, uint16 datadr, uint16 reqlen);
int     MBSL_CoilMap_Write(char * dp, uchar code, uint16 datadr, uint16 reqlen, uint16 bytelen);
int     MBSL_MessageTimeout(void);

void    MBMA_Eval(void);
void    MBMA_RX(char c);
int     MBMA_TXs(char * dp, char * sp, int len);

// *****************************************************************************
// externe Funktionen
// *****************************************************************************
void    COMtx_Kick(char * cp, int len);
void    COM2tx_Kick(char * cp, int len);

int     EEPreadn(char * dp, uint16 adr, int len);
void    EEPwriteb(uint16 adr, uint16 val);

float   fPSETmax(void);

#ifdef MF40
    #define mbsl_isActive   (MF40_Setup.MODBUS)
#else
    #define mbsl_isActive   1
#endif

// *****************************************************************************
// Lokale Variablen
// *****************************************************************************
// MBSL
#define mbsl_adr        MF40_Userdata.MB_Slave_Adr
//#define mbsl_adr      50
#define mbsl_tx_max     512                 // max. TX-Puffergröße
#define mbsl_rx_max     512                 // max. RX-Puffergröße
char    mbsl_tx_str[mbsl_tx_max];
char    mbsl_tx_tempstr[mbsl_tx_max];
char    mbsl_rx_str[mbsl_rx_max];
int     mbsl_rx_state;                      // MBSL RX state machine
int     mbsl_rx_len;
uint16  mbsl_rxcs;                          // CRC
char *  mbsl_rxp;               
uint32  mbsl_rx_ts;                         // RX ts, MilliSekunden32 basierend
uint    mbsl_state_CR_count;                // Anzahl empfangener Einzel-CR

// MBMA
#define mbma_tx_max     512                 // max. TX-Puffergröße
#define mbma_rx_max     512                 // max. RX-Puffergröße
char    mbma_tx_str[mbma_tx_max];
char    mbma_tx_tempstr[mbma_tx_max];
char    mbma_rx_str[mbma_tx_max];
int     mbma_rx_state;                      // MBMA RX state machine
uint16  mbma_rxcs;                          // CRC
char *  mbma_rxp;               
uint32  mbma_rx_ts;                         // RX ts, MilliSekunden32 basierend

uint32  coil32=0x55;                        // Coil Test



#define mb_exception_IllegalFunction        1
#define mb_exception_IllegalDataAddress     2
#define mb_exception_IllegalDataValue       3
#define mb_exception_SlaveDeviceFailure     4


// *****************************************************************************
// *****************************************************************************
// +++ MODBUS SLAVE +++ 										   12.06.2018 HR
// *****************************************************************************
// *****************************************************************************
enum {
    mbsl_state_IDLE = 0,
    mbsl_state_TRUNC,
    mbsl_state_NOTME,
    mbsl_state_EVAL,
    mbsl_state_CR,
    mbsl_state_CMD_BEGIN,
    mbsl_state_CMD_TRUNC,
    mbsl_state_CMD
};
// -----------------------------------------------------------------------------
// Message-Timeout erreicht --> 1, sonst --> 0      // 13.06.2018 HR : Erstellt
// -----------------------------------------------------------------------------
int MBSL_MessageTimeout(void)
{
    return (((MilliSekunden32 - mbsl_rx_ts)>3) ? 1 : 0);
}
// -----------------------------------------------------------------<PIC+ECS>---
// MBSL_RX - Auswertung des empfangenen MBSL Zeichens c
// Das empfangene, an "mich" adressierte Telegramm wird in mbsl_rx_STR abgelegt.
// Sobald es gültig ist, wird mbsl_rx_LEN = Länge des Telegramm gesetzt.
// -----------------------------------------------------------------------------
// 13.06.2018 HR : Erstellt
// -----------------------------------------------------------------------------
void MBSL_RX(char c)
{
    int i;
    uint16 len;

    switch(mbsl_rx_state) {
        // ...................................
        default:
            mbsl_rx_state = mbsl_state_IDLE;
        // ...................................
        case mbsl_state_IDLE:
            mbsl_rx_ts = MilliSekunden32;
            if ((uchar)c == mbsl_adr) {
                mbsl_rxp = mbsl_rx_str;
                *(mbsl_rxp++) = c;
                mbsl_rx_len = 1;
                mbsl_rx_state = mbsl_state_TRUNC;
            } else {
                if (c==0x0D) mbsl_rx_state = mbsl_state_CR;
                else         mbsl_rx_state = mbsl_state_NOTME;
            }
            break;
        // ...................................
        case mbsl_state_TRUNC:
            mbsl_state_CR_count = 0;
            mbsl_rx_ts = MilliSekunden32;
            if (mbsl_rx_len<mbsl_rx_max) {
                *(mbsl_rxp++) = c;
                mbsl_rx_len++;
            }
            break;
        // ...................................
        case mbsl_state_NOTME:
            mbsl_state_CR_count = 0;
            mbsl_rx_ts = MilliSekunden32;
            break;
        // ...................................
        case mbsl_state_CR:
            mbsl_rx_ts = MilliSekunden32;
            mbsl_rx_state = mbsl_state_NOTME;
            break;
        // ...................................
        case mbsl_state_EVAL:
            // während der Auswertung sollten vom Master keine Zeichen kommen...
            break;
        // ...................................
        case mbsl_state_CMD_BEGIN:
            mbsl_rxp = mbsl_rx_str;
            mbsl_rx_len = 0;
            mbsl_rx_state = mbsl_state_CMD_TRUNC;
        case mbsl_state_CMD_TRUNC:
            switch(c) {
                case 0x0D:
                    mbsl_rx_state = mbsl_state_CMD;
                    break;
                default:
                    if (mbsl_rx_len<mbsl_rx_max-1) {
                        *(mbsl_rxp++) = c;
                        mbsl_rx_len++;
                    }
                    break;
            }
            break;
        // ...................................
        case mbsl_state_CMD:
            // während der Auswertung sollten vom Master keine Zeichen kommen...
            break;
        // ...................................
    }
}
// -----------------------------------------------------------------------------
// MBSL_Eval() wird von main() zyklisch aufgerufen
// -----------------------------------------------------------------------------
void MBSL_Eval(void)
{
    if (!mbsl_isActive) {   // nicht aktiv -->  empfangene Zeichen ignorieren
        mbsl_rx_state=mbsl_state_IDLE;
        return;
    }
    switch (mbsl_rx_state) {
        case mbsl_state_TRUNC:
            if (MBSL_MessageTimeout()) {    
                mbsl_rx_state=mbsl_state_EVAL;
                // CRC
                if (mbsl_rx_len>2) {
                    mbsl_rxcs = CRC16str(mbsl_rx_str, mbsl_rx_len-2); 
                    // DoEval
                    if (((uchar)mbsl_rx_str[mbsl_rx_len-2]==(mbsl_rxcs>>8)) &&
                        ((uchar)mbsl_rx_str[mbsl_rx_len-1]==(mbsl_rxcs&0xff))) {
                        MBSL_DoEval(mbsl_rx_str, mbsl_rx_len-2);
                    }
                }
                mbsl_rx_state=mbsl_state_IDLE;
            }
            break;
        case mbsl_state_NOTME:
            if (MBSL_MessageTimeout()) {
                mbsl_rx_state=mbsl_state_IDLE;
            }
            break;
        case mbsl_state_CR:
            if (MBSL_MessageTimeout()) {
                if (++mbsl_state_CR_count>=4) mbsl_rx_state=mbsl_state_CMD;
                else                          mbsl_rx_state=mbsl_state_IDLE;
            }
            break;
        case mbsl_state_CMD:
            mbsl_rx_str[mbsl_rx_len]=0;                     // 0-term
            if (!MBSL_DoCmdEval(mbsl_rx_str, mbsl_rx_len)) {// ==0 -->
                mbsl_rx_state=mbsl_state_CMD_BEGIN;         //  OK, weiter mit CMD einlesen
            } else {                                        // !=0 --> 
                mbsl_rx_state=mbsl_state_IDLE;              //  Error/Abbruch
            }
        default: break;
    }
}
// -----------------------------------------------------------------------------
// MBSL_DoEval()
// wird von MBSL_Eval() aufgerufen, das von main() zyklisch aufgerufen wird.
// -----------------------------------------------------------------------------
// 10.02.2019 HR - Bugfix bei Code 6 (reqlen wurde verwendet, obwohl statisch=1)
// 21.12.2018 HR - Bugfix bei Code 5
// 13.06.2018 HR - Erstellt
// -----------------------------------------------------------------------------
void MBSL_DoEval(char * sp, int len)
{
int i, j, r=0;
uint16  crc;
uchar   adr;
uchar   code;
uchar   errcode=mb_exception_SlaveDeviceFailure;
uint16  datadr;
uint16  reqlen;     // requested len (number of registers), bytelen=reqlen*2
uint16  bytelen;

    if (len>=6) {
        i=0;
        adr = (uchar)sp[i++];                   // len=1
        code = (uchar)sp[i++];                  // len=2
        datadr = ((uint)(uchar)sp[i++])<<8;     // len=4
        datadr += ((uint)(uchar)sp[i++]);
        reqlen = ((uint)(uchar)sp[i++])<<8;     // len=6
        reqlen += ((uint)(uchar)sp[i++]);
        switch(code) {
            case 1:                             // Read Coil Status
            case 2:                             // Read Discrete Input Status
                j=0;
                mbsl_tx_str[j++] = adr;
                mbsl_tx_str[j++] = code;
                j++;                            // Byte-Anzahl reservieren
                r=MBSL_CoilMap_Read(mbsl_tx_str+j, code, datadr, reqlen);
                if (r<0) {errcode = -r; goto ERROR;}
                mbsl_tx_str[j-1] = r;
                j+=r;
                goto SENDOUT;
            case 5:                             // Write Single Coil
                r=MBSL_CoilMap_Write(sp+i-2, code, datadr, 1, 2);
                if (r<0) {errcode = -r; goto ERROR;}
                j=6; memcpy(mbsl_tx_str, sp, j);// Bugfix am 21.12.2018: j=4 --> j=6
                goto SENDOUT;
            case 15:                            // Write Multiple Coils
                bytelen = (uint)(uchar)sp[i++];
                r=MBSL_CoilMap_Write(sp+i, code, datadr, reqlen, bytelen);
                if (r<0) {errcode = -r; goto ERROR;}
                j=6; memcpy(mbsl_tx_str, sp, j);
                goto SENDOUT;
            case 3:                             // Read Holding Register(s)
            case 4:                             // Read Input Register(s)
                j=0;
                mbsl_tx_str[j++] = adr;
                mbsl_tx_str[j++] = code;
                j++;                            // Byte-Anzahl reservieren
                r=MBSL_Map_Read(mbsl_tx_str+j, code, datadr, reqlen);
                if (r<0) {errcode = -r; goto ERROR;}
                mbsl_tx_str[j-1] = r;
                j+=r;
SENDOUT:        crc = CRC16str(mbsl_tx_str, j); 
                swapis(mbsl_tx_str+j, crc); j+=2;
#ifdef MBSL_VIA_COM2            
                COM2tx_Kick(mbsl_tx_str, j);
#else                
                COMtx_Kick(mbsl_tx_str, j);
#endif
                break;
            case 6:                             // Write Single Register
                r=MBSL_Map_Write(sp+i-2, code, datadr, 1, 2); // Bugfix 10.02.2019: reqlen wurde verwendet, statt statisch=1
                if (r<0) {errcode = -r; goto ERROR;}
                j=6; memcpy(mbsl_tx_str, sp, j);
                goto SENDOUT;
            case 16:                            // Write Multiple Registers
                bytelen = (uint)(uchar)sp[i++];
                r=MBSL_Map_Write(sp+i, code, datadr, reqlen, bytelen);
                if (r<0) {errcode = -r; goto ERROR;}
                j=6; memcpy(mbsl_tx_str, sp, j);
                goto SENDOUT;
            default:
                errcode = mb_exception_IllegalFunction;
ERROR:          j=0;
                mbsl_tx_str[j++] = adr;
                mbsl_tx_str[j++] = code | B7;   
                mbsl_tx_str[j++] = errcode;
                goto SENDOUT;
        }
    }
}


struct MBtab_RW_Struct {
    void * varp;
    uchar bsize;
    uchar writeable;        // 0:ro, 1:rw
    int32 uw;
    int32 ow;
    int protectbit;         // 0:not used, 1..n für Bit B0..Bn-1
};
struct MBtab_RO_Struct {
    void * varp;
    uchar bsize;
    uchar writeable;        // 0:ro, 1:rw
    int32 uw;
    int32 ow;
};
float mbt_spare[2];
#define ro              0
#define rw              1
#define usefPSETmax     1234
#define tbtabsize_RW    12      // 10 bis V2.02, 12 ab V2.03
                                // in V2.01.22 versehentlich 18 (bei 10 Zeilen)!
const struct MBtab_RW_Struct MF40MBTAB_RW[tbtabsize_RW] = {
    {(void*)&SCADA_Cmd0, 4, rw, 0,0, 0},                    // 1    CMD32 ---nur Platzhalter---
    {(void*)&MF40_Userdata.P_SET, 4, rw, 0,usefPSETmax, 1}, // 2    P_SET
    {(void*)&MF40_Userdata.TEMP_SET,4, rw,0,TEMP_max_SET,2},// 3    TEMP_SET
    {(void*)&MF40_Userdata.TIMER_SET, 4, rw, 0,18*3600, 3}, // 4    TIMER_SET
    {(void*)&MF40_Userdata.PID[0].Kp, 4, rw, 0,500, 4},     // 5    PID_Kp
    {(void*)&MF40_Userdata.PID[0].Ki, 4, rw, 0,500, 5},     // 6    PID_Ki
    {(void*)&MF40_Userdata.PID[0].Kd, 4, rw, 0,500, 6},     // 7    PID_Kd
    {(void*)&MF40_Userdata.PID[0].GENOFFuse,2, rw, 0,1, 7}, // 8    GenOffUse
    {(void*)&MF40_Userdata.TEMP_stop,2,rw,0,TEMP_max_SET,8},// 9    StopAtTemp
    {(void*)&MF40_Userdata.TIMER_delayed, 2, rw, 0,1, 9},   // 10   TimerDelay
    {(void*)&mbt_spare[0], 4, rw, -10000,10000, 0},         // 11   Spare
    {(void*)&mbt_spare[1], 4, rw, -10000,10000, 0},         // 12   Spare
};
#define tbtabsize_RO    8   // in V2.01.22 versehentlich 18 (bei 8 Zeilen)!
const struct MBtab_RO_Struct MF40MBTAB_RO[tbtabsize_RO] = {
    {(void*)&SCADA_Status0, 4, ro, 0,0},                    // 1    STATUS32 ---nur Platzhalter---
    {(void*)&MF40_Ist_P, 4, ro, 0,0},                       // 2    P
    {(void*)&MF40_Ist_Temp, 4, ro, 0,0},                    // 3    TEMP
    {(void*)&MF40_Ist_Timer, 4, ro, 0,0},                   // 4    TIMER
    {(void*)&MF40_Ist_I, 4, ro, 0,0},                       // 5    I
    {(void*)&MF40_Ist_Freq, 4, ro, 0,0},                    // 6    F
    {(void*)&MF40_Ist_cosPhi, 4, ro, 0,0},                  // 7    COS_PHI
    {(void*)&MF40_Regel_Steller, 4, ro, 0,0},               // 8    REG%
};
// -----------------------------------------------------------------------------
// MBSL_Map_Read()
// Wird bei code 3 + 4 aufgerufen
// -----------------------------------------------------------------------------
// 09.02.2019 HR - erste Volldefinition
// 13.06.2018 HR - Erstellt
// -----------------------------------------------------------------------------
int MBSL_Map_Read(char * dp, uchar code, uint16 datadr, uint16 reqlen)
{
int j,ji, bsize, little=0;
uint16 ui16, da;
uint32 pi32, ui32;
float ff;
    j=0;
    if (code!=3) return (mb_exception_IllegalFunction * -1);        // hier nur "Holding Registers" (code=3)
    if (datadr>=1000) {datadr-=1000; little=1;}
    if ((datadr>=100)&&(datadr<=199)) {datadr-=100;}
    while(1) {
        ji=j;
        switch(datadr) {
            case 0:
                j+=cswap2s(dp+j,(char*)&SCADA_Cmd0,little); break;  // uint16
            case 1:
                j+=cswap2s(dp+j,(char*)&SCADA_Cmd1,little); break;  // uint16
            case 200:
                SCADA_Status0 = getSCADA_Status0();                 // globalisieren
                j+=cswap2s(dp+j,(char*)&SCADA_Status0,little);break;// uint16
            case 201:
                SCADA_Status1 = LED_status16();                     // globalisieren
                j+=cswap2s(dp+j,(char*)&SCADA_Status1,little);break;// uint16
            default:
                da = datadr;
                if (da<200) {
                    if (da<tbtabsize_RW*2) {                        // 0,2,4,...
                        if (da&B0) return (mb_exception_IllegalDataAddress * -1);
                        pi32 = ((uint32)MF40MBTAB_RW[da/2].varp);
                        bsize = MF40MBTAB_RW[da/2].bsize;
                        // .........................................................
    //                  if (pi32==(uint32)&SCADA_Status) {...}
                        // .........................................................
                        if (bsize==2) {
                            ff = (float)*(uint16*)pi32;
                            pi32 = (uint32)&ff;
                        }
                        j += cswap4s(dp+j,(char*)pi32,little);
                    } else return (mb_exception_IllegalDataAddress * -1);
                } else {
                    da -= 200;
                    if (da<tbtabsize_RO*2) {
                        if (da&B0) return (mb_exception_IllegalDataAddress * -1);
                        pi32 = ((uint32)MF40MBTAB_RO[da/2].varp);
                        bsize = MF40MBTAB_RO[da/2].bsize;
                        // .........................................................
    //                  if (pi32==(uint32)&SCADA_Status) {...}
                        // .........................................................
                        if (bsize==2) {
                            ff = (float)*(uint16*)pi32;
                            pi32 = (uint32)&ff;
                        }
                        j += cswap4s(dp+j,(char*)pi32,little);
                    } else return (mb_exception_IllegalDataAddress * -1);
                }
                break;
        }
        if ((j-ji)<2) return j;
        if (j >= reqlen*2) return j;
        datadr += (j-ji)/2;
    }
}

// -----------------------------------------------------------------------------
// Prüft diverse Freigabemasken auf Bit pb
// Ein gesetztes Bit in der Freigabemaske hebt die Sperre auf
// return   : 1=gesperrt, 0=nicht gesperrt
// -----------------------------------------------------------------------------
// 11.02.2019 HR - Erstellt
// -----------------------------------------------------------------------------
int EvalProtectBit(int pb)
{
    if (!pb) return 0;
    if (pb<=16) return ((SCADA_Cmd1&(1<<(pb-1)))?0:1);
    return 0;
}

// -----------------------------------------------------------------------------
// MBSL_Map_Write()
// Wird bei code 3 aufgerufen
// -----------------------------------------------------------------------------
// 10.02.2019 HR - erste Volldefinition
// 14.06.2018 HR - Erstellt
// -----------------------------------------------------------------------------
int MBSL_Map_Write(char * dp, uchar code, uint16 datadr, uint16 reqlen, uint16 bytelen)
{
int j,ji, bsize, little=0, protect=0, pb;
uint16 ui16;
uint32 pi32,ui32;
int32 uw,ow;
float ff, fuw,fow;
    j=0;
    if (datadr>=1000) {datadr-=1000; little=1;}
    if ((datadr>=100)&&(datadr<=199)) {datadr-=100; protect=1;}
    while(1) {
        ji=j; pb=0;
        switch(datadr) {
            case 0:
                j+=cswap2s((char*)&SCADA_Cmd0,dp+j,little);break;// uint16
            case 1:
                j+=cswap2s((char*)&SCADA_Cmd1,dp+j,little);break;// uint16
            default:
                if (datadr<tbtabsize_RW*2) {                     // 0,2,4,...
                    if (datadr&B0) return (mb_exception_IllegalDataAddress * -1);
                    if (!MF40MBTAB_RW[datadr/2].writeable) return (mb_exception_IllegalDataAddress * -1);
                    pi32 = ((uint32)MF40MBTAB_RW[datadr/2].varp);
                    bsize = MF40MBTAB_RW[datadr/2].bsize;
                    uw=MF40MBTAB_RW[datadr/2].uw; fuw=(float)uw;
                    ow=MF40MBTAB_RW[datadr/2].ow; fow=(float)ow;
                    if (ow==usefPSETmax) fow=fPSETmax();
                    if (protect) pb=EvalProtectBit(MF40MBTAB_RW[datadr/2].protectbit);
                    // .........................................................
//                  if (pi32==(uint32)&SCADA_Status) {...; goto WWW;}
                    // .........................................................
                    if (bsize==2) {
                        j += cswap4s((char*)&ff,dp+j,little);
                        if (ff>fow) ff=fow; else if (ff<fuw) ff=fuw;
                        if (!pb) *(int16*)pi32 = (int16)(ff+0.5);
                    } else {
WWW:                    j += cswap4s((char*)&ff,dp+j,little);
                        if (ff>fow) ff=fow; else if (ff<fuw) ff=fuw;
                        if (!pb) *(float*)pi32 = ff;
                    }
                } else return (mb_exception_IllegalDataAddress * -1);
                break;
        }
        if ((j-ji)<2) return j;
        if (j >= reqlen*2) return j;
        if (j >= bytelen) return j;
        datadr += (j-ji)/2;
    }
}
// -----------------------------------------------------------------------------
// MBSL_Map_Read()                                    // Erstellt: 13.06.2018 HR
// ALTE FORM, NICHT MEHR IN VERWENDUNG
// Wird bei code 3 + 4 aufgerufen
// -----------------------------------------------------------------------------
int MBSL_Map_Read_explicit(char * dp, uchar code, uint16 datadr, uint16 reqlen)
{
int j,ji;
uint16 ui16;
uint32 ui32;
    j=0;
    if (code!=3) return (mb_exception_IllegalFunction * -1);        // hier nur "Holding Registers" (code=3)
    while(1) {
        ji=j;
        switch(datadr) {
            case 0:
                j+=swap4s(dp+j,(char*)&MF40_Userdata.P_SET);        // float32
                break;
            case 2:
                j+=swap4s(dp+j,(char*)&MF40_Userdata.TIMER_SET);    // float32
                break;
            case 4:
                j+=swap4s(dp+j,(char*)&MF40_Userdata.TEMP_SET);     // float32
                break;
            case 6:
                ui32=0x12345678UL; j+=swap4s(dp+j,(char*)&ui32);    // uint32
                break;
            case 8:
                ui16=0x1234; j+=swap2s(dp+j,(char*)&ui16); break;   // uint16
            case 9:
                ui16=0x5678; j+=swap2s(dp+j,(char*)&ui16); break;   // uint16
            case 10:
                ui16=0x4455; j+=swap2s(dp+j,(char*)&ui16); break;   // uint16
            case 11:
                ui16=0xbbcc; j+=swap2s(dp+j,(char*)&ui16); break;   // uint16
            case 12:
            case 13:
            case 14:
            case 15:
                dp[j++] = 0; dp[j++] = 0; break;                    // uint16 NULL
            default:
                return (mb_exception_IllegalDataAddress * -1);
        }
        if ((j-ji)<2) return j;
        if (j >= reqlen*2) return j;
        datadr += (j-ji)/2;
    }
}

// -----------------------------------------------------------------------------
// MBSL_Map_Write()                                   // Erstellt: 14.06.2018 HR
// ALTE FORM, NICHT MEHR IN VERWENDUNG
// Wird bei code 6 (bytelen==2) + 16 (bytelen>=4) aufgerufen
// -----------------------------------------------------------------------------
int MBSL_Map_Write_explicit(char * dp, uchar code, uint16 datadr, uint16 reqlen, uint16 bytelen)
{
int j,ji;
uint16 ui16;
uint32 ui32;
    j=0;
    while(1) {
        ji=j;
        switch(datadr) {
            case 0:
                j+=swap4s((char*)&MF40_Userdata.P_SET,dp+j);        // float32
                break;
            case 2:
                j+=swap4s((char*)&MF40_Userdata.TIMER_SET,dp+j);    // float32
                break;
            case 4:
                j+=swap4s((char*)&MF40_Userdata.TEMP_SET,dp+j);     // float32
                break;
            default:
                return (mb_exception_IllegalDataAddress * -1);
        }
        if ((j-ji)<2) return j;
        if (j >= reqlen*2) return j;
        if (j >= bytelen) return j;
        datadr += (j-ji)/2;
    }
}

// -----------------------------------------------------------------------------
// MBSL_CoilMap_Read()                                // Erstellt: 21.08.2018 HR
// Wird bei code 1 + 2 aufgerufen
// reqlen hier: Anzahl der Bits!
// -----------------------------------------------------------------------------
int MBSL_CoilMap_Read(char * dp, uchar code, uint16 datadr, uint16 reqlen)
{
int i,j;
uint32 ui32;
uint32 val;
    j=0;                                // Bitzähler
    ui32=0;                             // Ergebnis mit 0 vorbelegen
    // code=1   --> datadr Bereich 0..31, Bit1..32 aus val
    // code=2   --> datadr Bereich 0..31, Bit1..32 aus (val<<16)
    if (datadr>=32) return (mb_exception_IllegalDataAddress * -1);
    if ((reqlen>32)||(reqlen==0))  return (mb_exception_IllegalDataAddress * -1);
    val = coil32;
    if (code==2) val <<= 16;
    while(1) {
        // ui32 wird ab B0 an mit dem jeweiligen Bit aus coil32 aufgefüllt:
        ui32 |= (val & (1UL<<datadr)) ? (1UL<<j) : 0;
        j++;
        datadr++;
        if (j >= reqlen) {              // angeforderte Byteanzahl erreicht?
            j = (j-1)/8;                // Anzahl gelesener Bits --> Byteanzahl-1
            j++;                        // --> Byteanzahl 1..4
            for (i=0;i<j;i++) {
                *(dp++) = (char)((ui32>>(i*8))&0xff);
            }
            return j;
        }
    }
}

// -----------------------------------------------------------------------------
// MBSL_CoilMap_Write()                               // Erstellt: 22.08.2018 HR
// Wird bei code 5 (bytelen==2) + 15 (bytelen>=1) aufgerufen
// reqlen : Anzahl der Coils / Bits
// bytelen: Anzahl der Bytes ab dp
// -----------------------------------------------------------------------------
int MBSL_CoilMap_Write(char * dp, uchar code, uint16 datadr, uint16 reqlen, uint16 bytelen)
{
int i;
uint32 *ui32p;
    // code=1   --> datadr Bereich 0..31,    Bit1..32 aus uval32
    // code=2   --> datadr Bereich 100..131, Bit1..32 aus (uval32*2)
    if (datadr>=32) return (mb_exception_IllegalDataAddress * -1);
    if ((reqlen>32)||(reqlen==0))  return (mb_exception_IllegalDataAddress * -1);
    ui32p = &coil32;
    if (code==5) {
        if ( ((uchar)*dp==0xff) && (*(dp+1)==0) )   *ui32p |=  (1UL<<datadr);
        else                                        *ui32p &= ~(1UL<<datadr);
    } else {
        for (i=0;i<reqlen;i++) {
            if  (*(dp+(i/8)) & (1<<(i&0x7))) {
                *ui32p |=  (1UL<<datadr);
            } else {
                *ui32p &= ~(1UL<<datadr);
            }
            datadr++;
            if (datadr>=32) break;      // Achtung, ohne Prüfung wird bei (1UL<<datadr) quasi roliert, nicht herausgeschoben!
                                        // Dies ist eine GCC Eigenheit, dass die Shift-Anzahl stets modulo der Registerbreite
                                        // (hier 32) genommen wird!
        }
    }
    return 0;
}

// -----------------------------------------------------------------------------
// MBSL_DoCmdEval()
// wird von MBSL_Eval() aufgerufen, das von main() zyklisch aufgerufen wird.
// return:  >0  Error/Abbruch
//          0   OK
// -----------------------------------------------------------------------------
// 02.09.2021 HR - Erstellt
// -----------------------------------------------------------------------------
int MBSL_DoCmdEval(char * sp, int len)
{
    return 0;
}

// *****************************************************************************
// *****************************************************************************
// +++ MODBUS MASTER +++ 										   13.06.2018 HR
// *****************************************************************************
// *****************************************************************************
enum {
    mbma_state_IDLE = 0,
    mbma_state_START,
    mbma_state_TRUNC,
};
// -----------------------------------------------------------------------------
// MBMA_RX - Auswertung des empfangenen MBMA Zeichens c
// Das empfangene, an "mich" adressierte Telegramm wird in mbma_rx_STR abgelegt.
// Sobald es gültig ist, wird mbma_rx_LEN = Länge des Telegramm gesetzt.
// -----------------------------------------------------------------------------
// 13.06.2018 HR : Erstellt
// -----------------------------------------------------------------------------
void MBMA_RX(char c)
{
    int i;
    uint16 len;
    
    mbma_rx_ts = MilliSekunden32;

    switch(mbma_rx_state) {
        // ...................................
        case mbma_state_IDLE:
            break;
        // ...................................
        case mbma_state_START:
            break;
        // ...................................
        case mbma_state_TRUNC:
            break;
        // ...................................
        default:
            mbma_rx_state = mbma_state_IDLE;
            break;
    }
}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int MBMA_TXs(char * dp, char * sp, int len)
{
}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void MBMA_Eval(void)
{
}


/*******************************************************************************
 End of File
*/

