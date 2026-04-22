// *****************************************************************************
// PIC32.h
//
//  Rüffler Computer Hard+Soft
//  Hans Rüffler
//  (c) Copyright 2016-2022 by Hans Rüffler
//
//  Common header file for all PIC32 projects
//  25.04.2022 HR
//
// *****************************************************************************
// Achtung, die notwendigen Compiler-Optionen sind in main.c beschrieben!
// *****************************************************************************
#ifndef _PIC32_H        // Guard against multiple inclusion
#define _PIC32_H

// Liste der möglichen Geräte, GENAU EINES davon MUSS definiert sein:
#define     MF40        // PIC_MF40
//#define   S0IN12      // U1624 S0IN12 TBUS-Modul

#ifdef MF40 // -----------------------------------------------------------------
    #define     WITH_DISPLAY            // EVE Display-Routinen werden vorgehalten wenn definiert

    // Versionsnummern sind für alle mit diesem Code erzeugten Geräte identisch
    #define     MF40_LAYOUT             // MF40-Layout
//  #define     HF25_LAYOUT             // alternatives HF25-Layout
    #define		SetVersionString		"2.04"
    #define		SetVersionInt			204
    #define		SetVersionIntExt		20402           // vnnuu
    #define		SetVersionDateString    "25.04.2022"    // Format: "DD.MM.YYYY" 
    #define     SetCopyrightString      "2017..2022 HR/ALPHOTRONIC"
    #define		SetDeviceString         "PIC-MFHF"
    #define     SetDeviceType           37              // noch nicht festgelegt
    #define     SetDeviceSubType        0
    
    #define     COM1_MODBUS_SLAVE   // wenn definiert --> COM1: MODBUS_SLAVE statt TBUS
    #define     COM1_Baudrate_Default   19200
    #define     COM2_MODBUS_MASTER  // wenn definiert --> COM2: MODBUS_MASTER
    #define     COM2_Baudrate_Default   19200
    // Test-Phasen Aktivierungen
//  #define MBSL_4W                     // MODBUS-SLAVE 4-wire (TX:COM1, RX:COM2)
//  #define MBSL_VIA_COM2               // MODBUS-SLAVE auf COM2 statt COM1

#endif


#ifdef WITH_DISPLAY
    #define	SetDeviceString_Disp480 "DISP480"       // unschön lang: "DISP480x272"
    #define    SetDeviceType_Disp480   61
    #define	SetDeviceString_Disp800 "DISP800"       // unschön lang: "DISP800x480"
    #define    SetDeviceType_Disp800   62
    #define    USE_EVE
#endif

#define USE_WATCHDOG            // define to use watchdog (~1s Period with WDTPS="PS1024", non-windowed)
#define LITTLE_ENDIAN           // PIC32 unterstützt nur Little Endian

// ----- TYPEDEFs --------------------------------------------------------------

typedef unsigned char uchar;
typedef char int8;
typedef uchar uint8;
typedef unsigned int uint;      // 16/32 Bit (bei PIC32 32 Bit)
typedef unsigned long ulong;    // 32 Bit
typedef short int int16;
typedef unsigned short int uint16;
typedef long int int32;
typedef unsigned long int uint32;

// ----- MAKROS ----------------------------------------------------------------
#define ei()        __builtin_enable_interrupts()
#define di()        __builtin_disable_interrupts()
#define reject()    
#define reject_exact()

#define strcmpi(x,y)    strcasecmp(x,y)
#define strncmpi(x,y,z) strncasecmp(x,y,z)
#define swapi16(x)      ((x>>8)|(x<<8))

#define strncpy0(x,y,z) {strncpy(x,y,z);x[z]=0;}

#ifdef MF40
    // Zustand der 16 Eingänge IN-1..IN-16, 1=Eingang aktiv, Low; 0=Eingang nicht-aktiv, High(Pull-Up)
    #define S0inputs        ( (((~(*(&PORTB + ((PORT_CHANNEL_B - 1) * 0x40)))) & (0xf000)) >> 12) + (((~(*(&PORTB + ((PORT_CHANNEL_C - 1) * 0x40)))) & (0x6000)) >> (13-4)) + (((~(*(&PORTB + ((PORT_CHANNEL_D - 1) * 0x40)))) & (0x00F0)) << (6-4)) + (((~(*(&PORTB + ((PORT_CHANNEL_E - 1) * 0x40)))) & (0x0007)) << (10)) + (((~(*(&PORTB + ((PORT_CHANNEL_E - 1) * 0x40)))) & (0x00E0)) << (12-4)) )
                            // RE-7..5+2..0, RD.7..4, RC.14+13, RB.15..12
    #define IN_START        ( (((~(*(&PORTB + ((PORT_CHANNEL_E - 1) * 0x40)))) & B4) >> 4) )        // RE.4
    #define IN_STOP         ( (((~(*(&PORTB + ((PORT_CHANNEL_B - 1) * 0x40)))) & B13) >> 13)?0:1 )  // RB.13 invertiert
    #define IN_TASTER       ( (((~(*(&PORTB + ((PORT_CHANNEL_B - 1) * 0x40)))) & B10) >> 10) )      // RB.10
    #define IN_RTC_INT      ( (((~(*(&PORTB + ((PORT_CHANNEL_D - 1) * 0x40)))) & B1) >> 1) )        // RD.1
    #define LED_RUN         LED_val(0)  // entprellter Zustand der RUN-LED
    #define LED_StdBy       LED_val(1)  // entprellter Zustand der StdBy-LED
    #define TBUSrotsw       (9)
    #define DevCodeInputs   (0)
    #define TypeCodeInputs  (0)
#else
    // Zustand der 12 S0-Inputs in B11..B0 (0:Eingang offen, 1:24V)
    #define S0inputs    (((~(*(&PORTB + ((PORT_CHANNEL_B - 1) * 0x40)))) & (0xfff0)) >> 4)
                    //  S0inputs = (~SYS_PORTS_Read( PORTS_ID_0, PORT_CHANNEL_B ) & (0xfff0)) >> 4;
    // TBUS-Adresse in B3..B0 (0..F)
    #define TBUSrotsw   (((~(*(&PORTB + ((PORT_CHANNEL_D - 1) * 0x40)))) & (0x00f0)) >> 4)
                    //  TBUSadr = (~SYS_PORTS_Read( PORTS_ID_0, PORT_CHANNEL_D ) & (0x00f0)) >> 4;
    // DevCode 0..2 in RE0..RE2 (mit internen Pull-Ups, wird invertiert)
    #define DevCodeInputs   (((~(*(&PORTB + ((PORT_CHANNEL_E - 1) * 0x40)))) & (0x0007)))
    // TypeCode 0..2 in RE5..RE7 (mit internen Pull-Ups, wird invertiert)
    #define TypeCodeInputs   (((~(*(&PORTB + ((PORT_CHANNEL_E - 1) * 0x40)))) & (0x00e0)) >> 5)
#endif

#ifdef USE_WATCHDOG
 #define watchdog()  SYS_WDT_TimerClear()
#else
 #define watchdog() 
#endif

// ----- Funktionen aus main() -------------------------------------------------
uint32  usVal(void);
void    mus(int state);
void    InputEvalBlock(int state);

int     EEPreadn(char * dp, uint16 adr, int len);
void    EEPwriteb(uint16 adr, uint16 val);
int     FLASHreadn_page(char * dp, uint32 adr, int len);
int     FLASHreadn(char * dp, uint32 adr, int len);
void    FLASHwriteULBPR(void);
void    FLASHwriten_page(char *sp, uint32 adr, int len);
void    FLASHwriten(char *sp, uint32 adr, int len);
#define Sector_Erase   -111
#define Block_Erase    -222
int     RTCreadn(char * dp, uint16 adr, int len);
void    RTCwriteb(uint16 adr, uint16 val);
void    RTCwriten(char *sp, uint16 adr, int len);
void    DoEvents(void);
int     OUT_RESET_Control(int val);

// ----- Geräte Definitionen
#define maxS0   12

#define maxPeriod   (1024*60)   // max. S0IN Periodendauer in 1/1024s: 60 Sekunden (Max=60*1024)

// ----- Bit Definitions
#define B0 0x01
#define B1 0x02
#define B2 0x04
#define B3 0x08
#define B4 0x10
#define B5 0x20
#define B6 0x40
#define B7 0x80
#define B8 0x100
#define B9 0x200
#define B10 0x400
#define B11 0x800
#define B12 0x1000
#define B13 0x2000
#define B14 0x4000
#define B15 0x8000

// ----- Zeichen Definitionen
#define O_NAK           ('U'-0x40)	// NAK
#define O_ACK           ('F'-0x40)	// ACK

#define Ctrl_X  		0x18
#define Ctrl_Y  		0x19
#define Ctrl_S  		0x13
#define Ctrl_Q  		0x11
#define SUB     		0x1a
#define ESC     		0x1b
#define CR				0x0d
#define LF				0x0a

// ----- TBUS
//#define tbus_MASTER                       // wenn definiert --> TBUS-MASTER, sonst --> TBUS_SLAVE
#define tbus_c_START	0x12				// TBUS Startzeichen
#define tbus_c_ZIP      0x14				// ZIP-Zeichen  (DC4)

#define cmdErr_OK               0
#define cmdErr_SyntaxError      1
#define cmdErr_RangeError       2
#define cmdErr_AccessDenied     9
#define cmdErr_OK_SN            (B14+0)
#define cmdErr_OK_SI            (B14+1)
#define cmdErr_OK_SPLASHONLY    (B14+2)
#define cmdErr_OK_RAMP          (B15+1)
#define cmdErr_OK_GENOFF        (B15+2)
#define cmdErr_OK_MODBUS        (B15+3)
#define cmdErr_OK_NO_SI         (B15+7)

#endif /* _PIC32_H */
// + + + End of File + + +
