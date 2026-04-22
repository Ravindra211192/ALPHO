// *****************************************************************************
// PIC_VAR.h
//
//  Rüffler Computer Hard+Soft
//  Hans Rüffler
//  (c) Copyright 2016-2020 by Hans Rüffler
//
//  Global variables of project
//  22.06.2020 HR
//
// *****************************************************************************
#ifndef _PIC_VAR_H                // Guard against multiple inclusion
#define _PIC_VAR_H
// -----------------------------------------------------------------------------
#ifdef eee
    #undef eee
#endif
#define eee extern
#ifdef IST
    #undef IST
#endif
#define IST(x)
// -----------------------------------------------------------------------------

eee uint16  isMF40  ;                           // zentraler, dynamischer GeräteTyp

// -----------------------------------------------------------------------------
struct counterInputStruct {
    int16  ain;
    uint16 aflanke;                             // [8] 0: --__, 1:__--
    uint16 apuls;                               // [7] minimale Pulsdauer [1/1024s]
    uint16 apegel;                              // [9] 
    uint16 act;
    uint16 pulsecount;                          // [2] Pulszähler
    uint16 period;                              // [3] Perioden/Pulsabstandsdauer [1/1024s]; max. 60s --> 1024*60 = 0xF000
    uint16 period_last;                         // [4] letzte Perioden/Pulsabstandsdauer [1/1024s]; max. 60s --> 1024*60 = 0xF000
};
// -----------------------------------------------------------------------------
#define MF40_Setup_FLASH_Adr        (0*4096)    // 0x000000 1. FLASH-Block
#define MF40_Userdata_FLASH_Adr     (1*4096)    // 0x001000 2. FLASH-Block
#define MF40_Lifedata_FLASH_Adr     (7*4096)    // 0x007000 8. FLASH-Block

#define MF40_Setup_Magic            0x1236      // bis 21.01.2019: 0x1235, bis 14.12.2018: 0x1234
#define MF40_Setup_MagicEnd         0x8765

#define MF40_Userdata_Magic         0x567B      // bis 21.01.2019: 0x567A, bis 14.12.2018: 0x5678
#define MF40_Userdata_MagicEnd      0x8765

#define MF40_Lifedata_Magic         0xABCD      // ab 06.02.2019
#define MF40_Lifedata_MagicEnd      0x8765

#define RampSetsMax                 6           // verwendbare Rampenanzahl
#define RampSetsMaxPlaned           8           // geplanter Speicherplatz
#define RampPointsMax               8
#define RampNameLen                 26

#define PIDsetsMax                  5           // 0(B),1..4
#define PIDsetsMaxPlaned            8

#define maxSetupToggle              8

#define TEMP_max_SET                2500        // ab 15.09.2019 eingeführt
#define ResetTimerSET               4           // [1/10s]

#define PIDmode_V3                  0           // Standard
#define PIDmode_V2                  1           // Legacy
        
#define LED_300ms                   0           // 100ms (fix ab V2.02.08)

// -----------------------------------------------------------------------------
struct MF40_RampPointStruct {
    uint16 dt;
    int16  Temp;
    int16  PIDset;
};
struct MF40_RampSetStruct {
    uint16 RampPointsUsed;                      // Anzahl der verwendeten RAMP Punkte 1..8
    struct MF40_RampPointStruct RP[RampPointsMax];
    char   Ramp_Name[RampNameLen];
};
struct MF40_PIDsetStruct {
    float   Kp;                                 // Proportionalbeiwert
    float   Ki;                                 // Integrierbeiwert     Ki = Kp / Tn   Tn: Nachstellzeit
    float   Kd;                                 // Differenzierbeiwert  Kd = Kp * Tv   Tv: Vorhaltezeit
    uint16  GENOFFuse;                          // 1:Generator-Off wird verwendet, 0: nicht verwendet
};
struct MF40_CalStruct {                         // noch nicht verwendet
    int32   v[2];
    uint16  R4_20;
};
struct MF40_ScaleStruct {
    int32   v[2];
    uint16  R4_20;
};
// -----------------------------------------------------------------------------
struct MF40_Setup_Struct {                      // max. 4096 Bytes (1. FLASH Block mit 4096 Byte)
    uint16 Magic;                               // MF40_Setup_Magic
    uint16 Version;                             // für Upgrade interessant, siehe MF40_Setup_Default()
    //..........................................   ToggleIndex:
    uint16 RAMP_Option;                         // [0]
    uint16 GENOFF_Option;                       // [1]
    uint16 MODBUS;                              // [2]  1:MODBUS Slave verfügbar, 0:nicht verfügbar
    uint16 ANA_Sample200ms;                     // [3]  0:100ms, 1:200ms Analog-Mittelwertbildungszeit
    uint16 SIM_sim;                             // [4]  1:Temperatur-Simulation EIN, 0:AUS
    uint16 ExtTestDisp;                         // [5]
    uint16 NoSI;                                // [6]  1:keine Service-Intervall Erinnerungen (ab V2.03.03)
//  uint16 PID_Mode_;                           // [6]  bis V2.03.01, ab V2.03.02 22.06.2020 stets=0 "PIDmode=V3"
    uint16 MF0_HF1;                             // [7]
    uint16 spare1[16-maxSetupToggle];
    //..........................................
    uint16 DisplaySelect;
    uint16 NoRegLEDs;
    uint16 PSETMAX;                             // [kW]
    uint16 PISTMAX;                             // [kW]
    uint16 IMAX;                                // [A]
    uint16 FMAX;                                // [kHz]
    struct MF40_CalStruct Pyro_1_Cal;
    struct MF40_CalStruct Pyro_2_Cal;
    //..........................................
    // Bis V2.03.01:
    //  uint16 spare2[32];
    //  uint16 MagicEnd;                        // 0x8764
    //..........................................
    // In V2.03.02 (einmal ausgeliefert am 22.06.2020):
    //  uint16 spare2[32-2];
    //  uint32 DEVKEY;                          // DEVKEY
    //  uint32 SN;                              // Seriennummer max. 9-stellig
    //  uint16 MagicEnd;                        // 0x8764
    //..........................................
    // In V2.03.03 24.06.2020:
    //  uint16 spare2[32-2-2];
    //  uint32 SI;                              // Service-Intervall YYYYMM
    //  uint32 DEVKEY;                          // DEVKEY
    // Ab V2.04.01 03.09.2021:
    uint16 spare2[32-2-2-1];
    uint16 SplashOnly;                          // 1:nur Splash anzeigen, 0:normal
    uint32 SI;                                  // Service-Intervall YYYYMM
    uint32 DEVKEY;                              // DEVKEY
    //..........................................
    uint32 SN;                                  // Seriennummer max. 9-stellig
    //..........................................
    uint16 MagicEnd;                            // 0x8764
    // Summe: 144 Bytes (22.06.2020, ab V2.03.02)
    // Summe: 140 Bytes (06.02.2019, bis V2.03.01)
    // Summe: 132 Bytes (05.10.2018)
};
// -----------------------------------------------------------------------------
struct MF40_Userdata_Struct {                   // max. 4096 Bytes (2. FLASH Block mit 4096 Byte)
    uint16 Magic;                               // MF40_Userdata_Magic
    uint16 Version;                             // für Upgrade interessant, siehe MF40_Userdata_Default()
    //..........................................
    float  P_SET;                               // [kW] P SET/MAX
    float  TIMER_SET;                           // [s]  TIMER für NO-RAMP
    float  TEMP_SET;                            // [°C] TEMP für NO-RAMP
    //..........................................
    uint16 ars;                                 // ActiveRampSet
    uint16 spare1[4-2];
    struct MF40_RampSetStruct RP[RampSetsMaxPlaned];
    //..........................................// 09.10.2018>>> 
    uint8 spare2[64];
    struct MF40_ScaleStruct Pyro_1_Sc;
    struct MF40_ScaleStruct Pyro_2_Sc;
    uint8 spare3[64];
    //..........................................
    struct MF40_PIDsetStruct PID[PIDsetsMaxPlaned];
    float   PID_Ta;                             // [s] Zeitraster
    uint16  TEMP_used;                          // Temp.Measuring: 0=OFF, 1=0..20mA, 2=4..20mA
    uint16  TEMP_fahrenheit;                    // 0:°C, 1:°F
    uint16  TEMP_max;                           // [°C]
    uint16  TEMP_stop;                          // [°C]
    uint16  DisplayRoll;                        // 1:Roll display pages (1>2>3>1>2...), 0:not (1>2>3)
    uint16  TIMER_max;                          // [s]
    uint8   MB_Slave_Adr;                       // 1..250
    uint8   MB_Slave_Parity;                    // 0:N, 1:O, 2:E
    uint32  MB_Slave_Baud;                      // 9600,...
    uint16  TIMER_delayed;                      // 0:OFF, 1:TEMP-SET
    //..........................................// 09.10.2018>>> 
    uint8 spare4[64-2];
    //..........................................
    uint16 MagicEnd;                            // 0x8764
    // Summe:1004 Bytes (06.02.2019)
    // Summe:1000 Bytes (21.01.2019)
    // Summe: 992 Bytes (20.01.2019)
    // Summe: 988 Bytes (19.01.2019)
    // Summe: 724 Bytes (18.01.2019)
    // Summe: 624 Bytes (09.10.2018)
    // Summe: 384 Bytes (05.10.2018)
};

// -----------------------------------------------------------------------------
struct MF40_Lifedata_Struct {                   // max. 4096 Bytes (8. FLASH Block mit 4096 Byte)
    uint16 Magic;                               // MF40_Lifedata_Magic
    //..........................................
    uint32 OprTime;                             // [s]
    uint32 OprTimeGenerator;                    // [s]
    uint32 GeneratorCycles;                     // Generator-Zyklen
    // für Wartungsintervall:
    uint32 OprTime_1;                           // [s]
    uint32 OprTimeGenerator_1;                  // [s]
    uint32 GeneratorCycles_1;                   // Generator-Zyklen
    //..........................................
    uint16 spare[64];
    //..........................................
    uint16 MagicEnd;                            // 0x8764
    // Summe: 160 Bytes (06.02.2019)
};

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
eee volatile int8   etz4_Master;
eee volatile uint8  etz_flags_Master;

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

#ifdef WITH_DISPLAY
 #define tbus_rx_max     (2048+512)             // max. Empfangspuffergröße
#else
 #define tbus_rx_max     (512)                  // max. Empfangspuffergröße
#endif
// *****************************************************************************
// ***** TBUS
// *****************************************************************************
eee char    MAC48[6];
// TBUS RX
eee char    tbus_ADR;                           // Adresse des Slaves
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
struct bmp_struct {
    char dummy;
    char bmp_adr[3];    // bei Übergabe an FT8xx bmp->bmp_reg verwenden, da hier die Adresse durch wr8n() abgelegt wird
    char bmp[2048];
    int x, y;
    int cx, cy;
    int new;
    int font;           // 0=Font6x8LCD, 1=Font6x8WEST, 2=Font6x8EAST, 3=Font6x8RUSS
};
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
eee struct bmp_struct Bitmap_3;                 // Bitmap für Background-Grafik
eee struct bmp_struct * BBMP IST (&Bitmap_3);
eee uint16 BMP_ms;
eee uint16 BMP_grafik;                          // 0:nur TBMP, B0:+GBMP, B1:+BBMP
eee char   key_string[20];
eee int    DeviceBacklightOff;
eee int    DeviceLayout;                        // 0x14=U1604, 0x20=REG-D                      

#define DispLEDmax   8
eee char DispLED_name[DispLEDmax][10];          // = {"Status","LAN-L","LAN-R","LON","REL-1","REL-2","REL-3","REL-4"};
eee char DispLED_val[DispLEDmax];               // = {2,0,0,0,1,3,0,1};    // 0:OFF, 1:R, 2:G, 3:Y

// ----- Counter zu statistischen Zwecken
eee int D_T_count;                              // Anzahl Display-T Aufrufe
eee int D_G_count;                              // Anzahl Display-G Aufrufe
eee int D_K_count;                              // Anzahl Display-K Aufrufe

eee uint32 mus_startval, mus_max, mus_min;

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
eee uint    MF40_BlockWM;
eee int     MF40_RunRamp_TempJump;  

eee float   MF40_Ist_P;
eee float   MF40_Ist_I;
eee float   MF40_Ist_Freq;
eee float   MF40_Ist_cosPhi;
//eee int     MF40_PID_esum;

eee float   MF40_Regel_Steller;     // 0..100.0%
eee float   MF40_Ist_Temp;          // [°C]
eee float   MF40_Ist_Timer;         // [s]  float Timer, aus MF40_RUN10 und MF40_RUNTIME10 ermittelt
eee int     MF40_Regel_Temp;        // [°C] int
eee int     MF40_Regel_PIDset;      // 0(B),1..4
eee int     MF40_Temp_Error;

eee uint    I2C_Inputs;             // GPB0/B8:I_OFF, GPB5/B13,GPB6/B14,GPB7/B15:Rotary
eee uint    I2C_LEDstate;
eee int     I2C_initialized;
eee int     I2C_InitRequest;
eee uint32  I2C_CB_count;
eee uint32  I2C_Error_count;
eee int     I2C_NO;

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

// *****************************************************************************
#endif /* _PIC_VAR_H */
// + + + End of File + + +
