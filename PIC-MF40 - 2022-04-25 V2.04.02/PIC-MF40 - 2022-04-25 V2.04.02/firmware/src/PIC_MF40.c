// *****************************************************************************
// PIC_MF40.c
//
//  Rüffler Computer Hard+Soft
//  Hans Rüffler
//  (c) Copyright 2017..2020 by Hans Rüffler
//
//  PIC_MF40 relevant routines are located in this file
//  22.06.2020 HR
//
// *****************************************************************************
// Hardware relevante Informationen:
// - Bei allen Eingängen mit vorgeschaltetem Spannungsteiler (z.B. IN-1..16)
//   dürfen keine Pull-Ups gewählt sein, da der Spannungsteiler vor dem Eingang 
//   sonst positiv vorgespannt werden würde und dadurch der Eingang nie Low zeigt.
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "system/common/sys_module.h"   // SYS function prototypes
#include "peripheral/oc/plib_oc.h"

#include "PIC32.h"
#include "system_definitions.h"
#include "PIC_VAR.h"
#include "PIC_Func.h"
#include "PIC_FT8X1.h"

#include "PIC_MF40.h"

// *****************************************************************************
// hier enthaltene lokale Funktionen
// *****************************************************************************
void    ADCsetup(void);
uint32  ADCbuf32(int kan);
int     LED_val(int x);
int     OUT_EIN_Control(int val);
int     OUT_GENOFF_Control(int val);
int     OUT_RESET_Control(int val);
void    LED_Debounce(void);
void    I2C_INIT(int mode);
float   P_Regler(int w, int x);
float   PID_Regler(int w, int x);
void    PID_Init(int ps);
void    PID_ChangeCheck(int ps);
int     PID_Iactive(void);
void    SIM_Init(void);
int     TempStop(void);
float   fPSETmax(void);

// *****************************************************************************
// externe Funktionen
// *****************************************************************************
int     getIntendedWorkMode(int override);

// *****************************************************************************
// Variablen
// *****************************************************************************

// MF40_LAYOUT -----------------------------------------------------------------
            // ******MF40 LAYOUT******
            // 0:P         1:P SET
            // 2:I         3:FREQ
            // 4:cos(PHI)  5:TIMER
            // 6:TEMP      7:TEMP SET
            // ***********************
const char  MG_name_mf[8][10]   = {"P",     "P-SET",    "I",    "FREQ", "HF-Factor", "TIMER",   "TEMP", "TEMP-SET"};// "" --> Messfeld="-"
const char  MG_unit_mf[8][10]   = {"kW",    "kW",       "A",    "kHz",  "",          "s",        "°C",    "°C"};    // Achtung: Grad-Zeichen "°" nur mit "°C" oder "°F" verwenden (Sonderbehandlung notwendig)
const char  MG_val_mf[8][12]    = {"0.0",   "0.0",      "0.0",  "630",  "0.00",      "0:00:00",  "0",     "1200"};
const char  MG_fix_mf[8]        = { 1,       1,          1,      0,      2,           0,          0,       0};
const float MG_fac_mf[8]        = { 1,       1,          1.0,    1.0,    0.01,        1.0,        12.0,    12.0};   // Achtung: nicht ändern, da u.U. weitere Skalierungen in ANAval() darauf angewendet werden!
                           //         r=red, g=green, y=yellow, _=grey, ""=nicht anzeigen, s=Reset-Spezial (s muss bei ON+OFF angegeben werden!)
const char LED_nameON_mf[16][10]  = {"rRUN", "gReady", "sRESET", "gI-REG", "gU-REG", "gB-REG", "gP-REG", "gF-REG", "rI >", "rFI", "rPHI", "rId >", "rT >", "rH2O", "rUc >", "rEXT"};
                           //  Pos:   1       2         3         4         5         6         7         8         9       10     11      12       13      14      15	    16									
const char LED_nameOFF_mf[16][10] = {"_RUN", "_Ready", "sRESET", "_I-REG", "_U-REG", "_B-REG", "_P-REG", "_F-REG", "_I >", "_FI", "_PHI", "_Id >", "_T >", "_H2O", "_Uc >", "_EXT"};

// Positions-Definition der LED 1..16: Eintrag 1..16 für LED 1..16 oder -1, falls Position nichts enthalten soll
								// Pos1 Pos2 Pos3 Pos4 Pos5 Pos6 Pos7 Pos8 -- Pos9 Pos10 Pos11 Pos12 Pos13 Pos14 Pos15 Pos16 
const char LEDpos_mf[16] =         {  1,   3,  -1,  10,  11,  12,  13,  14,      4,    5,    6,    7,    8,    9,   15,   16};

                        //      Ana_P Ana_I Ana_cosPHI  Ana_Freq Ana_Pyro_1 Ana_P_Reg_Ext
const char  TabAna2MG_mf[6]     = {0,    2,    4,          3,       6,         1};             // Übersetzung 0-based ANA-Kanal 0..7 in Panel-Pos. 0..7
const char  TabAnaName_mf[6][10]= {"P",  "I",  "HF-Factor", "Freq",  "Pyro-1",  "ExtRun#"};     // Namen des 0-based ANA-Kanal 0..7 (bis V2.02.02: 5->"PregExt")

// HF25_LAYOUT -----------------------------------------------------------------
                            // ******HF40 LAYOUT******
                            // 0:P         1:P SET
                            // 2:I         3:FREQ
                            // 4:cos(PHI)  5:TIMER
                            // 6:TEMP      7:TEMP SET
                            // ***********************
const char  MG_name_hf[8][10]   = {"Id",    "U-SET",    "U-HF", "FREQ", "",          "TIMER",   "TEMP", "TEMP-SET"};// "" --> Messfeld="-"
const char  MG_unit_hf[8][10]   = {"A",     "V",        "V",    "kHz",  "",          "s",        "°C",    "°C"};    // Achtung: Grad-Zeichen "°" nur mit "°C" oder "°F" verwenden (Sonderbehandlung notwendig)
const char  MG_val_hf[8][12]    = {"0.0",   "0.0",      "0.0",  "630",  "0.00",      "0:00:00",  "0",     "1200"};
const char  MG_fix_hf[8]        = { 1,       1,          1,      0,      2,           0,          0,       0};
const float MG_fac_hf[8]        = { 1,       1,          1.0,    1.0,    0.01,        1.0,        12.0,    12.0};   // Achtung: nicht ändern, da u.U. weitere Skalierungen in ANAval() darauf angewendet werden!
                        //            r=red, g=green, y=yellow, _=grey, ""=nicht anzeigen, s=Reset-Spezial (s muss bei ON+OFF angegeben werden!)
const char LED_nameON_hf[16][10]  = {"rRUN", "gReady", "sRESET", "gI-REG", "gU-REG", "_-",   "gT-REG",    "_-", "rI >", "rFI",   "rF min", "rId >", "rT >", "rH2O", "rLast >", "rEXT"};
                        //     Pos:   1       2         3         4         5         6       7            8     9       10       11        12       13      14      15	        16									
const char LED_nameOFF_hf[16][10] = {"_RUN", "_Ready", "sRESET", "_I-REG", "_U-REG", "_-",   "_T-REG",    "_-", "_I >", "_FI",   "_F min", "_Id >", "_T >", "_H2O", "_Last >", "_EXT"};

// Positions-Definition der LED 1..16: Eintrag 1..16 für LED 1..16 oder -1, falls Position nichts enthalten soll
								// Pos1 Pos2 Pos3  Pos4 Pos5 Pos6 Pos7 Pos8 -- Pos9 Pos10 Pos11 Pos12 Pos13 Pos14 Pos15 Pos16 
const char LEDpos_hf[16] =         {  1,   3,  -1,   10,  11,  12,  13,  14,      4,    5,    6,    7,    8,    9,   15,   16};

                        //      Ana_P Ana_I Ana_cosPHI  Ana_Freq Ana_Pyro_1 Ana_P_Reg_Ext
const char  TabAna2MG_hf[6]     = {0,    2,    4,          3,       6,         1};             // Übersetzung 0-based ANA-Kanal 0..7 in Panel-Pos. 0..7
const char  TabAnaName_hf[6][10]= {"Id","U-HF","-",       "Freq",  "Pyro-1",  "PregExt"};      // Namen des 0-based ANA-Kanal 0..7
// -----------------------------------------------------------------------------
// In die folgenden Variablen werden die Vorgaben kopiert:
char  MG_name[8][10];
char  MG_unit[8][10];
char  MG_val[8][12];
char  MG_fix[8];
float MG_fac[8];
char  LED_nameON[16][10];
char  LED_nameOFF[16][10];
char  LEDpos[16];
char  TabAna2MG[6];
char  TabAnaName[6][10];
// -----------------------------------------------------------------------------


// ----- PID-Regler -----
float PID_Kp;                 //
float PID_Ki_m_Ta;            // Verbundfaktor PID_Ki * PID_Ta  zur Laufzeit-Optimierung
float PID_Kd_d_Ta;            // Verbundquotient PID_Kd / PID_Ta  zur Laufzeit-Optimierung
int PID_esum;
int PID_ealt;
int PID_last;

// ----- Temperatur-Simulation -----
float SIM_QDegC=0.001;        // kWh/°C Energieinhalt pro °C
float SIM_QKdegC=0.00005;     // kWh/°C*s Energie-Verlust pro °C Differenz zu TAMB pro s
float SIM_TAMB=25.0;          // [°C] Raumtemperatur
float SIM_TBEG=35.0;          // [°C] Gemessene Temperatur bei Beginn

float SIM_Temp;               // [°C] aktuelle simulierte Temperatur
float SIM_E;                  // [kWh]Energie-Gehalt Integrations-Register

// *****************************************************************************
// Lokale #defines
// *****************************************************************************

// *****************************************************************************
// Lokale Variablen
// *****************************************************************************
DRV_HANDLE  I2C_Handle;
DRV_I2C_BUFFER_HANDLE I2C_BufferHandle;
volatile int I2C_busy;      // muss volatile sein!
volatile int AnaTempErr;    // von Ana() ermittelt, wird dann in MF40_Eval globalisiert
uchar ti_rbuf[10];
int ti_2ndHalf;


// *****************************************************************************
// Funktionen
// *****************************************************************************

#define AnaMax      6
int    AnaSumCount = 100;   // Mittelung über 100 Werte (100ms);  200ms bis V2.02.06; Hinweis: bis V2.02.07 per #define
uint32 ANA_SUM[AnaMax];     // Summe über AnaSumCount Werte
uint32 ANA_MEAN[AnaMax];    // *10 : 0..10230
uint32 ANA_SUM_CNT;
// *****************************************************************************
// Wird von Timer1_MilliSekunden() Interrupt jede 1/1024 s aufgerufen
// *****************************************************************************
void MF40_MilliSekunden(void)
{
register int i;
    // .........................................................................
    for (i=0;i<AnaMax;i++) {
        ANA_SUM[i] += ADCval32(i);
    }
    if (++ANA_SUM_CNT >= AnaSumCount) {
        for (i=0;i<AnaMax;i++) {
            ANA_MEAN[i] = ANA_SUM[i] / (AnaSumCount/10);  // *10 skaliert
            ANA_SUM[i] = 0;
        }
        ANA_SUM_CNT = 0;
    }
    // .........................................................................
    if (IN_START) {
        if (START_Taster_ms<8192) START_Taster_ms++;
    } else {
        START_Taster_ms=0;
    }
    if (IN_STOP) {
        if (STOP_Taster_ms<8192) STOP_Taster_ms++;
    } else {
        STOP_Taster_ms=0;
    }
    if (LED_val(0)) {
        if (RUN_LED_ms<8192) RUN_LED_ms++;
    } else {
        RUN_LED_ms=0;
    }
    // .........................................................................
    LED_Debounce();
    IOExtenderOP();
}

// *****************************************************************************
// Hinweis: Die Verwendung von I2C_busy Flag bringt nicht den gewünschten Effekt!
// Zeitdauer von DRV_I2C_Receive oder Transmit bis Callback ca. 126..180us
// *****************************************************************************
void I2C_Callback(DRV_I2C_BUFFER_EVENT event, DRV_I2C_BUFFER_HANDLE handle, uintptr_t context)
{
int devadr=0x40;
    I2C_CB_count++;
    I2C_busy=0;         // Doch, doch, wenn volatile (!): Verwendung von I2C_busy Flag bringt nicht den gewünschten Effekt!
    switch(event) {
        case DRV_I2C_BUFFER_EVENT_COMPLETE: 
            if (ti_2ndHalf) {
                ti_2ndHalf=0;
                I2C_BufferHandle = DRV_I2C_Receive ( I2C_Handle, devadr, &ti_rbuf[0],1, NULL );    // Lesen von GPIOB
                if (I2C_BufferHandle==NULL) ti_2ndHalf=99;  // erzwingt I2C_INIT() bei nächstem IOExtenderOP() Aufruf (kommt anscheinend nicht vor)
            }
            break;
        case DRV_I2C_BUFFER_EVENT_ERROR:    break;       // Error handling here.
        default:                            break;
    }
}

// *****************************************************************************
// I2C_WaitBusy
// *****************************************************************************
// 08.11.2017 Erstellt HR
// *****************************************************************************
int I2C_WaitBusy(void)
{
//  while(I2C_busy);
int busy=1;
uint16 ms=MilliSekunden;
    
    while (busy) {
        switch(DRV_I2C_TransferStatusGet(I2C_Handle, I2C_BufferHandle)) {
            case DRV_I2C_BUFFER_EVENT_COMPLETE: I2C_NO=0; return 0;
            case DRV_I2C_BUFFER_EVENT_ERROR: return 1;
        }
        if ((MilliSekunden-ms)>20) {I2C_NO++; return -1;}
    }
}

// *****************************************************************************
// IOExtenderSetup : Setup des MCP23017 I2C IO-Extenders
// *****************************************************************************
// 14.02.2020 GPB0 = I-Anteil_aktiv# (zuvor Output=0V)
// 08.11.2017 Erstellt HR
// *****************************************************************************
void IOExtenderSetup(void)
{
int devadr=0x40;
int i;
static uchar tbuf[32];
static uchar rbuf[10];

    // Zu allererst MCP23017 in Default-Betriebsmode bringen, Schritt 1/2
    i=0; I2C_busy=1;
    tbuf[i++]=0x05;                 // Register=IOCON @BANK=1
    tbuf[i++]=0x00;                 // IOCON=0 (Default-->BANK=0=16Bit,SEQOP=0=enabled)
    I2C_BufferHandle = DRV_I2C_Transmit ( I2C_Handle, devadr, &tbuf[0], i, NULL );          
    I2C_WaitBusy();
    // Schritt 2/2: falls vor Schritt 1/2 bereits BANK=0 galt, muss ICON mit BANK=0 wiederholt werden
    i=0; I2C_busy=1;
    tbuf[i++]=0x0A;                 // Register=IOCON @BANK=0
    tbuf[i++]=0x00;                 // IOCON=0 (Default-->BANK=0=16Bit,SEQOP=0=enabled)
    I2C_BufferHandle = DRV_I2C_Transmit ( I2C_Handle, devadr, &tbuf[0], i, NULL );          
    I2C_WaitBusy();
    // Alle MCP23017 Register initialisieren, Betriebsmode nun sicher BANK=0=16Bit,SEQOP=0=enabled
    i=0; I2C_busy=1;
    tbuf[i++]=0x00;                 // Register=IODIRA; Default:IOCON=0
    tbuf[i++]=0x00;                 // [00]IODIRA: all outputs(0)
    tbuf[i++]=0b11100001;           // [01]IODIRB: B8,B13..15=inputs(1),other=output(0)
    tbuf[i++]=0x00;                 // [02]IPOLA:
    tbuf[i++]=0x00;                 // [03]IPOLB:
    tbuf[i++]=0x00;                 // [04]GPINTENA:
    tbuf[i++]=0x00;                 // [05]GPINTENB:
    tbuf[i++]=0x00;                 // [06]DEFVALA:
    tbuf[i++]=0x00;                 // [07]DEFVALB:
    tbuf[i++]=0x00;                 // [08]INTCONA:
    tbuf[i++]=0x00;                 // [09]INTCONB:
    tbuf[i++]=0x00;                 // [0A]IOCON:
    tbuf[i++]=tbuf[i-1];            // [0B]IOCON: gleicher Wert
    tbuf[i++]=0x00;                 // [0C]GPPUA: no 100k Pullups
    tbuf[i++]=0b11100000;           // [0D]GPPUB: B13-15=100k Pullups(1)
    tbuf[i++]=0x00;                 // [0E]INTFA:
    tbuf[i++]=0x00;                 // [0F]INTFB:
    tbuf[i++]=0x00;                 // [10]INTCAPA:
    tbuf[i++]=0x00;                 // [11]INTCAPB:
    tbuf[i++]=0x00;                 // [12]GPIOA: Default=0, schreibt in OLATA
    tbuf[i++]=0x00;                 // [13]GPIOB: alle=0, schreibt in OLATB
    I2C_BufferHandle = DRV_I2C_Transmit ( I2C_Handle, devadr, &tbuf[0], i, NULL );          
    I2C_WaitBusy();
    // Abchließend MCP23017 in Betriebsmode BANK=0, SEQOP disabled -->
    // Im Nachfolgenden werden lediglich die Register GPIOA oder GPIOB adressiert, beim
    // Lesen oder Schreiben togglen dann die Registeradressen nur zischen GPIOA/GPIOB
    i=0; I2C_busy=1;
    tbuf[i++]=0x0A;                 // Register=IOCON @BANK=0
    tbuf[i++]=0b00100000;           // IOCON=0 (Default-->BANK/B7=0=16Bit,SEQOP/B5=1=disabled)
    I2C_BufferHandle = DRV_I2C_Transmit ( I2C_Handle, devadr, &tbuf[0], i, NULL );          
    I2C_WaitBusy();
    
    I2C_initialized=1;
}

// *****************************************************************************
void fMIMA(float *fp, float UW, float OW)                          // 20.01.2019
// *****************************************************************************
// If *fp < UW --> *fp = UW
// If *fp > OW --> *fp = OW
// *****************************************************************************
{
    if (*fp<UW) *fp=UW;
    else if (*fp>OW) *fp=OW;
}

// *****************************************************************************
int IncDecStepwise(int val, int step,int incdec, int uw,int ow) // 09.11.2017 HR
// *****************************************************************************
// val   : Quellwert
// step  : Reihe 1,2,5,10, aber auch alle sonstigen >0
//         0: Spezialfall, abhängig von ow:
//         ow>=3600 --> "Zeitdauer" 0..59(1),60..559(5),600..ow(10)
// incdec: +/- Anzahl der Increments/Decrements mit step
// uw,ow : Unterwert bzw. Oberwert, wird nicht unter/überschritten
// *****************************************************************************
{
int i;
    if (!step) {                            // Spezialfall step=0
        step=1;                             // Default step=1, falls keine weitere Bedingung gültig
        if (ow>=3600) {                     // ow>=3600 --> "Zeitdauer"
            if (val<60) step=1;
            else if (val<600) step=5;
            else if (val<3600) step=10;
            else if (val<3600*6) step=60;
            else step=60*5;
        }
    }
    if ((incdec==0)||(step<=0)) return val; // unverändert
    if ((uw==9600)&&(step==9600)) {
        i=val;
        if (incdec>0) {
            if (i<19200) i=19200;
            else if (i<38400) i=38400;
            else if (i<57600) i=57600;
            else if (i<115200) i=115200;
            else if (i<230400) i=230400;
            else if (i<460800) i=460800;
            else if (i<921600) i=921600;
            if (i>ow) i=ow;
        } else {
            if (i>460800) i=460800;
            else if (i>230400) i=230400;
            else if (i>115200) i=115200;
            else if (i>57600) i=57600;
            else if (i>38400) i=38400;
            else if (i>19200) i=19200;
            else if (i>9600)  i=9600;
            if (i<uw) i=uw;
        }
        goto Fertig;
    }        
    if (incdec>0) {
        i = val+(incdec*step);
        i/=step; i*=step; 
        if (i>ow) i=ow;
    } else {
        i=val+(incdec*step); 
        i =(i+step-1)/step; i*=step;
        if (i<uw) i=uw;
    }
Fertig:
    return i;
}

// *****************************************************************************
int getRotarySwitch(void)                                       // 09.11.2017 HR
// *****************************************************************************
{
    if (RotarySwitch) {
        RotarySwitch=0;
        I2C_LEDstate++;
        return 1;
    }
    return 0;
}
// *****************************************************************************
int getRotaryUpDown(void)                                       // 09.11.2017 HR
// *****************************************************************************
{
int r=0;
    if (RotaryUpDown) {
        di(); r=RotaryUpDown; RotaryUpDown=0; ei();
    }
    return r;
}

// *****************************************************************************
// IOExtenderRotaryEval - wird von IOExtenderOP() alle 1/1024s oder seltener
// aufgerufen, im Timer1_MilliSekunden() Interrupt Kontext.
// int in.B0 : Rotary.Clk - Flanke H->L relevant
// int in.B1 : Rotary.Dat - LO @Flanke H->L --> rechts, 
//                          HI @Flanke H->L --> links
// int in.B2 : Rotary.Key - aktiv low, LO --> Taste gedrückt
// *****************************************************************************
// 20.11.2017 Neues Verfahren zur Entprellung des RotarySwitch
// 08.11.2017 Erstellt HR
// *****************************************************************************
void IOExtenderRotaryEval(int in)
{
int dir=0;
uint32 tt;
static int hist_clk;
static int hist_dat;
//static int hist_sw;
static uint32 ms32;                 // Zeit des letzten IncDec wird hier abgelegt
static int RotSw_ms;
    
    hist_clk<<=1; if (in&B0) hist_clk|=B0;
    hist_dat<<=1; if (in&B1) hist_dat|=B0;
//  hist_sw<<=1; if (in&B2) hist_sw|=B0;
    
    if ((hist_clk&0b111)==0b110) {  // auch ohne Entprellung mit 2ms Scan gutes Ergebnis
        if ((hist_dat&0b11)==0b11)   {RotaryUpDown--;dir--;} // Links/Down
        else if ((hist_dat&0b11)==0) {RotaryUpDown++;dir++;} // Rechts/Up
    }
    if (dir) {  // Letzter IncDec kürzer als 50ms her --> "2 drauf"
        if ((tt=(MilliSekunden32-ms32))<50) {
            RotaryUpDown += dir * ((tt<20)?4:2);
        }
        ms32 = MilliSekunden32;
    }
//  Zählt in falsche Richtung beim schnellen Drehen "kein Vor- oder Rückwärtskommen"!
//  if ((hist_clk&0xf)==0b1100) {
//      if ((hist_dat&0x3)==0b11) MF40_P_SET   -= (!(in&B2)) ? 1.0 : 0.1;
//      else if ((hist_dat&0x3)==0) MF40_P_SET += (!(in&B2)) ? 1.0 : 0.1;
//  }

//  if ((hist_sw&0x3)==0b10) RotarySwitch++;    // Achtung, so gibt es Geisterimpulse (ganz selten)
    if (!(in&B2)) {
        RotSw_ms++;
        if (RotSw_ms==10) RotarySwitch++;       // RotarySwitch muss mindestens 10ms gedrückt sein
    } else {
        RotSw_ms=0;
    }
}

// *****************************************************************************
void I2C_INIT(int mode)
// *****************************************************************************
{
    if (mode<0) {I2C_InitRequest++; return;}    // Alternative: Ausführung in MF40_Eval(). Funktioniert, dauert aber u.U. deutlich länger, bis I2C wieder nutzbar ist 
//mus(1);
    DRV_I2C_Close(DRV_I2C_INDEX_0);
    DRV_I2C0_DeInitialize();
    DRV_I2C0_Initialize();
    I2C_Handle = DRV_I2C_Open(DRV_I2C_INDEX_0, DRV_IO_INTENT_READWRITE);
//mus(-1);
    if (mode>0) IOExtenderSetup();
}
        
// *****************************************************************************
// IOExtenderOP - wird von MF40_MilliSekunden() alle 1/1024s aufgerufen, 
// im Timer1_MilliSekunden() Interrupt Kontext.
// *****************************************************************************
// 13.11.2017 I2C_InitRequest wird (wieder) verwendet. I2C_NO neu.
// 08.11.2017 Erstellt HR
// *****************************************************************************
void IOExtenderOP(void)
{
int devadr=0x40;
int i;
uint32 tm;
static uchar tbuf[10];
//static uchar rbuf[10];
static int count;
static int valid=0;

    if (I2C_NO>=2) {                                    // Kein I2C-IO-Extender vorhanden-->
        I2C_Inputs = 0;                                 // I2C_Inputs nullen
        return;                                         // fertig
    }
    if (!I2C_initialized || I2C_InitRequest) return;
//  if (++count< 2 /* n ms */ ) return; else count=0;   // IOExtenderOP() wird mit 1024 Hz aufgerufen, arbeitet aber nur alle n ms

    if (ti_2ndHalf==99) {
        I2C_Error_count+=100000;                        // Fehler zählen (dieser tritt aber anscheinend nicht auf)
        valid=ti_2ndHalf=0;
        I2C_INIT(0);
    }
    if (valid) {                                        // Daten in rbuf valide?
        I2C_Inputs = (ti_rbuf[0]<<8) + 0;               // rbuf[0] enthält GPIOB
//      I2C_Inputs = (rbuf[1]<<8) + rbuf[0];            // 16-Bit Alternative, setzt Lesen von 2 Byte in DRV_I2C_Receive voraus!
        IOExtenderRotaryEval(I2C_Inputs>>13);           // sofortiges Auswerten der Rotary-Encoders
    }    
    i=0;
    tbuf[i++]=0x13;                 // Register=GPIOB (MCP23017 im Mode BANK=0=16Bit,SEQOP=0=enabled, Lesen/Schreiben wechselt also zwischen GPIOB und GPIOA)
    tbuf[i++]=I2C_LEDstate;         // B8..15 (-->Register=GPIOA)
    tbuf[i++]=I2C_LEDstate>>8;      // B0..7  (-->Register=GPIOB)
//mus(1);
    I2C_BufferHandle = DRV_I2C_Transmit ( I2C_Handle, devadr, &tbuf[0],i, NULL );   // danach gilt: Register=GPIOB
//mus(0);//-->11us
    ti_2ndHalf++;
//mus(1);
//      I2C_BufferHandle = DRV_I2C_Receive ( I2C_Handle, devadr, &rbuf[0],1, NULL );    // Lesen von GPIOB
//mus(0);//-->11us
//    }
    // HIER kein Warten mit I2C_WaitBusy() erforderlich, da im festen Takt >= 1ms. Und ja, wir sind im Millisekunden-Interrupt!

    if (I2C_BufferHandle==NULL) {       // die tatsächliche Fehlerbedingung!
        // Wenn diese auftritt, MUSS Close+DeInitialize+Initialize+Open ausgeführt werden, NICHT jedoch IOExtenderSetup(), da der MCP23017 einwandfrei arbeitet!
        I2C_Error_count++;              // Fehler zählen
        valid=ti_2ndHalf=0;
        // I2C-Treiber vollständig neu aufsetzen, da der Fehler dort auftritt. Der MCP23017 arbeitet einwandfrei, muss nicht initialisiert werden.
//mus(1);
        I2C_INIT(0);                    // Ausführung sofort (trotz Timer1_MilliSekunden-Interrupt Kontext), da Dauer nur ca. 3us
//mus(0);// --> 3us
    } else {
        valid=1;
    }
}

// *****************************************************************************
// Wird von main() einmalig aufgerufen
// *****************************************************************************
// 02.10.2017
// 28.09.2017 Erstellt HR
// *****************************************************************************
void MF40_Init(void)
{
char s[10];
    // ----- TIMER 2 : Free running with 40MHZ PBCLK, Prescaler=16 --> 2.5 MHz
    PLIB_TMR_Period16BitSet(TMR_ID_2, 8000);   // Period = 8000 --> PWM-Frequenz 312.5 Hz,
                                               // 100% --> 15V==V150
    // ------ Ripple(max@15V/2): -------------- Einschwingzeit (auf 90%):-------                    
    // 5mV~rms  mit 100K+100nF + 470K+100nF     200ms
    // 20mV~rms mit 100K+100nF + 100K+100nF     80ms    <FAVORIT>
    // 50mV~rms mit 470K+100nF                  400ms? (nicht gemessen)
    // ca.150mV~rms mit 100K+100nF              80ms?  (nicht gemessen)
    // Hinweis: Funktioniert nur mit TLV2372 mit "Vorstufe". Da diese jedoch träge ist, darf 
    // PWM-Frequenz nicht höher als ca. 300 Hz sein, sonst werden oben und unten Werte nahe der Grenzwerte
    // auf 15V oder 0V abgebildet (bei 400 Punkten getestet, bereits bei 600Hz fehlen oben und unten 2 Punkte)
    // Mit dem PWM Widerstand kann der Ripple ohne Probleme eingestellt werden, der Zwang zu 
    // kleineren Werten (~50K), wie dies bei Verwendung mit dem LM358 notwendig ist, besteht nicht.
    // Beim LM358 musste die PWM-Frequenz auf ca. 18 KHZ erhöht werden (Prescaler=1), um mit 47K zu arbeiten.
    // Es lässt sich mit ihm auch nur 0..10V realisieren (bei Versorgung mit 15V), auf 15V hoch
    // kommt er nicht (max. 13.2V). Mit dem Nullpunkt sieht es auch nicht rosig aus, hier kommt er
    // nicht unter 0.07 V (der TLV schafft es bis 0.000 V). Ripple (max@10V/2) = 10mV~rms mit 47K + 100nF
    // Aber: zur Reduzierung des Ripple mit TLV2372 auf einen vernünftigen Wert (z.B. 20mV~rms), ohne 
    // die Einschwingzeit zu verschlechtern, ist ein zweistufiger Filter notwendig!
    PLIB_TMR_Start(TMR_ID_2);
    
    // ----- OC 2 : PWM-1
    PLIB_OC_Enable(OC_ID_4);
    PLIB_OC_PulseWidth16BitSet(OC_ID_4, 0);     // 0V Ausgabe
    // ----- OC 3 : PWM-2
    PLIB_OC_Enable(OC_ID_3);
    PLIB_OC_PulseWidth16BitSet(OC_ID_3, 0);     // 0V Ausgabe
    
    // ----- I2C 0
    I2C_Handle = DRV_I2C_Open(DRV_I2C_INDEX_0, DRV_IO_INTENT_READWRITE);
    DRV_I2C_BufferEventHandlerSet(I2C_Handle, I2C_Callback, 0); // Sinnlos! Callback wird zwar aufgerufen, aber nicht nutzbar,da anscheinend nicht aus Interrupt aufgerufen wird.
    IOExtenderSetup();
            
    // ----- ADC
    ADCsetup();
    
    // -----------------------------------------
    OUT_EIN_Control(0);             // OUT_EIN nicht aktiv (Vorgabe Latch=0 bei PowerOn)
    OUT_RESET_Control(1);           // OUT_RESET aktiv (kurz nach PowerOn), da Vorgabe Latch=0 bei PowerOn
    ResetTimer=ResetTimerSET;       // Reset für 0.7s anstehen lassen
    
    // ----- RTC RX-4035LC ---------------------
    RTCreadn(s,0xF,1);              // Lese Control-2 Register (0xF) in s[0]
    if (s[0]&B4) {                  // PON gesetzt? (Test ab V2.02.08)
        // PON ist gesetzt --> RTC-Init muss durchgeführt werden:
        s[2]=0x00+B7;s[1]=0x00;s[0]=0x00;   // 00:00:00     (24h)
        s[3]=0;                             // DoW=0
        s[4]=0x01;s[5]=0x01;s[6]=0x20;      // 01.01.2020
        RTCwriten(s,0,7);
    }
    RTCwriteb(0xE, 0b00000011);     // CT2..CT0 = 011 = Pulse mode 1 Hz (50% duty)
    RTCwriteb(0xF, 0b00000000);     // Bank0 ...
}

// *****************************************************************************
int getRampZeitSum(int ramp)
// *****************************************************************************
{
int i, sum;
struct MF40_Userdata_Struct *UD = &MF40_Userdata;
    for (i=sum=0; i<UD->RP[ramp].RampPointsUsed; i++) {
        sum += UD->RP[ramp].RP[i].dt;
    }
    return sum;
}

// *****************************************************************************
int getRampTemp(int ars, int32 t10, int *PIDsetp)      // 20.11.2017, 19.01.2019
// *****************************************************************************
{
int i, dt, m, temp,temp0;
int t,tsum,tsum0;
struct MF40_Userdata_Struct *UD = &MF40_Userdata;
    t = t10/10;         // Rampenzeit [s] ab 0 (komplette 0. Sekunde)
    // Punkt bestimmen
    for (i=tsum=temp=0; i<UD->RP[ars].RampPointsUsed; i++) {
        tsum0=tsum;
        tsum += (dt=UD->RP[ars].RP[i].dt);
        temp0=temp;
        temp=UD->RP[ars].RP[i].Temp;
        if (t<tsum) {
            *PIDsetp = UD->RP[ars].RP[i].PIDset;
            if (dt!=0) {
                m = ((temp-temp0)*10000) / dt; // Steigung berechnet
                return (temp0 + (((t-tsum0)*m)/10000));
            } else {
                return (temp);
            }
        }
    }
    // kommt eigentlich nicht vor:
    *PIDsetp=0; return 0;
}


// *****************************************************************************
// Wird von main() zyklisch aufgerufen
// *****************************************************************************
// 04.10.2018 PID Regelung und Temperatur-Simulation
// 09.11.2017 I2C_InitRequest
// 26.09.2017 Erstellt HR
// *****************************************************************************
void MF40_Eval(void)
{
int i, j;
uint32 leds;
uint32 tm;
int nzsReg=0;
int tempi;
static int first=1;
static int Regelungsbeginn=1;
static int StdBy=0;
static int last_WorkMode=0;
static int ExtStart=0;
static int TimerDelayed=0;

    if (first) {
        first=0;
        MF40_Setup_Read();
        MF40_Userdata_Read();
        MF40_Lifedata_Read();
        SIM_Init();
    }
    // .........................................................................
    leds = inputs;
//  if (leds&B0) leds &= 0xfffb;					// Bei RUN --> StdBy=0
    LED_inputs = (leds&(B0+B14+B15)) + ((leds&B2)>>1) + ((leds&(B9+B10+B11+B12+B13))>>6) + ((leds&(B3+B4+B5+B6+B7+B8))<<5);
    // 0  2  -1 9  10 11 12 13 3  4 5  6  7  8  14 15 : Abbildung
    // 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 : LED Index 0..15
    // .........................................................................
    // Hinweise:
    // - Mit LED_val() wird der entprellte LED-Wert, basierend auf LED_inputs, mit 1024 Hz entprellt, gelesen
    // - "RUN --> StdBy=0" wird ab 11.10.2018 hier nicht mehr angewendet, da !StdBy --> STOP TIMER
    // .........................................................................
    if (NeueZehntelSekunde_MF40) {  // synchron zur 1/1024s Zeitbasis
        NeueZehntelSekunde_MF40=0;
        nzsReg++;
        // >0 --> zählt runter bis 0, <0 --> zählt weiter runter (wird aber hochzählend interpretiert)
        if (MF40_RUN10) {
            if (MF40_RUN10>0) if (TimerDelayed) {
                MF40_RUN10++;
            }
            if (!--MF40_RUN10) {
                OUT_EIN_Control(0);
                MF40_BlockWM=15;            // 1.5s blockieren (falls RUN-LED nicht gleich ausgehen sollte)
                MF40_WorkMode=0;
            }
            MF40_RUNTIME10++;
        } else {
            MF40_RUNTIME10=0;
        }
        // TIMER globalisieren in float
        if (MF40_RUN10<0) MF40_Ist_Timer = (float)(-MF40_RUN10) * 0.1;
        else              MF40_Ist_Timer = (float)(MF40_RUN10) * 0.1;

        if (LED_StdBy) StdBy=3; else if (StdBy) StdBy--;    // 0.3s LED_StdBy halten
        // .....................................................................
        // OUT_RESET (für Steuerkarte) steuern
        if (ResetTimer) {ResetTimer--; OUT_RESET_Control(1);}
        else            {OUT_RESET_Control(0);}
        // .....................................................................
        if (MF40_BlockWM) MF40_BlockWM--;
        // .....................................................................
        if (ADCval32(5)<200) {        // 0 -> ExtStart++ / 797 -> ExtStart=0
            ExtStart++; 
        } else {
            ExtStart=0;
        }
        // .....................................................................
        // Änderung der Baudrate oder Parity prüfen und ggf. Änderung globalisieren:
        if (Com1_Baudrate != MF40_Userdata.MB_Slave_Baud) {
            Com1_Baudrate = MF40_Userdata.MB_Slave_Baud;
        }
        if (Com1_Parity != MF40_Userdata.MB_Slave_Parity) {
            Com1_Parity = MF40_Userdata.MB_Slave_Parity;
        }
        // .....................................................................
        // Messungen globalisieren
        MF40_Ist_P=ANAval(Ana_P);
        MF40_Ist_I=ANAval(Ana_I);
        MF40_Ist_Freq=ANAval(Ana_Freq);
        MF40_Ist_cosPhi=ANAval(Ana_cosPHI);
    }
    // .........................................................................
    if (NeueSekunde_MF40) {
        NeueSekunde_MF40=0;
        MF40_Lifedata.OprTime++; MF40_Lifedata.OprTime_1++;
        if (MF40_WorkMode)  {MF40_Lifedata.OprTimeGenerator++; MF40_Lifedata.OprTimeGenerator_1++;}
        else                {if (last_WorkMode) MF40_Lifedata_Save();}
        last_WorkMode = MF40_WorkMode;
    }
    // .........................................................................
    // SCADA Befehlsauswertung
    SCADA_STOP = SCADA_START = 0;
    SCADA_StartCmd = SCADA_Cmd0&0xFF;           // B0..B7
    if (SCADA_StartCmd_last!=SCADA_StartCmd) {
        switch (SCADA_StartCmd) {
            case 0:     SCADA_STOP=1; break;
            case 0x81:  SCADA_START=1; break;   // Start
            case 0x82:                          // Start Ramp
                        if (!MF40_Setup.RAMP_Option) break;
                        if (i=((SCADA_Cmd0&0x0F00)>>8)) MF40_Userdata.ars = i-1;
                        SCADA_START=2; 
                        break;
            case 0xC0:  ResetTimer=ResetTimerSET; break;    // RESET
        }
        SCADA_StartCmd_last = SCADA_StartCmd;
    }
    // .........................................................................
    if ((STOP_Taster_ms>5) || ResetTimer || !StdBy || TempStop() || SCADA_STOP || MF40_Temp_Error) {    // MF40_Temp_Error Beachtung ab V2.02.05 06.03.2020
        if (MF40_WorkMode!=MF40_ExtRun) {
            MF40_RUN10 = 0;
            OUT_EIN_Control(0);
            MF40_BlockWM=5;                                 // 0.5s blockieren (falls RUN-LED nicht gleich ausgehen sollte)
            MF40_WorkMode=0;
//          if (STOP_Taster_ms>200) {
//              if (ExtStart>) ResetTimer=ResetTimerSET;    // RESET wenn ExtStart gedrückt
////            for (i=j=0;i<16;i++) j+=LED_val((i==2)?0:i);// funktioniert noch nicht
////            if (!j) ResetTimer=ResetTimerSET;           // RESET wenn alle LEDs dunkel !! NICHT NOTWENDIG LAUT KAI !!
//          }
        } else if (MF40_WorkMode==MF40_ExtRun) {
            if (StdBy) ResetTimer=ResetTimerSET;            // zusätzlich abhängig von StdBy ab V2.03.01
//          ResetTimer=ResetTimerSET;                       // bis V2.03.00
            OUT_EIN_Control(0);
            MF40_WorkMode=0;
        }
        if (STOP_Taster_ms>500) SIM_Init();
    } else {
        if ( ((START_Taster_ms>5)&&(START_Taster_ms<50)) || // Vermeiden, dass bei gedrücktem Starttaster und abgelaufener Zeit der Vorgang wiederholt wird
              SCADA_START) {                                // oder SCADA_START
            if (StdBy && (MF40_Setup.DisplaySelect&B5)) {   // StdBy muss gelten und TIMER Feld muss sichtbar sein!
                if (MF40_WorkMode!=MF40_ExtRun) {           // Wenn Steuerung von außen, START ignorieren
                    if (!MF40_Temp_Error) {                 // bei Pyrometer-Drahtbruch (nur bei 4..20mA möglich) START verhindern
                        if (!MF40_RUN10) {                  // kein START, wenn bereits läuft
                            switch (MF40_WorkMode=getIntendedWorkMode(SCADA_START)) {
                                case  MF40_IntRun:
                                        MF40_Regel_Temp = (int)(MF40_Userdata.TEMP_SET+0.5);
                                        if (MF40_Userdata.TIMER_SET>0.05) MF40_RUN10 = (int32)((MF40_Userdata.TIMER_SET*10.0)+0.5); else MF40_RUN10=-1;
                                        if (MF40_RUN10>0) if (!SCADA_START) if (MF40_Userdata.TIMER_delayed) TimerDelayed=1;
                                        break;
                                case  MF40_IntRunRamp:
                                        MF40_RUN10=getRampZeitSum(MF40_Userdata.ars)*10;
                                        MF40_RunRamp_TempJump=1;
                                        break;
                            }
                            if (MF40_WorkMode) {
#ifdef UD_SAVE_ON_START                        
                                MF40_Userdata_Save(1);
                                MF40_Lifedata.GeneratorCycles++; MF40_Lifedata.GeneratorCycles_1++;
                                MF40_Lifedata_Save();
#endif                        
                                OUT_EIN_Control(1);
                            }
                        }
                    }
                }
            }
        }
    }
    // .........................................................................
    if (TEMPSETavl()) {                     // Temperaturmessung möglich?
        if (MF40_Setup.SIM_sim) {
            MF40_Ist_Temp = SIM_Temp;
            MF40_Temp_Error = 0;
        } else {
            MF40_Ist_Temp = ANAval(Ana_Pyro_1);
            MF40_Temp_Error = AnaTempErr;
            if (MF40_Ist_Temp<0) MF40_Ist_Temp = 0;
        }
        tempi = (int)(MF40_Ist_Temp+0.5);
    } else {
        MF40_Ist_Temp = 0;
        MF40_Temp_Error = 0;                // ab V2.02.05 06.03.2020
        tempi = 0;
    }
    // .........................................................................
    switch(MF40_WorkMode) {
        case 0:
            if (!MF40_BlockWM) {            // Blockierung nach letztem Timer beachten (0.5s)
                if (ExtStart) {             // nun ohne Verzögerung (ExtStart>=200ms ab V2.02.05 06.03.2020)
                    if (RUN_LED_ms>700) {
                        ResetTimer=ResetTimerSET; // RESET
                        break;
                    }
//              if ((ADCvalMean(5)<150)&&(!RUN_LED_ms)) {    // ExtStart#=0 ab V2.02.05 06.03.2020     
//              if (RUN_LED_ms>20) {        // bis V2.02.04
                    MF40_RUN10=-1;
                    OUT_EIN_Control(0);     // sicherstellen
                    MF40_WorkMode=MF40_ExtRun;
#ifdef UD_SAVE_ON_START                     // Speichern bei ExtRun-Start erst ab V2.02.00                      
                    MF40_Userdata_Save(1);
                    MF40_Lifedata.GeneratorCycles++; MF40_Lifedata.GeneratorCycles_1++;
                    MF40_Lifedata_Save();
#endif                        
                }
//              if (IN_Taster_ms>100) {
//                  MF40_RUN10=-1;
//                  OUT_EIN_Control(1);     // sicherstellen
//                  MF40_WorkMode=MF40_ExtRun;
//              }
            }
            MF40_Regel_PIDset = 0;
            break;
        case MF40_IntRun:
            MF40_Regel_Temp = (int)(MF40_Userdata.TEMP_SET+0.5);
            MF40_Regel_PIDset = 0;
            break;
        case MF40_IntRunRamp:
            MF40_Regel_Temp = getRampTemp(MF40_Userdata.ars, MF40_RUNTIME10, &MF40_Regel_PIDset);
            if (MF40_RunRamp_TempJump) {
                if ((MF40_Regel_Temp<tempi) && (MF40_RUN10>4)) {
                    MF40_RUNTIME10+=2; MF40_RUN10-=2;
                } else MF40_RunRamp_TempJump=0;
            }
            break;
        case MF40_ExtRun:
            if (!ExtStart) {                // ExtStart#=1 ab V2.02.05 06.03.2020     
//          if (!RUN_LED_ms) {              // bis V2.02.04
                MF40_RUN10=0;
                OUT_EIN_Control(0);         // sicherstellen
                MF40_WorkMode=0;
            }
//          if (!IN_TASTER) {
//              MF40_RUN10=0;
//              OUT_EIN_Control(0);         // sicherstellen
//              MF40_WorkMode=0;
//          }
            MF40_Regel_Temp = (int)(MF40_Userdata.TEMP_SET+0.5);
            MF40_Regel_PIDset = 0;
            break;
    }
    // .........................................................................
    if (TEMPSETavl()) {          // Regelung nur, wenn Temperaturmessung möglich
        if (MF40_WorkMode) {
            // Regelung erfolgt in jedem MF40_WorkMode
            // zuerst werden bei Regelungs-Beginn einmalig einige Faktoren ermittelt
            if (nzsReg) {
                if (Regelungsbeginn) {
                    Regelungsbeginn = 0;
                    PID_Init(MF40_Regel_PIDset);
                }
                PID_ChangeCheck(MF40_Regel_PIDset);
                MF40_Regel_Steller = PID_Regler(MF40_Regel_Temp, tempi);    // begrenzt auf 0..100 [%]
            }
    //      MF40_Regel_Steller = P_Regler(MF40_Regel_Temp, tempi);

            // ----- Steuerkarten-Abschaltung -----
            if (GENOFFopt) if (MF40_Userdata.PID[MF40_Regel_PIDset].GENOFFuse) {
                if (MF40_Regel_Steller < 1) {   // [%]
                    OUT_GENOFF_Control(1);      // Hinweis: wird mit nächstem OUT_EIN_Control() =0 gesetzt
                } else {
                    OUT_GENOFF_Control(0);
                }
            }
        } else {
            MF40_Regel_Steller = 0.0;
            Regelungsbeginn = 1;                // Vormerkung
        }
        if (MF40_WorkMode == MF40_IntRun) {
            if (TimerDelayed) {
                if (tempi >= MF40_Regel_Temp) TimerDelayed = 0;
            }
        } else TimerDelayed = 0;
    } else {
        // ansonsten Regel-Steller fest auf 100% oder bei Rampenbetrieb = MF40_Regel_Temp
        if (MF40_WorkMode==MF40_IntRunRamp) {
            i = MF40_Regel_Temp; if (i<0) i=0; else if (i>100) i=100;
            MF40_Regel_Steller = (float)i;
        } else {
            MF40_Regel_Steller = 100.0;
        }
        TimerDelayed = 0;
    }
    // .........................................................................
    if (MF40_Setup.SIM_sim) if (nzsReg) {
        SIM_E = SIM_E + (MF40_Userdata.P_SET*MF40_Regel_Steller*0.01*(MF40_Userdata.PID_Ta/3600))-((SIM_Temp-SIM_TAMB)*SIM_QKdegC*MF40_Userdata.PID_Ta);
        SIM_Temp = SIM_E / SIM_QDegC;
    }
    // .........................................................................
    // PWM Ausgabe
    PLIB_OC_PulseWidth16BitSet(OC_ID_4, (uint16)(MF40_Userdata.P_SET*(8000.0/fPSETmax())));   // PWM-1 : 0..PSETMAX [kW] --> 0..8000 (0..15V); Ausgang abhängig von Genauigkeit der +15V Versorgung
//    PLIB_OC_PulseWidth16BitSet(OC_ID_4, (uint16)(MF40_Userdata.P_SET*200.0)); // PWM-1 : 0..40.0 [kW] --> 0..8000 (0..15V); Ausgang abhängig von Genauigkeit der +15V Versorgung
    PLIB_OC_PulseWidth16BitSet(OC_ID_3, (uint16)(MF40_Regel_Steller*53.3));     // PWM-2 : 0..100.0 [%] --> 0..5333 (0..10V); Ausgang abhängig von Genauigkeit der +15V Versorgung
    // .........................................................................
    if (I2C_InitRequest) {
//tm=usVal();
        DRV_I2C_Close(DRV_I2C_INDEX_0);
        DRV_I2C0_DeInitialize();
        DRV_I2C0_Initialize();
        I2C_Handle = DRV_I2C_Open(DRV_I2C_INDEX_0, DRV_IO_INTENT_READWRITE);
//tm=usVal()-tm; if (tm>usTest) usTest=tm;// --> 3us
//      IOExtenderSetup();      // nicht notwendig, da der MCP23017 den Fehler nicht verursacht und weiterhin einwandfrei arbeitet
        I2C_InitRequest=0;
    }
}

// *****************************************************************************
int MF40_Setup_MF_HF(struct MF40_Setup_Struct * SU)             // 22.03.2020 HR
// *****************************************************************************
{
int i,j;
    // isMF40 wird aus der MF40_Setup_Struct ermittelt und danach
    // die davon abhängenden String/float/char Arrays vorbesetzt:
    isMF40 = SU->MF0_HF1 ? 0 : 1; 
    AnaSumCount = SU->ANA_Sample200ms ? 200 : 100;
    for (i=0;i<8;i++) {
        //  char  MG_name[8][10];
        strcpy(MG_name[i],isMF40?MG_name_mf[i]:MG_name_hf[i]);
        //  char  MG_unit[8][10];
        strcpy(MG_unit[i],isMF40?MG_unit_mf[i]:MG_unit_hf[i]);
        //  char  MG_val[8][12];
        strcpy(MG_val[i],isMF40?MG_val_mf[i]:MG_val_hf[i]);
        //  char  MG_fix[8];
        MG_fix[i] = isMF40?MG_fix_mf[i]:MG_fix_hf[i];
        //  float MG_fac[8];
        MG_fac[i] = isMF40?MG_fac_mf[i]:MG_fac_hf[i];
    }
    for (i=0;i<16;i++) {
        //  char  LED_nameON[16][10];
        strcpy(LED_nameON[i],isMF40?LED_nameON_mf[i]:LED_nameON_hf[i]);
        //  char  LED_nameOFF[16][10];
        strcpy(LED_nameOFF[i],isMF40?LED_nameOFF_mf[i]:LED_nameOFF_hf[i]);
        //  char  LEDpos[16];
        LEDpos[i] = isMF40?LEDpos_mf[i]:LEDpos_hf[i];
    }
    for (i=0;i<6;i++) {
        //  char  TabAna2MG[6];
        TabAna2MG[i] = isMF40?TabAna2MG_mf[i]:TabAna2MG_hf[i];
        //  char  TabAnaName[6][10];
        strcpy(TabAnaName[i],isMF40?TabAnaName_mf[i]:TabAnaName_hf[i]);
    }
}


// *****************************************************************************
int MF40_Setup_Save(int onchange)
// *****************************************************************************
{
int i, len, adr;
char *sp, *dp;
    if (onchange) {
        sp=(char*)&MF40_Setup; dp=(char*)&MF40_Setup_backup;
        for (i=0;i<sizeof(MF40_Setup);i++) {
            if (*(sp++)!=*(dp++)) goto SAVE;
        }
        return 0;
    }
SAVE:
    FLASHwriten((char*)&MF40_Setup, MF40_Setup_FLASH_Adr, sizeof(MF40_Setup));
    memcpy(&MF40_Setup_backup,&MF40_Setup, sizeof(MF40_Setup_backup));
    MF40_Setup_MF_HF(&MF40_Setup);
    return 1;
}
// *****************************************************************************
void MF40_Setup_Read(void)
// *****************************************************************************
{
    FLASHreadn((char*)&MF40_Setup, MF40_Setup_FLASH_Adr, sizeof(MF40_Setup));
    if (MF40_Setup_Default(&MF40_Setup, 0)) {
        // Bei Änderung der Magic oder bei einem Versionsupgrade werden
        // ab dem 06.02.2019 diese Änderungen sofort ins FLASH geschrieben:
        FLASHwriten((char*)&MF40_Setup, MF40_Setup_FLASH_Adr, sizeof(MF40_Setup));
    }
    memcpy(&MF40_Setup_backup,&MF40_Setup, sizeof(MF40_Setup_backup));
    MF40_Setup_MF_HF(&MF40_Setup);
}
// *****************************************************************************
void MF40_Setup_Restore(void)
// *****************************************************************************
{
    memcpy(&MF40_Setup,&MF40_Setup_backup, sizeof(MF40_Setup));
    MF40_Setup_MF_HF(&MF40_Setup);
}
// *****************************************************************************
int MF40_Setup_Default(struct MF40_Setup_Struct * SU, int force)
// *****************************************************************************
{
int i, r=0;
char *dp;
    if ((SU->Magic != MF40_Setup_Magic) || force) {
        r++;
        for(i=0,dp=(char*)SU; i<sizeof(MF40_Setup); i++) *(dp++)=0;
        SU->Magic = MF40_Setup_Magic;
        SU->Version=0;
        SU->RAMP_Option=0;
        SU->GENOFF_Option=0;
        SU->MODBUS=0;
        SU->MF0_HF1=0;
        SU->SIM_sim=0;
        SU->ExtTestDisp=0;
//        SU->PSETmal2=0;
//        SU->IISTmal2=0;
//        SU->FREQmal_5=0;
        SU->DisplaySelect=0xff;
        SU->NoRegLEDs=0;
        SU->PSETMAX=40;
        SU->PISTMAX=40;
        SU->IMAX=100;
        SU->FMAX=100;
        // Pyros sind nicht hier, sondern im User-Setup. Reserviert für CAL-Daten
        SU->Pyro_1_Cal.v[0]=0;
        SU->Pyro_1_Cal.v[1]=500;
        SU->Pyro_2_Cal.v[0]=0;
        SU->Pyro_2_Cal.v[1]=500;
        SU->MagicEnd = MF40_Setup_MagicEnd;
        
        MF40_Setup_MF_HF(SU);
    }
    return r;
}

// *****************************************************************************
int MF40_Userdata_Save(int onchange)
// ********************************************************************
{
int i, len, adr;
char *sp, *dp;
    if (onchange) {
        sp=(char*)&MF40_Userdata; dp=(char*)&MF40_Userdata_backup;
        for (i=0;i<sizeof(MF40_Userdata);i++) {
            if (*(sp++)!=*(dp++)) goto SAVE;
        }
        return 0;
    }
SAVE:
    FLASHwriten((char*)&MF40_Userdata, MF40_Userdata_FLASH_Adr, sizeof(MF40_Userdata));
    memcpy(&MF40_Userdata_backup, &MF40_Userdata, sizeof(MF40_Userdata_backup));
    return 1;
}
// *****************************************************************************
void MF40_Userdata_Read(void)
// *****************************************************************************
{
    FLASHreadn((char*)&MF40_Userdata, MF40_Userdata_FLASH_Adr, sizeof(MF40_Userdata));
    if (MF40_Userdata_Default(&MF40_Userdata, 0)) {
        // Bei Änderung der Magic oder bei einem Versionsupgrade werden
        // ab dem 06.02.2019 diese Änderungen sofort ins FLASH geschrieben:
        FLASHwriten((char*)&MF40_Userdata, MF40_Userdata_FLASH_Adr, sizeof(MF40_Userdata));
    }
    memcpy(&MF40_Userdata_backup, &MF40_Userdata, sizeof(MF40_Userdata_backup));
}
// *****************************************************************************
void MF40_Userdata_Restore(void)
// *****************************************************************************
{
    memcpy(&MF40_Userdata,&MF40_Userdata_backup, sizeof(MF40_Userdata));
}

// *****************************************************************************
int MF40_Lifedata_Default(struct MF40_Lifedata_Struct * SU, int force)  // 06.02.2019 HR - Erstellt
// *****************************************************************************
{
int i, r=0;
char *dp;
    if ((SU->Magic != MF40_Lifedata_Magic) || force) {
        r++;
        for(i=0,dp=(char*)SU; i<sizeof(MF40_Lifedata); i++) *(dp++)=0;
        SU->Magic = MF40_Lifedata_Magic;
        SU->MagicEnd = MF40_Lifedata_MagicEnd;
    }
    return r;
}
// *****************************************************************************
int MF40_Lifedata_Save(void)                            // 06.02.2019 HR - Erstellt
// *****************************************************************************
{
int i, len, adr;
char *sp, *dp;
    FLASHwriten((char*)&MF40_Lifedata, MF40_Lifedata_FLASH_Adr, sizeof(MF40_Lifedata));
    return 1;
}
// *****************************************************************************
void MF40_Lifedata_Read(void)                        // 06.02.2019 HR - Erstellt
// *****************************************************************************
{
    FLASHreadn((char*)&MF40_Lifedata, MF40_Lifedata_FLASH_Adr, sizeof(MF40_Lifedata));
    if (MF40_Lifedata_Default(&MF40_Lifedata, 0)) {
        // Bei Änderung der Magic oder bei einem Versionsupgrade werden
        // ab dem 06.02.2019 diese Änderungen sofort ins FLASH geschrieben:
        FLASHwriten((char*)&MF40_Lifedata, MF40_Lifedata_FLASH_Adr, sizeof(MF40_Lifedata));
    }
}

const uint16 pxDefaults[RampPointsMax]={10, 10, 5,  15, 5,  15, 20, 39};
const uint16 pyDefaults[RampPointsMax]={200,550,800,800,950,950,500,150};
// *****************************************************************************
void RampPointsInit(struct MF40_Userdata_Struct * UD)
// *****************************************************************************
{
int i,j;
char s[20];
    for (j=0;j<RampSetsMaxPlaned;j++) { // alle, auch geplante Rampen initialisieren
        for (i=0;i<RampPointsMax;i++) {
            UD->RP[j].RP[i].dt   = pxDefaults[i];
            UD->RP[j].RP[i].Temp = pyDefaults[i];
            UD->RP[j].RP[i].PIDset = 0;
        }
        UD->RP[j].RampPointsUsed = 8;
        sprintf(s,"RAMP M%i",j+1);
        strcpy(UD->RP[j].Ramp_Name,s);
    }
    UD->ars=0;
}

// *****************************************************************************
void PIDsetsInit(struct MF40_Userdata_Struct * UD)
// *****************************************************************************
{
int j;
    for (j=0;j<PIDsetsMaxPlaned;j++) { // alle, auch geplante PIDsets initialisieren
        UD->PID[j].Kp = 10.0;
        UD->PID[j].Ki = 5.0;
        UD->PID[j].Kd = 0;
        UD->PID[j].GENOFFuse = 0;
    }
}

// *****************************************************************************
void RampShift(int up)
// *****************************************************************************
{
int i, from, to;
char c, * sp, * dp;
struct MF40_Userdata_Struct *UD = &MF40_Userdata;
//uint16 RampPointsUsed;
//struct MF40_RampPointStruct RP[RampPointsMax];
//char   Ramp_Name[RampNameLen];
    from = UD->ars;
    to = (from - (up ? 1 : -1) + RampSetsMax) % RampSetsMax;
    sp = (char*)&UD->RP[from].RampPointsUsed;
    dp = (char*)&UD->RP[to].RampPointsUsed;
    for (i=0;i<sizeof(MF40_Userdata.RP[0]); i++) {
        c=*sp; *(sp++)=*dp; *(dp++)=c;
    }
    UD->ars = to;
//    RampPointsUsed = UD->RP[from].RampPointsUsed;
//    UD->RP[from].RampPointsUsed = UD->RP[to].RampPointsUsed;
//    strcpy(Ramp_Name, UD->RP[from].Ramp_Name);
//    strcpy(UD->RP[from].Ramp_Name, UD->RP[to].Ramp_Name);
//    strcpy(UD->RP[to].Ramp_Name, Ramp_Name);
//    for (i=0; i<RampSetsMax; i++) {
//    }
}

// *****************************************************************************
int MF40_Userdata_Default(struct MF40_Userdata_Struct * UD, int force)
// *****************************************************************************
// 09.10.2018 : UD->Version 1
// *****************************************************************************
{
int i, r=0;
char *dp;
    if ((UD->Magic != MF40_Userdata_Magic) || force) {
        r++;
        for(i=0,dp=(char*)UD; i<sizeof(MF40_Userdata); i++) *(dp++)=0;
        UD->Magic = MF40_Userdata_Magic;
        UD->Version=0;          // für weitere Upgrades bei 0 anfangen!
        UD->TIMER_SET=0;
        UD->P_SET=0.0;
        UD->TEMP_SET=100.0;
        RampPointsInit(UD);
    }
    // .........................................................................
    if (UD->Version<1) {
        r++;
        UD->Version=1;
        UD->P_SET = 40;
        for(i=0;i<sizeof(UD->spare2);i++) UD->spare2[i]=0;
        UD->Pyro_1_Sc.v[0]=50;
        UD->Pyro_1_Sc.v[1]=975;
        UD->Pyro_2_Sc.v[0]=0;
        UD->Pyro_2_Sc.v[1]=500;
        for(i=0;i<sizeof(UD->spare3);i++) UD->spare3[i]=0;
        PIDsetsInit(UD);
        UD->PID_Ta = 0.1;           // Wert nicht ändern!
        UD->TEMP_used = 1;          // 0:OFF, 1:0..20mA, 2:4..20mA
        UD->TEMP_fahrenheit = 0;    // 0=°C
        UD->TEMP_max = 950;
        UD->TEMP_stop = 0;          // 0:OFF
        UD->DisplayRoll = 1;
        UD->TIMER_max = 3600;
        UD->MB_Slave_Adr = 50;
        UD->MB_Slave_Baud = 57600;
        UD->MB_Slave_Parity = 0;    // 8-N-1
        for(i=0;i<sizeof(UD->spare4);i++) UD->spare4[i]=0;
    }
    // .........................................................................
    UD->MagicEnd = MF40_Userdata_MagicEnd;
    return r;
}

// *****************************************************************************
// 6 Kanäle: Lese ADC Buffer von kan 0..5 (erstes Sample oder 6..11 (zweites Sample)
// 8 Kanäle: Lese ADC Buffer von kan 0..7 (erstes Sample oder 8..15 (zweites Sample)
// *****************************************************************************
// 02.10.2017 Erstellt HR
// *****************************************************************************
uint32 ADCbuf32(int kan)
{
    if (kan>=16) return 0;
    return (*(uint32*)((uchar*)&ADC1BUF0 + (0x10*kan)));
/*  switch(kan){
        case 0: return ADC1BUF0;
        case 1: return ADC1BUF1;
        case 2: return ADC1BUF2;
        case 3: return ADC1BUF3;
        case 4: return ADC1BUF4;
        case 5: return ADC1BUF5;
        case 6: return ADC1BUF6;
        case 7: return ADC1BUF7;
    } */
}

// *****************************************************************************
// Lese 2-fach gemittelten ADC Buffer Wert 0..1023 von kan 0..7
// *****************************************************************************
// 19.06.2020 Wieder zurück von 6 auf 8 Kanäle (fix) wegen Übersprechen von 
//            ANA-6 auf ANA-1 im 6 Kanalbetrieb. ANA-7+8 werden nicht verwendet.
// 07.06.2018 Umstellung von 8 --> 6 Kanäle (AnaMax)
// 02.10.2017 Erstellt HR
// *****************************************************************************
uint32 ADCval32(int kan)
{
    if (kan>=AnaMax) return 0;
//  return ((ADCbuf32(kan)+ADCbuf32(kan+AnaMax))/2);
//  return ((ADCbuf32(kan)+ADCbuf32(kan+7))/2);
    return ((ADCbuf32(kan)+ADCbuf32(kan+8))/2);
}
// *****************************************************************************
// Lese  ADC Mittel-Wert 0..1023 von kan 0..7
// *****************************************************************************
// 02.10.2017 Erstellt HR
// *****************************************************************************
uint32 ADCvalMean(int kan)
{
    if (kan>=AnaMax) return 0;
    return ((ANA_MEAN[kan]+5)/10);
}

// *****************************************************************************
// Lese gemittelten, normierten(vorskalierten) Analog-Wert von kan 0..7
// --------------------------------------------------
// kan  Bezeichnung         Bereich genutzt    return
// --------------------------------------------------
// 0    ANA-1   P           0..15V  0..10V --> 0..150.0
// 1    ANA-2   I           0..15V  0..10V --> 0..150.0
// 2    ANA-3   cos_PHI     0..15V  0..10V --> 0..150.0
// 3    ANA-4   Freq        0..15V  0..10V --> 0..150.0
// 4    ANA-5   Pyro_1      0..10V/0..20mA --> 0..100.0
// 5    ANA-6   P_Reg_Ext   0..15V  0..10V --> 0..150.0
// *****************************************************************************
// 07.06.2018 8-->6 Kanäle
// 09.11.2017 Erstellt HR
// *****************************************************************************
float ANAvaln(int kan)
{
uint32 ul;
float ff;
    if (kan>=AnaMax) return 0.0;
    ul = ANA_MEAN[kan];     // 0..10230 (gemittelt über AnaSumCount Schleifen, Faktor x10)
    // ----- Nullwert erzwingen wenn ADC <= 2
    if (ul<=20) ul=0;	
	// ----- Vor-Skalieren
	// VREF = AVD (3.3V, max. 2% Gesamtfehler)
    // Widerstandsteiler:           0..15V UIN --> 0..2.991V    0..618..927 (0..10V..15V)
    // Widerstandsteiler Pyrometer: 0..10V UIN --> 0..2.994V    0..928      (0..10V/20mA)
    
    if (kan==Ana_Pyro_1) {
        ul *= 100;
        ul /= 928;      // 0..1000
    } else {
        ul *= 150;
        ul /= 927;      // 0..1500
    }
    return (((float)ul)/10.0);
}
// *****************************************************************************
float ANAval(int kan) // Lese skalierten Analog-Wert von kan 0..7 (ANA-1..ANA-8)
// *****************************************************************************
// 05.02.2019 4..20mA mit Option MF40_Userdata.TEMP_used==2
// 09.11.2017 Erstellt HR
// *****************************************************************************
{
float ana, a,b;
    if (kan>=AnaMax) return 0.0;
    if (kan==Ana_Pyro_1) {
        // 0..100.0
        a = (float)MF40_Userdata.Pyro_1_Sc.v[1] - (b=(float)MF40_Userdata.Pyro_1_Sc.v[0]);
        ana = ANAvaln(kan);
        if (MF40_Userdata.TEMP_used==2) {
            // 4..20mA (20..100)
            AnaTempErr = (ana<15)?1:0;  // bei <3mA Drahtbruchfehler
            ana = ((ana-20.0) * a * (1.25 / 100.0)) + b;
//          oder explizit:
//          ana = (ana-20.0)*1.25;
//          ana = (ana * a / 100.0) + b;
        } else {
            // 0..20mA (0..100))
            AnaTempErr = 0;             // keine Drahtbrucherkennung
            ana = (ana * a / 100.0) + b;
        }
    } else {
        // 0..150.0 (0..15V), verwendet wird nur 0..100.0 (0..10V)
        ana = ANAvaln(kan) * MG_fac[TabAna2MG[kan]];
    }
    switch(kan) {                                                  // fac Nom Unit
        case Ana_P:         ana *= MF40_Setup.PISTMAX*0.01; break; //  1  100 [kW]
        case Ana_I:         ana *= MF40_Setup.IMAX*0.01; break;    //  1  100 [A]
        case Ana_Freq:      ana *= MF40_Setup.FMAX*0.01; break;    //  1  100 [kHz]
        case Ana_P_Reg_Ext: ana *= fPSETmax()*0.01; break;         //  1  100 [kW]
    }
    return ana;
}

// *****************************************************************************
int PSETmax10(void)                        // 12.11.2017 Erstellt HR, 21.01.2019
// *****************************************************************************
{
int val;
    val=10*MF40_Setup.PSETMAX;
    if (val<40) val=36;
    return val;
}
// *****************************************************************************
float fPSETmax(void)                                   // 21.01.2019 Erstellt HR
// *****************************************************************************
{
int val;
    val=10*MF40_Setup.PSETMAX;
    if (val<40) val=36;
    return ((float)val*0.1);
}
// *****************************************************************************
int TEMPSETavl(void)                                  // 05.02.2019 Erstellt HR
// *****************************************************************************
{
    if (!(MF40_Setup.DisplaySelect&B7)) return 0;
    if (!MF40_Userdata.TEMP_used) return 0;
    return 1;
}
// *****************************************************************************
int TEMPSETmax(void)             // Stand: 05.02.2019 HR, 12.11.2017 HR Erstellt
// *****************************************************************************
{
    return (TEMPSETavl() ? MF40_Userdata.TEMP_max : 100);
}

// *****************************************************************************
// ADC Setup
// Auto-Scan der Kanäle AN 4..11 (ANA-1..ANA-8) zweimal hintereinander, das
// Ergebnis (32-Bit unsigned) liegt im ADC1BUF 0..8 (erster Scan) 
// und ADC1BUF 9..F (zweiter Scan) vor (wird permanent refreshed)
// Zeitbedarf für das Scannen dieser 2x8 Kanäle: 96us == 10416 Hz
// *****************************************************************************
// 19.03.2020 Zurück von 6 auf 8 Kanäle (zweimal 8 hintereinander), da ANA-6 
//            ANA-1 etwas beeinflusste (1..2 digits).
//            Hinweis: ANA-7+8 werden aktuell nicht verwendet.
// 07.06.2018 Umstellung von 8 auf 6 Kanäle (zweimal 6 hintereinander)
// 02.10.2017 Erstellt HR
// *****************************************************************************
void ADCsetup(void)
{
    //          M-S--FRMsrcC-ASD
    AD1CON1 = 0b0000010011100100UL;                     // ADC noch OFF
    //          VRFo-C--F-SMPIbA
//  AD1CON2 = 0b0000010000101100UL;                     // 2x6=12 Samples --> SMPI=0b1011
//  AD1CON2 = 0b0000010000110100UL;                     // 2x7=14 Samples --> SMPI=0b1101
    AD1CON2 = 0b0000010000111100UL;                     // 2x8=16 Samples --> SMPI=0b1111
    //          C--SAMC.adcs....
    AD1CON3 = 0b0000001100000111UL;                     // SAMC=0b00011=3xTAD
                                                        // ADCS=0b00000111=16xTPB-->25ns*16=400ns=TAD=2.5MHZ
                                                        // 1 Converion: 3xTAD + (2+10)xTAD = 15 TAD = 6us --> 166KSamples/s
                                                        // 2x6 Conversions je 6 us benötigen also 72us == 13888 Hz
                                                        // 2x8 Conversions je 6 us benötigen also 96us == 10416 Hz
    //          N---PPPPA---pppp----------------
    AD1CHS  = 0b00000000000000000000000000000000UL;
    //         31-------------------BA9876543210        
//  AD1CSSL = 0b00000000000000000000001111110000UL;     // AN 4..9  (ANA-1..ANA-6)
//  AD1CSSL = 0b00000000000000000000011111110000UL;     // AN 4..10 (ANA-1..ANA-7)
    AD1CSSL = 0b00000000000000000000111111110000UL;     // AN 4..11 (ANA-1..ANA-8)

    AD1CON1 |= B15;                                     // ADC ON !
}

// *****************************************************************************
// Zustand einer der 16 MF40-LEDs abfragen
// x : 0-based LED-Pos 0..15
// *****************************************************************************
// 13.11.2017 REG-LEDs werden nun von oben nach unten priorisiert (I-REG..FREQ),
//            es kann nun nur die oberste der aktiven REG-LEDs leuchten.
// 26.09.2017 Erstellt HR
// *****************************************************************************
int LED_val(int x)
{
register uint i;
    if ((x>=3)&&(x<=7)) {
        // x bezieht sich auf eine der REG-LEDs 3,4,5,6,7
        for (i=3;i<=7;i++) {        // alle REG-LEDs, beginnend mit oberster (I-REG)
            if (LED_dbc[i]>LEDminTime) {
                if (x>i)  return 0; // es gibt eine aktive höhere REG-LED --> 0
                if (x==i) return 1; // das ist sie
            }
        }
        return 0;                   // keine der REG-LEDs aktiv --> 0
    }
    // alle anderen LEDs ohne Sonderbehandlung
    return ( (LED_dbc[x]>LEDminTime) ? 1 : 0 );
//    return ( (LED_inputs&(1<<x)) ? 1 : 0 );
//uint32 leds;
//    leds = inputs;
//    if (leds&B0) leds &= 0xfffb;						// Bei RUN --> StdBy=0
//    return ((leds&(1<<(LEDpos[x]-1))) ? 1 : 0);
}
// *****************************************************************************
void LED_Debounce(void) // Aufruf mit 1024Hz von MF40_MilliSekunden, Cycle: 16*1/1024s
// *****************************************************************************
// 13.11.2017 REG-LED Priorisierung
// 26.09.2017 Erstellt HR
// *****************************************************************************
{
register int j, holdtime;
static uint i=15;
    if (++i>15) i=0;
    // REG-LEDs haben neben der priorisierenden Sonderbehandlung auch eine längere holdtime:
    if ((i>=3)&&(i<=7)) holdtime=LEDholdTimeReg; else holdtime=LEDholdTime;
    j = LED_dbc[i];
    if (LED_inputs&(1<<i)) {
        if (++j > LEDminTime) j = holdtime;
        LED_dbc[i] = j;
    } else {
        if (j) LED_dbc[i]--;
    }
}
// -----------------------------------------------------------------------------
// Erzeuge Bitfeld mit 16 LEDs B0..B15
// B0                                      B7
// RUN StdBy RESET I-REG U-REG-B-REG P-REG F-REG
// B8                                      B15
// I>  FI    PHI   Id>   T>    H2O   Uc>   EXT
// -----------------------------------------------------------------------------
// 09.02.2019 HR - Erstellt
// -----------------------------------------------------------------------------
uint LED_status16(void)
{
int i;
uint led;
char c;

    led=0;
    for (i=0; i<16; i++) {
        if (MF40_Setup.NoRegLEDs && ((i>=3)&&(i<=7))) continue;
        if (i==1) if (LED_RUN) continue;                // "RUN --> StdBy=0" Funktionslität
        if (!LED_RUN) if ((i>=3)&&(i<=7)) continue;     // REG-LEDs nur bei RUN anzeigen
        if ((c=LED_nameON[i][0])==LED_nameOFF[i][0]) {  // LED stets gleicher (Farb-)Modus?
            switch(c) {
                case 0: case ' ':                       // nicht vorhanden --> 0
                case '_':   continue;                   // ausgegraut      --> 0
                case 's':   if (!ResetTimer) continue;  // Reset-Spezial   --> ResetTimer
                default:    led |= 1<<i; continue;      // sonst           --> 1
            }
        }
        if (LED_val(i)) led |= 1<<i;
    }
    return led;
}

// *****************************************************************************
// Steuerung/Abfrage von OUT_EIN:
//  val>0 --> LED=1=ON, val==0 --> LED=0=OFF
//  val<0 return LED
// *****************************************************************************
// 05.10.2018 OUT_GENOFF_Control(0) Aufruf bei Zuweisung
// 26.09.2017 Erstellt HR
// *****************************************************************************
int OUT_EIN_Control(int val)
{
    if (val>=0) {
        if (val) {
            SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_3);
            MF40_OUT_EIN = 1;
        } else {
            SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_3);
            MF40_OUT_EIN = 0;
        }
        OUT_GENOFF_Control(0);
    }
    return MF40_OUT_EIN;
}
// *****************************************************************************
// Steuerung/Abfrage von OUT_GENOFF:
//  val>0 --> LED=1=ON, val==0 --> LED=0=OFF
//  val<0 return LED
// *****************************************************************************
// 05.10.2018 Erstellt HR
// *****************************************************************************
int OUT_GENOFF_Control(int val)
{
    if (val>=0) {
        if (val) {
            SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_10);
            MF40_OUT_GENOFF = 1;
        } else {
            SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_10);
            MF40_OUT_GENOFF = 0;
        }
    }
    return MF40_OUT_GENOFF;
}

// *****************************************************************************
// Steuerung/Abfrage von OUT_RESET:
//  val>0 --> OUT_RESET=1=kein_Reset, val==0 --> OUT_RESET=0=Reset
//  val<0 --> return LED
// *****************************************************************************
// 07.06.2018 RG8 --> RB11
// 26.09.2017 Erstellt HR
// *****************************************************************************
int OUT_RESET_Control(int val)
{
static int OUT_RESET=0;
    if (val>=0) {
        if (val) {
            SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_11);
//          SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_8);
            OUT_RESET = 1;
        } else {
            SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_11);
//          SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_8);
            OUT_RESET = 0;
        }
    }
    return OUT_RESET;
}

// *****************************************************************************
int BCD2i(char bcd)
// *****************************************************************************
{
    return ((((bcd&0xf0)>>4)*10) + (bcd&0xf));
}

// *****************************************************************************
char getParityChar(int parity)                         // 05.02.2019 HR Erstellt
// *****************************************************************************
{
    switch (parity) {
        case 0: return 'N';
        case 1: return 'O';
        case 2: return 'E';
    }
}

// *****************************************************************************
char * getParityStr(int parity)                        // 12.02.2019 HR Erstellt
// *****************************************************************************
{
    switch (parity) {
        case 0: return "8-N-1";
        case 1: return "8-O-1";
        case 2: return "8-E-1";
    }
}

// *****************************************************************************
uint16 getSCADA_Status0(void)                          // 12.02.2019 HR Erstellt
// *****************************************************************************
{
uint16 ui16;    
    ui16 = 0;
    ui16 |= MF40_WorkMode&0x7;                          // B0..B2:Workmode
    if (MF40_Setup.RAMP_Option) {                       // 
        ui16 |= ((MF40_Userdata.ars+1)&0xf)<<4;         // B4..B7:RampSelected (0=NoRamp,1..6)
        ui16 |= (MF40_Regel_PIDset&0x7)<<8;             // B8..B10:PID-Set     (0=B,1..4)
    }     
    if (MF40_Setup.GENOFF_Option) ui16 |= B11;          // B11:GenOffAvailable
    if (TEMPSETavl()) {ui16 |=B12;                      // B12:TmpAvl
        if (MF40_Temp_Error) ui16 |= B13;               // B13:TmpErr (nur bei TmpAvl)
    }
    return ui16;
}

// *****************************************************************************
int TempStop(void)                                                 // 20.01.2019
// *****************************************************************************
{
int stop;
    if (stop=MF40_Userdata.TEMP_stop) {
        if ((int)(MF40_Ist_Temp+0.5)>=stop) return 1;
    }
    return 0;
}

// *****************************************************************************
// PID Regler
// http://rn-wissen.de/wiki/index.php/Regelungstechnik
// *****************************************************************************
// Aufruf im festen Zeitraster PID_Ta [s]
// int w : Sollwert
// int x : Istwert
// return: 
// float y : Stellgröße 0..100
// *****************************************************************************
// 19.03.2020 Verfahrensbeschreibung zur Wind-Up Vermeidung:
//            Integrator wird gelöscht, solange dE * Kp > 100 --> Wind-Up Vemeidung beim Hochfahren
//            Integrator großzügig begrenzen auf 2000 (2s @ dT=100°C)
//            Integrator nicht unter 0 laufen lassen, da Energie nur gepusht, jedoch nicht gepullt werden kann (P stets >= 0)
// 18.03.2020 Neues Verfahren zur Wind-Up Vermeidung begonnen
// 06.03.2020 Windup zunächst entfernt
// 04.10.2019 Beachtung von PID_Iactive()
// 04.10.2018 Finalisierung, Umstellung auf int Input und Verbundfaktoren
// 03.10.2018 Optimierung und Simulation
// 02.10.2017 Erstellt HR
// *****************************************************************************
float PID_Regler(int w, int x)
{
int e;
float y, kp_e;

    e = w - x;                              // Vergleich
    kp_e = (PID_Kp*(float)e);               // nur zur Optimierung
    
    if (PID_Iactive()) {
        PID_esum += e;                          // Integration I-Anteil
    } else {
        PID_esum=0;                             // bei inaktivem I-Anteil Integrator löschen --> I-Anteil wirkt sich nachfolgend nicht aus
    }
    
//  MF40_Setup.PID_Mode = PIDmode_V3;           // ab V2.03.02 gilt stets PIDmode_V3, da sich dieses neue Verfahren seit 18.03.2020 bewährt hat.
    // Wind-Up Vermeidung, Verfahren ab 18.03.2020:
//  if (MF40_Setup.PID_Mode==PIDmode_V3) {
        if (fabs(kp_e)>100.0) PID_esum=0;       // Integrator wird gelöscht, solange dE * Kp > 100 --> Wind-Up Vemeidung beim Hochfahren
        if (PID_esum > 2000) PID_esum=2000;     // Integrator großzügig begrenzen auf 2000 (2s @ dT=100°C)
        if (PID_esum<0) PID_esum=0;             // Integrator nicht unter 0 laufen lassen, da Energie nur gepusht, jedoch nicht gepullt werden kann (P stets >= 0)
//  }    
    // Reglergleichung:
    y = kp_e + PID_Ki_m_Ta*(float)PID_esum + PID_Kd_d_Ta*(float)(e-PID_ealt);     
//  y = PID_Kp*(float)e + PID_Ki_m_Ta*(float)PID_esum + PID_Kd_d_Ta*(float)(e-PID_ealt);     
    PID_ealt = e;
//  if (MF40_Setup.PID_Mode==PIDmode_V3) {
        if (y>100.0) {y=100.0;}                 // Ausgang-Begrenzung auf <=100.0
        else if (y<0.0){y=0.0;}                 // Ausgang-Begrenzung auf >=0.0
//  }
//    if (MF40_Setup.PID_Mode==PIDmode_V2) {
//        if (y>100.0) {y=100.0; PID_esum=0;}     // Begrenzung und Wind-up Vermeidung
//        else if (y<0){y=0.0; PID_esum=0;}       // Begrenzung und Wind-up Vermeidung
//    }
    return y;
}
// *****************************************************************************
void PID_Init(int ps) 
{
    if (ps>=PIDsetsMax) ps=0;
    PID_esum = PID_ealt = 0;
    PID_Kp = MF40_Userdata.PID[ps].Kp;
    PID_Ki_m_Ta = MF40_Userdata.PID[ps].Ki * MF40_Userdata.PID_Ta;
    PID_Kd_d_Ta = MF40_Userdata.PID[ps].Kd / MF40_Userdata.PID_Ta;
    PID_last = ps;
}
// *****************************************************************************
void PID_ChangeCheck(int ps) 
{
    if (ps>=PIDsetsMax) ps=0;
    if (PID_last!=ps) {
        PID_Init(ps);
    }
}
// *****************************************************************************
// Überprüft, ob I-Anteil der Regelung deaktiviert werden soll (4.5V Pegel auf
// ANA-6, gemessener Wert = 267)
// Hinweis: bei älteren Geräten, die ANA-6 nicht zur Steuerung des I-Anteils
// verwenden, ist ANA-6=0 --> I-Anteil aktiv
// return:
//      0 : I-Anteil nicht aktiv
//      1 : I-Anteil aktiv (auch stets bei MF40_WorkMode==0)
// *****************************************************************************
// 22.06.2020 : ab V2.03.02 stets aktiv
// 04.10.2019 : erstellt
// *****************************************************************************
 int PID_Iactive(void)
{
    return 1;                       // ab V2.03.02 stets aktiv
/* bis V2.03.01
    if (!MF40_WorkMode) return 1;
    if (I2C_Inputs&B8) return 0;
    return 1;
*/
/* bis V2.02.04   
    if (ADCvalMean(5)>150) {        // 0..1023 von ANA-6
         return 0;                  // Prüfung auf >150 == >2.5V --> I-Anteil nicht aktiv
    } else {
         return 1;                  // <=150 == <=2.5V --> I-Anteil aktiv
    }
*/
}

// *****************************************************************************
// Ursprünglicher einfacher P Regler
// int w : Sollwert
// int x : Istwert
// return: 
// float y : Stellgröße
// *****************************************************************************
// 04.10.2018 Ausgelagert aus MF40_Eval())
// 02.10.2017 Erstellt HR
// *****************************************************************************
float P_Regler(int w, int x)
{
int i;    
    if (w > (i=x)) {
        i = w - i;
        if (i>20) return 100.0;             // [%]
        else      return (100.0 * ((float)i / 20.0));
    } else {
        return 0.0;
    }
}


// *****************************************************************************
void SIM_Init(void) 
{
    if (MF40_Setup.SIM_sim) {
        SIM_E = SIM_QDegC * SIM_TBEG;
        SIM_Temp = SIM_E / SIM_QDegC;
    }
}

// *****************************************************************************
// Prüft auf anstehendes Service-Intervall
// Die Prüfung findet nur statt, wenn SI!=0 und NoSI==0
// year  : 2020..2099 oder 20..99
// month : 1..12
//       : Sonderfall year=0,month==0 --> Zeit wird mit RTCreadn() ermittelt
// return: 1:Service steht an, 0:nicht
// *****************************************************************************
int CheckSI(int year, int month)
{
int si, ti, r=0;
char sd[18];
    si = MF40_Setup.SI;
    if (MF40_Setup.NoSI || !si) return 0;
    if (!year && !month) {
        RTCreadn(sd, 0, 7);     // BCD : 0:SS 1:MM 2:24HH 3:-- 4:DD 5:mm 6:YY
        year=BCD2i(sd[6])+2000;
        month=BCD2i(sd[5]);
    }
    if (year<100) year+=2000;
    ti = year*100+month;
//  return ti; // nur zum Testen
    if (ti>=si) r=1;
    return r;
}

// *****************************************************************************
int DevKeyOp(unsigned int ui)
{
int r=cmdErr_OK;
    switch(ui) {
        case 0x1174fa45:    MF40_Setup.RAMP_Option=1; r=cmdErr_OK_RAMP; goto WRITE;
        case 0x219ab643:    MF40_Setup.GENOFF_Option=1; r=cmdErr_OK_GENOFF; goto WRITE;
        case 0x3139ced7:    MF40_Setup.MODBUS=1; r=cmdErr_OK_MODBUS; goto WRITE;
        case 0x71f80ba9:    MF40_Setup.NoSI=1; r=cmdErr_OK_NO_SI; goto WRITE;
    }
    return cmdErr_AccessDenied;
WRITE:
    MF40_Setup.DEVKEY=0x00000000;       // neuer DEVKEY
    MF40_Setup_Save(0);                 // bedingungsloses Speichern des Factory-Setups
    return r;
}
// *****************************************************************************
int MF40_Interpreter(char * bp, int mode)
{
int i, r=cmdErr_OK;
unsigned int ui;
struct MF40_Lifedata_Struct *LD = &MF40_Lifedata;
    switch(mode) {
        case 0:
            if (!strcmpi(bp,"INITLIFE"))        {MF40_Lifedata_Default(LD, 1); MF40_Lifedata_Save(); goto Fertig;}
            if (!strcmpi(bp,"INITSERVICE"))     {LD->OprTime_1=LD->OprTimeGenerator_1=LD->GeneratorCycles_1=0; MF40_Lifedata_Save(); goto Fertig;}
            if (!strcmpi(bp,"INITUSER"))        {MF40_Userdata_Default(&MF40_Userdata, 1); MF40_Userdata_Save(0); goto Fertig;}
            if (!strncmpi(bp,"SN=",3))          {i=atoi(bp+3); if ((i<0) || (i>999999999)) {r=cmdErr_RangeError; goto Fertig;} MF40_Setup.SN=i; r=cmdErr_OK_SN; goto Fertig;}
            if (!strncmpi(bp,"SI=",3))          {i=atoi(bp+3); if ((i<202001) || (i>209912)) {r=cmdErr_RangeError; goto Fertig;} MF40_Setup.SI=i; r=cmdErr_OK_SI; goto Fertig;}
            if (!strncmpi(bp,"SPLASHONLY=",11)) {i=atoi(bp+11); if ((i<0) || (i>1)) {r=cmdErr_RangeError; goto Fertig;} MF40_Setup.SplashOnly=i; r=cmdErr_OK_SPLASHONLY; goto Fertig;}
            break;
        case 1:
            if (!strncmpi(bp,"CMD=",4))      {ui=xtoi(bp+4); r=DevKeyOp(ui); goto Fertig;}
            if (!strcmpi(bp,"INITUSER"))     {MF40_Userdata_Default(&MF40_Userdata, 1); MF40_Userdata_Save(0); goto Fertig;}
            break;
    }
    r=cmdErr_SyntaxError;
Fertig:
    return r;
}

// *****************************************************************************
// End of File

