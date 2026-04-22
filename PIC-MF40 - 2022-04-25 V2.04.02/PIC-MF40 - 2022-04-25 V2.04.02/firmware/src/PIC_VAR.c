// *****************************************************************************
// PIC_VAR.c
//
//  Rüffler Computer Hard+Soft
//  Hans Rüffler
//  (c) Copyright 2016-2020 by Hans Rüffler
//
//  Global variables of project
//  20.03.2020 HR
//
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// NOTE:    Always include PIC32.h before including this header file
// *****************************************************************************
// *****************************************************************************
#include "PIC32.h"
#include "PIC_VAR.h"
// -----------------------------------------------------------------------------
#ifdef eee
    #undef eee
#endif
#define eee
#ifdef IST
    #undef IST
#endif
#define IST(x)  = x
// -----------------------------------------------------------------------------

eee uint16  isMF40=0;                           // zentraler, dynamischer GeräteTyp

// *****************************************************************************
// ***** SYSTEM
// *****************************************************************************
eee volatile uint16 MilliSekunden;              // 1/1024s umlaufender 16-Bit Zähler (64 Sekunden)
eee volatile uint32 MilliSekunden32;            // 1/1024s umlaufender 32-Bit Zähler, läuft vollkommen synchron mit MilliSekunden
eee volatile uint16 RTC_MilliSekunden;          // 0..1023 Millisekunden-Anteil der Sekundenzeit
eee volatile uint16 RTC_MilliSekunden_999;      // 0..999 Millisekunden-Anteil der Sekundenzeit
eee volatile uint32 time, utctime, oprtime;     // Sekdundenzähler; hier werden alle gleich behandelt, da es keinen non-volatile Speicher gibt
eee volatile uint16 MilliSekunden_Master;       // 1/1024s umlaufender 16-Bit Zähler (64 Sekunden), vom Master synchronisiert (kann also Zeitsprünge aufweisen)
eee volatile uint32 MilliSekunden32_Master;     // 1/1024s umlaufender 32-Bit Zähler (64 Sekunden in B15..B0), vom Master synchronisiert (kann also Zeitsprünge aufweisen)
eee volatile uint16 RTC_MilliSekunden_Master;   // 0..1023 Millisekunden-Anteil der Sekundenzeit
eee volatile uint32 utctime_Master;
eee volatile char   etz4_Master;
eee volatile uchar  etz_flags_Master;

eee volatile int    NeueSekunde, NeueSekunde_MF40;
eee volatile int    NeueZehntelSekunde, NeueZehntelSekunde_MF40;
eee volatile int    NeueHundertstelSekunde;
eee volatile uint16 PowerOnTimer;               // zählt ab PowerOn mit 1/1024s hoch bis 0xffff (bleibt stehen)
eee volatile uint16 doResetRequest;             // wenn =590 gesetzt wird, wartet main() auf Watchdog-Reset
// ***** 1s Timeouts ***********************************************************
eee volatile uint16 MasterConnectedTO;      
eee volatile uint16 LEDovrTO;
eee volatile uint16 SysOpen;
// ***** 1/10s Timeouts ***********************************************************
eee volatile uint16 ResetTimer;                 // wenn >0 wird Steuerkarten-Reset aktiv, wird mit 1/10s von MF40_Eval() heruntergezählt.

eee volatile char   Configured;                 // wird vom Master implizit auf die Slave-Adresse gesetzt. Unconfigured solange, wie Configured==0

eee volatile int    Com1_Baudrate;              // 0:USART_8N1, 1:USART_8O1, 2:USART_8E1
eee volatile int    Com1_Parity;                // 9600,19200,..921600
eee volatile int    Com2_Baudrate;              // 0:USART_8N1, 1:USART_8O1, 2:USART_8E1
eee volatile int    Com2_Parity;                // 9600,19200,..921600

// *****************************************************************************
// ***** TBUS
// *****************************************************************************
eee char    MAC48[6];
// TBUS RX
eee char    tbus_ADR;                           // Adresse des Slaves '0' + 0..15
eee char    tbus_ADR2;                          // >0 --> alternative Adresse des Slaves für DISP 'D' + 0..3
eee char    tbus_rx_STR[tbus_rx_max];           // TBUS RX Byte-String wird hier abgelegt
eee uint16  tbus_rx_LEN;                        // =0 : in Arbeit/Idle
                                                // =len(tbus_rxstr) : Auswertung kann durchgeführt werden
eee char    tbus_tx_dbc;                        // Zustand Display-Broadcast bei tbus_rx_LEN>0 Zuweisung
eee char    tbus_tx_dbc_t;                      // Arbeitsvar. hierzu                     

// *****************************************************************************
// ***** SCADA
// *****************************************************************************
eee uint16  SCADA_Cmd0;                         //
eee uint16  SCADA_Cmd1;                         //
eee uint16  SCADA_StartCmd,SCADA_StartCmd_last; //
eee uint16  SCADA_START, SCADA_STOP;
eee uint16  SCADA_Status0;
eee uint16  SCADA_Status1;

// *****************************************************************************
// ***** EVE
// *****************************************************************************
eee int    EVE_available;                       // >0: EVE verfügbar: 801(EVE1,CAP), 813(EVE2,CAP), 815(EVE3,CAP) 
eee int    EVE_Com_Error;
eee int    EVE_Com_Error_Cnt;
eee int    EVE_Check_Error_Cnt;
eee uint16 EVE_CMD_busy_msmax;
eee uint32 EVE_frequency IST (60000000L);       // Default 60000000 Hz
eee struct bmp_struct Bitmap_0;                 // Ziel-Bitmap (enthält alle anderen geodert)
eee struct bmp_struct * BMP IST (&Bitmap_0);
eee struct bmp_struct Bitmap_1;                 // Bitmap für Text
eee struct bmp_struct * TBMP IST (&Bitmap_1);
eee struct bmp_struct Bitmap_2;                 // Bitmap für Grafik
eee struct bmp_struct * GBMP IST (&Bitmap_2);
eee uint16 BMP_ms;                              // für Timeout der Display-Daten
eee uint16 BMP_grafik;                          // 0:nur TBMP, B0:+GBMP
eee char   key_string[20];
eee int    DeviceBacklightOff;
eee int    DeviceLayout;                        // 0x14=U1604, 0x20=REG-D

eee char DispLED_name[DispLEDmax][10]  = {"Status","LAN-L","LAN-R","LON","REL-1","REL-2","REL-3","REL-4"};
eee char DispLED_val[DispLEDmax]       = {2,0,0,0,1,3,0,1};    // 0:OFF, 1:R, 2:G, 3:Y

// ----- Counter zu statistischen Zwecken
eee int D_T_count;                              // Anzahl Display-T Aufrufe
eee int D_G_count;                              // Anzahl Display-G Aufrufe
eee int D_K_count;                              // Anzahl Display-K Aufrufe

eee uint32 mus_startval, mus_max, mus_min=123456789;

eee uint32 Count_DISP, tCount_DISP, Count_DISP_s;                 // Anzahl FT801_Display_U1604() Aufrufe; _s : pro Sekunde
eee uint32 Count_InputEval, tCount_InputEval, Count_InputEval_s;  // Anzahl "positiver" InputEval() Aufrufe
eee uint32 Count_DoEvents, Count_EEPto;
        
eee uint32 IntCount_UART1;
eee uint32 IntCount_UART2;
eee uint32 IntCount_TMR0;
eee uint32 IntCount_SPI1;
eee uint32 IntCount_SPI2, tIntCount_SPI2, IntCount_SPI2_s;
eee uint32 IntCount_I2C0;

// *****************************************************************************
// ***** S0-Eingangsverarbeitung
// *****************************************************************************
eee struct counterInputStruct CI[maxS0];
eee uint32 counter, counter_s, counts_per_second;
eee uint32 dilsw;
eee unsigned long inputs;
// *****************************************************************************
eee int     LED;                                // globaler Zustand der LED
eee uint32  LEDovr;                             // LED "Bits" bei LEDovrTO>0
                                                // LED Sequenz 32-Bit (1 Bit = 1/16s --> 32 Bit = 2s )
// *****************************************************************************
// ***** PIC_MF40 Variablen
// *****************************************************************************
#ifdef MF40
eee int     MF40_OUT_EIN IST (0);
eee int     MF40_OUT_GENOFF IST (0);

eee int32   MF40_RUN10;             // [1/10s] 0:STOP, >0:Rückwärts bis 0 zählend, <0:Dauerzähler-- 
eee int32   MF40_RUNTIME10;         // [1/10s] hochhzählend, wenn MF40_RUN10 !=0, sonst =0
eee int     MF40_WorkMode;
eee uint    MF40_BlockWM;           // 1/10s Timeout
eee int     MF40_RunRamp_TempJump;  

eee float   MF40_Ist_P;
eee float   MF40_Ist_I;
eee float   MF40_Ist_Freq;
eee float   MF40_Ist_cosPhi;

eee float   MF40_Regel_Steller;     // 0..100.0%
eee float   MF40_Ist_Temp;          // [°C]
eee float   MF40_Ist_Timer;         // [s]  float Timer, aus MF40_RUN10 und MF40_RUNTIME10 ermittelt
eee int     MF40_Regel_Temp;        // [°C] int
eee int     MF40_Regel_PIDset;      // 0(B),1..4
eee int     MF40_Temp_Error;
//eee int     MF40_PID_esum;

eee uint    I2C_Inputs;             // GPB0/B8:I_OFF, GPB5/B13,GPB6/B14,GPB7/B15:Rotary
eee uint    I2C_LEDstate;
eee int     I2C_initialized;
eee int     I2C_InitRequest;
eee uint32  I2C_CB_count;
eee uint32  I2C_Error_count;
eee int     I2C_NO;                 // wird zur Prüfung auf KEIN I2C Extender verwendet

eee uint    RotarySwitch;           // wird bei Rotary Tastendruck hochgezählt
eee int     RotaryUpDown;           // Rotary Drehung rechts--> +1, links--> -1

eee uint    START_Taster_ms;        // wird bei gedrücktem START-Taster im ms Raster hochgezählt (0..8191)
eee uint    STOP_Taster_ms;         // wird bei gedrücktem STOP-Taster im ms Raster hochgezählt (0..8191)
eee uint    RUN_LED_ms;             // wird bei aktiver RUN-LED im ms Taster hochgezählt  (0..8191)

eee uint32  LED_inputs;
eee uint    LED_dbc[16];

eee int     SI_anstehend;

eee struct MF40_Setup_Struct MF40_Setup, MF40_Setup_backup;
eee struct MF40_Userdata_Struct MF40_Userdata, MF40_Userdata_backup;
eee struct MF40_Lifedata_Struct MF40_Lifedata;

eee uint32  usTest;
eee int     iTest,jTest,kTest;

#endif // MF40

// *****************************************************************************
// Zum Testen:
eee int useDoEvents;

/*******************************************************************************
 End of File
*/

