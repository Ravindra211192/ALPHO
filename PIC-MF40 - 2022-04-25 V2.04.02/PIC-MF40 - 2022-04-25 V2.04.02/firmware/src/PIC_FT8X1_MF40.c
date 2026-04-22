// *****************************************************************************
// PIC_FT8X1_MF40.c
//
//  Rüffler Computer Hard+Soft
//  Hans Rüffler
//  (c) Copyright 2016-2020 by Hans Rüffler
//
//  FTDI EVE FT8X1 relevant routines for PIC_MF40 are located in this file
//  22.06.2020 HR
//  22.03.2020 HR
//  14.07.2019 HR
//  31.03.2019 HR
//  12.02.2019 HR
//  09.02.2019 HR
//  09.10.2018 HR
//  27.11.2017 HR
// *****************************************************************************
//#define FLASH_TESTING
#define UHRZEIT_VIA_CALC
#define MF40_BMP                        // definieren wenn ausschließlich TBMP verwendet wird, reduziert auf 128x64
//#define NO_RTC_USE
//#define DL_TESTused                   // nur zur testweisen Messung des Display-List Füllstands aktivieren! Verlangsamt das Gerät erheblich
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

#include "PIC32.h"
#include "system_definitions.h"
#include "PIC_VAR.h"
#include "PIC_Func.h"
#include "PIC_FT8X1.h"
#include "PIC_MF40.h"

#include "ECSfont.C"


#define ALLOW_MF40                      // wenn definiert, kann MF40 Demo abgerufen werden (TestMode wird zusätzlich beachtet)
#define TestModeDefault 0               // Vorgabe für TestMode: 0-->Screen und Company ohne Funktion, 1-->mit Funktion
                                        // TestMode wird durch Aufruf von FT801_Display_SliderDialCalc() getoggelt (0,1))

// *****************************************************************************
// hier enthaltene Funktionen
// *****************************************************************************
void    FT801_Init(void);
int     FT801_Init_Check(void);
void    FT801_Display_Hub(void);
void    FT81X_Display_MF40(int first_req);
void    FT81X_Display_News_MF40(int first_req);
void    FT81X_Display_Calculator(int first_req);

int     CMD_busy(void);

uint8   rd8(uint32 adr);
uint16  rd16(uint32 adr);
uint32  rd32(uint32 adr);
int     wr8(uint32 adr, int8 data8);
int     wr8n(uint32 adr, int8 * a, int len);
int     wr16(uint32 adr, int16 data16);
int     wr32(uint32 adr, int32 data32);
void    host_command(uint8 hc);
void    host_command_par(uint8 hc, uint8 par);

void    bmp_clear(struct bmp_struct * bmp);
void    bmp_prtc(struct bmp_struct * bmp, char c);
void    bmp_prts(struct bmp_struct * bmp, char * sp, int wrap);
void    bmp_prts_0(struct bmp_struct * bmp, char * sp, int wrap);
void    bmp_prts_0_font(struct bmp_struct * bmp, char * sp, int wrap, int font);
void    drawbmp(void);

void    Ramp_Copy(struct MF40_Userdata_Struct * UD, int ars);
void    Ramp_Paste(struct MF40_Userdata_Struct * UD, int ars);

int     SwipeWindows(int first, int ausnahme);
int     sprtTimer(char * s, uint t);
char    cPIDset(int ps);
void    ShowEllipsen(int x, int y, int vert, int hipos);
void    showAlphoFooter(int showTest);
void    showSetupFooter(int x, int y);

// *****************************************************************************
// externe Funktionen
// *****************************************************************************
void    EVEreadn(uint32 adr, uchar * dp, int len);
void    EVEwriten(uchar * sp, int len);
int     EVE_busy(void);
int     EVE_PD_Control(int val);

// *****************************************************************************
// Lokale #defines
// *****************************************************************************
#define hub_Default         0
#define hub_U1604           1
#define hub_News            2
#define hub_Calculator      3
#define hub_Ramp            4
#define hub_RampSetup       5
#define hub_Setup           6
#define hub_UserSetup1      7
#define hub_UserSetup2      8
#define hub_Keyboard        9
#define hub_SliderDialCalc  10
#define hub_MF40            20
#define hub_Swipe_L         50
#define hub_Swipe_R         51
#define hub_return          98
#define hub_Exception       99
#define hub_Splash          100

// *****************************************************************************
// Lokale Variablen
// *****************************************************************************
// Initial Values for RIVERDI_43_480x272
    // *** FT801 with 4,3" WQVGA Diplay 480x272
    ft_int16_t FT_DispWidth = 480;
    ft_int16_t FT_DispHeight = 272;
    ft_int16_t FT_DispHCycle =  548;
    ft_int16_t FT_DispHOffset = 43;
    ft_int16_t FT_DispHSync0 = 0;
    ft_int16_t FT_DispHSync1 = 41;
    ft_int16_t FT_DispVCycle = 292;
    ft_int16_t FT_DispVOffset = 12;
    ft_int16_t FT_DispVSync0 = 0;
    ft_int16_t FT_DispVSync1 = 10;
    ft_uint8_t FT_DispPCLK = 5;
    ft_char8_t FT_DispSwizzle = 0;
    ft_char8_t FT_DispPCLKPol = 1;
    ft_char8_t FT_DispCSpread = 1;
    ft_char8_t FT_DispDither = 1;
    //..........................................
    ft_uint16_t FT_PWM_DUTY = 128;              // komplett EIN; Hinweis: Bereich ist nicht 0..100 sondern 0..128 (ab V1.03 berücksichtigt)
    ft_uint16_t FT_PWM_HZ = 270;                // 270 Hz, bis V1.03: 250Hz (Default), bis V1.02: 1kHz
    //..........................................
    ft_uint32_t FT_TOUCH_TRANSFORM_A;
    ft_uint32_t FT_TOUCH_TRANSFORM_B;
    ft_uint32_t FT_TOUCH_TRANSFORM_C;
    ft_uint32_t FT_TOUCH_TRANSFORM_D;
    ft_uint32_t FT_TOUCH_TRANSFORM_E;
    ft_uint32_t FT_TOUCH_TRANSFORM_F;

// *****************************************************************************
    int     hub_state=0;                // globaler state von FT801_Display_Hub())
    int     hub_state_last;             // der vor hub_state gültige Hub
    int     hub_lastMain;               // speichert den letzten Haupt-Hub
    int     ft_state_force=0;
    int     drawbmp_mode=0;             // 0:GMC, 1:>>>, 2: <<< für drawbmp())
    
    int     TestMode=TestModeDefault;
    
    uint    testext;                    // wird in FT81X_Display_News_MF40 gebildet, ab 18.03.2020 im ganzen Modul ausgewertet

    uint    DL_LEN_USED;
    uint    Exception_DL_LEN_USED;
    
    char    Keyboard_Title[81];
    char    Keyboard_String[81];
    int     Keyboard_Len;
    int     Keyboard_Mode;
    int     Keyboard_ReadyState;
    int     Keyboard_Callers_hub_state;

    int     showTestValues;

// ----- für ShowGraph() u.a.
    int     ZeitMax;                    // [s]]
    uint16  gx0,gy0,gxm,gym;

// ----- für Ramp_Copy/Paste
struct MF40_RampSetStruct CLIP_RP;
int CLIP_used=0;

// ----- Temperatur-Messkurve
#define maxCurvePoints  240 // belastet DL_BUF mit n*4. Stand 15.10.2018: FT81X_Display_Ramp_MF40 --> DL_BUF ~3359 von 4096 Bytes
//#define maxCurvePoints  300 // ACHTUNG, STÜRZT BEI DL_BUF 3619 AB! // belastet DL_BUF mit n*4. Stand 14.10.2018: FT81X_Display_Ramp_MF40 --> DL_BUF ~3600 von 4096 Bytes
struct curveStruct {
    int xn[maxCurvePoints];
    int yn[maxCurvePoints];
    int n;
    int go;
    int xact;
    int yact;
} TempCurve; 


// ----- zu Testzwecken
//#define TESTTESTshow
int TESTTEST;   

// *****************************************************************************
// Variablen aus anderen Modulen
// *****************************************************************************
extern int PID_esum;


// *****************************************************************************
#define FT_BUF_OPTIMIZE         // muss stets definiert sein, direktes Schreiben war nur für die Anfangsphase
#ifdef FT_BUF_OPTIMIZE
    uchar    DL_BUF[4096+3];    // Größe für Comand-RAM (RAM_CMD) eingestellt (halb so groß wie RAM_DL), 
                                // da aktuell alle DL Einträge via RAM_CMD laufen.
                                // PLUS sicherheitshalber 3 Byte für wr8n() Framing, obwohl volle Länge mit dem Ringbuffer keinen Sinn macht.
    uchar *  DL_P;
    uint     DL_L;
    // ..............................................
    #define DLinit()    {DL_P = DL_BUF+3; DL_L=3;}
    #define DLexec()    {wr8n(RAM_DL, DL_BUF, DL_L);}
    void DL(uint32 x) {
        *(DL_P++) = (x&0xff); 
        *(DL_P++) = ((x>>8)&0xff); 
        *(DL_P++) = ((x>>16)&0xff); 
        *(DL_P++) = ((x>>24)&0xff); 
        DL_L+=4;
    }
    void DL2(uint16 x) {
        *(DL_P++) = (x&0xff); 
        *(DL_P++) = ((x>>8)&0xff); 
        DL_L+=2;
    }
    // ..............................................
    #define CDLinit()   DLinit()
    int CDLexec(void) {
        uint32   DL_UI32;
        uint16   DL_UI16;
        // ......
        if (DL_L>DL_LEN_USED) DL_LEN_USED=DL_L;      // Längenmessung mit MAX-Bildung (ab 16.11.2017)
        if (DL_LEN_USED>3800) {
            hub_state=hub_Exception;
            Exception_DL_LEN_USED = DL_LEN_USED;    // mit Exception_DL_LEN_USED ab 16.10.2018
            DL_LEN_USED = 0;
            return 1;
        }
        // ......
        DL_UI32 = RAM_CMD + (DL_UI16=rd16(REG_CMD_WRITE)&(4096-1));
        wr8n(DL_UI32, DL_BUF, DL_L);
        wr16(REG_CMD_WRITE, (DL_UI16 + DL_L-3)&(4096-1));
        return 0;
    }
    #define CDL(x)      DL(x)  
    // ..............................................
#else
    uint32   DLP;
    #define DLinit()  {DLP = RAM_DL; }
    #define DLexec()  {}
    void DL(uint32 x) {
        wr32(DLP,x); 
        DLP+=4;
    }
#endif



// *****************************************************************************
// *****************************************************************************
// *****************************************************************************

// -----------------------------------------------------------------------------
uint8 rd8(uint32 adr)
{
uchar a[1];    
    EVEreadn(adr, a, 1);
    return a[0];
}

// -----------------------------------------------------------------------------
uint16 rd16(uint32 adr)
{
uchar a[2];    
    EVEreadn(adr, a, 2);
    return ((uint16)a[1]<<8) + a[0];    // LSB comes first
}

// -----------------------------------------------------------------------------
uint32 rd32(uint32 adr)
{
uchar a[4];    
    EVEreadn(adr, a, 4);
    return ((uint32)a[3]<<24) + ((uint32)a[2]<<16) + ((uint32)a[1]<<8) + a[0];  // LSB comes first
}

// -----------------------------------------------------------------------------
int wr8(uint32 adr, int8 data8)
{
uchar a[4];
    a[0] = 0b10000000 + ((adr>>16)&0x3f);
    a[1] = ((adr>>8)&0xff);
    a[2] = (adr&0xff);
    a[3] = data8;
    EVEwriten(a, 4);
}

// -----------------------------------------------------------------------------
// Write len bytes in total to EVE out of a[]
// First 3 bytes of a[] are filled with (write_code + adr_24_bit) here 
// -----------------------------------------------------------------------------
int wr8n(uint32 adr, int8 * a, int len)
{
    a[0] = 0b10000000 + ((adr>>16)&0x3f);
    a[1] = ((adr>>8)&0xff);
    a[2] = (adr&0xff);
    EVEwriten(a, len);
}

// -----------------------------------------------------------------------------
int wr16(uint32 adr, int16 data16)
{
uchar a[5];
    a[0] = 0b10000000 + ((adr>>16)&0x3f);
    a[1] = ((adr>>8)&0xff);
    a[2] = (adr&0xff);
    a[3] = (data16&0xff);       // LSB  data16 in little endian
    a[4] = ((data16>>8)&0xff);  // MSB
    EVEwriten(a, 5);
}

// -----------------------------------------------------------------------------
int wr32(uint32 adr, int32 data32)
{
uchar a[7];
    a[0] = 0b10000000 + ((adr>>16)&0x3f);
    a[1] = ((adr>>8)&0xff);
    a[2] = (adr&0xff);
    a[3] = (data32&0xff);        // LSB  data16 in little endian
    a[4] = ((data32>>8)&0xff);   // ..
    a[5] = ((data32>>16)&0xff);  // ..
    a[6] = ((data32>>24)&0xff);  // MSB
    EVEwriten(a, 7);
}

// -----------------------------------------------------------------------------
// 3 bytes are written to EVE:
//  1. Byte = hc    (HostCommand)
//  2. + 3. Byte = 0
// -----------------------------------------------------------------------------
void host_command(uint8 hc)
{
uchar a[3];
    a[0] = hc;
    a[1] = 0;
    a[2] = 0;
    EVEwriten(a, 3);
}
// -----------------------------------------------------------------------------
// 3 bytes are written to EVE:
//  1. Byte = hc    (HostCommand)
//  2. Byte = par 
//  3. Byte = 0
// -----------------------------------------------------------------------------
void host_command_par(uint8 hc, uint8 par)
{
uchar a[3];
    a[0] = hc;
    a[1] = par;
    a[2] = 0;
    EVEwriten(a, 3);
}

// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
char xlat(char c)   // Übersetzt ANSI-HI in Spezialzeichen <0x20 oder Ersatzzeichen (ab 16.11.2017)
// *****************************************************************************
{   
    switch(c) {
        case 'Ä': return 'A';
        case 'Ö': return 'O';
        case 'Ü': return 'U';
        case 'ä': return 'a';
        case 'ö': return 'o';
        case 'ü': return 'u';
        case 'ß': return 's';
        case '°': return '*';
        default : return c;
    }
}
// *****************************************************************************

void DL_CMD_BGCOLOR(u32t rgb)
                {DL(CMD_BGCOLOR);DL(rgb);}
void DL_CMD_BUTTON(i16t x,i16t y,u16t w,u16t h,u16t font,u16t opt,char *cp)
                {char c;u16t i;DL(CMD_BUTTON);DL2(x);DL2(y);DL2(w);DL2(h);DL2(font);DL2(opt);
                 while(c=*(cp++)){*(DL_P++)=c;DL_L++;}
                 *(DL_P++)=0;DL_L++; 
                 while((DL_L-3)&3){*(DL_P++)=0;DL_L++;}
                }
void DL_CMD_CALIBRATE(u32t result)
                {DL(CMD_CALIBRATE);DL(result);}
void DL_CMD_CLOCK(i16t x,i16t y,i16t r,u16t opt,u16t h,u16t m,u16t s,u16t ms)
                {DL(CMD_CLOCK);DL2(x);DL2(y);DL2(r);DL2(opt);DL2(h);DL2(m);DL2(s);DL2(ms);}
void DL_CMD_DIAL(i16t x,i16t y,i16t r,u16t opt,u16t val)
                {DL(CMD_DIAL);DL2(x);DL2(y);DL2(r);DL2(opt);DL2(val);DL2(0);}
void DL_CMD_FGCOLOR(u32t rgb)
                {DL(CMD_FGCOLOR);DL(rgb);}
void DL_CMD_GAUGE(i16t x,i16t y,i16t r,u16t opt,u16t major,u16t minor,u16t val,u16t range)
                {DL(CMD_GAUGE);DL2(x);DL2(y);DL2(r);DL2(opt);DL2(major);DL2(minor);DL2(val);DL2(range);}
void DL_CMD_GRADIENT(i16t x0,i16t y0,u32t rgb0,i16t x1,i16t y1,u32t rgb1)
                {DL(CMD_GRADIENT);DL2(x0);DL2(y0);DL(rgb0);DL2(x1);DL2(y1);DL(rgb1);}
void DL_CMD_KEYS(i16t x,i16t y,i16t w,i16t h,i16t font,u16t opt,char *cp)
                {char c;u16t i;DL(CMD_KEYS);DL2(x);DL2(y);DL2(w);DL2(h);DL2(font);DL2(opt);
                 while(c=*(cp++)){*(DL_P++)=c;DL_L++;}
                 *(DL_P++)=0;DL_L++; 
                 while((DL_L-3)&3){*(DL_P++)=0;DL_L++;}
                }
void DL_CMD_NUMBER(i16t x,i16t y,i16t font,u16t opt,i32t n)
                {DL(CMD_NUMBER);DL2(x);DL2(y);DL2(font);DL2(opt);DL(n);}
#ifdef FT_81X_ENABLE
 void DL_CMD_ROMFONT(u32t font, u32t romslot)
                {DL(CMD_ROMFONT);DL(font);DL(romslot);}
#endif
void DL_CMD_SCROLLBAR(i16t x,i16t y,i16t w,i16t h,u16t opt,u16t val,u16t size,u16t range)
                {DL(CMD_SCROLLBAR);DL2(x);DL2(y);DL2(w);DL2(h);DL2(opt);DL2(val);DL2(size);DL2(range);}
void DL_CMD_SLIDER(i16t x,i16t y,i16t w,i16t h,u16t opt,u16t val,u16t range)
                {DL(CMD_SLIDER);DL2(x);DL2(y);DL2(w);DL2(h);DL2(opt);DL2(val);DL2(range);DL2(0);}
void DL_CMD_TEXT(i16t x,i16t y,i16t font,u16t opt,char *cp)
                {char c;u16t i;DL(CMD_TEXT);DL2(x);DL2(y);DL2(font);DL2(opt);
                 while(c=*(cp++)){*(DL_P++)=xlat(c);DL_L++;}                    // mit Übersetzung ab 16.11.2017
                 *(DL_P++)=0;DL_L++; 
                 while((DL_L-3)&3){*(DL_P++)=0;DL_L++;}
                }
void DL_CMD_TOGGLE(i16t x,i16t y,i16t w,i16t font,u16t opt,u16t state,char *cp)
                {char c;u16t i;DL(CMD_TOGGLE);DL2(x);DL2(y);DL2(w);DL2(font);DL2(opt);DL2(state);
                 while(c=*(cp++)){if(c=='|') c=0xff; *(DL_P++)=c;DL_L++;}       // '|' --> 0xff (zur Trennung beider Optionen)
                 *(DL_P++)=0;DL_L++; 
                 while((DL_L-3)&3){*(DL_P++)=0;DL_L++;}
                }
void DL_CMD_TRACK(i16t x,i16t y,i16t w,i16t h,i16t tag)
                {DL(CMD_TRACK);DL2(x);DL2(y);DL2(w);DL2(h);DL2(tag);DL2(0);}
// Fehler-Test: z.B. mit DL_CMD_DIAL, bei dem ein DL2 fehlt:
void DL_CMD_DIAL_FEHLER(i16t x,i16t y,i16t r,u16t opt,u16t val)
                {DL(CMD_DIAL);DL2(x);DL2(y);DL2(r);DL2(opt);DL2(val);}
// Ist der Fehler "drin", kommt FT801 nur noch mit PowerOn-Reset raus.
// Hier muss noch eine Lösung gefunden werden, wie eine FT801 Reset richtig ausgeführt wird

// *****************************************************************************
// Einstellen der Display-Größe abhängigen Variablen
// inch_val :   43, 50, 70 (andere Werte --> 43)
// *****************************************************************************
void FT801_Init_Vars(int inch_val)
{
    switch(inch_val) {
        case 50:// RIVERDI_50_800x480
            // *** FT812or FT813 with 5" WVGA Diplay 800x480
            // Hinweis //d weist auf Abweichung bei 5" Display gegenüber 7" Display hin
            FT_DispWidth = 800;
            FT_DispHeight = 480;
            FT_DispHCycle =  928;
            FT_DispHOffset = 42;     //d
            FT_DispHSync0 = 0;
            FT_DispHSync1 = 3;       //d
            FT_DispVCycle = 525;
            FT_DispVOffset = 23;     //d erste und letzte Pixelzeile ist damit zu sehen (PCLKPol=1)
            FT_DispVSync0 = 0;
            FT_DispVSync1 = 3;
            FT_DispPCLK = 2;         // --> RGB_PCLK = 30MHZ @60MHZ EVE_CLK
            FT_DispSwizzle = 0;
            FT_DispPCLKPol = 1;
            FT_DispCSpread = ((EVE_available>=815) ? 0 : 1); // 1 für FT810..813, 0 für BT815..816
//          FT_DispCSpread = 1;
            FT_DispDither = 1;
            break;
        case 70:// RIVERDI_70_800x480
            // *** FT813 with 7" WVGA Diplay 800x480
            FT_DispWidth = 800;
            FT_DispHeight = 480;
            FT_DispHCycle =  928;
            FT_DispHOffset = 88;
            FT_DispHSync0 = 0;
            FT_DispHSync1 = 48;
            FT_DispVCycle = 525;
            FT_DispVOffset = 32;     // erste und letzte Pixelzeile ist damit zu sehen (PCLKPol=1)
            FT_DispVSync0 = 0;
            FT_DispVSync1 = 3;
            FT_DispPCLK = 2;         // --> RGB_PCLK = 30MHZ @60MHZ EVE_CLK
            FT_DispSwizzle = 0;
            FT_DispPCLKPol = 1;
            FT_DispCSpread = ((EVE_available>=815) ? 0 : 1); // 1 für FT810..813, 0 für BT815..816
//          FT_DispCSpread = 1;
            FT_DispDither = 1;
            break;
        case 43: default:
            break;  // alles bereits initialisiert
    }
}

// *****************************************************************************
// Init FT801 Controller
// *****************************************************************************
void FT801_Init(void)
{
uint16 ms;
    // use SPI CLK <11MHZ
    //PLIB_SPI_BaudRateSet( SPI_ID_2 , SYS_CLK_PeripheralFrequencyGet(CLK_BUS_PERIPHERAL_2), 10000000 );

    EVE_PD_Control(0);              // 5ms PD=0 anstehen lassen --> EVE-Reset
    delay(6);
    EVE_PD_Control(1);              // Reset aufheben
    delay(5);                       // ???
    
#ifdef FT_81X_ENABLE    
    // --- FT813 -------------------
    host_command(HC_ACTIVE);        // send host command "ACTIVE" to FT813
    delay(20);
    host_command(HC_CLKINT);        // send command to "CLKINT" to FT813
    delay(10);
#ifdef DIFFERENT_CLOCK_SETTING              // nur zur Veranschaulichung, wie Änderungen bereits vor HC_CORERST durchgeführt werden (Achtung: EVE Typ an dieser Stelle noch nicht lesbar)
    host_command(HC_SLEEP);                 // send host command "ACTIVE" to FT813        (notwendig für nachfolgende Einstellungen)
//  host_command_par(0x61, 0);              // CLKSEL Default 12MHZ * 5 = 60 MHZ          (Einstellung setzt SLEEP voraus)
    host_command_par(0x61, 0b01000000 + 6); // CLKSEL Default 12MHZ * 6 = 72 MHZ (Einstellung setzt SLEEP voraus)
    host_command(HC_ACTIVE);                // send host command "ACTIVE" after "SLEEP"
    // Hinweis: hier kein delay() notwendig (s.u.)
    EVE_frequency = 72000000L;
    wr32(REG_FREQUENCY, EVE_frequency);     // Hinweis: REG_FREQUENCY ist nach EVE Reset = 60000000L
#endif
    host_command_par(0x49, 0);      // PD_ROMS all ON (wirklich notwendig?)
    host_command(HC_CORERST);       // send command to "CORERST"
    ms = MilliSekunden;
    while (rd8(REG_ID) != 0x7c){    // maximal 350ms auf Beendigung der internen Initialisierung warten
        if ((MilliSekunden-ms)>350) {
            EVE_available = 0;
            return;                 // nichts mehr zu tun
        }
        delay(10);                  // Raster 10ms
    }
    // CHIP-ID @0xC0001:
    // -----------------------------
    // 0x10 for FT810  // Res. Touch
    // 0x11 for FT811  // Cap. Touch
    // 0x12 for FT812  // Res. Touch
    // 0x13 for FT813  //*Cap. Touch
    // 0x15 for BT815  //*Cap. Touch
    // 0x16 for BT816  // Res. Touch
    EVE_available = 810 + (rd8(0xC0001)&0xf);   // ab V2.02
//  EVE_available = 813;                        // FT813 (bis V2.01)
//  if (rd8(0xC0001)==0x12) EVE_available = 812;// FT812 (bis V2.01)
    if (EVE_available>=815) {                   // BT815 --> CLK kann/sollte von 60MHZ auf 72MHZ erhöht werden:
        host_command(HC_SLEEP);                 // send host command "ACTIVE" to FT813     (notwendig für nachfolgende Einstellungen)
        host_command_par(0x61, 0b01000000 + 6); // CLKSEL Default 12MHZ * 6 = 72 MHZ       (Einstellung setzt SLEEP voraus)
        host_command(HC_ACTIVE);                // send host command "ACTIVE" after "SLEEP"
        delay(5);                               // Delay notwendig (1ms reicht, wir nehmen sicherheitshalber 5ms)
        EVE_frequency = 72000000L;
        wr32(REG_FREQUENCY, EVE_frequency);     // Hinweis: REG_FREQUENCY ist nach EVE Reset = 60000000L
    }

    //  wr16(REG_OUTBITS, 0x1b6);   // statt 8 werden nur 6 Bits an das LCD übermittelt [Flimmern und Verläufe werden dadurch aber nicht besser]
//  wr16(REG_OUTBITS, 0);           // 8 Bits werden an das LCD übermittelt (Default)

#else
    // --- FT801 -------------------
    host_command(HC_CORERST);       // send command to "CORERST" to FT801 (eingeführt ab 07.11.2016, da ohne CORERST bei Engine-Fehler ein PowerOn-Reset notwendig war)
delay(20);
    host_command(HC_CLKEXT);        // send command to "CLKEXT" to FT801
    host_command(HC_ACTIVE);        // send host command "ACTIVE" to FT801
delay(20);
    wr8(REG_CPURESET,1);            // Co-Prozessor Reset (ab V1.03), set REG_CPURESET to 1, to hold the co-processor engine in the reset condition
    wr16(REG_CMD_WRITE,0);          // set REG_CMD_READ and REG_CMD_WRITE to zero
    wr16(REG_CMD_READ,0);
    wr8(REG_CPURESET,0);            // aufheben, set REG_CPURESET to 0, to restart the co-processor engine
#endif   
    // -----------------------------
    FT801_Init_Vars(INCH);            
    // Configure display registers. After reset these registers contain appropriate default values.
    wr16(REG_HSIZE, FT_DispWidth); 
    wr16(REG_VSIZE, FT_DispHeight);
    wr16(REG_HCYCLE, FT_DispHCycle); 
    wr16(REG_HOFFSET, FT_DispHOffset); 
    wr16(REG_HSYNC0, FT_DispHSync0); 
    wr16(REG_HSYNC1, FT_DispHSync1); 
    wr16(REG_VCYCLE, FT_DispVCycle); 
    wr16(REG_VOFFSET, FT_DispVOffset); 
    wr16(REG_VSYNC0, FT_DispVSync0); 
    wr16(REG_VSYNC1, FT_DispVSync1); 
    wr8(REG_SWIZZLE, FT_DispSwizzle); 
    wr8(REG_PCLK_POL, FT_DispPCLKPol); 
    wr8(REG_CSPREAD, FT_DispCSpread); 
    wr8(REG_DITHER, FT_DispDither); 

    wr16(REG_PWM_DUTY, FT_PWM_DUTY);
    wr16(REG_PWM_HZ, FT_PWM_HZ);

    /* write first display list */ 
    wr32(RAM_DL+0,CLEAR_COLOR_RGB(0,0,0));      // schwarz 
    wr32(RAM_DL+4,CLEAR(1,1,1)); 
    wr32(RAM_DL+8,DISPLAY());
    wr8(REG_DLSWAP,DLSWAP_FRAME);   // display list swap
    
#ifdef FT_81X_ENABLE
    wr8(REG_GPIO_DIR,0x80 | rd8(REG_GPIO_DIR)); // Dir of DISP = Output  
    wr8(REG_GPIO,0x080 | rd8(REG_GPIO));        // enable DISP display bit
//  wr8(REG_GPIO_DIR,0xff);         // Dir of DISP = Output  
//  wr8(REG_GPIO,0xff);             // enable DISP display bit
#else    
    wr8(REG_GPIO_DIR,0x80 | rd8(REG_GPIO_DIR)); // Dir of DISP = Output  
    wr8(REG_GPIO,0x080 | rd8(REG_GPIO));        // enable DISP display bit
//  wr8(REG_GPIO_DIR,0x80);         // Dir of DISP = Output  
//  wr8(REG_GPIO,0x080);            // enable DISP display bit
#endif
    wr8(REG_PCLK, FT_DispPCLK);     // after this display is visible on the LCD
    // -----------------------------
    // now SPI CLK can be up to 30MHZ
    //PLIB_SPI_BaudRateSet( SPI_ID_2 , SYS_CLK_PeripheralFrequencyGet(CLK_BUS_PERIPHERAL_2), 20000000 );
    // -----------------------------
    // Touch-Screen Kalibrierdaten (wird nur für TRACKER benötigt) eintragen (müssen einmalig bestimmt worden sein)
    switch(INCH) {
        case 43: // RIVERDI_43_480x272
        default:
            FT_TOUCH_TRANSFORM_A = 0x00006318;
            FT_TOUCH_TRANSFORM_B = 0x00000000;
            FT_TOUCH_TRANSFORM_C = 0xfff58c64;
            FT_TOUCH_TRANSFORM_D = 0xfffffebb;
            FT_TOUCH_TRANSFORM_E = 0x00006656;
            FT_TOUCH_TRANSFORM_F = 0xfff3C568;
            break;
        case 50: // RIVERDI_50_800x480
            FT_TOUCH_TRANSFORM_A = 0x000104e4;
            FT_TOUCH_TRANSFORM_B = 0x00000000;
            FT_TOUCH_TRANSFORM_C = 0xffed2581;
            FT_TOUCH_TRANSFORM_D = 0xfffffee0;
            FT_TOUCH_TRANSFORM_E = 0x00010935;
            FT_TOUCH_TRANSFORM_F = 0xfff13b83;
            break;
        case 70: // RIVERDI_70_800x480
            FT_TOUCH_TRANSFORM_A = 0x000073ce;
            FT_TOUCH_TRANSFORM_B = 0x00000113;
            FT_TOUCH_TRANSFORM_C = 0xfff0f3a0;
            FT_TOUCH_TRANSFORM_D = 0xfffffed4;
            FT_TOUCH_TRANSFORM_E = 0x00007c87;
            FT_TOUCH_TRANSFORM_F = 0xfff695c0;
            break;
    }
    wr32(REG_TOUCH_TRANSFORM_A, FT_TOUCH_TRANSFORM_A);
    wr32(REG_TOUCH_TRANSFORM_B, FT_TOUCH_TRANSFORM_B);
    wr32(REG_TOUCH_TRANSFORM_C, FT_TOUCH_TRANSFORM_C);
    wr32(REG_TOUCH_TRANSFORM_D, FT_TOUCH_TRANSFORM_D);
    wr32(REG_TOUCH_TRANSFORM_E, FT_TOUCH_TRANSFORM_E);
    wr32(REG_TOUCH_TRANSFORM_F, FT_TOUCH_TRANSFORM_F);
    // ----- Überprüfung ob EVE Controller verfügbar ist (bei FT813 bereits erledigt)
    // Die Überprüfung findet erst hier statt, damit der Controller in jedem
    // Falle korrekt initialisiert ist (Display ist dunkel), falls die 
    // Überprüfung aus irgend einem Grund versagen sollte.
    // Achtung:
    // Das Verfahren ist nur für FT80x geeignet, da bei FT81x REG_ID eine 
    // andere Adresse hat und bei Start-Up 300ms gewartet werden muss, bis REG_ID
    // gültig ist (wird als "ready" verwendet). 
#ifndef FT_81X_ENABLE    
    EVE_available = 801;            // FT801
    if (rd8(REG_ID) != 0x7c) {      // sicherheitshalber 2x prüfen
        if (rd8(REG_ID) != 0x7c) {
            EVE_available = 0;      // keine EVE gefunden
            return;                 // nichts mehr zu tun
        }
    }
#endif    
}

// *****************************************************************************
// Prüfung der durch FT801_Init() gesetzten Parameter
// return: Anzahl der Fehler
// *****************************************************************************
int FT801_Init_Check(void)
{
int r=0;
    if (rd16(REG_HSIZE)!=FT_DispWidth) r++; 
    if (rd16(REG_VSIZE)!=FT_DispHeight) r++;
    if (rd16(REG_HCYCLE)!=FT_DispHCycle) r++; 
    if (rd16(REG_HOFFSET)!=FT_DispHOffset) r++; 
    if (rd16(REG_HSYNC0)!=FT_DispHSync0) r++; 
    if (rd16(REG_HSYNC1)!=FT_DispHSync1) r++; 
    if (rd16(REG_VCYCLE)!=FT_DispVCycle) r++; 
    if (rd16(REG_VOFFSET)!=FT_DispVOffset) r++; 
    if (rd16(REG_VSYNC0)!=FT_DispVSync0) r++; 
    if (rd16(REG_VSYNC1)!=FT_DispVSync1) r++; 
    if (rd8(REG_SWIZZLE)!=FT_DispSwizzle) r++; 
    if (rd8(REG_PCLK_POL)!=FT_DispPCLKPol) r++; 
    if (rd8(REG_CSPREAD)!=FT_DispCSpread) r++; 
    if (rd8(REG_DITHER)!=FT_DispDither) r++; 
    
    if (rd16(REG_PWM_HZ)!=FT_PWM_HZ) r++;
    
    if (rd32(REG_TOUCH_TRANSFORM_A)!=FT_TOUCH_TRANSFORM_A) r++;
    if (rd32(REG_TOUCH_TRANSFORM_B)!=FT_TOUCH_TRANSFORM_B) r++;
    if (rd32(REG_TOUCH_TRANSFORM_C)!=FT_TOUCH_TRANSFORM_C) r++;
    if (rd32(REG_TOUCH_TRANSFORM_D)!=FT_TOUCH_TRANSFORM_D) r++;
    if (rd32(REG_TOUCH_TRANSFORM_E)!=FT_TOUCH_TRANSFORM_E) r++;
    if (rd32(REG_TOUCH_TRANSFORM_F)!=FT_TOUCH_TRANSFORM_F) r++;

    return r;
}

// ----- zu Testzwecken --------------------------------------------------------
int DL_TEST_CMD_DL;                     // Füllstand Display-List
int DL_TEST_CIR_BUF;                    // Füllstand Circular Buffer
/* -----------------------------------------------------------------------------
DL_TEST() misst den Füllstand des Circular-Buffers (4095 Bytes max.), als auch
den Füllstand der Display-List (8192 Bytes max., letzteres nur, wenn DL_TESTused
definiert ist. 
Diese Füllstände werden folgendermaßen ermittelt:
    DL_TEST_CIR_BUF = DL_L;     // Füllstand Circular Buffer 4095 max.
  #ifdef DL_TESTused            // Alles weitere nur wenn DL_TESTused definiert
    if (CDLexec()) return;      // Übertragen DL-Buffer --> Circular Buffer
    while (CMD_busy());         // Warten, bis Übertragung tatsächlich fertig
    CDLinit();                  // Init DL-Buffer; dennoch ist Display-List noch
                                // nicht ausgeführt (erst nach dieser Funktion)
    DL_TEST_CMD_DL=rd32(REG_CMD_DL);  // Füllstand Display-List 8192 max.
    // Nun erfolgt Anzeige beider Füllstände links oben, in schwarzer Schrift
    // auf hellgrünem Untergrund, kleinster Font: "DL=1234,CB=1234"
  #endif
Eine dauerhafte Verwendung der Display-List Füllstandsmessung ist nicht
ratsam, da die Seitendarstellungs-Funktion bei der Übertragung der Daten
in den Circular-Buffer und während der Arbeit des Co-Prozessors blockiert ist!
Daher darf DL_TESTused nur zu Testzwecken definiert werden.
Hinweis: der Füllstand des Circular-Buffers wird auch ohne das #define 
ermittelt und in DL_TEST_CIR_BUF abgelegt.
----------------------------------------------------------------------------- */
void DL_TEST(void) {
 char s[20];    
    DL_TEST_CIR_BUF = DL_L;             // Füllstand Circular Buffer 4095 max.
#ifdef DL_TESTused
    if (CDLexec()) return;              // Übertragen des DL-Buffers in den Circular Buffer
    while (CMD_busy());                 // tatsächlich warten, bis Übertragung fertig
    CDLinit();                          // Init DL-Buffer; dennoch ist Display-List noch nicht ausgeführt (erst nach dieser Funktion)
    DL_TEST_CMD_DL=rd32(REG_CMD_DL);    // Füllstand Display-List 8192 max.
    // Links oben, schwarz auf hell-grün: "DL=1234,CB=1234"
    DL(TAG(255));
    DL_CMD_FGCOLOR(0x00FF00);
    DL(COLOR_RGB(0,0,0));
    sprintf(s,"DL=%i,CB=%i",DL_TEST_CMD_DL,DL_TEST_CIR_BUF);
    DL_CMD_BUTTON(0, 0, 122, 18, 18 , OPT_FLAT, s);
#endif
}


// *****************************************************************************
// Grafik-Buffer Funktionen
// *****************************************************************************

// -----------------------------------------------------------------------------
// Löschen des Grafik-Buffers, Nullen der x,y und cx,cy Werte
// -----------------------------------------------------------------------------
void bmp_clear(struct bmp_struct * bmp)
{
int i;
char *cp;
    bmp->x = bmp->y = 0;
    bmp->cx = bmp->cy = 0;
    cp = bmp->bmp;
    for (i=0;i<sizeof(bmp->bmp);i++) *(cp++) = 0;
}

// -----------------------------------------------------------------------------
// Drucke Zeichen z im Textkontext (cx, cy in bmp)
// -----------------------------------------------------------------------------
void bmp_prtc(struct bmp_struct * bmp, char c)
{
uchar uc;
int i,j,x,x8,y;
const char * fontp;

    // Grafik-Position bestimmen (x 0..127, y 0..127:
    x = bmp->cx*6;
    if ((uint)x>127) return;
    x8 = x>>3;
    y = bmp->cy*8;
    if ((uint)y>127) return;
    switch(bmp->font) {
        case 0: fontp = &Font6x8LCD [(uchar)c][0]; break;
        default:
        case 1: fontp = &Font6x8WEST[(uchar)c][0]; break;
        case 2: fontp = &Font6x8EAST[(uchar)c][0]; break;
        case 3: fontp = &Font6x8RUSS[(uchar)c][0]; break;
    }
    // max. 2 Bytes sind nun von diesen 6 Bit "betroffen" (B5..B0)
    switch(x&7) {
        case 0: for(i=0;i<8;i++) {                          // xxxxxx..
                    uc = (uchar)*(fontp++);
                    bmp->bmp[((y+i)<<4)+x8] |= uc<<2;
                }
                break;
        case 2: for(i=0;i<8;i++) {                          // ..xxxxxx
                    uc = (uchar)*(fontp++);
                    bmp->bmp[((y+i)<<4)+x8] |= uc;
                }
                break;
        case 4: for(i=0;i<8;i++) {                          // ....xxxx xx......
                    uc = (uchar)*(fontp++);
                    bmp->bmp[j=(((y+i)<<4)+x8)] |= uc>>2;
                    bmp->bmp[j+1] |= (uc&0x3)<<6;
                }
                break;
        case 6: for(i=0;i<8;i++) {                          // ......xx xxxx....
                    uc = (uchar)*(fontp++);
                    bmp->bmp[j=(((y+i)<<4)+x8)] |= uc>>4;
                    if (bmp->cx<21) bmp->bmp[j+1] |= (uc&0xf)<<4;   // beim 22. Zeichen nur die ersten 2 Bits
                }
                break;
    }
}


// -----------------------------------------------------------------------------
// Drucke String sp im Textkontext (cx, cy in bmp), cx, cy werden danach angepasst
// Spezielle Zeichen im 0-terminierten String (nur wenn wrap >= 0):
//  \r  : CR --> cx=0
//  \n  : LF --> cy++
// wrap :
//  0   : kein Wrap, Zeichen außerhalb des Bereiches werden unterdrückt
//  n   : Zeichen-Wrap, nach dem n. Zeichen erfolgt \r\n
//        Beispiel n=21 --> Wrap nach dem noch vollständig darstellbarem 21. Zeichen
//        Beispiel n=22 --> Wrap nach dem 22. Zeichen, von dem nur die ersten 2 Pixel sichtbar sind. 
//  -n  : wie n, jedoch werden CR und LF nicht beachtet
// -----------------------------------------------------------------------------
void bmp_prts(struct bmp_struct * bmp, char * sp, int wrap)
{
char c;
    if (wrap>=0) { 
        while(c=*(sp++)) {
            switch(c) {
                case '\r':  bmp->cx=0; break;
                case '\n':  bmp->cy++; break;
                default:    if (wrap) {
                                if (bmp->cx>=wrap) {
                                    bmp->cx=0; bmp->cy++;
                                }
                            }
                            if ((bmp->cx<22) && (bmp->cy<16)) {
                                bmp_prtc(bmp,c);
                                bmp->cx++;
                            }
                            break;
            }
        }
    } else {
        wrap *= -1;
        while(c=*(sp++)) {
            if (bmp->cx>=wrap) {
                bmp->cx=0; bmp->cy++;
            }
            if ((bmp->cx<22) && (bmp->cy<16)) {
                bmp_prtc(bmp,c);
                bmp->cx++;
            }
        }
    }
}

// -----------------------------------------------------------------------------
// wie bmp_prts(), jedoch mit bmp_clear()
// -----------------------------------------------------------------------------
void bmp_prts_0(struct bmp_struct * bmp, char * sp, int wrap)
{
    bmp_clear(bmp);
    bmp_prts(bmp, sp, wrap);
}

// -----------------------------------------------------------------------------
// wie bmp_prts(), jedoch mit bmp_clear() und Font-Nummern Übergabe
// -----------------------------------------------------------------------------
void bmp_prts_0_font(struct bmp_struct * bmp, char * sp, int wrap, int font)
{
    bmp_clear(bmp);
    bmp->font = font;
    bmp_prts(bmp, sp, wrap);
}

// -----------------------------------------------------------------------------
void bmp_setPixel(struct bmp_struct * bmp, int x, int y)
{
    bmp->bmp[(y*16)+((x&0x7f)>>3)] |= B7>>(x&7);
}
// -----------------------------------------------------------------------------
void bmp_prtTemp(struct bmp_struct * bmp)
{
int x,y;
    x = (MF40_RUNTIME10/10);    // [s]
    x = (x * 128) / ZeitMax;
    y = ((int)MF40_Ist_Temp * 64) / TEMPSETmax();
    y = 63-y; if (y<0) y=0; if (y>63) y=63;
    bmp_setPixel(bmp,x,y);
}


// *****************************************************************************
// Tasten-Codierungen:
//		'1': Ta_F1
//		'2': Ta_F2
//		'3': Ta_F3
//		'4': Ta_F4
//		'5': Ta_F5
//		'+': Ta_AUF
//		'-': Ta_AB
//		'<': Ta_LINKS
//		'>': Ta_RECHTS
//		'#': Ta_ENTER
//		'M': Ta_MENU
//		'!': Ta_ESC
//		'Q': Ta_ESCESC
//		'S': Ta_SETUP
//		'L': Ta_LOESCH
//		'X': Ta_X
//		'T': Ta_ACK
// REG-D spezifisch:
//		'A': Ta_AUTO
//		'H': Ta_MANU
//		'R': Ta_LR_LOC
//		'E': Ta_REMOTE
// *****************************************************************************
key_append(char key)
{
int i;
    i = strlen(key_string);
    if (i < (sizeof(key_string)-1)) {
        key_string[i++] = key;
        key_string[i++] = 0;
    }
}

// *****************************************************************************
void ShowEllipsen(int x, int y, int vert, int hipos)
// *****************************************************************************
{
    DL(SAVE_CONTEXT());
    DL(BEGIN(POINTS));
    DL(POINT_SIZE(50));
    DL(VERTEX_FORMAT(0));
    if (!vert) {
        // horizontale Ellipsen
        DL(COLOR_RGB(206, 182, 207));
        if (hipos==1) DL(COLOR_RGB(0, 0, 0));
        DL(VERTEX2F(x-15, y));
        if (hipos) DL(COLOR_RGB(206, 182, 207));
        if (hipos==2) DL(COLOR_RGB(0, 0, 0));
        DL(VERTEX2F(x,    y));
        if (hipos) DL(COLOR_RGB(206, 182, 207));
        if (hipos==3) DL(COLOR_RGB(0, 0, 0));
        DL(VERTEX2F(x+15, y));
    } else {
        // vertikale Ellipsen
        if (vert==2) {
            // 2 Ellipsen grau, ausgewählt --> schwarz
            DL(COLOR_RGB(0,0,90));
            if (hipos==2) DL(COLOR_RGB(220,220,220));
            DL(VERTEX2F(x, y-8));
            if (hipos) DL(COLOR_RGB(0,0,90));
            if (hipos==1) DL(COLOR_RGB(220,220,220));
            DL(VERTEX2F(x, y+8));
//            DL(COLOR_RGB(0,0,90));
//            if (hipos==3) DL(COLOR_RGB(220,220,220));
//            DL(VERTEX2F(x, y-15));
//            if (hipos) DL(COLOR_RGB(0,0,90));
//            if (hipos==2) DL(COLOR_RGB(220,220,220));
//            DL(VERTEX2F(x, y));
//            if (hipos) DL(COLOR_RGB(0,0,90));
//            if (hipos==1) DL(COLOR_RGB(220,220,220));
//            DL(VERTEX2F(x, y+15));
        } else {
            // 3 Ellipsen grau, ausgewählt --> schwarz
            DL(COLOR_RGB(206, 182, 207));
            if (hipos==3) DL(COLOR_RGB(0, 0, 0));
            DL(VERTEX2F(x, y-15));
            if (hipos) DL(COLOR_RGB(206, 182, 207));
            if (hipos==2) DL(COLOR_RGB(0, 0, 0));
            DL(VERTEX2F(x, y));
            if (hipos) DL(COLOR_RGB(206, 182, 207));
            if (hipos==1) DL(COLOR_RGB(0, 0, 0));
            DL(VERTEX2F(x, y+15));
        }            
    }
    DL(END());
    DL(RESTORE_CONTEXT());
}

// *****************************************************************************
// FT801 Display: Slider_Dial_Toggle_Keys                             08.11.2016
// *****************************************************************************
void FT801_Display_SliderDialCalc(int first_req)
{
uint32 tracker;
uint32 ttA,ttB,ttC,ttD,ttE,ttF;
uint32 xy;
uint16 x,y;
char sa[40];
char sb[40];
char sc[40];
char sd[40];
char se[40];
char s1[80];
char s2[80];
char zs[20];
char c;
uchar id;
static  int     docal;
static  char    cl;
static  int     zahl, zahl_x;
static  char    op, clrnext;
static  uint16  dial=8000;
static  uint16  toggle=30000;
static  uint16  scroll=64000;
static  char    foundkey=' ';
static  int     first;
    // .........................................................................
    if (first_req) {
        first = 10;             // 10 Durchläufe keine Touch-Events auswerten
        TestMode = TestMode?0:1;
    }
    // .........................................................................
#ifdef FT_81X_ENABLE  
    if (docal>1) {
        if (rd16(REG_CMD_WRITE)!=rd16(REG_CMD_READ)) return;  // warten, bis CMD Fifo abgearbeitet
        docal=0;
    } 
#endif
    // .........................................................................
    if (CMD_busy()) return;     // warten, bis CMD Fifo abgearbeitet
    wr8(REG_CTOUCH_EXTENDED,0); // 0-->Extended Mode einschalten
    // .........................................................................
    id = rd8(REG_ID);           // liefert generell 0x7c 
    xy = rd32(REG_CTOUCH_TOUCH0_XY);
    x = xy >> 16;
    y = xy & 0xffff;
    if (x==32768)   sprintf(sa,"REG_ID = 0x%02x (0x%02x)",id,foundkey);
//  if (x==32768)   sprintf(sa,"x1= ---, y1= --- (0x%02x)",foundkey);
    else            sprintf(sa,"x1=%03i, y1=%03i (0x%02x)",(int16)x,(int16)y,foundkey);
    xy = rd32(REG_CTOUCH_TOUCH1_XY);
    x = xy >> 16;
    y = xy & 0xffff;
    sprintf(sb,x==32768?"":"x2=%03i, y2=%03i",(int16)x,(int16)y);
    xy = rd32(REG_CTOUCH_TOUCH2_XY);
    x = xy >> 16;
    y = xy & 0xffff;
    sprintf(sc,x==32768?"":"x3=%03i, y3=%03i",(int16)x,(int16)y);
    xy = rd32(REG_CTOUCH_TOUCH3_XY);
    x = xy >> 16;
    y = xy & 0xffff;
    sprintf(sd,x==32768?"":"x4=%03i, y4=%03i",(int16)x,(int16)y);
    x = rd16(REG_CTOUCH_TOUCH4_X);
    y = rd16(REG_CTOUCH_TOUCH4_Y);
    sprintf(se,x==32768?"":"x5=%03i, y5=%03i",(int16)x,(int16)y);

    ttA = rd32(REG_TOUCH_TRANSFORM_A);
    ttB = rd32(REG_TOUCH_TRANSFORM_B);
    ttC = rd32(REG_TOUCH_TRANSFORM_C);
    ttD = rd32(REG_TOUCH_TRANSFORM_D);
    ttE = rd32(REG_TOUCH_TRANSFORM_E);
    ttF = rd32(REG_TOUCH_TRANSFORM_F);
    sprintf(s1,"ttA=%08lx,ttB=%08lx,ttC=%08lx",ttA,ttB,ttC);
    sprintf(s2,"ttD=%08lx,ttE=%08lx,ttF=%08lx",ttD,ttE,ttF);

    sprintf(zs,"%li",zahl);
    // .........................................................................
    CDLinit();
    DL(CMD_DLSTART);
    // .........................................................................
    DL(CLEAR(1, 1, 1));
    DL_CMD_FGCOLOR(0x1F3F7F);   // Standard-Foreground-Color, wird nicht mit CLEAR(1,1,1) auf Default gesetzt
    if (docal) {
#ifdef FT_81X_ENABLE        
        docal++;
#else        
        docal=0;
#endif
        wr8(REG_CTOUCH_EXTENDED,1); // Extended Mode ausschalten
        DL_CMD_CALIBRATE(0);
        DL(DISPLAY());
        DL(CMD_SWAP);
        CDLexec();
        return;
    }
    DL(SAVE_CONTEXT());         // aktuell hier nicht notwendig...

    DL_CMD_FGCOLOR(0xFF0000);
    DL_CMD_BGCOLOR(0xD89000);
    DL_CMD_GRADIENT(23, 49, 0xF8FFB2, 24, 198, 0xAA00FF);
    // ~~~~~
    DL_CMD_TRACK(230, 130, 1,1, 0x11);
    DL(TAG(0x11));
    DL_CMD_DIAL(230, 130, 71, 0, dial);
    // ~~~~~
    DL_CMD_TRACK(51,69, 100,27, 0x16);
    DL(TAG(0x16));
    DL_CMD_TOGGLE(51, 69, 100, 27, 0, toggle,"RUN|CAL");
    // ~~~~~
    DL_CMD_TRACK(46,22, 392,20, 0x13);
    DL(TAG(0x13));
    DL_CMD_SCROLLBAR(46, 22, 392, 20, 0, scroll, 1404, 65535);
    // ~~~~~
    DL(TAG(0x14));
    DL_CMD_NUMBER(230, 202, 28, OPT_CENTERX, dial);
    DL(TAG(0x15));
    DL_CMD_BUTTON(4, 112, 26, 37, 27, 0, "<");
    DL(TAG(0x1F));
    DL_CMD_TEXT(12, 180, 26, 0, sa);
    DL_CMD_TEXT(12, 195, 26, 0, sb);
    DL_CMD_TEXT(12, 210, 26, 0, sc);
    DL_CMD_TEXT(12, 225, 26, 0, sd);
    if (se[0]) {
        DL_CMD_TEXT(12, 240, 26, 0, se);
        DL_CMD_TEXT(12, 255, 18, 0, "");
    } else {
        DL_CMD_TEXT(12, 240, 18, 0, s1);
        DL_CMD_TEXT(12, 255, 18, 0, s2);
    }
    DL_CMD_FGCOLOR(0x3EFF30);
    DL(COLOR_RGB(7, 7, 3));
    DL_CMD_KEYS(327, 100, 140, 37, 29, 'y', "789*");
    DL_CMD_KEYS(327, 140, 140, 37, 29, 'y', "456-");
    DL_CMD_KEYS(327, 180, 140, 37, 29, 'y', "123+");
    DL_CMD_KEYS(327, 220, 140, 37, 29, 'y', "<0C=");
    DL_CMD_TEXT(458, 66, 28, OPT_RIGHTX, zs);
    DL(RESTORE_CONTEXT());      // aktuell hier nicht notwendig...
    DL(DISPLAY());
    // .........................................................................
    DL(CMD_SWAP);
    if (CDLexec()) return;
    // ~~~~~
    // Tracker für dial toggle und scroll
    tracker = rd32(REG_TRACKER);
    if (scroll<4000) scroll+=50;
    if (!first) switch(tracker&0xff) {
        case 0x11: dial = tracker>>16; break;
        case 0x16: toggle = tracker>>16; if (!toggle) {toggle=0x8000;docal=1;} break;
        case 0x13: scroll = tracker>>16; break;
        case 0x15: hub_state=hub_News; return;  // Back-Button
    }
    FT_PWM_DUTY = (scroll/511);                 // 0..65535 --> 0..128
    wr16(REG_PWM_DUTY, FT_PWM_DUTY);
    // Tastatur (verwendet nicht den TRACKER, aber den TOUCH_TAG)
    if ((c=rd8(REG_TOUCH_TAG))>0) {
        if (first) cl=c;
        if (cl!=c) {
            foundkey=c;
            switch(foundkey) {
                case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                    if (clrnext) {clrnext=0; zahl=0;} 
                    if (abs(zahl)<99999999) { // max. 9 Stellen
                        if (zahl>=0) zahl = (zahl*10)+(c-'0');
                        else         zahl = -1*((zahl*-10)+(c-'0'));
                    }
                    break;
                case '+': zahl_x=zahl; op=c; clrnext++; break;
                case '-': zahl_x=zahl; op=c; clrnext++; break;
                case '*': zahl_x=zahl; op=c; clrnext++; break;
                case '=': switch(op) {
                            case '+': zahl = zahl_x+zahl; clrnext++; break; 
                            case '-': zahl = zahl_x-zahl; clrnext++; break; 
                            case '*': zahl = zahl_x*zahl; clrnext++; break; 
                          }
                          break;
                case '<': zahl /= 10; break;
                case 'C': zahl=0; op=0; break;
            }
        }
    }
    cl=c;
    if (first) first--;
}

// *****************************************************************************
// Entfernen der hinteren Nullen einer Gleitkommazahl
// Wenn Nullen entfernt werden (Kriterium: im String ist ein '.'), wird bei
// Bedarf auch der Dezimalpunkt entfernt.
// *****************************************************************************
void RemoveTrailingZeros(char * zs)
{
char * cp;
    if (strstr(zs,".")) {
        cp = &zs[strlen(zs)-1];
        while(*(cp)=='0') *(cp--)=0;
        if(*cp=='.') *cp=0;
    }
}

// *****************************************************************************
// Drucke die double64 zahl nach dp mit bis zu 10 Nachkommastellen
// *****************************************************************************
int DruckeZahl(char * dp, long double zahl)
{
int len;
    if ((zahl>1E10)||(zahl<-1E10)) {strcpy(dp,"Error"); return 1;}
    sprintf(dp,"%.9Lf",zahl);     // Erst ab XC32 V1.44 funktioniert "lf" oder "LF" (gleichwertig) bei long double (64 Bit), zuvor war Ausgabe von long double eigentlich nicht möglich!
//  sprintf(dp,"%.9Lf",zahl);   // Erst ab XC32 V1.44 funktioniert "lf" oder "LF" (gleichwertig) bei long double (64 Bit), zuvor war Ausgabe von long double eigentlich nicht möglich!
                                // Hinweis: die Compiler-Option -fno-short-double wird nicht (mehr) verwendet.
    if ((len=strlen(dp))>14) {
        len=(14+10)-len; if (len<0) len=0; 
        sprintf(dp,"%.*Lf",len,zahl);
    }
    RemoveTrailingZeros(dp);
    return 0;
//void Test(void) {    
//    float ff;
//    long double FF;
//    char dest[50];
//
//    FF = 1.73205080756887;      // FF = sqrt(3);
//    ff = float(FF);
//    sprintf(dest,"%.4f,ff");    // --> "1.7321"       *** OK ***
//    sprintf(dest,"%.4Lf,FF");   // --> "Lf"           !!! ERROR !!!
//    sprintf(dest,"%.4f,FF");    // --> "-4294967295"  !!! expected error !!!
//}
}

// *****************************************************************************
// ZEITSTRING zs in RTC schreiben
// mode     : 1 --> ZEIT, 2 --> DATUM, 3 --> ZEIT oder DATUM
// return   : 0:ok, sonst Err
// *****************************************************************************
// 06.02.2019 HR - mode 3 --> ZEIT oder DATUM
// 30.11.2017 HR - Erstellt
// *****************************************************************************
int RTC_Schreiben(int mode,char *zs)
{
int slen, r=0;
char s[20],rs[20];
    s[0]=0;
    slen = strlen(zs);
    strcpy(s,zs);
    if (mode==3) {
        if (slen >= 7) mode=2; else mode=1;
    }
    switch(mode) {
        case 1: // ZEIT
            if (slen==4) {s[0]='0'; s[1]='0'; strcpy(s+2,zs);slen+=2;}
            if (slen==5) {s[0]='0'; strcpy(s+1,zs);slen++;}
            if (slen==6) {
                rs[0]=((s[4]-'0')<<4)+(s[5]-'0');
                rs[1]=((s[2]-'0')<<4)+(s[3]-'0');
                rs[2]=(((s[0]-'0')<<4)+(s[1]-'0'))|B7;
                RTCwriten(rs,0,3);
            }
            break;
        case 2: // DATUM
            if (slen==7) {s[0]='0'; strcpy(s+1,zs);slen++;}
            if (slen==8) {s[4]=s[6];s[5]=s[7];s[6]=0;slen=6;}
            if (slen==5) {s[0]='0'; strcpy(s+1,zs);slen++;}
            if (slen==6) {
                rs[0]=((s[0]-'0')<<4)+(s[1]-'0');
                rs[1]=((s[2]-'0')<<4)+(s[3]-'0');
                rs[2]=((s[4]-'0')<<4)+(s[5]-'0');
                RTCwriten(rs,4,3);
            }
            break;
    }
    return r;
}

// *****************************************************************************
// FT801 Display: U1604 "Nachrichten-Format",                         19.11.2016
// Hinweis: Abgeleitet von "Erster Test mit Touch-Erkennung und 4 Buttons"
// *****************************************************************************
void FT81X_Display_Calculator(int first_req)
{
uint32 tracker;
uint32 xy;
uint16 x,y,xd,yd, bx,bxd, w,h;
int i,a, lauf;  
uint32 zeit;
uint32 ui32;
uint16 zeitms999;
uint32 col1,col2;
uint16 pwm;
uint16 offx=0, offy=0;
char c, *cp;
char s[50], zs[50];
char sinfo[80];
char sd[20];
char sh[20];
char sz[20];
float   ff;
static  int         ft_state=1;
static  char        cl;
static  uint        downcnt;
static  int         first;
static  char        touchkey, tk;
static  long double zahl, zahl_x, zahl_y;
static  char        zahlenstr[40];
static  char        op, clrnext;
static  char        sequence[10];
    // .........................................................................
    if (first_req) {
        first = 10;             // 10 Durchläufe keine Touch-Events auswerten
        ft_state = 2;           // "CALC"
        if (DruckeZahl(zahlenstr, zahl)) clrnext++;
        touchkey=tk=0;
        downcnt=0;
        getRotaryUpDown();getRotarySwitch();// zu Beginn alte, noch anstehende Anforderungen abräumen
        sequence[0]=0;
    }
    // .........................................................................
    if (CMD_busy()) return;     // warten, bis CMD Fifo abgearbeitet
    wr8(REG_CTOUCH_EXTENDED,0); // 0-->Extended Mode einschalten
    // .........................................................................
    zeit = utctime_Master;
    zeit += (int32)etz4_Master * 3600 / 4;
    ui32 = (uint32)RTC_MilliSekunden_Master * 1000;
    ui32 /= 1024;
    zeitms999 = ui32;
    lauf = ((zeit%60)*1000)+zeitms999;  // 0..59999
    lauf %= 3000;                       // 0..3000 in 3s
    // .........................................................................
    CDLinit();
    DL(CMD_DLSTART);
    // .........................................................................
    col1=0x1F3F7F;              // normaler Standard-Foreground
//  col2=0x269799;              // verdunkelt
//  col2=0x0C1934;              // verdunkelt
    col2=0x3C9AFE;              // aufgehellter Standard-Foreground
    DL(CLEAR(1, 1, 1));
    DL_CMD_FGCOLOR(col1);       // Standard-Foreground-Color, wird nicht mit CLEAR(1,1,1) auf Default gesetzt
    DL_CMD_BGCOLOR(0xD89000);   // Tasten-gedrückt Farbe: hellbraun

    lauf /= 4;                          // 0..750 in 3s
    a = 12+lauf;
    offx=(800-480)/2; offy=0;           // Offsets für News-Bereich (Buttons und Text)
    DL_CMD_GRADIENT(23, 42, 0x9EA8CA, 23, 262, 0x070AA8);
//  DL_CMD_GRADIENT(23, 42, 0xFF0123, 23, 262, 0xD21DFF);
    DL_CMD_TEXT(238+offx, 435, 27, OPT_CENTERX, "Calculator");
    bx=12+offx-60; bxd=119;     // bisher bei 4 Buttons: 12,131,250,371+offx
    DL(TAG(0x11));
    DL_CMD_FGCOLOR(((ft_state==1)?col2:col1));
    DL_CMD_BUTTON(bx, 395, 95, 32, 30,  (ft_state==1)?OPT_FLAT:0, "INFO"); bx+=bxd;
    DL(TAG(0x12));
    DL_CMD_FGCOLOR(((ft_state==2)?col2:col1));
    DL_CMD_BUTTON(bx, 395, 95, 32, 30, (ft_state==2)?OPT_FLAT:0, "CALC"); bx+=bxd;
    DL(TAG(0x15));
    DL_CMD_FGCOLOR(col1);
    DL_CMD_BUTTON(bx, 395, 95, 32, 30, 0,                        "SETUP"); bx+=bxd;
    DL(TAG(0x13));
    DL_CMD_FGCOLOR(((ft_state==3)?col2:col1));
    DL_CMD_BUTTON(bx, 395, 95, 32, 30, (ft_state==3)?OPT_FLAT:0, "TEST"); bx+=bxd;
    DL(TAG(0x14));
    DL_CMD_FGCOLOR(((ft_state==4)?col2:col1));
    DL_CMD_BUTTON(bx, 395, 95, 32, 30, (ft_state==4)?OPT_FLAT:0, ">>>"); bx+=bxd;
    DL_CMD_FGCOLOR(col1);
    DL(TAG(0x19));
//  DL_CMD_CLOCK(120, 120, 100, OPT_NOBACK, (zeit%86400)/3600, (zeit%3600)/60, zeit%60, zeitms999);
//  sprintf(sz,"%02i:%02i:%02i",(zeit%86400)/3600, (zeit%3600)/60, zeit%60);
//  DL_CMD_TEXT(120, 210, 28, OPT_CENTERX, sz);
    RTCreadn(sd, 0, 7);     // BCD : 0:SS 1:MM 2:24HH 3:-- 4:DD 5:mm 6:YY
    DL_CMD_CLOCK(120, 120, 100, OPT_NOBACK, BCD2i(sd[2]&0x7f), BCD2i(sd[1]), BCD2i(sd[0]), 0);
    sprintf(sz,"%02i:%02i:%02i",BCD2i(sd[2]&0x7f),BCD2i(sd[1]),BCD2i(sd[0]));
    DL(TAG(0x1A));
    DL_CMD_TEXT(120, 208, 29, OPT_CENTERX, sz);
    sprintf(sz,"%02i.%02i.%02i",BCD2i(sd[4]),BCD2i(sd[5]),BCD2i(sd[6])+2000);
    DL(TAG(0x1B));
    DL_CMD_TEXT(120, 236, 29, OPT_CENTERX, sz);

    DL(TAG(0));
    DL(SAVE_CONTEXT());
    DL_CMD_FGCOLOR(0xE511A0);
    DL(COLOR_RGB(255, 247, 14));
    DL(SCISSOR_XY(10, 0));
    DL(SCISSOR_SIZE(780, 480));
    DL(VERTEX_FORMAT(0));
    DL(BEGIN(LINES));
    DL(VERTEX2F(12, 463));
    DL(VERTEX2F(a, 463));
    DL(VERTEX2F(a+20, 463));
    DL(VERTEX2F(a+20+60, 463));
    DL(VERTEX2F(a+20+60+20, 463));
    DL(VERTEX2F(790, 463));
    DL(END());
    offx=400-(302/2); offy=8  ;                   // neue Offsets für Calculator
    DL(RESTORE_CONTEXT());

    DL(SAVE_CONTEXT());
    DL(SCISSOR_XY(offx, offy));
    DL(SCISSOR_SIZE(302, 362));
    DL_CMD_GRADIENT(475+offx, 107+offy, 0xFFFFFF, 475+offx, 285+offy, 0x828282); // weiß->grau
    
    DL_CMD_FGCOLOR(0x38B114);               // Tasten Farbe: GMC-grün
    DL_CMD_BGCOLOR(0x144207);               // Tasten-gedrückt Farbe: dunkelgrün
    x=21+offx; y=70+offy; yd=57; w=260; h=52;
//  x=290+offx; y=110+offy; yd=50; w=220; h=45;
    switch(tk) {
        default:   DL_CMD_KEYS(x, y, w, h, 29, tk,  "C  /"); break;
        case 0x17: DL_CMD_KEYS(x, y, w, h, 29, ' ', "C ./"); break;
        case 0x18: DL_CMD_KEYS(x, y, w, h, 29, '.', "C ./"); break;
    } y += yd;
    DL_CMD_KEYS(x, y, w, h, 29,  tk, "789*"); y += yd;
    DL_CMD_KEYS(x, y, w, h, 29, tk, "456-"); y += yd;
    DL_CMD_KEYS(x, y, w, h, 29, tk, "123+"); y += yd;
    DL_CMD_KEYS(x, y, w, h, 29, (tk==0x16)?' ':tk,  " 0.=");
    DL(TAG(0x16));
    DL(COLOR_RGB(0, 0, 0));
    DL_CMD_TEXT(x+13, y+9, 29, 0, "+/-");
    DL(COLOR_RGB(255, 255, 255));
    DL_CMD_TEXT(x+13+1, y+9+1, 29, 0, "+/-");
    DL(TAG(0x17));
    DL(COLOR_RGB(0, 0, 0));
    DL_CMD_TEXT(x+20+(w/4), y+9-(yd*4), 29, 0, "<<");
    DL(COLOR_RGB(255, 255, 255));
    DL_CMD_TEXT(x+20+(w/4)+1, y+9-(yd*4)+1, 29, 0, "<<");
    DL(TAG(0x18));
    DL(COLOR_RGB(0, 0, 0));
    DL_CMD_TEXT(x+16+(w/2), y+9-(yd*4), 29, 0, "Sqr");
    DL(COLOR_RGB(255, 255, 255));
    DL_CMD_TEXT(x+16+(w/2)+1, y+9-(yd*4)+1, 29, 0, "Sqr");
    DL(COLOR_RGB(0, 0, 0));
    DL(TAG(0));
//  DL(TAG(0x1A));
    DL_CMD_TEXT(offx+w+10, offy+27, 30, OPT_RIGHTX, zahlenstr);
    DL(RESTORE_CONTEXT()); // notwendig wegen DL_TEST() bei eventueller Füllstands-Anzeige links oben
    // .........................................................................
    DL_TEST();
    DL(DISPLAY());
    // .........................................................................
    DL(CMD_SWAP);
    if (CDLexec()) return;
    // ~~~~~
    // Tastatur (verwendet nicht den TRACKER, aber den TOUCH_TAG)
    if ((c=rd8(REG_TOUCH_TAG))>0) {
        if (first) {
            cl=c;
            downcnt=0;
        }
        downcnt++;
        if (cl!=c) {
            if ((i=strlen(sequence))<sizeof(sequence)-2) {sequence[i]=c; sequence[++i]=0;}
            if (!strcmp(sequence,"C/*-+=")) {hub_state=hub_Setup; return;}
            touchkey=c;
            switch(c) {
                case 0x11: hub_state=hub_News; return;
                case 0x12: break;
                case 0x13: hub_state=hub_News; ft_state_force=3; return;
                case 0x14: hub_state=hub_return; return;                // Back-Button
                case 0x15: hub_state=hub_UserSetup1; return;             // SETUP-Button
                // .............................................................
                case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                            if (clrnext) {clrnext=0; sprintf(zahlenstr,"%c",c); break;}
                            cp=zahlenstr;
                            i=strlen(cp);
                            if (i>14) break;
                            if (!strcmp(cp,"0")) {*cp=c; break;}
                            cp += i;
                            *(cp++)=c;
                            *(cp++)=0;
                            break;
                case '.':   if (clrnext) {clrnext=0; strcpy(zahlenstr,"0."); break;}
                            cp=zahlenstr;
                            i=strlen(cp);
                            if (i>14) break;
                            if (strstr(cp,".")) break;
                            cp += i;
                            *(cp++)='.';
                            *(cp++)=0;
                            break;
                case '+':   zahl=zahl_x=_datof(zahlenstr); op=c; clrnext++; break;
                case '-':   zahl=zahl_x=_datof(zahlenstr); op=c; clrnext++; break;
                case '*':   zahl=zahl_x=_datof(zahlenstr); op=c; clrnext++; break;
                case '/':   zahl=zahl_x=_datof(zahlenstr); op=c; clrnext++; break;
                case '=':   if (op&B7) {
                                switch(op&0x7f) {
                                    case '+': zahl += zahl_y; clrnext++; goto PZAHL; 
                                    case '-': zahl -= zahl_y; clrnext++; goto PZAHL; 
                                    case '*': zahl *= zahl_y; clrnext++; goto PZAHL; 
                                    case '/': zahl /= zahl_y; if (zahl_y==0) zahl=1E20; clrnext++; goto PZAHL; 
                                }
                            } else {
                                op |= B7;
                                switch(op&0x7f) {
                                    case '+': zahl = zahl_x + (zahl_y=_datof(zahlenstr)); clrnext++; goto PZAHL; 
                                    case '-': zahl = zahl_x - (zahl_y=_datof(zahlenstr)); clrnext++; goto PZAHL; 
                                    case '*': zahl = zahl_x * (zahl_y=_datof(zahlenstr)); clrnext++; goto PZAHL; 
                                    case '/': zahl = zahl_x / (zahl_y=_datof(zahlenstr)); if (zahl_y==0) zahl=1E20; clrnext++; goto PZAHL; 
                                }
                            }
                            break;
                case 'C':   if (zahl==0) op=0;
                            zahl=0;                     // C - CLEAR
                            strcpy(zahlenstr,"0");
                            break;
                case 0x16:  cp=zahlenstr;               // +/-
                            i=strlen(cp);
                            if (*cp=='-') {
                                strcpy(s,cp+1);
                                strcpy(cp,s);
                                if (*cp==0) strcpy(cp,"0");
                                break;
                            }
                            if (i>14) break;
                            if (!strcmp(cp,"0")) break;
                            if (!strcmp(cp,"-")) {strcpy(cp,"0"); break;}
                            if (strstr(cp,"-")) break;
                            strcpy(s+1,zahlenstr); s[0]='-';
                            strcpy(zahlenstr,s);
                            break;
                case 0x17:  cp=zahlenstr;               // <<
                            i=strlen(cp);
                            if (i>1) {
                                cp += i-1;
                                *cp = 0;
                                if (!strcmp(zahlenstr,"-")) zahlenstr[0]='0';
                            } else {
                                strcpy(zahlenstr,"0");
                            }
                            clrnext=0;                  // nach << können wieder Zeichen angehängt werden (ab 06.02.2019)
                            break;
                case 0x18:                              // Sqr
                            zahl = _datof(zahlenstr);
//                          if (zahl<0) zahl=0;         // Fehlerbetrachtung ohne Fehler (-->0)
                            if (zahl>=0.0) zahl = sqrt(zahl); else zahl=1e20;
                            clrnext++;
 PZAHL:                     if (DruckeZahl(zahlenstr, zahl)) clrnext++;
                            break;  
                case ' ':   touchkey=0; break;          // Leerzeichen-Tasten, die mit anderer Beschriftung überlagert sind, lösen ab und an dieses Event aus
                //......................................
#ifdef UHRZEIT_VIA_CALC
                case 0x19:  // TIME or DATE
                            RTC_Schreiben(3,zahlenstr); clrnext++; break;
                case 0x1A:  // TIME
                            RTC_Schreiben(1,zahlenstr); clrnext++; break;
                case 0x1B:  // DATE
                            RTC_Schreiben(2,zahlenstr); clrnext++; break;
#endif
            }
        }
    } else {
        downcnt=0;
    }
    if (downcnt) tk = touchkey; else tk=0;
    cl=c;
//  if (STOP_Taster_ms>1000) hub_state=hub_Setup; 
    // ----- Swipe windows
    if (SwipeWindows(first, 0)) return;

    if (first) first--;
}

// *****************************************************************************
// FT81X Display: MF40 "Nachrichten-Format",                          04.09.2017
// Hinweis: Abgeleitet von "Erster Test mit Touch-Erkennung und 4 Buttons"
// *****************************************************************************
void FT81X_Display_News_MF40(int first_req)
{
uint32 tracker;
int a, lauf;  
uint32 xy;
uint16 x,y,xd,yd, bx,bxd;
uint32 zeit;
uint32 ui32;
uint16 zeitms999;
uint32 col1,col2;
uint16 offx=0, offy=0;
uint16 pwm;
int i,j;
char c;
char s[120];
char sinfo[80];
char sa[40];
char sb[40];
char sc[40];
char sd[40];
char se[40];
char sh[40];
char sz[40];

static  int     ft_state=1;
static  uint16  dial=57700;             // 7700..57700  Delta=50000
static  char    cl;
static  uint    downcnt;
static  uint    testcnt;
//static  uint    testext;
static  int     first;
static  int     showTimesetHelp;
    // .........................................................................
    if (first_req) {
        first = 10;                         // 10 Durchläufe keine Touch-Events auswerten
        if (ft_state_force) {
            ft_state = ft_state_force;
            ft_state_force = 0;
        } else {
            ft_state = 1;
        }
        showTimesetHelp=0;
        getRotaryUpDown();getRotarySwitch();// zu Beginn alte, noch anstehende Anforderungen abräumen
    }
    // .........................................................................
    if (CMD_busy()) return;     // warten, bis CMD Fifo abgearbeitet
    wr8(REG_CTOUCH_EXTENDED,0); // 0-->Extended Mode einschalten
    // .........................................................................
    zeit = utctime_Master;
    zeit += (int32)etz4_Master * 3600 / 4;
    ui32 = (uint32)RTC_MilliSekunden_Master * 1000;
    ui32 /= 1024;
    zeitms999 = ui32;
    lauf = ((zeit%60)*1000)+zeitms999;  // 0..59999
    lauf %= 3000;                       // 0..3000 in 3s
    // .........................................................................
    xy = rd32(REG_CTOUCH_TOUCH0_XY);
    x = xy >> 16;
    y = xy & 0xffff;
    if (x!=32768) {
        downcnt++;
        if (testcnt) if (++testcnt>100) {hub_state=hub_SliderDialCalc; testcnt=0; return;}
    } else {
        downcnt=0;
        testcnt=0;
    }
    if (ft_state==3) {
        if (EVE_available>=810) {
            i=sprintf(sinfo,"TFT Display 800x480, 16.7M");
        } else { 
            i=sprintf(sinfo,"TFT Display 480x272, 262K");
        }
        i+sprintf(sinfo+i," Colors, 5x Multi-Touch (%s%i %iMHz)",(EVE_available>=815)?"BT":"FT",EVE_available,EVE_frequency/1000000L);
        sprintf(sa,x==32768?"x1= ---, y1= ---":"x1=%03i, y1=%03i",(int16)x,(int16)y);
        xy = rd32(REG_CTOUCH_TOUCH1_XY);
        x = xy >> 16;
        y = xy & 0xffff;
        sprintf(sb,x==32768?"":"x2=%03i, y2=%03i",(int16)x,(int16)y);
        xy = rd32(REG_CTOUCH_TOUCH2_XY);
        x = xy >> 16;
        y = xy & 0xffff;
        sprintf(sc,x==32768?"":"x3=%03i, y3=%03i",(int16)x,(int16)y);
        xy = rd32(REG_CTOUCH_TOUCH3_XY);
        x = xy >> 16;
        y = xy & 0xffff;
        sprintf(sd,x==32768?"":"x4=%03i, y4=%03i",(int16)x,(int16)y);
        x = rd16(REG_CTOUCH_TOUCH4_X);
        y = rd16(REG_CTOUCH_TOUCH4_Y);
        sprintf(se,x==32768?"":"x5=%03i, y5=%03i",(int16)x,(int16)y);
    } else {
        if (MF40_Setup.SN) sprintf(sinfo,"ALPHOTRONIC GmbH - %s  V%s  %s HR - S/N:%i",SetDeviceString,SetVersionString,SetVersionDateString,MF40_Setup.SN);
        else               sprintf(sinfo,"ALPHOTRONIC GmbH - %s  V%s  %s HR",SetDeviceString,SetVersionString,SetVersionDateString); 
//      strcpy(sinfo,"ALPHOTRONIC GmbH - PIC-MF40 V2.00 04.10.2017");
        sa[0]=sb[0]=sc[0]=sd[0]=se[0]=0;
    }
    pwm=dial;               // 7700..57700
    pwm-=7700;              // 0..50000
    pwm/=500-1;             // 0..100%
    sprintf(sh,"%i%%",pwm);
    // .........................................................................
    CDLinit();
    DL(CMD_DLSTART);
    // .........................................................................
    col1=0x1F3F7F;              // normaler, blauer Standard-Foreground
//  col2=0x269799;              // verdunkelt (cyan)
//  col2=0x0C1934;              // verdunkelt (dunkelblau)
    col2=0x3C9AFE;              // aufgehellter Standard-Foreground
    DL(CLEAR(1, 1, 1));
    DL_CMD_FGCOLOR(col1);       // Standard-Foreground-Color, wird nicht mit CLEAR(1,1,1) auf Default gesetzt
#ifdef FT_81X_ENABLE
    lauf /= 4;                  // 0..750 in 3s
    a = 12+lauf;
    offx=(800-480)/2; offy=0;   // Offsets für News-Bereich
    DL_CMD_GRADIENT(23, 42, 0x9EA8CA, 23, 262, 0x070AA8);
//  DL_CMD_GRADIENT(23, 42, 0xFF0123, 23, 262, 0xD21DFF);
    DL_CMD_TEXT(238+offx, 435, 27, OPT_CENTERX, sinfo);
    bx=12+offx-60; bxd=119;     // bisher bei 4 Buttons: 12,131,250,371+offx
    DL(TAG(0x11));
    DL_CMD_FGCOLOR(((ft_state==1)?col2:col1));
    DL_CMD_BUTTON(bx, 395, 95, 32, 30,  (ft_state==1)?OPT_FLAT:0, "INFO"); bx+=bxd;
    DL(TAG(0x12));
    DL_CMD_FGCOLOR(((ft_state==2)?col2:col1));
    DL_CMD_BUTTON(bx, 395, 95, 32, 30, (ft_state==2)?OPT_FLAT:0, "CALC"); bx+=bxd;
    DL(TAG(0x15));
    DL_CMD_FGCOLOR(col1);
    DL_CMD_BUTTON(bx, 395, 95, 32, 30, 0,                        "SETUP"); bx+=bxd;
    DL(TAG(0x13));
    DL_CMD_FGCOLOR(((ft_state==3)?col2:col1));
    DL_CMD_BUTTON(bx, 395, 95, 32, 30, (ft_state==3)?OPT_FLAT:0, "TEST"); bx+=bxd;
    DL(TAG(0x14));
    DL_CMD_FGCOLOR(((ft_state==4)?col2:col1));
    DL_CMD_BUTTON(bx, 395, 95, 32, 30, (ft_state==4)?OPT_FLAT:0, ">>>");
    DL_CMD_FGCOLOR(col1);
    DL(TAG(0x16));
    x=400;y=40;yd=30;
    DL_CMD_TEXT(x, y, 28, OPT_CENTERX, sa); y+=yd;
    DL_CMD_TEXT(x, y, 28, OPT_CENTERX, sb); y+=yd;
    DL_CMD_TEXT(x, y, 28, OPT_CENTERX, sc); y+=yd;
    DL_CMD_TEXT(x, y, 28, OPT_CENTERX,sd); y+=yd;
    DL_CMD_TEXT(x, y, 28, OPT_CENTERX,se); y+=yd;
//  DL_CMD_CLOCK(120, 120, 100, OPT_NOBACK, (zeit%86400)/3600, (zeit%3600)/60, zeit%60, zeitms999);
//  sprintf(sz,"%02i:%02i:%02i",(zeit%86400)/3600, (zeit%3600)/60, zeit%60);
//  DL_CMD_TEXT(120, 210, 28, OPT_CENTERX, sz);
#ifdef NO_RTC_USE
    for (i=0;i<7;i++) sd[i]=0;
#else
    RTCreadn(sd, 0, 7);     // BCD : 0:SS 1:MM 2:24HH 3:-- 4:DD 5:mm 6:YY
#endif
    DL(TAG(0x17));
    if (ft_state==3) {
        DL_CMD_CLOCK(120, 120, 100, OPT_NOBACK, BCD2i(sd[2]&0x7f), BCD2i(sd[1]), BCD2i(sd[0]), ((IN_RTC_INT==1)?0:500));
    } else {
        DL_CMD_CLOCK(120, 120, 100, OPT_NOBACK, BCD2i(sd[2]&0x7f), BCD2i(sd[1]), BCD2i(sd[0]), 0);
    }    
    sprintf(sz,"%02i:%02i:%02i",BCD2i(sd[2]&0x7f),BCD2i(sd[1]),BCD2i(sd[0]));
    DL(TAG(0x18));
    DL_CMD_TEXT(120, 208, 29, OPT_CENTERX, sz);
    sprintf(sz,"%02i.%02i.%02i",BCD2i(sd[4]),BCD2i(sd[5]),BCD2i(sd[6])+2000);
//  sprintf(sz,"%02x:%02x:%02x %02x.%02x.%02x",(uchar)sd[2]&0x7f,(uchar)sd[1],(uchar)sd[0],(uchar)sd[4],(uchar)sd[5],(uchar)sd[6]);
    DL(TAG(0x19));
    DL_CMD_TEXT(120, 236, 29, OPT_CENTERX, sz);

if (ft_state==3) {
  if (testext>=2) {
 #ifdef FLASH_TESTING // -------------------------------------------------------- 
    FLASHreadn(sa, 0, 16);
    j=sprintf(s,"@0000:");
    for(i=0;i<8;i++) {
        j+=sprintf(s+j,"%02X",(uchar)sa[i]);
    }
    DL(TAG(0x1A));
    DL_CMD_TEXT(45, 280, 27, 0, s);
    DL(TAG(0x1B));
    DL_CMD_TEXT(8, 280, 27, 0, "SET");
    //.....
    FLASHreadn(sa, 4096, 16);
    j=sprintf(s,"@4096:");
    for(i=0;i<8;i++) {
        j+=sprintf(s+j,"%02X",(uchar)sa[i]);
    }
    DL(TAG(0x1C));
    DL_CMD_TEXT(45, 310, 27, 0, s);
    DL(TAG(0x1D));
    DL_CMD_TEXT(8, 310, 27, 0, "SET");
#endif // ----------------------------------------------------------------------
    DL(TAG(0x1E));
    x=8; y=340; yd=14;
    sprintf(s,"I2C-IntCount=%u, ErrCnt=%u",IntCount_I2C0,I2C_Error_count);
    DL_CMD_TEXT(x, y, 26, 0, s); y+=yd;
    sprintf(s,"I2C_IN=%04xh, dT=%u, DLL=%u, EC=%u",I2C_Inputs,mus_max,DL_LEN_USED,Count_EEPto);
    DL_CMD_TEXT(x, y, 26, 0, s); y+=yd;
    x=285; y=340; yd=14;
    sprintf(s,"sizeof(MF40_Setup) = %i",sizeof(MF40_Setup));
    DL_CMD_TEXT(x, y, 26, 0, s); y+=yd;
    sprintf(s,"sizeof(MF40_Userdata) = %i",sizeof(MF40_Userdata));
    DL_CMD_TEXT(x, y, 26, 0, s); y+=yd;
    y-=2*yd; x+=215;
    sprintf(s,"sizeof(MF40_Lifedata) = %i",sizeof(MF40_Lifedata));
    DL_CMD_TEXT(x, y, 26, 0, s); y+=yd;
  }
  if (testext>=1) {
    DL(TAG(0x1E));
    for (i=0,x=285,y=190,yd=17; i<6; i++,y+=yd) {
        sprintf(sb,"ANA-%i:",i+1);
        DL_CMD_TEXT(x, y, 27, 0, sb);
        sprintf(sb,"%lu",ADCvalMean(i));
        DL_CMD_TEXT(x+90, y, 27, OPT_RIGHTX, sb);
        sprintf(sb,"%.*f",2,ANAvaln(i));
        DL_CMD_TEXT(x+150, y, 27, OPT_RIGHTX, sb);
        sprintf(sb,"%.*f",2,ANAval(i));
        DL_CMD_TEXT(x+150+70, y, 27, OPT_RIGHTX, sb);
        DL_CMD_TEXT(x+150+70+10, y, 27, 0, TabAnaName[i]);
    }
    i=0;
    zeit=MF40_Lifedata.OprTime;
    i+=sprintf(s+i,"Operation time=%u:%02u:%02u, ",zeit/3600,(zeit/60)%60,zeit%60);
    zeit=MF40_Lifedata.OprTimeGenerator;
    i+=sprintf(s+i,"Generator=%u:%02u:%02u (%u cycles)",zeit/3600,(zeit/60)%60,zeit%60,MF40_Lifedata.GeneratorCycles);
    DL_CMD_TEXT(x, y+8, 27, 0, s);
    i=0; y+=yd;
    zeit=MF40_Lifedata.OprTime_1;
    i+=sprintf(s+i,"Service time=%u:%02u:%02u, ",zeit/3600,(zeit/60)%60,zeit%60);
    zeit=MF40_Lifedata.OprTimeGenerator_1;
    i+=sprintf(s+i,"Generator=%u:%02u:%02u (%u cycles)",zeit/3600,(zeit/60)%60,zeit%60,MF40_Lifedata.GeneratorCycles_1);
    DL_CMD_TEXT(x, y+8, 27, 0, s);

    //sprintf(s,"IN_TASTER = %i (%i, %i)",IN_TASTER,IN_START,IN_STOP);
    //DL_CMD_TEXT(x, y+8+yd, 27, 0, s);
  }
}
    
    DL(SAVE_CONTEXT());
    DL_CMD_FGCOLOR(0x9497D0);
//  DL_CMD_FGCOLOR(0xE511A0);
    DL_CMD_TRACK(680, 120, 1,1, 0x21);
    DL(TAG(0x21));
    DL_CMD_DIAL(680, 120, 65, 0, dial);
    DL(TAG(0x22));
    DL_CMD_GAUGE(680, 121, 110, OPT_NOBACK | OPT_NOPOINTER, 4, 8, 0, 375);
//  DL_CMD_TEXT(680, 195, 28, OPT_CENTERX, sh);
    DL_CMD_TEXT(680, 108, 28, OPT_CENTERX, sh);
    DL(RESTORE_CONTEXT());
    
    DL(COLOR_RGB(255, 247, 14));
    DL(SCISSOR_XY(10, 0));
    DL(SCISSOR_SIZE(780, 480));
    DL(VERTEX_FORMAT(0));
    DL(BEGIN(LINES));
    DL(VERTEX2F(12, 463));
    DL(VERTEX2F(a, 463));
    DL(VERTEX2F(a+20, 463));
    DL(VERTEX2F(a+20+60, 463));
    DL(VERTEX2F(a+20+60+20, 463));
    DL(VERTEX2F(790, 463));
    DL(END());
    
    if ((ft_state==1)&&(showTimesetHelp)) {
        DL(TAG(0x12));
        DL_CMD_BUTTON(240, 225, 95, 32, 30, 0, "CALC");
        DL_CMD_TEXT(400, 200, 29, OPT_CENTERX, "hhmmss");
        DL_CMD_TEXT(400, 255, 29, OPT_CENTERX, "ddmmyyyy");
        DL_CMD_CLOCK(492, 242, 29, OPT_FLAT | OPT_NOBACK | OPT_NOSECS, 11, 55, 17, 0);
        DL(BEGIN(LINES));
        DL(VERTEX2F(350, 242));
        DL(VERTEX2F(450, 242));
        DL(END());
        DL(POINT_SIZE(70));
        DL(BEGIN(POINTS));
        DL(VERTEX2F(350, 242));
        DL(VERTEX2F(400, 242));
        DL(VERTEX2F(450, 242));
        DL(END());
    } else showTimesetHelp=0;
#else
    *** NO FT801 SUPPORT ***
#endif
    // .........................................................................
    DL_TEST();
    DL(DISPLAY());
    // .........................................................................
    DL(CMD_SWAP);
    if (CDLexec()) return;
    // ~~~~~
    // Tracker für dial
    tracker = rd32(REG_TRACKER);
    if (!first) switch(tracker&0xff) {
        case 0x21: dial = tracker>>16; break;
    }
    if (i=getRotaryUpDown()) {
        i = (x=dial) + (i*500); if (i<0) i=0;  // ca. 2% pro Raste
        dial = i;
        if ((x>50000)&&(dial<10000)) dial=x;
    }
    if (dial>57700) dial=57700;
    else if (dial<7700) dial=7700;
    if (dial<7700+2000) dial+=50;
    pwm=dial;               // 7700..57700
    pwm-=7700;              // 0..50000
    pwm/=390-1;             // 0..128
//  pwm/=500-1;             // 0..100% bis V1.02
    FT_PWM_DUTY = pwm;
    wr16(REG_PWM_DUTY, FT_PWM_DUTY);
    // Tastatur (verwendet nicht den TRACKER, aber den TOUCH_TAG)
    if ((c=rd8(REG_TOUCH_TAG))>0) {
        if (first) cl=c;
        if (downcnt>3) cl=c;        // 
        if (cl!=c) {
            switch(c) {
                case 0x11:  ft_state=1; showTimesetHelp=0; break;
                case 0x12:  hub_state=hub_Calculator; break;
                case 0x13:  if (ft_state==3) {testcnt++; testext = (testext+1)%3;}
                            if (!MF40_Setup.ExtTestDisp) testext=testcnt=0;
                            ft_state=3; break;
//              case 0x14:  hub_state=hub_return; return;      // Back-Button
                case 0x14:  hub_state=hub_Splash; return;      // Back-Button
                case 0x15:  hub_state=hub_UserSetup1; return;  // SETUP-Button
                case 0x17:                                     // CLOCK
                case 0x18:                                     // TIME
                case 0x19:  showTimesetHelp++; break;          // DATE
#ifdef FLASH_TESTING                            
                case 0x1A:  // HEX-STRING 1
                            FLASHwriten("", 0, Sector_Erase); break;
                case 0x1B:  // HEX-STRING 1 SET
                            FLASHwriten("01234567", 0, 8); break;
                case 0x1C:  // HEX-STRING 2
                            FLASHwriten("", 4096, Sector_Erase); break;
                case 0x1D:  // HEX-STRING 2 SET
                            FLASHwriten("< !abc>", 4096, 7); break;
#endif
            }
        }
    }
    cl=c;
//  if (STOP_Taster_ms>1000) hub_state=hub_Setup; 
    // ----- Swipe windows
    if (ft_state!=3) if (SwipeWindows(first, 0)) return;

    if (first) first--;
}

// *****************************************************************************
int RampPointInsert(int point)  // 1..RampPointsMax                25.11.2017 HR
// *****************************************************************************
{
int i;
struct MF40_Userdata_Struct *UD = &MF40_Userdata;
    if ((point<1)||(point>UD->RP[UD->ars].RampPointsUsed)) return 0;
    UD->RP[UD->ars].RampPointsUsed++;
    for (i=UD->RP[UD->ars].RampPointsUsed; i>point; i--) {
        UD->RP[UD->ars].RP[i-1].dt=UD->RP[UD->ars].RP[i-2].dt;
        UD->RP[UD->ars].RP[i-1].Temp=UD->RP[UD->ars].RP[i-2].Temp;
    }
    
}
// *****************************************************************************
int RampPointDelete(int point)  // 1..RampPointsMax                25.11.2017 HR
// *****************************************************************************
{
int i;
struct MF40_Userdata_Struct *UD = &MF40_Userdata;
    if ((point<1)||(point>UD->RP[UD->ars].RampPointsUsed)) return 0;
    if (UD->RP[UD->ars].RampPointsUsed==1) return 0;
    for (i=point;i<UD->RP[UD->ars].RampPointsUsed;i++) {
        UD->RP[UD->ars].RP[i-1].dt=UD->RP[UD->ars].RP[i].dt;
        UD->RP[UD->ars].RP[i-1].Temp=UD->RP[UD->ars].RP[i].Temp;
    }
    UD->RP[UD->ars].RampPointsUsed--;
}

//#define BOCKEL    // nur zum Test von variablen Strings in Konstanten

#ifdef BOCKEL
struct bockelstruct {
    int start;
    int zwei;
    char * cpf[6];
    int ende;
};

const struct bockelstruct cpfs = {
    1,2,"eins","2.123456789","drei","4.778877887788","fuenf","6",999
};
//const char * cpf[6] = {"eins","2.123456789","drei","4.778877887788","fuenf","6"};
#endif


// *****************************************************************************
void Ramp_Copy(struct MF40_Userdata_Struct * UD, int ars)
{
int i;
    CLIP_RP.RampPointsUsed = UD->RP[ars].RampPointsUsed;
    for (i=0;i<RampPointsMax;i++) {
        CLIP_RP.RP[i].Temp = UD->RP[ars].RP[i].Temp;
        CLIP_RP.RP[i].dt = UD->RP[ars].RP[i].dt;
    }
    CLIP_used++;
}
// *****************************************************************************
void Ramp_Paste(struct MF40_Userdata_Struct * UD, int ars)
{
int i;
    if (!CLIP_used) return;
    UD->RP[ars].RampPointsUsed = CLIP_RP.RampPointsUsed;
    for (i=0;i<RampPointsMax;i++) {
        UD->RP[ars].RP[i].Temp = CLIP_RP.RP[i].Temp;
        UD->RP[ars].RP[i].dt = CLIP_RP.RP[i].dt;
    }
    CLIP_used=0;
}

#define maxpt   UD->RP[UD->ars].RampPointsUsed
// *****************************************************************************
void FT81X_Display_RampSetup_MF40(int first_req)        // 17.11.207
// *****************************************************************************
{
uint32 tracker;
uint32 xy;
uint16 x,y,y0,xd,yd;
uint32 col1,col2,col3;
int i,j, f;
uint tsum;
uint32 ul;
char c;
char s[120];
struct MF40_Userdata_Struct *UD = &MF40_Userdata;
float ff;

static  char    cl;
static  uint16  track;
static  int16   plusminus;
static  uint    downcnt;
static  int     first;
static  int     selx,sely;
static  uint16  scroll;         // 0..65535
static  uint16  step=1;
    // .........................................................................
    if (first_req) {
        first = 10;                         // 10 Durchläufe keine Touch-Events auswerten
        getRotaryUpDown();getRotarySwitch();// zu Beginn alte, noch anstehende Anforderungen abräumen
        selx=sely=0;
        plusminus=track=0;
        if (Keyboard_ReadyState) {
            strcpy(UD->RP[UD->ars].Ramp_Name,Keyboard_String);
            Keyboard_ReadyState=0;
        }
    }
    // .........................................................................
    if (CMD_busy()) return;     // warten, bis CMD Fifo abgearbeitet
    // .........................................................................
    xy = rd32(REG_CTOUCH_TOUCH0_XY);
    x = xy >> 16;
    if (x!=32768) downcnt++; else downcnt=0;
    // .........................................................................
    // scroll Auflösung: 0..65535; Achtung, 0-->65535, 65535-->0, da oben null ist
    i=getRotaryUpDown();            // bitte stets "abräumen"
    if (i) {
        switch(selx) {
            case 1: UD->RP[UD->ars].RP[sely-1].dt=IncDecStepwise(UD->RP[UD->ars].RP[sely-1].dt, 0, i, 0, MF40_TIMER_MAX); break;
            case 2: UD->RP[UD->ars].RP[sely-1].Temp=IncDecStepwise(UD->RP[UD->ars].RP[sely-1].Temp, 2, i, 0, TEMPSETmax()); break;
            case 3: UD->RP[UD->ars].RP[sely-1].PIDset=IncDecStepwise(UD->RP[UD->ars].RP[sely-1].PIDset, 1, i, 0, MF40_PIDset_MAX); break;
        }
    }
    else if (plusminus) {
        switch(selx) {
            case 1: UD->RP[UD->ars].RP[sely-1].dt=IncDecStepwise(UD->RP[UD->ars].RP[sely-1].dt, 1, plusminus, 0, MF40_TIMER_MAX); break;
            case 2: UD->RP[UD->ars].RP[sely-1].Temp=IncDecStepwise(UD->RP[UD->ars].RP[sely-1].Temp, 1, plusminus, 0, TEMPSETmax()); break;
            case 3: UD->RP[UD->ars].RP[sely-1].PIDset=IncDecStepwise(UD->RP[UD->ars].RP[sely-1].PIDset, 1, plusminus, 0, MF40_PIDset_MAX); break;
        }
        plusminus=0;
    }
    else if (track) {
        switch(selx) {
            case 1: ff = (float)(uint16)(~scroll);          // dt
                    ff = ff * MF40_TIMER_MAX / 65535;
                    UD->RP[UD->ars].RP[sely-1].dt = ff;
                    break;
            case 2: ff = (float)(uint16)(~scroll);          // TEMP
                    ff = ff * TEMPSETmax() / 65535;
                    UD->RP[UD->ars].RP[sely-1].Temp = ff;
                    break;
            case 3: ff = (float)(uint16)(~scroll);          // PID
                    ff = ff * MF40_PIDset_MAX / 65535;
                    UD->RP[UD->ars].RP[sely-1].PIDset = ff;
                    break;
        }
        track=0;
    }
    switch(selx) {
        case 1: scroll = ~(uint16)(UD->RP[UD->ars].RP[sely-1].dt * 65535 / MF40_TIMER_MAX); break;        // dt
        case 2: scroll = ~(uint16)(UD->RP[UD->ars].RP[sely-1].Temp * 65535 / TEMPSETmax()); break;        // TEMP
        case 3: scroll = ~(uint16)(UD->RP[UD->ars].RP[sely-1].PIDset * 65535 / MF40_PIDset_MAX); break; // PID
    }
    // .........................................................................
    CDLinit();
    DL(CMD_DLSTART);
    DL(CLEAR(1, 1, 1));
    // .........................................................................
    col1 = 0x0505E8;            // blau
    col2 = 0xFFFF00;            // gelb
    col3 = 0xFFAA00;            // orange

    DL_CMD_FGCOLOR(0x1F3F7F);
    DL_CMD_GRADIENT(23, 42, 0xE2E2E2, 22, 332, 0xE2E2E2);

    x=50+10; y=y0=60;
    DL(COLOR_RGB(0, 0, 0));
    DL(TAG(0x5F));
    DL_CMD_TEXT(x+28+9+70, y-33, 29, OPT_CENTERX, "dt");
    DL_CMD_TEXT(x+28+9+155+50, y-33, 29, OPT_CENTERX, TEMPSETavl()?"TEMP":"%PSET");
    if (TEMPSETavl()) DL_CMD_TEXT(x+28+9+155+115+20, y-33, 29, OPT_CENTERX, "PID");
    if (maxpt<1) maxpt=1;
    // Punkte-Tabelle
    for (i=0,yd=45,tsum=0;    i<8;     i++,y+=yd) {
        DL_CMD_FGCOLOR(col1);
        DL(COLOR_RGB(255, 255, 255));
        if (i<maxpt) {
            DL(TAG(0x20+i));
            if ((sely==(i+1))&&(selx==1)) DL_CMD_FGCOLOR(0x000000); else DL_CMD_FGCOLOR(col1);
            sprtTimer(s,(uint)UD->RP[UD->ars].RP[i].dt);
            tsum += UD->RP[UD->ars].RP[i].dt;
            DL_CMD_BUTTON(x+28+9, y, 140, 34, 30, OPT_FLAT, s);
            DL(TAG(0x30+i));
            if ((sely==(i+1))&&(selx==2)) DL_CMD_FGCOLOR(0x000000); else DL_CMD_FGCOLOR(col1);
            sprintf(s,"%i",UD->RP[UD->ars].RP[i].Temp);
            DL_CMD_BUTTON(x+28+9+155, y, 100, 34, 30, OPT_FLAT, s);
            DL(TAG(0x70+i));
            if (TEMPSETavl()) {
                if ((sely==(i+1))&&(selx==3)) DL_CMD_FGCOLOR(0x000000); else DL_CMD_FGCOLOR(col1);
                sprintf(s,"%c",cPIDset(UD->RP[UD->ars].RP[i].PIDset));
                DL_CMD_BUTTON(x+28+9+155+115, y, 40, 34, 30, OPT_FLAT, s);
            }
        }
        DL(TAG(0x40+i));
        sprintf(s,"%i:",i+1);
        if (i<maxpt) DL(COLOR_RGB(0,0,0)); else DL(COLOR_RGB(0xE2,0xE2,0xE2));
        DL_CMD_FGCOLOR(col3);
        DL_CMD_BUTTON(x+28-52+9, y, 38, 34, 30, OPT_FLAT, s);
    }

    if (selx) {
        // Ramp Points INSERT
        DL_CMD_FGCOLOR(col1);
        DL(COLOR_RGB(255, 255, 255));
        if (UD->RP[UD->ars].RampPointsUsed<RampPointsMax) {DL(TAG(0x68)); DL_CMD_BUTTON(x+28+9+155, y, 65, 20, 26, OPT_FLAT, "INSERT");}  // INSERT Point
        if (UD->RP[UD->ars].RampPointsUsed>1)             {DL(TAG(0x69)); DL_CMD_BUTTON(x+28+9+155+89, y, 65, 20, 26, OPT_FLAT, "DELETE");}  // DELETE Point
    } else {
        // Copy/Paste ganz unten in Spalte TEMP
        if (CLIP_used) DL(COLOR_RGB(0, 0, 0)); else DL(COLOR_RGB(0xE2,0xE2,0xE2));
        DL(TAG(0x5C)); DL_CMD_BUTTON(x+28+9+155, y, 65, 20, 26, OPT_FLAT, "Paste");  //          PASTE
        DL(COLOR_RGB(0, 0, 0));
        DL(TAG(0x5B)); DL_CMD_BUTTON(x+28+9+155+89, y, 65, 20, 26, OPT_FLAT, "Copy");      // COPY
    }
    // Timer-Summe darstellen
    DL(TAG(0x5F));
    DL(COLOR_RGB(0,0,0));           // Graph Farbe
    DL(LINE_WIDTH(9));
    DL(VERTEX_FORMAT(0));
    DL(BEGIN(LINE_STRIP));
    DL(VERTEX2F(x+28+9+70-50, y-2));
    DL(VERTEX2F(x+28+9+70+50, y-2));
    DL(END());
    DL(VERTEX_FORMAT(4));
    sprtTimer(s,tsum);
    DL_CMD_TEXT(x+28+9+70, y-1, 28, OPT_CENTERX, s);

    // RAMP Namen
    x-=20; y=y0;
    DL_CMD_TEXT(x+440+((280-8)/2), y-33, 29, OPT_CENTERX, "RAMP");
    DL(COLOR_RGB(255,255,255));
    for (i=0; i<RampSetsMax; i++,y+=yd) {    // y+=(3*yd)/2
        DL(TAG(0x50+i));
        if (i==UD->ars) DL_CMD_FGCOLOR(0xFFAA00); else DL_CMD_FGCOLOR(0x9D9D9D);
        f = strlen(UD->RP[i].Ramp_Name);
        if (f<=15) f=29; else {if (f<=20) f=28; else f=27;}
//      DL_CMD_BUTTON(x+440, y, 280-8, 34, f, (i==UD->ars)?0:OPT_FLAT, UD->RP[i].Ramp_Name);
        DL_CMD_BUTTON(x+440, y, 280-8, 34, f, OPT_FLAT, UD->RP[i].Ramp_Name);
    }
    if (MF40_WorkMode!=MF40_IntRunRamp) {   // bei IntRunRamp keine Funktionsbuttons anzeigen
        y-=5;
        y+=yd/2;
        // RAMP Namen Steuerung
        DL_CMD_FGCOLOR(col3);
        DL(TAG(0x58)); DL_CMD_BUTTON(x+440+(280/2)-35-10, y-2, 80, 34, 27, OPT_FLAT, "Name...");
        DL(TAG(0x59)); DL_CMD_BUTTON(x+440+(280/2)-(30/2)-68-12, y-2, 30, 34, 27, OPT_FLAT, "/\\");     // UP
        DL(TAG(0x5A)); DL_CMD_BUTTON(x+440+(280/2)-(30/2)+68+2, y-2, 30, 34, 27, OPT_FLAT, "\\/");      // DOWN
        y+=yd+yd/3; // y=410;
        // SAVE/DISCARD
        if (memcmp((char*)UD,(char*)&MF40_Userdata_backup,sizeof(MF40_Userdata_backup))) {
            DL_CMD_FGCOLOR(0x1F3F7F);
            DL(TAG(0x5D)); DL_CMD_BUTTON(x+440+(280/2)-(80/2)+50-5, y-5, 80, 34+5, 27, 0, "Save");
            DL(TAG(0x5E)); DL_CMD_BUTTON(x+440+(280/2)-(80/2)-50-5, y-5, 80, 34+5, 27, 0, "Discard");
        }
    }
    // SLIDER
    x+=10; y0-=5;
    if (selx) {
        DL(SAVE_CONTEXT());
        DL_CMD_BGCOLOR(col1);
        DL_CMD_FGCOLOR(col2);
//      DL_CMD_TRACK(x+400-20, y0+10, 55, 335, 0x60); // "Verbreiterung" klappt nicht, arbeitet jedoch normal
        DL_CMD_TRACK( x+400-15, y0+15, 15, 335, 0x60);
        DL(TAG(0x60));
        DL_CMD_SLIDER(x+400-15, y0+15, 15, 335, 0, scroll, 65535);
        DL(RESTORE_CONTEXT());
    }
    // + / - Felder
    if (selx) {
        DL(SAVE_CONTEXT());
        DL(COLOR_RGB(0, 0, 0));
        DL_CMD_FGCOLOR(col2);
        DL(TAG('+'));
        DL_CMD_BUTTON(x+400-15+7-55-4, y0-34-9, 55, 40, 30, OPT_FLAT, "+");                // +/- über Slider
        DL(TAG('-'));
        DL_CMD_BUTTON(x+400-15+7+4, y0-34-9, 55, 40, 30, OPT_FLAT, "-");                   // +/- über Slider
        DL(RESTORE_CONTEXT());
    }
    // Ellipsen
//  ShowEllipsen(400,8,0,0);          // oben
    ShowEllipsen(8,240-10,1,3);       // links
    if (MF40_Userdata.DisplayRoll) ShowEllipsen(800-8,240-10,1,3);   // rechts
//  ShowEllipsen(400,455,0,3);        // unten
    showAlphoFooter(0);

    // .........................................................................
    DL_TEST();
    DL(DISPLAY());
    // .........................................................................
    DL(CMD_SWAP);
    if (CDLexec()) return;
    // ~~~~~
    // Tracker für dial
    tracker = rd32(REG_TRACKER);
    if (!first) switch(tracker&0xff) {
        case 0x60: scroll = tracker>>16; track++; break;   // 0..65535
    }
    // Tastatur (verwendet nicht den TRACKER, aber den TOUCH_TAG)
    if ((c=rd8(REG_TOUCH_TAG))>0) {
        // komplette Bedienungssperre bei IntRunRamp (außer Swipen):
        if (MF40_WorkMode==MF40_IntRunRamp) c=cl=0;
        if (first) cl=c;
        if ((c=='+') || (c=='-')) {if (downcnt>12) cl=~c;}      // Repeat nur von '+' und '-'
        else if (!((c>=0x40)&&(c<=0x47))) {if (downcnt>3) cl=c;}// ansonsten Sperre (Slider kann auch "außerhalb" bedient werden, ohne das andere Bereiche aktiv werden können) 
        if (cl!=c) {
            switch(c) {
                case 0x14:  hub_state=hub_return; return;       // Back-Button
                // .......
                case '+': plusminus++; break;
                case '-': plusminus--; break;
                // .......
                case 0x20:  selx=1; sely=1; break;
                case 0x21:  selx=1; sely=2; break;
                case 0x22:  selx=1; sely=3; break;
                case 0x23:  selx=1; sely=4; break;
                case 0x24:  selx=1; sely=5; break;
                case 0x25:  selx=1; sely=6; break;
                case 0x26:  selx=1; sely=7; break;
                case 0x27:  selx=1; sely=8; break;
                // .......
                case 0x30:  selx=2; sely=1; break;
                case 0x31:  selx=2; sely=2; break;
                case 0x32:  selx=2; sely=3; break;
                case 0x33:  selx=2; sely=4; break;
                case 0x34:  selx=2; sely=5; break;
                case 0x35:  selx=2; sely=6; break;
                case 0x36:  selx=2; sely=7; break;
                case 0x37:  selx=2; sely=8; break;
                // .......
                case 0x70:  selx=3; sely=1; break;
                case 0x71:  selx=3; sely=2; break;
                case 0x72:  selx=3; sely=3; break;
                case 0x73:  selx=3; sely=4; break;
                case 0x74:  selx=3; sely=5; break;
                case 0x75:  selx=3; sely=6; break;
                case 0x76:  selx=3; sely=7; break;
                case 0x77:  selx=3; sely=8; break;
                // .......
                case 0x40:  maxpt=1; goto UNSELECT;
                case 0x41:  maxpt=2; goto UNSELECT;
                case 0x42:  maxpt=3; goto UNSELECT;
                case 0x43:  maxpt=4; goto UNSELECT;
                case 0x44:  maxpt=5; goto UNSELECT;
                case 0x45:  maxpt=6; goto UNSELECT;
                case 0x46:  maxpt=7; goto UNSELECT;
                case 0x47:  maxpt=8; goto UNSELECT;
                // .......
                case 0x50:                                          // Auswahl der Rampe
                case 0x51:  
                case 0x52:  
                case 0x53:  
                case 0x54:  
                case 0x55:  
                case 0x56:  
                case 0x57:  UD->ars = c-0x50; goto UNSELECT;        // Auswahl der Rampe
                case 0x58:  Keyboard_Callers_hub_state=hub_state;   // Rampen-Name
                            hub_state=hub_Keyboard;
                            strcpy(Keyboard_String,UD->RP[UD->ars].Ramp_Name);
                            Keyboard_Len = RampNameLen-1;
                            sprintf(Keyboard_Title,"RAMP #%i Name",UD->ars+1);
#ifdef BOCKEL
//sprintf(Keyboard_Title,"RAMP #%i:%s",UD->ars+1,cpf[UD->ars]);
sprintf(Keyboard_Title,"RAMP #%i:%s",UD->ars+1,cpfs.cpf[UD->ars]);
#endif
                            Keyboard_Mode = 0;
                            return;
                case 0x59:  RampShift(1); goto UNSELECT;    // UP
                case 0x5A:  RampShift(0); goto UNSELECT;    // DOWN
                // .......
                case 0x5B:  Ramp_Copy(UD, UD->ars); goto UNSELECT;  // Copy
                case 0x5C:  Ramp_Paste(UD, UD->ars); goto UNSELECT; // Paste
                // .......
                case 0x5D:  MF40_Userdata_Save(1); goto UNSELECT;
                case 0x5E:  MF40_Userdata_Restore(); goto UNSELECT;
                case 0x5F:  UNSELECT: selx=sely=0; break;
                //   0x60:  SLIDER
                case 0x68:  RampPointInsert(sely); break; goto UNSELECT;
                case 0x69:  RampPointDelete(sely); if (UD->RP[UD->ars].RampPointsUsed<sely) goto UNSELECT; break;
            }
        }
    }
    cl=c;
//  if (STOP_Taster_ms>1000) hub_state=hub_Setup; 
    // ----- Swipe windows
    if (SwipeWindows(first, selx?2:0)) return;

    if (first) first--;
}

// *****************************************************************************
int sprtTimer(char * s, uint t)
// *****************************************************************************
{
    return sprintf(s,"%i:%02i:%02i",t/3600,(t/60)%60,t%60);
}

// *****************************************************************************
char cPIDset(int ps)                                               // 19.01.2019
// *****************************************************************************
{
    switch (ps) {
        case 0: return 'B';
        default:return '0'+ps;
    }
}

//                     P,p,I,C,F,T
const char iconv[8] = {0,1,2,4,3,6,5,7};
//                       P,I,C,F,T
//const char iconv[8] = {0,2,4,3,6,1,5,7};
// *****************************************************************************
void RampBoxes(uint xi,uint yi,int tag)
// *****************************************************************************
// 20.11.2017 Boxen vergrößert von Breite 84 --> 110, Raster 120 --> 130
// *****************************************************************************
{
uint x,y, xd;
int i,j;
char c, s[20];
    // Mess-Felder
    DL(SAVE_CONTEXT());
    for (i=0,x=xi,y=yi,xd=111; i<6; i++,x+=xd) {
//  for (i=0,x=xi,y=yi,xd=130; i<5; i++,x+=xd) {
        j = iconv[i];
        if (!(MF40_Setup.DisplaySelect&(1<<j))) continue;
        if ((!MF40_Userdata.TEMP_used) && (j>=6)) continue;
        DL(TAG(tag+i));
        c = MG_unit[j][0];              // erster Buchstabe der Einheit
        DL(VERTEX_FORMAT(0));
        DL(COLOR_RGB(255, 255, 0));     // gelbe Titel-Box
        DL(BEGIN(RECTS));
        DL(VERTEX2F(x, y));
        DL(VERTEX2F(x+110-7, y+19));
        DL(END());
        if (c) {
            DL(COLOR_RGB(255, 170, 0)); // orange Einheiten-Box
            DL(BEGIN(RECTS));
            DL(VERTEX2F(x+80-7, y+0));
            DL(VERTEX2F(x+110-7, y+19));
            DL(END());
        }
        DL(COLOR_RGB(5, 5, 232));       // blaue Werte-Box
        DL(BEGIN(RECTS));
        DL(VERTEX2F(x+0, y+20));
        DL(VERTEX2F(x+110-7, y+56));
//        if (i==0) DL(VERTEX2F(x+110, y+56+28));
//        else      DL(VERTEX2F(x+110, y+56));  
        DL(END());

        j = iconv[i];
        switch (i) {
            // ------Anordnung-------
            // 0:P, 1:P-SET, 2:I, 3:cos, 4:Freq, 5:Temp
            case 0: sprintf(MG_val[j],"%.*f",MG_fix[j],MF40_Ist_P); break;          // P
            case 1: sprintf(MG_val[j],"%.*f",MG_fix[j],MF40_Userdata.P_SET); break; // P SET
            case 2: sprintf(MG_val[j],"%.*f",MG_fix[j],MF40_Ist_I); break;          // I
            case 3: sprintf(MG_val[j],"%.*f",MG_fix[j],MF40_Ist_cosPhi); break;     // COS PHI
#ifdef TESTTESTshow
            case 4: sprintf(MG_val[j],"%i",TESTTEST); break;
#else            
            case 4: sprintf(MG_val[j],LED_RUN?"%.*f":"-",MG_fix[j],MF40_Ist_Freq); break;            // Freq
#endif            
            case 5: sprintf(MG_val[j],MF40_Temp_Error?"Error":"%.*f",MG_fix[j],MF40_Ist_Temp); break;// Temp
        }
        if (MG_name[j][0]==0) strcpy(MG_val[j],"-");
        DL(COLOR_RGB(255, 255, 255));
        DL_CMD_TEXT(x+55-5, y+39, 31, OPT_CENTER, MG_val[j]);
        DL(COLOR_RGB(0,0,0));
        if (c=='°') {
            DL_CMD_TEXT(x+95-5-4, y+10-3, 26, OPT_CENTER, "o ");
            DL_CMD_TEXT(x+95-5+4, y+10, 26, OPT_CENTER, MG_unit[j]+1);
        } else {
            DL_CMD_TEXT(x+95-5, y+10, 26, OPT_CENTER, MG_unit[j]);
        }
//      DL_CMD_TEXT(x+95-5, y+10, 26, OPT_CENTER, MG_unit[j]);
        if (c) DL_CMD_TEXT(x+40-5, y+9, 27, OPT_CENTER, MG_name[j]);
        else   DL_CMD_TEXT(x+40+14-5, y+9, 27, OPT_CENTER, MG_name[j]);
    }
    DL(RESTORE_CONTEXT());
}

// *****************************************************************************
// typ : 0=Display_MF40, 1=Display_Ramp             // ausgelagert am 11.10.2018
// *****************************************************************************
void LEDboxes(int typ)
{
int i,ledval;
char ledstr[10];
int offx, offy;
int sonder, graueschrift;
int flat=OPT_FLAT;              // 0-->3D mit fettem Display-List Verbrauch (+3300 Bytes!), OPT_FLAT-->ganz ok
    for (i=0; i<16; i++) {
        if (MF40_Setup.NoRegLEDs && ((i>=3)&&(i<=7))) continue;
        ledval = LED_val(i);
        if (i==1) if (LED_RUN) ledval = 0;                  // "RUN --> StdBy=0" Funktionalität, hier ab 11.10.2018
        if (!LED_RUN) if ((i>=3)&&(i<=7)) ledval=0;         // REG-LEDs nur bei RUN anzeigen
        strcpy(ledstr, ledval ? LED_nameON[i] : LED_nameOFF[i]);
        if (ledstr[0]) {
            sonder=graueschrift=0;
            switch (typ) {
                case 0: offx = (i>=8) ? 660 : 0;
                        offy = (i&7) * 55;
                        break;
                case 1: offx = (i>=8) ? 660+50+10 : 0-10;
                        offy = (i&7) * 55;
                        break;
            }
            if ((i==0) && (ledval) && (MF40_OUT_GENOFF)) ledstr[0]='a'; // RUN Spezial bei Generator Off: graue statt schwarze Schrift in roter Box
            switch (tolower(ledstr[0])) {
                default:
                case '_': DL_CMD_FGCOLOR(0x8D8D8D);           // dunkelgraue Box
                          DL(COLOR_RGB(226, 226, 226));       // graue Schrift (wie Hintergrund)
                          graueschrift++;
                          break;
                case 'r': DL_CMD_FGCOLOR(0xFF0000);           // rote Box
                          DL(COLOR_RGB(0, 0, 0));             // schwarze Schrift
                          break;
                case 'g': DL_CMD_FGCOLOR(0x2FEE04);           // grüne Box
                          DL(COLOR_RGB(0, 0, 0));             // schwarze Schrift
                          break;
                case 'y': DL_CMD_FGCOLOR(0xEEee00);           // gelbe Box
                          DL(COLOR_RGB(0, 0, 0));             // schwarze Schrift
                          break;
                // für RUN mit GEN-OFF
                case 'a': DL_CMD_FGCOLOR(0xFF0000);           // rote Box
                          DL(COLOR_RGB(226, 226, 226));       // graue Schrift (wie Hintergrund)
                          graueschrift++;
                          break;
                // für RESET während RESET
                case 's': DL_CMD_FGCOLOR(ResetTimer?0xFF0000:0x8D8D8D);// schwarze Box (oder rot während RESET)
                          DL(COLOR_RGB(226, 226, 226));       // graue Schrift (wie Hintergrund)
                          graueschrift++;
                          break;
            }
            if (!strcmp(ledstr+1,"H2O")) {strcpy(ledstr+1,"H  O"); sonder=1;}
            switch (typ) {
                case 0: DL(TAG(i+11));
                        if (i!=2)   DL_CMD_BUTTON(20+offx, 15+offy,   100, 44, 30, flat, ledstr+1); // sonstige Boxen
                        else        DL_CMD_BUTTON(20+offx, 15+offy+9, 100, 27, 29, flat, ledstr+1); // RESET Box
                        if (sonder) {                           // tiefgestellte 2 in H2O
                            if (!flat) {
                                if (graueschrift) DL(COLOR_RGB(0, 0, 0));
                                DL_CMD_TEXT(20+offx+(100/2)-7-1, 15+offy+14-1, 29, 0, "2");
                            }
                            if (graueschrift) DL(COLOR_RGB(226, 226, 226));
                            DL_CMD_TEXT(20+offx+(100/2)-7, 15+offy+14, 29, 0, "2");
                        }
                        break;
                case 1: DL(TAG(i+0x20));
                        if (i!=2)   DL_CMD_BUTTON(20+offx, 15+offy,   100-50, 44, 27, flat, ledstr+1); // sonstige Boxen
                        else        DL_CMD_BUTTON(20+offx, 15+offy+9, 100-50, 27, 26, flat, ledstr+1); // RESET Box
                        if (sonder) {                           // tiefgestellte 2 in H2O
                            if (!flat) {
                                if (graueschrift) DL(COLOR_RGB(0, 0, 0));
                                DL_CMD_TEXT(20+offx+((100-50)/2)-4-1, 15+offy+18-1, 26, 0, "2");
                            }
                            if (graueschrift) DL(COLOR_RGB(226, 226, 226));
                            DL_CMD_TEXT(20+offx+((100-50)/2)-4, 15+offy+18, 26, 0, "2");
                        }
                        break;
            }
        }
    }
}
/*
     for (i=0; i<16; i++) {
        if (MF40_Setup.NoRegLEDs && ((i>=3)&&(i<=7))) continue;
        strcpy(ledstr, LED_val(i) ? LED_nameON[i] : LED_nameOFF[i]);
        if (ledstr[0]) {
            offx = (i>=8) ? 660+50+10 : 0-10;
            offy = (i&7) * 55;
            if (i==0) if (MF40_OUT_GENOFF) ledstr[0]='a';     // RUN Spezial bei Generator Off: graue statt schwarze Schrift in roter Box
            switch (tolower(ledstr[0])) {
                default:
                case '_': DL_CMD_FGCOLOR(0x8D8D8D);           // dunkelgraue Box
                          DL(COLOR_RGB(226, 226, 226));       // graue Schrift (wie Hintergrund)
                          break;
                case 'r': DL_CMD_FGCOLOR(0xFF0000);           // rote Box
                          DL(COLOR_RGB(0, 0, 0));             // schwarze Schrift
                          break;
                case 'g': DL_CMD_FGCOLOR(0x2FEE04);           // grüne Box
                          DL(COLOR_RGB(0, 0, 0));             // schwarze Schrift
                          break;
                case 'y': DL_CMD_FGCOLOR(0xEEee00);           // gelbe Box
                          DL(COLOR_RGB(0, 0, 0));             // schwarze Schrift
                          break;
                case 'a': DL_CMD_FGCOLOR(0xFF0000);           // rote Box
                          DL(COLOR_RGB(226, 226, 226));       // graue Schrift (wie Hintergrund)
                          break;
                case 's': DL_CMD_FGCOLOR(ResetTimer?0xFF0000:0x8D8D8D);// schwarze Box (oder rot während RESET)
                          DL(COLOR_RGB(226, 226, 226));       // graue Schrift (wie Hintergrund)
                          break;
            }
            DL(TAG(i+0x20));
            if (i!=2)   DL_CMD_BUTTON(20+offx, 15+offy,   100-50, 44, 27, 0, ledstr+1); // sonstige Boxen
            else        DL_CMD_BUTTON(20+offx, 15+offy+9, 100-50, 27, 26, 0, ledstr+1); // RESET Box
        }
    }
*/

int last_xc=0;
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FillTempCurve(int xw, int yw)
{
int x,y;
int xc, xcw=maxCurvePoints;
    if (TempCurve.n>=maxCurvePoints-1) return;
    // Aktuelle Temperatur, skaliert auf 0..TEMP_max
    y = (((int)MF40_Ist_Temp) * yw) / TEMPSETmax();
    if (y<0) y=0; if (y>=yw) y=yw;
    // Laufzeit [s] ab 0s
    x = MF40_RUNTIME10;                 // [zs]
//  x = (MF40_RUNTIME10/10);            // [s]
    if (!TempCurve.n) {
        TempCurve.xn[0] = 0;
        TempCurve.yn[0] = y;
        last_xc = 0;
    }
    if (!x) return;
//TESTTEST=TempCurve.n;
    xc = (x * xcw) / (ZeitMax*10);      // TempCurve-Zeit 0..maxCurvePoints
    x = (x * xw) / (ZeitMax*10);        // Achsen-Zeit 0..xw
    TempCurve.xact = x;
    TempCurve.yact = y;
    if (xc>last_xc) {
        if ((xc-last_xc)>1) {
            while ((xc-last_xc)>1) {
                if (TempCurve.n>=maxCurvePoints-1) return;
                TempCurve.n++;
                TempCurve.xn[TempCurve.n] = x;
                TempCurve.yn[TempCurve.n] = y;
                last_xc++;
            }
            return;
        }
        TempCurve.n++;
        last_xc=xc;
        TempCurve.xn[TempCurve.n] = x;
        TempCurve.yn[TempCurve.n] = y;
    }
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ClrTempCurve(void) 
{
register int i;
    TempCurve.n=0;
//    TempCurve.go=0;
//    TempCurve.xn[0]=TempCurve.yn[0]=0; 
//    last_xc = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ShowTempCurve(int x, int y, int xw, int yw)
{
int i, k, TempMax;
float corx=1.0, cory=1.0;
static int last_ZeitMax, last_TempMax;

    TempMax = TEMPSETmax();
    if ((last_ZeitMax+last_TempMax) && ((last_ZeitMax!=ZeitMax) || (last_TempMax!=TempMax))) {
        if (ZeitMax) corx = (float)last_ZeitMax / (float)ZeitMax;
        if (TempMax) cory = (float)last_TempMax / (float)TempMax;
        for (i=0;i<TempCurve.n;i++) {
            TempCurve.xn[i] = (int)(((float)TempCurve.xn[i] * corx)+0.5);
            TempCurve.yn[i] = (int)(((float)TempCurve.yn[i] * cory)+0.5);
        }
        TempCurve.xact = (int)(((float)TempCurve.xact * corx)+0.5);
        TempCurve.yact = (int)(((float)TempCurve.yact * cory)+0.5);
    }
    last_ZeitMax=ZeitMax; last_TempMax=TempMax;

    x=110; y=120;
    DL(SAVE_CONTEXT());
//  DL(COLOR_RGB(0xff,0xaa,0));     // Kurvenfarbe hellbraun
//  DL(COLOR_RGB(0, 255, 0));       // Kurvenfarbe grün
    DL(COLOR_RGB(0xff, 0, 0));      // Kurvenfarbe rot
    DL(LINE_WIDTH(20));             // halb so dick wie Sollkurve
    if (k=TempCurve.n) {
        DL(BEGIN(LINE_STRIP));
        DL(VERTEX2F((x+TempCurve.xn[0])<<4, (y+yw-TempCurve.yn[0])<<4));
        for (i=1;i<k;i++) {
            DL(VERTEX2F(((x+TempCurve.xn[i])<<4), ((y+yw-TempCurve.yn[i])<<4)));
        }
        // letzten "Einzel-Punkt" nur beim Laufen zeigen (Temperatur sinkt beim letzten Punkt bereits ab!)
        if (MF40_RUNTIME10) DL(VERTEX2F(((x+TempCurve.xact)<<4), ((y+yw-TempCurve.yact)<<4)));
        DL(END());
    }  
    DL(RESTORE_CONTEXT());
}


#define maxPoints   MF40_Userdata.RP[MF40_Userdata.ars].RampPointsUsed
#define maxX        600
#define maxY        300
#define px(x)       MF40_Userdata.RP[MF40_Userdata.ars].RP[x].dt
#define py(x)       MF40_Userdata.RP[MF40_Userdata.ars].RP[x].Temp
// *****************************************************************************
void ShowGraph(uint xi, uint yi, int tag, int PointModify, int PMrotm)
// *****************************************************************************
{
uint x,y;
uint x0,y0;
int i,j, k,kc, d, v, len, ps, tmaxPoints;
uint t,tc;
char s[20];
static graph_neu=1;
    if (graph_neu) {graph_neu=0;}
    x=xi; y=yi;                     // left top
    DL(TAG(0));
    DL(COLOR_RGB(0,0,0));           // Graph Farbe
//  DL(COLOR_RGB(100, 100, 100));
//  DL(COLOR_RGB(12, 168, 1));
    DL(LINE_WIDTH(20));
    x0=x; y0=y+300;                 // 0:0 Koordinaten, Pixel-Bereich: x600 y300
    gx0=x0; gy0=y0; gxm=600; gym=300;
    // Koordinaten anzeigen
    DL(BEGIN(LINE_STRIP));
    DL(VERTEX2F((x0-18)<<4, y<<4));
    DL(VERTEX2F(x0<<4, y<<4));
    DL(VERTEX2F(x0<<4, y0<<4));

    DL(VERTEX2F((x0+300)<<4, y0<<4));
    DL(VERTEX2F((x0+300)<<4,(y0+5)<<4));
    DL(VERTEX2F((x0+300)<<4, y0<<4));

    DL(VERTEX2F((x0+600)<<4, y0<<4));
    DL(VERTEX2F((x0+600)<<4, (y0+19)<<4));
    DL(END());
    if (TEMPSETavl()) {
        sprintf(s,"%i",TEMPSETmax());
        DL_CMD_TEXT(x0+10-6, y-22+2, 27, OPT_RIGHTX, s);
        DL_CMD_TEXT(x0+8, y-22-1+2, 26, 0, "o");
        DL_CMD_TEXT(x0+8+8, y-22+2, 27, 0, "C");
    } else {
        sprintf(s,"%i%%",TEMPSETmax());
        DL_CMD_TEXT(x0+10-6, y-22+2, 27, OPT_RIGHTX, s);
    }
    sprtTimer(s, ZeitMax/2);
    DL_CMD_TEXT(x0+300-1, y0+5, 27, OPT_CENTERX, s);
    sprtTimer(s, ZeitMax);
    DL_CMD_TEXT(x0+600-3, y0+5, 27, OPT_RIGHTX, s);
    DL(SAVE_CONTEXT());

    // Kurve anzeigen
    tmaxPoints=maxPoints;           // für vorzeitge Abbruchbedingung
    DL(COLOR_RGB(0, 0, 255));       // Kurven Farbe
    DL(LINE_WIDTH(40));
    DL(BEGIN(LINE_STRIP));
    DL(VERTEX2F((x0+0)<<4, (y0-0)<<4));
    for (i=t=0;i<tmaxPoints;i++) {
        t+=px(i); tc=(maxX*t)/ZeitMax;
        v=(maxY*py(i))/TEMPSETmax();
        if ((x0+tc)>1000) {tmaxPoints=i; break;} // Grenze nicht erhöhen!
        DL(VERTEX2F((x0+tc)<<4, (y0-v)<<4));
    }
    DL(END());
    // Mess-Markierung an Achsen einzeichnen (im Vordergrund)
    DL(RESTORE_CONTEXT());
    DL(TAG(0));
    for (i=t=0;i<tmaxPoints;i++) {
        t+=px(i); tc=(maxX*t)/ZeitMax;
        v=(maxY*py(i))/TEMPSETmax();
        k=px(i);  kc=(maxX*k)/ZeitMax;
        if (k>0) {
            len=sprintf(s,k?"%is|":"",k);
            // Schrift #18, "|" muss durch Doppeldruck mit Versatz verlängert werden:
            // Vorteil: schärfer, Nachteil: 200 Byte mehr in Display-List!
            DL_CMD_TEXT(x0+tc+2, y0-16+((kc<(len*7))?-10:0), 18, OPT_RIGHTX, s);
            DL_CMD_TEXT(x0+tc+2, y0-16+((kc<(len*7))?-7:3), 18, OPT_RIGHTX, "|");
            if (kc<(len*7)) DL_CMD_TEXT(x0+tc+2, y0-16, 18, OPT_RIGHTX, "|");
            if (kc<(len*7)) DL_CMD_TEXT(x0+tc+2, y0-14, 18, OPT_RIGHTX, "|");
            // Schrift #27 (proportional), "|" wird unterlängig gezeichnet:
            // DL_CMD_TEXT(x0+tc+2, y0-19+((kc<(len*7))?-10:0), 27, OPT_RIGHTX, s);
            // if (kc<(len*7)) DL_CMD_TEXT(x0+tc+2, y0-19, 27, OPT_RIGHTX, "|");
        }
        k=py(i);
        if (k>=20) {
            sprintf(s,"%i-",k);
            DL_CMD_TEXT(x0, y0-v, 27, OPT_CENTERY | OPT_RIGHTX, s);
        }
    }
    // Kurvenpunkte anzeigen
    DL(COLOR_RGB(255, 255, 0));     // Punkt Farbe
//  DL(COLOR_RGB(238, 238, 0));     // Punkt Farbe
    ps = (MF40_WorkMode==MF40_IntRunRamp)?100:200;
    DL(POINT_SIZE(ps));
    DL(BEGIN(POINTS));
    for (i=t=0;i<tmaxPoints;i++) {
        if (i==PointModify-1) {DL(COLOR_RGB(255, 153, 10));DL(POINT_SIZE(250));}
        else                  {DL(COLOR_RGB(255, 255, 0));  DL(POINT_SIZE(ps));}
        DL(TAG(tag+i));
        t+=px(i); tc=(maxX*t)/ZeitMax;
        v=(maxY*py(i))/TEMPSETmax();
        DL(VERTEX2F((x0+tc)<<4, (y0-v)<<4));
    }
    DL(END());
    // Mess-Cursor (nur) während RAMP-RUN anzeigen
    if (MF40_WorkMode==MF40_IntRunRamp) {
//      DL(COLOR_RGB(255, 0, 0));       // Kurven Farbe rot
//      DL(COLOR_RGB(255, 0xAA, 0));    // Kurven Farbe hellbraun
        DL(COLOR_RGB(0, 255, 0));       // Kurven Farbe grün
        DL(LINE_WIDTH(20));
        DL(BEGIN(LINE_STRIP));
        tc=(maxX*(MF40_RUNTIME10/10))/ZeitMax;
        v=(maxY*MF40_Regel_Temp)/TEMPSETmax();
        DL(VERTEX2F((x0+0)<<4,  (y0-v)<<4));
        DL(VERTEX2F((x0+tc)<<4, (y0-v)<<4));
        DL(VERTEX2F((x0+tc)<<4, (y0-0)<<4));
        DL(END());

        DL_CMD_FGCOLOR(0xED22FF);
        DL(COLOR_RGB(255, 255, 255));
        sprtTimer(s,MF40_RUNTIME10/10);
        DL_CMD_BUTTON(x0, y0+6, 89, 22, 28, OPT_FLAT, s);
        sprintf(s,"%i",MF40_Regel_Temp);
        DL_CMD_BUTTON(x0+85+11, y0+6, 55, 22, 28, OPT_FLAT, s);
    } else if (PointModify) {
        DL(COLOR_RGB(255, 255, 255));
        // aktive Box: blau, inaktive Box: dunkelgrau
        if (PMrotm) DL_CMD_FGCOLOR(0x8D8D8D); else DL_CMD_FGCOLOR(0x0505E8);
        sprtTimer(s,px(PointModify-1));
        DL_CMD_BUTTON(x0, y0+6, 89, 22, 28, OPT_FLAT, s);
        if (PMrotm) DL_CMD_FGCOLOR(0x0505E8); else DL_CMD_FGCOLOR(0x8D8D8D);
        sprintf(s,"%i",py(PointModify-1));
        DL_CMD_BUTTON(x0+85+11, y0+6, 55, 22, 28, OPT_FLAT, s);
    }
}

// *****************************************************************************
int EvaluateRampSet(void)
// *****************************************************************************
{
int i, sum;
    for (i=sum=0; i<MF40_Userdata.RP[MF40_Userdata.ars].RampPointsUsed; i++) {
        sum += MF40_Userdata.RP[MF40_Userdata.ars].RP[i].dt;
    }
    return (((sum/60)+1)*60);
}

// *****************************************************************************
// FT81X Display: MF40 CHART                                          13.11.2017
// *****************************************************************************
// Zeitmessungen: EVE selber benötigt ca. 19ms (ca. 41ms max beim Kurvenpunktschieben),
// diese Routine max. 18ms (siehe busy Optimierung)
// *****************************************************************************
void FT81X_Display_Ramp_MF40(int first_req)
{
uint32  tracker;
uint16  x,y,xd,yd;
uint32  ui32;
uint32  col1,col2;
uint16  offx=0, offy=0;
int     i,j,k, tr;
uint32  ul;
char    c;
char    s[150], ledstr[10];

static  int     busy=0;
static  char    cl;
static  uint    downcnt;
static  int     first;
static  int     PointModify;
static  int     PMct,PMx,PMxa,PMy,PMrotm;
static  uint32  xy;
static  int     last_ZeitMax, last_WorkMode;
static  uint32  ms32_last;
//static  uint16  ms,maxms;
    // .........................................................................
    if (busy) {if (EVE_busy()) return; busy=0; goto BUSY_ENTRY;}
    // busy Optimierung ab 16.10.2018
    // max. Zeitbedarf (bei !first): 8ms (17ms bei voller TempCurve),
    // ohne busy Verwendung wären es (oder bei first!=0): 12ms (21ms)
    // Hinweis: weitere Optimierung z.B. durch Verwendung von EVE_busy() mit 
    // return beim Übertragen der SPI-Daten ist zwecklos, siehe EVE_busy()!
    // .........................................................................
    if (first_req) {
        first = 10;             // 10 Durchläufe keine Touch-Events auswerten
        PointModify=0;
        ZeitMax=EvaluateRampSet();
        getRotaryUpDown();getRotarySwitch();// zu Beginn alte, noch anstehende Anforderungen abräumen
    }
    // .........................................................................
    if (CMD_busy()) return;     // warten, bis CMD Fifo abgearbeitet
    // .........................................................................
    xy = rd32(REG_CTOUCH_TOUCH0_XY);
    if ((xy>>16)!=32768) {downcnt++;} else {downcnt=0;}
    // .........................................................................
    CDLinit();
    DL(CMD_DLSTART);
    // .........................................................................
    DL(CLEAR(1, 1, 1));
    DL_CMD_FGCOLOR(0x1F3F7F);
    DL_CMD_BGCOLOR(0x0000FF);
//  DL_CMD_GRADIENT(95, 115, 0xFFFFFF, 95, 363, 0xFFFFFF);
//  DL_CMD_GRADIENT(95, 115, 0xF0F0F0, 95, 363, 0xF0F0F0);
    DL_CMD_GRADIENT(95, 115, 0xE2E2E2, 95, 363, 0xE2E2E2);
    DL(TAG(0x1F));
    DL(COLOR_RGB(0, 0, 0));
    if (memcmp((char*)&MF40_Userdata,(char*)&MF40_Userdata_backup,sizeof(MF40_Userdata_backup)))
        sprintf(s,"%s%s",MF40_Userdata.RP[MF40_Userdata.ars].Ramp_Name," (Save)");
    else
        sprintf(s,"%s%s",MF40_Userdata.RP[MF40_Userdata.ars].Ramp_Name,"");
    DL_CMD_TEXT(401, 96, 30, OPT_CENTER, s);
    DL(COLOR_RGB(255, 3, 36));
    DL_CMD_TEXT(400, 95, 30, OPT_CENTER, s);

//ms = MilliSekunden; // >>>>>

    // Rampen-Kurve
    DL(TAG(0x1E));
    ShowGraph(110,120,0x30,PointModify,PMrotm);

    // Temperatur-Messkurve ggf. löschen
    if ((last_WorkMode!=MF40_WorkMode)&&(MF40_WorkMode==MF40_IntRunRamp)) {
//  if ((last_ZeitMax!=ZeitMax) || ((last_WorkMode!=MF40_WorkMode)&&(MF40_WorkMode==MF40_IntRunRamp))) {
        ClrTempCurve();
    }
    last_ZeitMax=ZeitMax; last_WorkMode = MF40_WorkMode;
    if (MF40_Userdata.TEMP_used) {
        // Temperatur-Messkurve füllen
        FillTempCurve(600,300);
        // Temperatur-Messkurve darstellen - über der Soll-Kurve
        DL(TAG(0x1D));
        ShowTempCurve(110,120,600,300); // belastet DL_BUF mit maxCurvePoints * 4 --> +1200 ==> total ca. 3600 Bytes
    }
//ms = MilliSekunden-ms; if (ms>maxms) maxms=ms; TESTTEST=maxms; // <<<<< Selbst bei 300 Messpunkten nicht mehr als 1ms

    // Mess-Felder
    RampBoxes(71,14,0x40);    // 5er: Breite=500-20
            
    // LEDs
    LEDboxes(1);

    showAlphoFooter(showTestValues);
    
//  ShowEllipsen(400,6,0,0);          // oben
    ShowEllipsen(4,240-10,1,2);       // links
    ShowEllipsen(800-4,240-10,1,2);   // rechts
//  ShowEllipsen(400,455,0,2);        // unten

//for (i=0;i<100;i++) DL_CMD_FGCOLOR(0x1F3F7F);
//TESTTEST=DL_L;
     
    // .........................................................................
    DL_TEST();
    DL(END());
    DL(DISPLAY());
    // .........................................................................
    DL(CMD_SWAP);
//ms = MilliSekunden; // >>>>>
//TESTTEST=DL_L;  // (2347 ohne TempCurve)
    if (CDLexec()) return;
    busy++;
    return;
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
BUSY_ENTRY:

    // Tastatur (verwendet nicht den TRACKER, aber den TOUCH_TAG)
    if ((c=rd8(REG_TOUCH_TAG))>0) {
        if (first) cl=c;
        switch(c) {
            case 0x20+2:if (downcnt>30) ResetTimer=ResetTimerSET; break;// RESET
        }
        if (cl!=c) {
            switch(c) {
                case 0x1e:  break;                          // Graph
                case 0x1f:  ZeitMax=EvaluateRampSet();      // Titel
                            MF40_Userdata_Save(1);
                            MF40_Lifedata_Save();
                            break;
                // RampBoxes ab 0x40
                case 0x40:  if (showTestValues) showTestValues = 0;
                            else {if ((MilliSekunden32-ms32_last)<500) showTestValues=1; ms32_last=MilliSekunden32;}
                            break;
            }
            if ((c>=0x20)&&(c<=0x2f)) {}            // LEDs 0x20..0x2f
            if ((c>=0x30)&&(c<=0x37)) {             // Points 0x30..
                PointModify=c-0x30+1;
            }
        }
    }
    cl=c;
    if (MF40_WorkMode==MF40_IntRunRamp) PointModify=0;  // keine Punkt-Modifikation während RAMP-RUN
    i=getRotaryUpDown();
    if (PointModify) {
        if (getRotarySwitch()) PMrotm=PMrotm?0:1;
        if (i) {
            PMct=0;
            if (PMrotm) {
                y=py(PointModify-1)+i;
                if (y&B15) y=0; else if (y>TEMPSETmax()) y=TEMPSETmax();
                py(PointModify-1)=y;
            } else {
                x=px(PointModify-1)+(i*1);
                if (x&B15) x=0; else if (x>ZeitMax) x=ZeitMax;
                else px(PointModify-1)=x;
            }
        } else {
            if ((x = xy >> 16) < 800) {
//          if ((x = (xy = rd32(REG_CTOUCH_TOUCH0_XY)) >> 16) < 800) {
                y = (xy&0xffff);
                if (++PMct>=20) {
                    if (PMct==20) {PMx=x-gx0;PMxa=px(PointModify-1);PMy=y;}
                    x=x-gx0;
                    if (x&B15) x=0; else if (x>gxm) x=gxm;
                    if ((i=PMxa+((x-PMx)*ZeitMax)/gxm)>=0) px(PointModify-1)=i;
                    y=gy0-y;
                    if (y&B15) y=0; if (y>gym) y=gym;
                    py(PointModify-1)=(y*TEMPSETmax())/gym;
                } else {
                    if ( (x<gx0-15)||(x>gx0+gxm+15) || (y<gy0-gym-15)||(y>gy0+15) )
                        {PointModify=0; PMct=0;}
                }
            } else PMct=0;
        }
    } else {
        PMrotm=PMct=0;
    }

//ms = MilliSekunden-ms; if (ms>maxms) maxms=ms; TESTTEST=maxms;

//  if (STOP_Taster_ms>1000) hub_state=hub_Setup; 
    // ----- Swipe windows
    if (!PMct) if (SwipeWindows(first, 0)) return;

    if (first) first--;
}


// *****************************************************************************
// + / - Buttons Darstellung rechts unten für alle Setups gemeinsam   14.07.2019
// *****************************************************************************
void showPlusMinusButtons(void)
{
uint16 x,y,xd,yd,x0,y0;
    x=x0=720-40; y=y0=480-87; xd=0; yd=44;
    DL(COLOR_RGB(255, 255, 255));
    DL_CMD_FGCOLOR(0x1F3F7F);
    DL(TAG('+'));
    DL_CMD_BUTTON(x, y, 60, 35, 30, 0, "+"); x+=xd; y+=yd;
    DL(TAG('-'));
    DL_CMD_BUTTON(x, y, 60, 35, 31, 0, "-"); x+=xd; y+=yd;
}

// *****************************************************************************
// *****************************************************************************
prtErrText(char * errstr,int m)
{
    switch(m) {
        case cmdErr_OK:
        case cmdErr_OK_SN:
        case cmdErr_OK_SI:
            strcpy(errstr, "---OK---"); break;
        default:
        case cmdErr_SyntaxError:
            strcpy(errstr, "---SYNTAX ERROR---"); break;
        case cmdErr_RangeError:
            strcpy(errstr, "---RANGE ERROR---"); break;
        case cmdErr_AccessDenied:
            strcpy(errstr, "---ACCESS DENIED---"); break;
        //........................................
        case cmdErr_OK_RAMP:
            strcpy(errstr, "---RAMP OPTION ACTIVE---"); break;
        case cmdErr_OK_GENOFF:
            strcpy(errstr, "---GENOFF OPTION ACTIVE---"); break;
        case cmdErr_OK_MODBUS:
            strcpy(errstr, "---MODBUS OPTION ACTIVE---"); break;
        case cmdErr_OK_NO_SI:
            strcpy(errstr, "---SI NOT ACTIVE---"); break;
    }
}

const char toggleText[maxSetupToggle][30] = {   // "" --> Toggle wird nicht angezeigt
        "NO RAMP|RAMP",
        "NO GenOFF|Use GenOFF",
        "NO MODBUS|MODBUS",
        "ANA 100ms|ANA 200ms",      // ab V2.02.08, zuvor: "R-LED 100ms|R-LED 300ms"
        "NO Temp.SIM|Temp.SIM",
        "NO ExtTest|ExtTestDisp",
        "SI active|SI not active",          // ab V2.03.03, in V2.03.02: "", zuvor: "PIDmode=V3|PIDmode=V2"   
        "PIC-MF|PIC-HF",            // ab V2.02.08
};
 const char DispText_mf[8][10] = {"P","P SET","I","Freq","HF-Factor","TIMER","TEMP","TEMP Set"};
 const char DispText_hf[8][10] = {"Id","U-SET","U-HF","Freq","-","TIMER","TEMP","TEMP Set"};

 // *****************************************************************************
// FT81X Display: MF40 SETUP                                          12.11.2017
// *****************************************************************************
void FT81X_Display_FactorySetup_MF40(int first_req)
{
uint32 tracker;
uint32 xy;
uint16 x,y,xd,yd,x0,y0;
uint32 ui32;
uint32 col1,col2;
uint16 offx=0, offy=0;
int i,j,m, r, tr, zeit;
char c;
char s[80], s1[30], s2[30];
uint16 * togglep;
float ff;

static  char    cl;
static  int     first;
static  uint    downcnt;
static  int     selx;
static  char    errstr[60];
static  int     showerr;
static struct   MF40_Setup_Struct SU;
static uint16   mfhf;

    togglep = &SU.RAMP_Option;
    // .........................................................................
    if (first_req) {
        first = 10;             // 10 Durchläufe keine Touch-Events auswerten
        getRotaryUpDown();getRotarySwitch();// zu Beginn alte, noch anstehende Anforderungen abräumen
        showerr=0;
        if (hub_state_last==hub_Keyboard) {
            if (Keyboard_ReadyState) {
                r=MF40_Interpreter(Keyboard_String, 0);
                if (r==cmdErr_OK_SN) {      // Sonderbehandlung für SN=n -->
                    SU.SN = MF40_Setup.SN;  // neue MF40_Setup.SN in SU kopieren
S_W:                hub_state=hub_News;     // ausnahmsweise in News springen (Anzeige der SN), zuvor
                    goto SAVE_WRITE;        // SU bedingungslos in MF40_Setup schreiben.
                }
                if (r==cmdErr_OK_SI) {
                    SU.SI = MF40_Setup.SI;  // neue MF40_Setup.SI in SU kopieren
                    goto S_W;
                }
                if (r==cmdErr_OK_SPLASHONLY) {
                    SU.SplashOnly = MF40_Setup.SplashOnly;  // neues MF40_Setup.SplashOnly in SU kopieren
                    goto S_W;
                }
                prtErrText(errstr,r);       // Fehlermeldung anzeigen
                showerr=300;                // Dauer in Schleifenanzahlen
            }
        } else {
            memcpy(&SU,&MF40_Setup,sizeof(SU));
            mfhf = SU.MF0_HF1;
        }
        Keyboard_ReadyState=0;
        selx=0;
        SysOpen=120; 
    } else {
        if (mfhf != SU.MF0_HF1) {
            MF40_Setup_MF_HF(&SU); mfhf = SU.MF0_HF1;
        }
    }
    // .........................................................................
    if (CMD_busy()) return;     // warten, bis CMD Fifo abgearbeitet
    // .........................................................................
    xy = rd32(REG_CTOUCH_TOUCH0_XY);
    if ((xy>>16)!=32768) downcnt++; else downcnt=0;
    // .........................................................................
    i=getRotaryUpDown();
    switch (selx-1) {
        case 0: break;
        case 1: break;
        case 2: break;
        case 3: break;
        case 4: break;
        
        case 5: if (i) SU.PSETMAX = IncDecStepwise(SU.PSETMAX,   1,   i, 3, 1000); break;
        case 6: if (i) SU.PISTMAX = IncDecStepwise(SU.PISTMAX,1,   i, 3, 1000); break;
        case 7: if (i) SU.IMAX    = IncDecStepwise(SU.IMAX,   1,   i, 0, 2000); break;
        case 8: if (i) SU.FMAX    = IncDecStepwise(SU.FMAX,   1,   i, 0, 2000); break;
        case 9: break;

        case 10:break;
        case 11:break;
        case 12:break;
        case 13:break;
        case 14:break;

        case 15:break;
        case 16:break;
        case 17:break;
        case 18:break;
        case 19:break;
    }
    // .........................................................................
    CDLinit();
    DL(CMD_DLSTART);
    // .........................................................................
    DL(CLEAR(1, 1, 1));
    DL_CMD_FGCOLOR(0x1F3F7F);
    DL_CMD_BGCOLOR(0x0000FF);
    DL_CMD_GRADIENT(23, 42, 0x2E9AFF, 23, 270, 0x1C1FFF);
    x=400; y=405;
    showSetupFooter(x, y);  // verwendet Tag 0x11(Def), 0x12(X), 0x13(SAVE), 0 (beim Verlassen)
    DL(TAG(0x1F));
    DL_CMD_TEXT(x, 20-5, 30, OPT_CENTERX, showerr?errstr:"FACTORY SETUP");
    if (showerr) showerr--;
    // ----- Toggles
    DL_CMD_FGCOLOR(0xFFFF00);
    for (i=0,x=x0=60,y=y0=85,xd=180,yd=60,tr=0x20; i<maxSetupToggle; i++,tr++,y+=yd) {
        switch(i) {
            case 5: x+=xd; y=y0; y+=4*yd; break;
            case 6: x+=xd; y=y0; break;
            case 7: x+=xd; y=y0; break;
        }
        if (toggleText[i][0]) {
            DL_CMD_TRACK( x, y, 135, 0, tr);
            DL(TAG(tr));
            DL_CMD_TOGGLE(x, y, 135, 28, 0, togglep[i]?0xffff:0, (char*)toggleText[i]);
        }
    }
    // ----- + / - Buttons
    if (selx) showPlusMinusButtons();
    // ----- Num. Input Buttons
    x=x0=50; y=y0=85; xd=0; yd=60;
    for (i=0; i<10; i++,y+=yd) {
        if (i) if (!(i%5)) {x+=180; y=y0;}
        m=0;
        switch(i) {
            // .................................................................
            case 5: ff=SU.PSETMAX; fMIMA(&ff,3.6,1000); sprintf(s1,"%.1f",ff); strcpy(s2,isMF40?"PSET-MAX [kW]":"USET-MAX [V]"); break;
            case 6: ff=SU.PISTMAX; fMIMA(&ff,3.6,1000); sprintf(s1,"%.1f",ff); strcpy(s2,isMF40?"PIST-MAX [kW]":"Id-MAX [A]"); break;
            case 7: sprintf(s1,"%.i",SU.IMAX); strcpy(s2,isMF40?"I-MAX [A]":"U-HF-MAX [V]"); break;
            case 8: sprintf(s1,"%.i",SU.FMAX); strcpy(s2,"FREQ-MAX [kHz]"); break;
//          case 9:
            // .................................................................
            default: continue;
        }
        DL(COLOR_RGB(255, 255, 255));
        DL(TAG(0x40+i));
        if (m)  {DL_CMD_FGCOLOR(0x1F3F7F);} // 0x55AAFF)
        else    {if (selx==i+1) DL_CMD_FGCOLOR(0x000000); else DL_CMD_FGCOLOR(0x0000FF);}
        DL_CMD_BUTTON(x, y-8, 150, 35, (m>15)?m:30, 0, s1);
        DL_CMD_TEXT(x+75, y-20-8, 27, OPT_CENTERX, s2);
    }
    
    // ----- Display+RegLED On/Off
    DL(TAG(0x1E));
    DL_CMD_TEXT(616, 140, 28, OPT_CENTERX, "Display + RegLED On/Off ");
    DL_CMD_FGCOLOR(0xFFFF00);
    DL(COLOR_RGB(0, 0, 0));
    for (i=0,x=x0=560,y=y0=175,xd=0,yd=45; i<8; i++) {
        DL(TAG(0x30+i));
        if (SU.DisplaySelect&(1<<i)) DL_CMD_FGCOLOR(0xFFFF00); else DL_CMD_FGCOLOR(0xA0A0A0);
        DL_CMD_BUTTON(x+((i&B0)?95:0), y+((i>>1)*yd), 79, 36, 27, OPT_FLAT, isMF40?(char*)DispText_mf[i]:(char*)DispText_hf[i]);
        DL_CMD_FGCOLOR(0x06A125);
    }
    DL(TAG(0x38));
    if (SU.NoRegLEDs==0) DL_CMD_FGCOLOR(0x35E305); else DL_CMD_FGCOLOR(0xA0A0A0);
    DL(COLOR_RGB(255, 255, 255));
    DL_CMD_BUTTON(500, 235, 40, 111, 27, OPT_FLAT, "LED");
    DL_CMD_TEXT(519, 260, 27, OPT_CENTERX, "Reg");

    // ----- Operation times
//  x=420; y=75; yd=20;
    x=400; y=365;
    i=0;
    DL(TAG(0x1C));
    zeit=MF40_Lifedata.OprTime;
    i+=sprintf(s+i,"Operation time=%u:%02u:%02u, ",zeit/3600,(zeit/60)%60,zeit%60);
    zeit=MF40_Lifedata.OprTimeGenerator;
    i+=sprintf(s+i,"Generator=%u:%02u:%02u (%u)",zeit/3600,(zeit/60)%60,zeit%60,MF40_Lifedata.GeneratorCycles);
    DL_CMD_TEXT(x-390, y, 27, 0, s);
//  DL_CMD_TEXT(x, y, 27, 0, s);
//  y+=yd;
    i=0;
    DL(TAG(0x1D));
    zeit=MF40_Lifedata.OprTime_1;
    i+=sprintf(s+i,"Service time=%u:%02u:%02u, ",zeit/3600,(zeit/60)%60,zeit%60);
    zeit=MF40_Lifedata.OprTimeGenerator_1;
    i+=sprintf(s+i,"Generator=%u:%02u:%02u (%u)",zeit/3600,(zeit/60)%60,zeit%60,MF40_Lifedata.GeneratorCycles_1);
//  DL_CMD_TEXT(x, y, 27, 0, s);
    DL_CMD_TEXT(x+390, y, 27, OPT_RIGHTX, s);

    // .........................................................................
    DL_TEST();
    DL(DISPLAY());
    // .........................................................................
    DL(CMD_SWAP);
    if (CDLexec()) return;
    // ~~~~~
    tracker = rd32(REG_TRACKER);
    if (!first) {
        i=(tracker&0xff);
        switch(i) {
            case 0: break;
        }
        if ((i>=0x20)&&(i<0x20+maxSetupToggle)) {
            togglep[i-0x20]=((tracker>>16)>0x8000)?1:0;
        }
    }
    // Tastatur (verwendet nicht den TRACKER, aber den TOUCH_TAG)
    if ((c=rd8(REG_TOUCH_TAG))>0) {
        if (first) cl=c;
        if ((c=='+') || (c=='-')) {if (downcnt>12) cl=~c;}  // Repeat nur von '+' und '-'
        if (cl!=c) {
            SysOpen=120;
            if (!(((c>=0x40)&&(c<=0x53))||(c=='+')||c=='-')) selx=0;
            switch(c) {
                case 0x11:  MF40_Setup_Default(&SU, 1);             // Default-Werte herstellen
                            break;
                case 0x12:  hub_state=hub_Default;                  // Cancel
                            MF40_Setup_MF_HF(&MF40_Setup);
                            break;
                case 0x13:  hub_state=hub_Default;                  // Save
 SAVE_WRITE:                memcpy(&MF40_Setup,&SU,sizeof(MF40_Setup));
                            MF40_Setup_Save(0);                     // komplett
                            MF40_Lifedata_Save();
                            return;
//              case 0x1C:  // "Operation time..."
//              case 0x1D:  // "Service time..."
//              case 0x1E:  // "Display + RegLED..."
                case 0x1F:  Keyboard_Callers_hub_state=hub_state;
                            hub_state=hub_Keyboard;
                            strcpy(Keyboard_String,"");
                            Keyboard_Len = 25;
                            sprintf(Keyboard_Title,"Enter Command (InitService, InitLife, InitUser, SN=n, SI=n): ");
                            Keyboard_Mode = 0;
                            return;
                case 0x30: case 0x31: case 0x32: case 0x33:  
                case 0x34: case 0x35: case 0x36: case 0x37:
                            if (SU.DisplaySelect&(1<<(c-0x30))) SU.DisplaySelect&=~(1<<(c-0x30));
                            else SU.DisplaySelect|=(1<<(c-0x30));
                            break;
                case 0x38:  SU.NoRegLEDs = SU.NoRegLEDs?0:1; break;
                case 0x40: case 0x41: case 0x42: case 0x43: case 0x44:
                case 0x45: case 0x46: case 0x47: case 0x48: case 0x49:
                case 0x4A: case 0x4B: case 0x4C: case 0x4D: case 0x4E:
                case 0x4F: case 0x50: case 0x51: case 0x52: case 0x53:
                           selx=c-0x40+1; break;
                case '+':  RotaryUpDown++; break;                  // Up
                case '-':  RotaryUpDown--; break;                  // Down
            }
        }
    }
    cl=c;
    // ----- Swipe windows
//  if (SwipeWindows(first, 0)) return; // Lahmgelegt, besser explizite Auswahl...

    if (first) first--;
}

// *****************************************************************************
// FT81X Display: MF40 USER-SETUP                                     09.10.2018
// *****************************************************************************
void FT81X_Display_UserSetup_MF40_1(int first_req)
{
uint32 tracker;
uint32 xy;
uint16 x,y,xd,yd,x0,y0;
uint32 ui32;
uint32 col1,col2;
uint16 offx=0, offy=0;
int i,j,k,m, r, tr;
char c;
char s[80],s1[25],s2[25];
uint16 * togglep;
int     noTemp;
int     RED=0;
static  char    cl;
static  uint    downcnt;
static  int     first;
static  int     selx;
static  char    errstr[60];
static  int     showerr;
static struct   MF40_Userdata_Struct UD;
    // .........................................................................
    if (first_req) {
        first = 10;             // 10 Durchläufe keine Touch-Events auswerten
        getRotaryUpDown();getRotarySwitch();// zu Beginn alte, noch anstehende Anforderungen abräumen
        showerr=0;
        if (hub_state_last==hub_Keyboard) {
            if (Keyboard_ReadyState) {
                r=MF40_Interpreter(Keyboard_String, 1);
                prtErrText(errstr,r);
                showerr=300;
            }
        } else {
            memcpy(&UD,&MF40_Userdata,sizeof(UD));
        }
        Keyboard_ReadyState=0;
        selx=0;
    }
    // .........................................................................
    if (CMD_busy()) return;     // warten, bis CMD Fifo abgearbeitet
    // .........................................................................
    xy = rd32(REG_CTOUCH_TOUCH0_XY);
    if ((xy>>16)!=32768) downcnt++; else downcnt=0;
    // .........................................................................
    i=getRotaryUpDown();
    switch (selx-1) {
        case 0: if (i) UD.TEMP_used=IncDecStepwise(UD.TEMP_used, 1, i, 0, 2); break;
        case 1: if (i) UD.Pyro_1_Sc.v[0]=IncDecStepwise(UD.Pyro_1_Sc.v[0], 0, i, -400, TEMP_max_SET); break;    // 1200 --> 2500=TEMP_max_SET am 15.09.2019
        case 2: if (i) UD.Pyro_1_Sc.v[1]=IncDecStepwise(UD.Pyro_1_Sc.v[1], 0, i, -400, TEMP_max_SET); break;    // 1200 --> 2500=TEMP_max_SET am 15.09.2019
        case 3: if (i) UD.TEMP_max=IncDecStepwise(UD.TEMP_max, 50, i, 50, TEMP_max_SET); break;                 // 1500 --> 2500=TEMP_max_SET am 15.09.2019
        case 4: if (i) UD.TEMP_stop=IncDecStepwise(UD.TEMP_stop, 2, i, 0, UD.TEMP_max); break;
        
        case 5: if (i) UD.TIMER_max=IncDecStepwise(UD.TIMER_max, 10*60, i, 10*60, 18*3600); break;
        case 6: UD.TIMER_delayed = UD.TIMER_delayed?0:1; selx=0; break;
        case 7: break;
        case 8: break;
        case 9: break;

        case 10:UD.DisplayRoll = UD.DisplayRoll?0:1; selx=0; break;
        case 11:break;
        case 12:break;
        case 13:break;
        case 14:break;

        case 15:if (i) UD.MB_Slave_Adr = IncDecStepwise(UD.MB_Slave_Adr, 1, i, 1, 250); break;
        case 16:if (i) UD.MB_Slave_Baud = IncDecStepwise(UD.MB_Slave_Baud, 9600, i, 9600, 230400); break;
        case 17:if (i) UD.MB_Slave_Parity = IncDecStepwise(UD.MB_Slave_Parity, 1, i, 0, 2); break;
        case 18:break;
        case 19:break;
    }
    // Ermitteln, ob Temperaturmessungs relevante Parameter angezeigt werden sollen, oder nicht:
    noTemp = UD.TEMP_used ? 0 : 1;
    if ((MF40_Setup.DisplaySelect&(B6+B7))!=(B6+B7)) noTemp=2;   // generell keine anzeigen?
    // .........................................................................
    CDLinit();
    DL(CMD_DLSTART);
    // .........................................................................
    DL(CLEAR(1, 1, 1));
    DL_CMD_FGCOLOR(0x1F3F7F);
    DL_CMD_BGCOLOR(0x0000FF);
    DL_CMD_GRADIENT(23, 42, 0x2E9AFF, 23, 270, 0x1C1FFF);
    x=400; y=405;
    showSetupFooter(x, y);  // verwendet Tag 0x11(Def), 0x12(X), 0x13(SAVE), 0 (beim Verlassen)
    DL(TAG(0x1F));
    DL_CMD_TEXT(x, 20-5, 30, OPT_CENTERX, showerr?errstr:"USER SETUP 1");
    if (showerr) showerr--;

    // ----- + / - Buttons
    if (selx) showPlusMinusButtons();
    // ----- Num. Input Buttons
    x=x0=50; y=y0=85; xd=0; yd=60;
    for (i=0; i<21; i++,y+=yd) {
//if (i==2) goto WEWEWEWE;    
        if (i) if (!(i%5)) {x+=180; y=y0;}
        m=RED=0;
        switch(i) {
            // .................................................................
            case 0: if (noTemp>=2) continue; sprintf(s1,"%s",UD.TEMP_used?(UD.TEMP_used==1?"0..20mA":"4..20mA"):"OFF"); strcpy(s2,"Temp.Measuring"); break;
            case 1: if (noTemp) continue; sprintf(s1,"%i",UD.Pyro_1_Sc.v[0]); strcpy(s2,UD.TEMP_used==2?"Pyro-1 4mA -->":"Pyro-1 0mA -->"); break;
            case 2: if (noTemp) continue; sprintf(s1,"%i",UD.Pyro_1_Sc.v[1]); strcpy(s2,"Pyro-1 20mA -->"); break;
            case 3: if (noTemp) continue; sprintf(s1,"%i",UD.TEMP_max); strcpy(s2,"Temp. max."); if (UD.TEMP_max>UD.Pyro_1_Sc.v[1]) RED++; break;
            case 4: if (noTemp) continue; sprintf(s1,UD.TEMP_stop?"%i":"OFF",UD.TEMP_stop); strcpy(s2,"STOP at Temp."); break;
            // .................................................................
            case 5: k=sprtTimer(s1,UD.TIMER_max); if (UD.TIMER_max<=600) strcpy(s1+k,".0"); strcpy(s2,"Timer max."); break;
            case 6: if (noTemp) continue; m=29; sprintf(s1,"%s",UD.TIMER_delayed?"TEMP-SET":"OFF"); strcpy(s2,"Timer Delay"); break;
//          case 7:
//          case 8:
//          case 9:
            // .................................................................
            case 10:m=29; sprintf(s1,"%s",UD.DisplayRoll?"3>1>2>3>1":"1>2>3"); strcpy(s2,"Roll display pages"); break;
//          case 11:
//          case 12:
//          case 13:
//          case 14:
            // .................................................................
            case 15: if (!MF40_Setup.MODBUS) continue; sprintf(s1,"%i",UD.MB_Slave_Adr);    strcpy(s2,"MODBUS Address"); break;
            case 16: if (!MF40_Setup.MODBUS) continue; sprintf(s1,"%lu",UD.MB_Slave_Baud);   strcpy(s2,"MODBUS Baudrate"); break;
            case 17: if (!MF40_Setup.MODBUS) continue; sprintf(s1,"8-%c-1",getParityChar(UD.MB_Slave_Parity)); strcpy(s2,"MODBUS Parity"); break;
//          case 18:
//          case 19:
//          case 20:
            // .................................................................
            default: continue;
        }
        DL(COLOR_RGB(255, 255, 255));
        DL(TAG(0x40+i));
        if (m)  {DL_CMD_FGCOLOR(0x1F3F7F);} // 0x55AAFF)
        else    {if (selx==i+1) DL_CMD_FGCOLOR(RED?0xC00000:0x000000); else DL_CMD_FGCOLOR(RED?0xFF0000:0x0000FF);}
        DL_CMD_BUTTON(x, y-8, 150, 35, (m>15)?m:30, 0, s1);
        DL_CMD_TEXT(x+75, y-20-8, 27, OPT_CENTERX, s2);
    }
WEWEWEWE: 
    if (UD.TEMP_used) {                     // PID-Seite nur beim Temperatur-Regelung
        ShowEllipsen(8,240-10,2,1);         // links
        ShowEllipsen(800-8,240-10,2,1);     // rechts
    }
    // .........................................................................
    DL_TEST();
    DL(DISPLAY());
    // .........................................................................
    DL(CMD_SWAP);
    if (CDLexec()) return;
    // ~~~~~
//    tracker = rd32(REG_TRACKER);
//    if (!first) {
//        i=(tracker&0xff);
//        switch(i) {
//            case 0: break;
//        }
//        if ((i>=0x20)&&(i<0x20+maxSetupToggle)) {
//            togglep[i-0x20]=((tracker>>16)>0x8000)?1:0;
//        }
//    }
    // Tastatur (verwendet nicht den TRACKER, aber den TOUCH_TAG)
    if ((c=rd8(REG_TOUCH_TAG))>0) {
        if (first) cl=c;
        if ((c=='+') || (c=='-')) {if (downcnt>12) cl=~c;}  // Repeat nur von '+' und '-'
        if (cl!=c) {
            if (!(((c>=0x40)&&(c<=0x53))||(c=='+')||c=='-')) selx=0;
            switch(c) {
                case 0x11:  // lokale Default-Werte herstellen
//                          UD.PID_Kp=20.0; UD.PID_Ki=20.0; UD.PID_Kd=0.0;
//                          UD.GENOFFuse=0; UD.SIM_sim=0; 
                            break;
                case 0x12:  hub_state=hub_Default; break;           // Cancel
                case 0x13:  hub_state=hub_Default;                  // Save
                            memcpy(&MF40_Userdata,&UD,sizeof(MF40_Userdata));
                            MF40_Userdata_Save(0);                  // komplett
                            MF40_Lifedata_Save();
                            return;
                case 0x1F:  Keyboard_Callers_hub_state=hub_state;
                            hub_state=hub_Keyboard;
                            strcpy(Keyboard_String,"");
                            Keyboard_Len = 25;
                            sprintf(Keyboard_Title,"Enter Command (InitUser, CMD=n): ");
                            Keyboard_Mode = 0;
                            return;
                case 0x40: case 0x41: case 0x42: case 0x43: case 0x44:
                case 0x45: case 0x46: case 0x47: case 0x48: case 0x49:
                case 0x4A: case 0x4B: case 0x4C: case 0x4D: case 0x4E:
                case 0x4F: case 0x50: case 0x51: case 0x52: case 0x53:
                            selx=c-0x40+1; break;
                case '+':  RotaryUpDown++; break;                  // Up
                case '-':  RotaryUpDown--; break;                  // Down
            }
        }
    }
    cl=c;
    // ----- Swipe windows
    if (UD.TEMP_used) {             // PID-Seite nur beim Temperatur-Regelung
        if (SwipeWindows(first, 0)) return;
    }
    if (first) first--;
}

// *****************************************************************************
// FT81X Display: MF40 USER-SETUP 2                                   19.01.2019
// *****************************************************************************
void FT81X_Display_UserSetup_MF40_2(int first_req)
{
uint32 tracker;
uint32 xy;
uint16 x,y,xd,yd,x0,y0;
uint32 ui32;
uint32 col1,col2;
uint16 offx=0, offy=0;
int i,j,m, pid, tr;
char c;
char s[80],s1[30],s2[30];
uint16 * togglep;
float val;

static  char    cl;
static  uint    downcnt;
static  int     first;
static  int     selx;
static struct   MF40_Userdata_Struct UD;
    // .........................................................................
    if (first_req) {
        first = 10;             // 10 Durchläufe keine Touch-Events auswerten
        getRotaryUpDown();getRotarySwitch();// zu Beginn alte, noch anstehende Anforderungen abräumen
        memcpy(&UD,&MF40_Userdata,sizeof(UD));
        selx=0;
    }
    // .........................................................................
    if (CMD_busy()) return;     // warten, bis CMD Fifo abgearbeitet
    // .........................................................................
    xy = rd32(REG_CTOUCH_TOUCH0_XY);
    if ((xy>>16)!=32768) downcnt++; else downcnt=0;
    // .........................................................................
    i=getRotaryUpDown();
    if (selx) {
        pid = (selx-1)/5;
        switch ((selx-1)%5) {
            // Änderung der Schrittweite am 06.03.2020: von 0.1 auf 0.05
            case 0: if (i) UD.PID[pid].Kp=0.05*(float)IncDecStepwise((int)((UD.PID[pid].Kp*20.0)+0.5), 1, i, 0, 10000); break;
            case 1: if (i) UD.PID[pid].Ki=0.05*(float)IncDecStepwise((int)((UD.PID[pid].Ki*20.0)+0.5), 1, i, 0, 10000); break;
            case 2: if (i) UD.PID[pid].Kd=0.05*(float)IncDecStepwise((int)((UD.PID[pid].Kd*20.0)+0.5), 1, i, 0, 10000); break;
            case 3: break;
            case 4: if (!GENOFFopt) break; UD.PID[pid].GENOFFuse = UD.PID[pid].GENOFFuse?0:1; selx=0; break;
        }
    }
    // .........................................................................
    CDLinit();
    DL(CMD_DLSTART);
    // .........................................................................
    DL(CLEAR(1, 1, 1));
    DL_CMD_FGCOLOR(0x1F3F7F);
    DL_CMD_BGCOLOR(0x0000FF);
    DL_CMD_GRADIENT(23, 42, 0x2E9AFF, 23, 270, 0x1C1FFF);
    x=400; y=405;
    showSetupFooter(x, y);  // verwendet Tag 0x11(Def), 0x12(X), 0x13(SAVE), 0 (beim Verlassen)
    DL(TAG(0x1F));
    DL_CMD_TEXT(x, 20-5, 30, OPT_CENTERX,   "USER SETUP 2 - PID");
    // ----- + / - Buttons
    if (selx) showPlusMinusButtons();
    // ----- Num. Input Buttons
    x=x0=50; y=y0=85; xd=0; yd=60;
    for (i=0; i<(MF40_Setup.RAMP_Option?25:5); i++,y+=yd) {
        if (i) if (!(i%5)) {x+=140; y=y0;}
        pid=i/5;
        m=0;
        switch(i%5) {
            // .................................................................
            case 0: sprintf(s2,pid?"PID %i":"PID Basis",pid);
                    DL(TAG(0x1F));
                    DL_CMD_TEXT(x+60, y-20-8, 28, OPT_CENTERX, s2); y+=22;
                    sprintf(s1,"%.2f",UD.PID[pid].Kp); strcpy(s2,"Kp"); break;
            case 1: sprintf(s1,"%.2f",val=UD.PID[pid].Ki); if (val>=0.001) {sprintf(s2,"Ki (Tn=%.1fs)",UD.PID[pid].Kp/val);} else strcpy(s2,"Ki"); break;
            case 2: sprintf(s1,"%.2f",val=UD.PID[pid].Kd); if (UD.PID[pid].Kp>=0.001) {sprintf(s2,"Kd (Tv=%.1fs)",val/UD.PID[pid].Kp);} else strcpy(s2,"Kd"); break;
//          case 3:
            case 4: if (!GENOFFopt) continue; m++; sprintf(s1,"%s",UD.PID[pid].GENOFFuse?"YES":"NO"); strcpy(s2,"Use Gen.Off"); break;
            // .................................................................
            default: continue;
        }
        DL(COLOR_RGB(255, 255, 255));
        DL(TAG(0x40+i));
        if (m)  {DL_CMD_FGCOLOR(0x1F3F7F);} // 0x55AAFF)
        else    {if (selx==i+1) DL_CMD_FGCOLOR(0x000000); else DL_CMD_FGCOLOR(0x0000FF);}
        DL_CMD_BUTTON(x, y-8, 120, 35, (m>15)?m:30, 0, s1);
        DL_CMD_TEXT(x+60, y-20-8, 27, OPT_CENTERX, s2);
    }
    ShowEllipsen(8,240-10,2,2);       // links
    ShowEllipsen(800-8,240-10,2,2);   // rechts
    // .........................................................................
    DL_TEST();
    DL(DISPLAY());
    // .........................................................................
    DL(CMD_SWAP);
    if (CDLexec()) return;
    // ~~~~~
//    tracker = rd32(REG_TRACKER);
//    if (!first) {
//        i=(tracker&0xff);
//        switch(i) {
//            case 0: break;
//        }
//        if ((i>=0x20)&&(i<0x20+maxSetupToggle)) {
//            togglep[i-0x20]=((tracker>>16)>0x8000)?1:0;
//        }
//    }
    // Tastatur (verwendet nicht den TRACKER, aber den TOUCH_TAG)
    if ((c=rd8(REG_TOUCH_TAG))>0) {
        if (first) cl=c;
        if ((c=='+') || (c=='-')) {if (downcnt>12) cl=~c;}  // Repeat nur von '+' und '-'
        if (cl!=c) {
            if (!(((c>=0x40)&&(c<=58))||(c=='+')||c=='-')) selx=0;
            switch(c) {
                case 0x11:  // lokale Default-Werte herstellen
//                          UD.PID_Kp=20.0; UD.PID_Ki=20.0; UD.PID_Kd=0.0;
//                          UD.GENOFFuse=0; UD.SIM_sim=0; 
                            break;
                case 0x12:  hub_state=hub_Default; break;           // Cancel
                case 0x13:  hub_state=hub_Default;                  // Save
                            memcpy(&MF40_Userdata,&UD,sizeof(MF40_Userdata));
                            MF40_Userdata_Save(0);                  // komplett
                            MF40_Lifedata_Save();
                            return;
                case 0x40: case 0x41: case 0x42: case 0x43: case 0x44:
                case 0x45: case 0x46: case 0x47: case 0x48: case 0x49:
                case 0x4A: case 0x4B: case 0x4C: case 0x4D: case 0x4E:
                case 0x4F: case 0x50: case 0x51: case 0x52: case 0x53:
                case 0x54: case 0x55: case 0x56: case 0x57: case 0x58:
                            selx=c-0x40+1; break;
                case '+':  RotaryUpDown++; break;                  // Up
                case '-':  RotaryUpDown--; break;                  // Down
            }
        }
    }
    cl=c;
    // ----- Swipe windows
    if (SwipeWindows(first, 0)) return;

    if (first) first--;
}

// *****************************************************************************
void FT81X_Display_Keyboard_MF40(int first_req)                 // 19.11.2017 HR
// *****************************************************************************
// Keyboard_String              - Input und Output String
// Keyboard_Len                 - max. Länge
// Keyboard_Callers_hub_state   - hub_state des Aufrufenden
// Keyboard_ReadyState          - 0: CANCEL, 1:OK (geändert oder nicht)
// Aufruf des Keyboards:
//  Keyboard_String vorgeben, Keyboard_Len (max. Länge) festlegen.
//  Keyboard_Callers_hub_state festlegen
//  hub_state = hub_Keyboard
// Der hub_state verbleibt nun bei hub_Keyboard, bis CANCEL oder OK gewählt
// wird. Dann geschieht:
//  Keyboard_String wird unverändert belassen oder mit der Änderung gefüllt
//  Keyboard_ReadyState : 0-->CANCEL, 1-->OK wurde gewählt
//  hub_state = Keyboard_Callers_hub_state
// *****************************************************************************
// 27.11.2017 - Cursor-Steuerung implementiert
// 25.11.2017 - Shift-Bedienung geändert (Shift=0,1,2), Edit-Feld mit 3D Effekt,
//              Repeat optimiert.
// 19.11.2017 - Erstellt HR
// *****************************************************************************
{
uint32 tracker;
uint32 xy;
uint16 x,y,xd,yd,x0,y0;
uint32 ui32;
uint32 col1,col2;
int i,j, tr;
char c;
char s[120];

static  char    cl;
static  uint    downcnt;
static  int     first;
static  struct  MF40_Setup_Struct SU;
static  int     Shift;
static  char    KB_s[81];
static  uint    cpos, KB_Len;
static  uint32  ms;
static  int     ignoreNext;
static  char    kp;             // Key pressed
// .........................................................................
    if (first_req) {
        first = 10;             // 10 Durchläufe keine Touch-Events auswerten
        Shift=0;
        strcpy(KB_s,Keyboard_String);
        cpos = strlen(KB_s);    // Cursor nach letztem Zeichen
        KB_Len=Keyboard_Len;
        Keyboard_ReadyState=0;
        ms=MilliSekunden32;
        ignoreNext=0;
        kp=0;
    }
    // .........................................................................
    if (CMD_busy()) return;     // warten, bis CMD Fifo abgearbeitet
    // .........................................................................
    xy = rd32(REG_CTOUCH_TOUCH0_XY);
    x = xy >> 16;
    if (x!=32768) downcnt++; else downcnt=0;
    // .........................................................................
    CDLinit();
    DL(CMD_DLSTART);
    // .........................................................................
    DL(CLEAR(1, 1, 1));
    DL_CMD_FGCOLOR(0x1F3F7F);
    DL_CMD_BGCOLOR(0x0000FF);
    DL_CMD_GRADIENT(23, 42, 0xE2E2E2, 23, 270, 0xAAAAAA);

    x=400; y=408;
    DL(COLOR_RGB(255, 0, 0));
    DL(TAG(0x02));
    DL_CMD_BUTTON(x-60-70, y, 120, 46, 31,  0, "X");
    DL(COLOR_RGB(255, 255, 255));
    DL(TAG(0x03));
    DL_CMD_BUTTON(x-60+70, y, 120, 46, 30, 0,  "OK");

    DL(TAG(0));
    DL(COLOR_RGB(255,255,255));
    DL_CMD_TEXT(397-1, 12-1, 29, OPT_CENTERX, Keyboard_Title);
    DL(COLOR_RGB(0,0,0));
    DL_CMD_TEXT(397, 12, 29, OPT_CENTERX, Keyboard_Title);

    DL_CMD_FGCOLOR(0x55AA00);
    DL_CMD_BGCOLOR(0x356A00);
    DL(COLOR_RGB(255,255,255));
    DL_CMD_KEYS(72, 147, 566, 39, 29,  kp, Shift?"!\"|$%&\\()=?":"1234567890/");
    DL_CMD_KEYS(95, 192, 570, 40, 29,  kp, Shift?"QWERTZUIOP*":"qwertzuiop+");
    DL_CMD_KEYS(118, 238, 518, 38, 29, kp, Shift?"ASDFGHJKL'":"asdfghjkl#");
    DL_CMD_KEYS(91, 284, 567, 41, 29,  kp, Shift?">YXCVBNM;:_":"<yxcvbnm,.-");
    DL_CMD_KEYS(243, 333, 310, 41, 29, kp, " ");
    
    // SHIFT
    DL(TAG(7));
    if (Shift) DL_CMD_FGCOLOR(0x356A00);
    DL_CMD_BUTTON(93, 334, 101, 38, 27, 0, (Shift==2)?"SHIFT":"Shift"); //Shift?OPT_FLAT:0
    DL_CMD_FGCOLOR(0x55AA00);
    // Backspace "<<<"
    DL(TAG(0x08));
    DL_CMD_BUTTON(680, 149, 52,  37, 28, 0, "<<<");
    // ENTER
    DL(TAG(0x0D));
    DL_CMD_BUTTON(680, 192, 52, 133, 27, 0, "Enter");
    // CURSOR
    DL(TAG(0x0B));
    DL_CMD_BUTTON(680+(52/2)-38-5, 334, 38, 38, 28, 0, "<");
    DL(TAG(0x0C));
    DL_CMD_BUTTON(680+(52/2)+5,    334, 38, 38, 28, 0, ">");
    
    DL(TAG(0));
    DL(COLOR_RGB(0,0,0));
    DL(BEGIN(RECTS));
    DL(VERTEX2F(2493-16, 894-16));
    DL(VERTEX2F(10493-16, 1707-16));
    DL(END());
    DL(COLOR_RGB(255, 255, 255));
    DL(BEGIN(RECTS));
    DL(VERTEX2F(2493, 894));
    DL(VERTEX2F(10493, 1707));
    DL(END());
    DL(COLOR_RGB(255, 1, 35));
    
    i=strlen(KB_s); if (i>KB_Len) KB_s[i=KB_Len]=0; strcpy(s,KB_s); 
    if (cpos<i) {sprintf(s+cpos,"%c%s",(MilliSekunden&B8)?'|':'\'',KB_s+cpos);}
    else        {sprintf(s+cpos,"%c",(MilliSekunden&B8)?'|':127);}
    DL_CMD_TEXT(172, 82, 30, OPT_CENTERY, s);
//    sprintf(s,"%s%c%s",KB_s,(MilliSekunden&B9)?'|':127,"");
//    DL_CMD_TEXT(172, 82, 29, OPT_CENTERY, s);
//  sprintf(s,"%s%s%s",KB_s,(MilliSekunden&(B9+B8))?"|":":","asdf");
//  DL_CMD_TEXT(172, 82, 30, OPT_CENTERY, s);

    // .........................................................................
    DL(END());
    DL(DISPLAY());
    // .........................................................................
    DL(CMD_SWAP);
    if (CDLexec()) return;
    // ~~~~~
    tracker = rd32(REG_TRACKER);
    if (!first) {
        i=(tracker&0xff);
        switch(i) {
        }
    }
    kp=0;
    // Tastatur (verwendet nicht den TRACKER, aber den TOUCH_TAG)
    if ((c=rd8(REG_TOUCH_TAG))>0) {
        if (first) cl=c;
        kp=c;
        if ((c>=8) && (c<0x7f)) {if (cl!=c) downcnt=0; if (downcnt>25) {cl=~c;downcnt-=5;}} // Repeat nur von "Zeichen"
        if (cl!=c) {
            if (i=ignoreNext) ignoreNext=0;
            if (!(i && ((MilliSekunden32-ms)<100))) {
                switch(c) {
                    case 0x02:  hub_state=Keyboard_Callers_hub_state;   // Cancel
                                Keyboard_ReadyState=0;
                                return;
                    case 0x0D:
                    case 0x03:  hub_state=Keyboard_Callers_hub_state;   // ENTER
                                strcpy(Keyboard_String, KB_s);
                                Keyboard_ReadyState=1;
                                return;
                    case 0x07:  if (!Shift) {Shift++; ms=MilliSekunden32; break;}// SHIFT (ohne Repeat)
                                if (Shift==2) {Shift=0; break;}
                                if ((MilliSekunden32-ms)<1000) Shift=2; else Shift=0;
                                break;
                    case 0x08:  i=strlen(KB_s);                         // BACKSPACE
                                if (cpos) {                                 // am Stringbeginn kann nichts entfernt werden
                                    if (cpos==i) {if (i) i--; KB_s[i]=0;}   // am Ende entfernen
                                    else         {strcpy(s,KB_s); sprintf(s+cpos-1,"%s",KB_s+cpos); strcpy(KB_s,s);}    // dazwischen
                                    cpos--;
                                }
                                break;
                    case 0x0B:  if (cpos) cpos--; goto RAUS;                // CURSOR LINKS
                    case 0x0C:  if (cpos<strlen(KB_s)) cpos++; goto RAUS;   // CURSOR RECHTS
                }
                if ((c>=20)&&(c<0x7f)) {                // druckbares Zeichen an cpos einfügen
                    if ((i=strlen(KB_s))<KB_Len) {      // nur wenn noch Platz ist
                        strcpy(s,KB_s);
                        if (cpos<i) {sprintf(s+cpos,"%c%s",c,KB_s+cpos);}   // zwischendrin einfügen
                        else        {sprintf(s+cpos,"%c",c);}               // anhängen
                        strcpy(KB_s,s);
                        if (cpos<KB_Len) cpos++;
                    }
                    if (Shift==1) {Shift=0; ignoreNext++; ms=MilliSekunden32;}
                }
            }
        }
    }
RAUS:
    cl=c;
    // ----- Swipe windows
//  if (SwipeWindows(first, 0)) return; // Lahmgelegt, besser explizite Auswahl...

    if (first) first--;
}

// *****************************************************************************
// FT81X Display: MF40 SETUP                                          12.11.2017
// *****************************************************************************
void FT81X_Display_Exception(int first_req)
{
char s[80];
char c;

static  char    cl;
static  int     first;
    // .........................................................................
    if (first_req) {
        first = 10;             // 10 Durchläufe keine Touch-Events auswerten
    }
    // .........................................................................
    if (CMD_busy()) return;     // warten, bis CMD Fifo abgearbeitet
    // .........................................................................
    CDLinit();
    DL(CMD_DLSTART);
    // .........................................................................
    DL(CLEAR(1, 1, 1));
    DL_CMD_FGCOLOR(0x1F3F7F);
    DL_CMD_BGCOLOR(0x0000FF);
    DL_CMD_GRADIENT(23, 42, 0x2E9AFF, 23, 270, 0x1C1FFF);
    DL(COLOR_RGB(255, 255, 255));
    DL(TAG(0x12));
    DL_CMD_TEXT(400, 20, 30, OPT_CENTERX, "Exception");
    sprintf(s,"RAM_CMD/DL_LEN = %u (Limit=4096)", Exception_DL_LEN_USED);
    DL_CMD_TEXT(400, 20+60, 29, OPT_CENTERX, s);
    DL_CMD_BUTTON(340, 410, 120, 46, 30,  0, "OK");

    DL(DISPLAY());
    // .........................................................................
    DL(CMD_SWAP);
    if (CDLexec()) return;
    // ~~~~~~
    // Tastatur (verwendet nicht den TRACKER, aber den TOUCH_TAG)
    if ((c=rd8(REG_TOUCH_TAG))>0) {
        if (first) cl=c;
        if (cl!=c) {
            switch(c) {
                case 0x12:  hub_state=hub_Default; break;           // Cancel
            }
        }
    }
    cl=c;
    // ----- Swipe windows
    if (SwipeWindows(first, 0)) return;

    if (first) first--;
}

// *****************************************************************************
//
// *****************************************************************************
void drawbmp(void)
{
int i, j;
static int einmalig;
static int drawbmp_cnt;

#ifdef MF40_BMP
    drawbmp_mode=0;     // keine Sonderlocken
    BMP_grafik=0;       // es wird nun TBMP verwendet
#endif
    if (drawbmp_mode>2) drawbmp_mode=0;
    if (drawbmp_mode==0) {
        if ((MilliSekunden-BMP_ms)>4000) {
            bmp_clear(TBMP);
            bmp_clear(GBMP);
            TBMP->new=GBMP->new=1;
            BMP_grafik=0;
        }
        if (BMP_grafik==0) {
            if (!einmalig) {
                TBMP->new = 1;
                einmalig++;
            }
        } else einmalig=0;
        if (TBMP->new || GBMP->new) {
            switch(BMP_grafik&1) {
                case 0: memcpy(BMP->bmp,TBMP->bmp,sizeof(BMP->bmp));
                        break;
                case 1: for (i=0;i<sizeof(BMP->bmp);i++) BMP->bmp[i] = TBMP->bmp[i] | GBMP->bmp[i];
                        break;
            }
            TBMP->new=GBMP->new=0;
            BMP->new=1;
        }
        if (BMP->new) {
            wr8n(RAM_G, BMP->bmp_adr, 3+sizeof(BMP->bmp));
            BMP->new=0;
            BMP_ms = MilliSekunden;
        }
    } else {
        DLinit();
        for(i=0;i<16*256;i++) DL_P[i] = 0;
        for(j=0;j<128;j++) {
            for(i=0;i<16;i++) {
                switch(drawbmp_mode&3) {
                    case 2: DL_P[(j*16)+i] = 1<<((j+(drawbmp_cnt>>1))&7); break;
                    case 1: DL_P[(j*16)+i] = 0b10000000>>((j+(drawbmp_cnt>>1))&7); break;
                }
                if ((j==127)||(j==0)) DL_P[(j*16)+i]=0xFF;
            }
        }
        wr8n(RAM_G, DL_BUF, 3+(16*128));
        drawbmp_cnt+=2;
    }
}

// *****************************************************************************
// Prüft CMD Fifo Status:
// return:  1=busy, Fifo wird noch abgearbeitet
//          0=CMD Fifo leer
// *****************************************************************************
int CMD_busy(void)
{
uint16 dms;
static uint16 first=1;
static uint16 ms=0;
    if (first) {
        first=0;
        ms=MilliSekunden;
    }
    if (rd16(REG_CMD_WRITE)!=rd16(REG_CMD_READ)) {
        if ((dms=(MilliSekunden-ms))>100) EVE_Com_Error++;
        // normale, maximale Busy-Zeit ermitteln: (Stand 26.11.2016: 18ms)
        if (dms>EVE_CMD_busy_msmax) EVE_CMD_busy_msmax=dms;
        return 1;
    } else {
        ms = MilliSekunden;
        return 0;
    }
}

// *****************************************************************************
// FT81X Display: MF40                                                08.12.2016
// -----------------------------------------------------------------------------
// 09.11.2017 : Rotary enabled
// 08.12.2016 : Bedienung P, TIMER und TEMP SET mit SLIDER und +/- nun möglich
// 07.12.2016 : begonnen
// *****************************************************************************
void FT81X_Display_MF40(int first_req)
{
#ifdef FT_81X_ENABLE

uint32 tracker;
int i,j, pm_show, timerset_avl;
uint32 xy;
uint16 x,y;
uint32 col1,col2,col3;
uint16 offx=0, offy=0;
char c, s[150];
uint32 ul;
float ff;
uint32 tm;

static  uint16  scroll;         // 0..65535
static  uint16  track;
static  int16   plusminus;
static  char    cl;
static  uint    downcnt;
static  int     first;
static  int     MF40_setup;
static  uint32  ms32_last;
static  int aha=0;
    // .........................................................................
    if (first_req) {
        first = 10;                         // 10 Durchläufe keine Touch-Events auswerten
        MF40_setup = 0;
        getRotaryUpDown();getRotarySwitch();// zu Beginn alte, noch anstehende Anforderungen abräumen
        plusminus=track=0;
    }
    // .........................................................................
    if (CMD_busy()) return;     // warten, bis CMD Fifo abgearbeitet
    wr8(REG_CTOUCH_EXTENDED,0); // 0-->Extended Mode einschalten
    // .........................................................................
    xy = rd32(REG_CTOUCH_TOUCH0_XY);
    x = xy >> 16;
    y = xy & 0xffff;
    if (x!=32768) {
        downcnt++;
    } else {
        downcnt=0;
    }
    // .........................................................................
    // scroll Auflösung: 0..65535; Achtung, 0-->65535, 65535-->0, da oben null ist
    i=getRotaryUpDown();            // bitte stets "abräumen"
    if (MF40_setup) {
        if (i) {
            switch(MF40_setup) {
                case 1: MF40_Userdata.P_SET=(float)IncDecStepwise((int)((MF40_Userdata.P_SET*10.0)+0.5), 1, i, 0, PSETmax10())/10.0; break;
                case 2: if (MF40_TIMER_TS&&(MF40_Userdata.TIMER_SET<(i>0?MF40_TIMER_TSlim:(MF40_TIMER_TSlim+0.1)))) {
                            MF40_Userdata.TIMER_SET += i*0.1; fMIMA(&MF40_Userdata.TIMER_SET,0,MF40_TIMER_TSlim);
                        } else {
                            MF40_Userdata.TIMER_SET=(float)IncDecStepwise((int)(MF40_Userdata.TIMER_SET+0.5),   0, i, 0, MF40_TIMER_MAX);
                        } break;
                case 3: MF40_Userdata.TEMP_SET=(float)IncDecStepwise((int)(MF40_Userdata.TEMP_SET+0.5),    2, i, 0, TEMPSETmax()); break;
            }
        }
        else if (plusminus) {
            switch(MF40_setup) {
                case 1: MF40_Userdata.P_SET += plusminus * 0.1; fMIMA(&MF40_Userdata.P_SET,0,fPSETmax()); break;
                case 2: MF40_Userdata.TIMER_SET += plusminus * ((MF40_TIMER_TS&&(MF40_Userdata.TIMER_SET<(plusminus>0?MF40_TIMER_TSlim:(MF40_TIMER_TSlim+0.1))))?0.1:1.0); fMIMA(&MF40_Userdata.TIMER_SET,0,MF40_TIMER_MAX); break;
                case 3: MF40_Userdata.TEMP_SET += plusminus; fMIMA(&MF40_Userdata.TEMP_SET,0,TEMPSETmax()); break;
            }
            plusminus=0;
        }
        else if (track) {
            switch(MF40_setup) {
                case 1: ff = (float)(uint16)(~scroll);          // P SET
                        ff = ff * fPSETmax() / 65535;
                        MF40_Userdata.P_SET = ff;
                        break;
                case 2: ff = (float)(uint16)(~scroll);          // TIMER SET
                        ff = ff * MF40_TIMER_MAX / 65535;
                        MF40_Userdata.TIMER_SET = ff;
                        break;
                case 3: ff = (float)(uint16)(~scroll);          // TEMP SET
                        ff = ff * TEMPSETmax() / 65535;
                        MF40_Userdata.TEMP_SET = ff;
                        break;
            }
            track=0;
        }
        switch(MF40_setup) {
            case 1: scroll = ~(uint16)(MF40_Userdata.P_SET * 65535 / fPSETmax()); break;              // P SET
            case 2: if (!MF40_TIMER_TS || (MF40_TIMER_TS && (MF40_Userdata.TIMER_SET>=(MF40_TIMER_TSlim-0.05)))) MF40_Userdata.TIMER_SET = (float)(int)(MF40_Userdata.TIMER_SET+0.5);
                    scroll = ~(uint16)(MF40_Userdata.TIMER_SET * 65535 / MF40_TIMER_MAX); break;            // TIMER SET
            case 3: scroll = ~(uint16)((int)(MF40_Userdata.TEMP_SET+0.5) * 65535 / TEMPSETmax()); break;    // TEMP SET
        }
    }
    // .........................................................................
    CDLinit();
    DL(CMD_DLSTART);
    // .........................................................................
    col1 = 0x0505E8;            // blau
    col2 = 0xFFFF00;            // gelb
    col3 = 0xFFAA00;            // orange
    DL(CLEAR(1, 1, 1));
    DL_CMD_FGCOLOR(0x1F3F7F);   // Standard-Foreground-Color, wird nicht mit CLEAR(1,1,1) auf Default gesetzt
//  DL_CMD_ROMFONT(12, 32);     // ROMFONT 32 auf Handle 12 ist bereits im Splash geladen worden
    DL_CMD_GRADIENT(95, 115, 0xE2E2E2, 95, 363, 0xE2E2E2);

    // Mess-Felder  P-IST, P-SET, I, FREQ, COS, TIMER, TEMP, TEMP-SET
    for (i=timerset_avl=0; i<8; i++) {
        if (!(MF40_Setup.DisplaySelect&(1<<i))) continue;
        if ((!MF40_Userdata.TEMP_used) && (i>=6)) continue;
        if (i==7) timerset_avl=1;           // lokal festhalten, ob TEMP-SET genutzt werden kann
        c = MG_unit[i][0];                  // erster Buchstabe der Einheit
        offx = -430; offy = -126;           // Koordinaten nullen auf beschriebenes Object (links oben=x=430,y=126)
        offx += 150+20; offy += 15;         // auf altes Objekt-Koordinaten anpassen
        offx += (i&1) ? 280-20 : 0;
        offy += (i>>1) * 110;
        DL(TAG(i+1));
        DL(VERTEX_FORMAT(0));
        DL(COLOR_RGB(255, 255, 0));         // gelbe Box links oben
        DL(BEGIN(RECTS));
        DL(VERTEX2F(430+offx, 126+offy));
        DL(VERTEX2F(630+offx, 160+offy));
        DL(END());
        if (c) {
            DL(COLOR_RGB(255, 170, 0));     // orange Box recht oben
            DL(BEGIN(RECTS));
            DL(VERTEX2F(583+offx, 126+offy));
            DL(VERTEX2F(630+offx, 160+offy));
            DL(END());
        }
        DL(COLOR_RGB(5, 5, 232));           // große blaue Box
        pm_show=0;
        if ( (MF40_RUN10 && (i==5)) || (MF40_RUN10 && (MF40_WorkMode==MF40_IntRunRamp) && (i==7))) {
            switch(MF40_WorkMode) {
                case MF40_IntRun:       DL(COLOR_RGB(37, 177,   2)); break; // abgedunkeltes LED-grün
                case MF40_IntRunRamp:   DL(COLOR_RGB(173, 54, 194)); break; // abgedunkeltes Mangenta
                default:                DL(COLOR_RGB(62, 156, 177)); break; // abgedunkeltes stahlblau
            }
        } else {
            switch (MF40_setup) {
                case 1: if (i!=1) break;
MARKIERTEBOX:       DL(COLOR_RGB(0, 4, 0)); // markiert (schwarz)
                    pm_show=1; break;
                case 2: if (i!=5) break; goto MARKIERTEBOX;
                case 3: if (i!=7) break; goto MARKIERTEBOX;
            }
        }
        DL(BEGIN(RECTS));
        DL(VERTEX2F(430+offx, 160+offy));
        DL(VERTEX2F(630+offx, 225+offy));
        DL(END());
        DL(COLOR_RGB(255, 255, 255));
        switch (i) {
            // ------Anordnung-------
            // 0:P         1:P SET
            // 2:I         3:FREQ
            // 4:cos(PHI)  5:TIMER
            // 6:TEMP      7:TEMP SET
            case 0: sprintf(MG_val[i],"%.*f",MG_fix[i],MF40_Ist_P); break;                  // P
            case 1: sprintf(MG_val[i],"%.*f",MG_fix[i],MF40_Userdata.P_SET); break;         // P SET
            case 2: sprintf(MG_val[i],"%.*f",MG_fix[i],MF40_Ist_I); break;                  // I
            case 3: sprintf(MG_val[i],LED_RUN?"%.*f":"-",MG_fix[i],MF40_Ist_Freq); break;   // Freq
            case 4: sprintf(MG_val[i],"%.*f",MG_fix[i],MF40_Ist_cosPhi); break;             // COS PHI
            case 5: if (!MF40_RUN10) {                                                      // TIMER SET
                        if (MF40_Userdata.TIMER_SET) {
                            tm = (uint)((MF40_Userdata.TIMER_SET*10.0)+0.5);
                            if (tm<(MF40_TIMER_TSlim*10)) {
                                sprintf(MG_val[i],"%i.%i",(tm/10)%60,tm%10);
                            } else {
                                sprintf(MG_val[i],"%i:%02i:%02i",tm/36000,(tm/600)%60,(tm/10)%60);
                            }
                        } else {
                            sprintf(MG_val[i],"-");
                        }
                    } else {
                        if (MF40_RUN10<0) tm=(-MF40_RUN10); else tm=MF40_RUN10;
                        if (tm<(MF40_TIMER_TSlim*10)) {
                            sprintf(MG_val[i],"%i.%i",(tm/10)%60,tm%10);
                        } else {
                            sprintf(MG_val[i],"%i:%02i:%02i",tm/36000,(tm/600)%60,(tm/10)%60);
                        }
                    } break;
            case 6: sprintf(MG_val[i],MF40_Temp_Error?"Error":"%.*f",testext?1:MG_fix[i],MF40_Ist_Temp); break;     // Temp
            case 7: if (MF40_WorkMode!=MF40_IntRunRamp) sprintf(MG_val[i],"%.*f",MG_fix[i],MF40_Userdata.TEMP_SET); // TEMP SET
                    else                                sprintf(MG_val[i],"%u",MF40_Regel_Temp);                    // TEMP REG
                    break;
        }
        if (MG_name[i][0]==0) strcpy(MG_val[i],"-");
        DL_CMD_TEXT(530+offx, 192+offy, 12, OPT_CENTER, MG_val[i]);
        DL(COLOR_RGB(0, 0, 0));
        if (c) DL_CMD_TEXT(506+offx, 143+offy, 30, OPT_CENTER, MG_name[i]);
        else   DL_CMD_TEXT(506+offx+25, 143+offy, 30, OPT_CENTER, MG_name[i]);
        if (c=='°') {
            DL_CMD_TEXT(605+offx-5, 143+offy-5, 27, OPT_CENTER, "o ");
            DL_CMD_TEXT(605+offx+7, 143+offy,   29, OPT_CENTER, MG_unit[i]+1);
        } else {
            DL_CMD_TEXT(605+offx,     143+offy, 29, OPT_CENTER, MG_unit[i]);
        }
        if (pm_show) {
//          DL_CMD_FGCOLOR(0xFFFF00);   // gelb wie Namen
            DL_CMD_FGCOLOR(0xFFAA00);   // hellbraun wie Einheiten
            DL(TAG('+'));
            DL_CMD_BUTTON(400-23, 126+offy, 46, 42, 30, OPT_FLAT, "+");
//          DL_CMD_BUTTON(430-40-3+offx, 126+offy, 40, 40, 30, OPT_FLAT, "+");
            DL(TAG('-'));
            DL_CMD_BUTTON(400-23, 126+59+offy-2, 46, 42, 31, OPT_FLAT, "-");
//          DL_CMD_BUTTON(430-40-3+offx, 126+59+offy, 40, 40, 31, OPT_FLAT, "-");
        }
    }
    
    // LEDs
    LEDboxes(0);

    // SLIDER
    if (MF40_setup) {
        DL(COLOR_RGB(179, 179, 179));
        DL_CMD_BGCOLOR(0x0000FF);
        DL_CMD_FGCOLOR(0xFFFF00);
        DL_CMD_TRACK(650, 25+5, 15, 410-5, 21);
        DL(TAG(21));
        DL_CMD_SLIDER(650, 25+5, 15, 410-5, 0, scroll, 65535);
        // Hinweis: Verwendung der SCROLLBAR nicht sinnvoll, da deren Slider 
        // "überschießt" (Value muss stets mit Slider Length korrelieren)!
        //    DL_CMD_TRACK(650, 25, 15, 410, 21);
        //    DL_CMD_SCROLLBAR(650, 25, 15, 410, 0, scroll, 10000, 60000);
    }

    // OK Button, bei aktiver Eingabe
    if (MF40_setup) {
//      DL_CMD_FGCOLOR(0xFFff00);           // gelbe Box (wie Namen)
        DL_CMD_FGCOLOR(0xFFAA00);           // hellbraune Box (wie Einheiten)
        DL(COLOR_RGB(0, 0, 0));             // schwarze Schrift
        DL(TAG(99));
        DL_CMD_BUTTON(400-23, 135, 46, 80, 30, OPT_FLAT, "OK");
    }    
    showAlphoFooter(showTestValues);
    
    if (MF40_Setup.RAMP_Option) {
//      ShowEllipsen(400,8,0,0);          // oben
        if (MF40_Userdata.DisplayRoll) ShowEllipsen(8,240-10,1,1); // links
        ShowEllipsen(800-8,240-10,1,1);   // rechts
    }
//  ShowEllipsen(400,455,0,1);            // unten

    // .........................................................................
    DL_TEST();
    DL(DISPLAY());
    // .........................................................................
    DL(CMD_SWAP);
    if (CDLexec()) return;
    // ~~~~~
    // Tracker für scroll
    tracker = rd32(REG_TRACKER);
    if (!first) switch(tracker&0xff) {
        case 21: scroll = tracker>>16; track++; break;  // 0..65535
    }
    // Tastatur (verwendet nicht den TRACKER, aber den TOUCH_TAG)
    if ((c=rd8(REG_TOUCH_TAG))>0) {
        if (first) cl=c;
        switch(c) {
            case '+':
            case '-':   if (downcnt>12) cl=~c; break;
            case 11+2:  if (downcnt>30) ResetTimer=ResetTimerSET; break;    // RESET
            default:    if (downcnt>3) cl=c; break;
        }
        if (cl!=c) {
            switch(c) {
                case 1: if (showTestValues) showTestValues = 0;
                        else {if ((MilliSekunden32-ms32_last)<500) showTestValues=1; ms32_last=MilliSekunden32;}
                case 3: case 4: case 5: case 7: case 9:
                case 99:MF40_setup=0; break;
                case 2: MF40_setup=1; break;    // P SET
                case 6: MF40_setup=2; break;    // TIMER SET
                case 8: MF40_setup=3; break;    // TEMP SET
                case '+': plusminus++; break;
                case '-': plusminus--; break;
            }
        }
    }
    cl=c;
//  if (STOP_Taster_ms>1000) hub_state=hub_Setup; 
    
    // ----- Swipe windows
    if (SwipeWindows(first, MF40_setup?1:0)) return;

    // Rotary
    if (getRotarySwitch()) {
        MF40_setup++; MF40_setup %= 4;
        if (MF40_RUN10 && (MF40_setup==2)) MF40_setup++;
    }
    // Beschränkung der Eingabefelder
    if (MF40_RUN10) {
        if (MF40_WorkMode==MF40_IntRunRamp) {
            if ((MF40_setup>=2)) MF40_setup=0;  // bei RAMP nur PSET möglich
        } else {
            if ((MF40_setup==2)) MF40_setup=0;  // sonst: kein TIMER
        }
    }
    if (!timerset_avl) if (MF40_setup==3) MF40_setup=0;
    
    if (first) first--;
#else
    hub_state = 0;
#endif
}

// *****************************************************************************
// FT81X Display: MF40 Splash                                         31.03.2019
// Hier erfolgt die globale ROM_FONTS 32..35 Bereitstellung auf Handle 12..14
// -----------------------------------------------------------------------------
// 31.03.2019 HR : Gradienten-Änderung nun in Abhängigkeit von MilliSekunden
// 16.10.2018 HR : Erstellt
// *****************************************************************************
void FT81X_Display_Splash_MF40(int first_req)
{
int i;
char c;
int splashdauer=2048;   // [ms]

static  int     used;
static  char    cl;
static  int     first;
static  uint16  ms;
static  uint32  rgb[6], rgbT;
static  int     init=1;         // nur einmal nach PowerOn
    // .........................................................................
    if (first_req || !used) {
        first = 10;             // 10 Durchläufe keine Touch-Events auswerten
        used = 1;
        ms = MilliSekunden;
        rgb[0]=0x9E; rgb[1]=0xA8; rgb[2]=0xCA;
        rgb[3]=0x07; rgb[4]=0x0A; rgb[5]=0xA8;
        rgbT=255;
    } else {
        i = (255*(int)(MilliSekunden-ms)) / splashdauer;  // 0..2047
        i = 255 - i; if (i<0) i=0;
        rgbT = i;
    }
    // .........................................................................
    if (CMD_busy()) return;     // warten, bis CMD Fifo abgearbeitet
    // .........................................................................
    CDLinit();
    DL(CMD_DLSTART);
    // .........................................................................
    // ROM-Fonts 32,33,34 auf Handle 12,13,14 global bereitstellen (einmalige Operation)
    if (init) {
        DL_CMD_ROMFONT(12, 32);     // wird in MF40 verwendet
        DL_CMD_ROMFONT(13, 33);     // wird hier verwendet
        DL_CMD_ROMFONT(14, 34);
        init=0;
    }
    DL(CLEAR(1, 1, 1));
    DL_CMD_FGCOLOR(0x1F3F7F);
    DL_CMD_BGCOLOR(0x0000FF);
    DL(TAG(0x12));
//  DL_CMD_GRADIENT(23, 42, 0x9EA8CA, 22, 332, 0x070AA8);   // News
//  DL_CMD_GRADIENT(23, 42, 0xE2E2E2, 22, 332, 0xE2E2E2);   // Main
    DL_CMD_GRADIENT(23, 42 , (rgb[0]<<16)+(rgb[1]<<8)+rgb[2], 22, 332, (rgb[3]<<16)+(rgb[4]<<8)+rgb[5]);
    DL(COLOR_RGB(rgbT, rgbT, 255)); if (rgbT<0) rgbT=0;
//  DL(COLOR_RGB(rgbT, rgbT, 255)); rgbT-=2; if (rgbT<0) rgbT=0;
//  DL(COLOR_RGB(rgbT, rgbT, rgbT)); rgbT-=2; if (rgbT<0) rgbT=0;
    DL_CMD_TEXT(400, 200, 13, OPT_CENTERX, "ALPHOTRONIC GmbH"); // 13 --> Font 33
//  DL_CMD_TEXT(400, 210, 31, OPT_CENTERX, "ALPHOTRONIC GmbH");
    for (i=0;i<6;i++) {
        rgb[i]++; if ((i==3)||(i==4)) rgb[i]++; if (rgb[i]>0xE2) rgb[i]=0xE2;
    }
    DL(DISPLAY());
    // .........................................................................
    DL(CMD_SWAP);
    if (CDLexec()) return;
    // ~~~~~~
    // Tastatur (verwendet nicht den TRACKER, aber den TOUCH_TAG)
    if ((c=rd8(REG_TOUCH_TAG))>0) {
        if (first) cl=c;
        if (cl!=c) {
            switch(c) {
                case 0x12:  hub_state=hub_Default; break;
            }
        }
    }
    cl=c;
    if ((MilliSekunden-ms)>splashdauer) {hub_state=hub_Default; return;}
    if (first) first--;
}

// *****************************************************************************
int SwipeWindows(int first, int ausnahme)
// *****************************************************************************
// return: 
// *****************************************************************************
{
int newhubL,newhubR,newhubU,newhubO;
uint32 xy;
uint16 x,y;
int Rand = 30;//25;
static int last_mainhub = hub_Default;
    newhubL=newhubR=newhubU=newhubO = hub_state;
    switch (hub_state) {
        case hub_Default: case hub_MF40:
            last_mainhub = hub_state;
            if (MF40_Setup.RAMP_Option) {if (MF40_Userdata.DisplayRoll) newhubL=hub_RampSetup; newhubR=hub_Ramp; newhubU=hub_News;}
            else             {newhubU=hub_News;}
            break;
        case hub_Ramp:
            last_mainhub = hub_state;
            if (MF40_Setup.RAMP_Option) {newhubL=hub_Default; newhubR=hub_RampSetup; newhubU=hub_News;}
            else             {newhubL=newhubR=hub_Default; newhubU=hub_News;}
            break;
        case hub_RampSetup:
            last_mainhub = hub_state;
            if (MF40_Setup.RAMP_Option) {newhubL=hub_Ramp; if (MF40_Userdata.DisplayRoll) newhubR=hub_Default; newhubU=hub_News;}
            else                        {newhubL=newhubR=hub_Default; newhubU=hub_News;}
            break;
        case hub_UserSetup1:
            newhubL=hub_UserSetup2; newhubR=hub_UserSetup2;
            break;
        case hub_UserSetup2:
            newhubL=hub_UserSetup1; newhubR=hub_UserSetup1;
            break;
        default:
        case hub_News: case hub_Calculator:
            newhubU=newhubO=hub_return;
//          newhubU=newhubO=last_mainhub;
            if (SysOpen) newhubO=hub_Setup;
            break;
    }
    if ((x = (xy = rd32(REG_CTOUCH_TOUCH0_XY)) >> 16) < 800) {
        if (first) return 1;  // first wird dann nicht runtergezählt!
        y = (xy&0xffff);
        if (x<Rand) {                                       // Links
            if ((y>130)&&(y<170)) if (x>10) return 0; 
            {hub_state=newhubL; return 1;}
        }
        if (x>800-Rand) {hub_state=newhubR; return 1;}      // Rechts
        if (y < 800) {
            switch (ausnahme) {
                case 1: if ( ((x>370)&&(x<470))|| ((y>240) && ((x>600)&&(x<720))) ) return 0;
//              case 1: if ( ((x>370)&&(x<470))||((x>600)&&(x<720)) ) return 0;
                        break;
                case 2: if ( (x>400)&&(x<510) ) return 0;
                        break;
            }
            if (y<Rand)       {hub_state=newhubO; return 1;}  // Oben
            if (y>480-Rand)   {hub_state=newhubU; return 1;}  // Unten
        }
    }
    return 0;
}
// *****************************************************************************
//#define FOOTER_18
#ifdef FOOTER_18
 const char Uhrzeit_Alpho[120] = "%02i:%02i:%02i %02i.%02i.20%02i      ALPHOTRONIC GmbH * DE-76297 Stutensee * www.alphotronic.de";
 #define footer_font 18
#else
 const char Uhrzeit_Alpho[120] = "%02i:%02i:%02i  %02i.%02i.20%02i    ALPHOTRONIC GmbH * Am Hasenbiel 7 * 76297 Stutensee * Germany * www.alphotronic.de";
 #define footer_font 26
#endif
// *****************************************************************************
void showAlphoFooter(int showTest)
// *****************************************************************************
{
int i, f, ps;
char s[120], s1[25];
char sd[18];
struct MF40_Userdata_Struct *UD = &MF40_Userdata;

    RTCreadn(sd, 0, 7);                                 // BCD : 0:SS 1:MM 2:24HH 3:-- 4:DD 5:mm 6:YY
    SI_anstehend = CheckSI(BCD2i(sd[6]),BCD2i(sd[5]));  // SI aus der gelesenen Zeit global ermitteln
    if (!showTest) {
        sprintf(s,Uhrzeit_Alpho,BCD2i(sd[2]&0x7f),BCD2i(sd[1]),BCD2i(sd[0]), BCD2i(sd[4]&0x7f),BCD2i(sd[5]),BCD2i(sd[6]));
        DL(COLOR_RGB(0, 0, 0));
        DL_CMD_TEXT(5, 462, footer_font, 0, s);              // am 19.01.2019 von Font 26 auf Font 18 gewechselt (wesentlich genügsamer bezüglich Display-List Auslastung)
        if (SI_anstehend) {
        DL(TAG(255));
            DL_CMD_FGCOLOR(0x00FF00);
            sprintf(s,"SERVICE");
            DL_CMD_BUTTON(735, 480-22, 800, 22, 18, OPT_FLAT, "");
        } else {
            sprintf(s,"V%s HR",SetVersionString);
        }
        DL_CMD_TEXT(794, 462, footer_font, OPT_RIGHTX, s);
    } else {
//      i=sprintf(s,"%02i:%02i:%02i %02i.%02i.20%02i   ",BCD2i(sd[2]&0x7f),BCD2i(sd[1]),BCD2i(sd[0]),  BCD2i(sd[4]&0x7f),BCD2i(sd[5]),BCD2i(sd[6]));
        i=sprintf(s,"%02i:%02i:%02i   ",BCD2i(sd[2]&0x7f),BCD2i(sd[1]),BCD2i(sd[0]));
        sprintf(s1,"REG=%.0f%%",MF40_Regel_Steller);        i+=sprintf(s+i,"%-8s  ",s1);
        sprintf(s1,"f=%.*fkHz",MG_fix[3],MF40_Ist_Freq);          i+=sprintf(s+i,"%-8s  ",s1);
        if (TEMPSETavl()) {
            ps=MF40_Regel_PIDset;
            sprintf(s1,"PIDset=%c", cPIDset(ps));                               i+=sprintf(s+i,"%s  ",s1);
            sprintf(s1,"Kp=%.2f", UD->PID[ps].Kp);                              i+=sprintf(s+i,"%-9s ",s1);
            sprintf(s1,PID_Iactive()?" Ki=%.2f":"(Ki=%.2f)",UD->PID[ps].Ki);    i+=sprintf(s+i,PID_Iactive()?"%-10s ":"%-11s",s1);
            sprintf(s1,"Kd=%.2f", UD->PID[ps].Kd);                              i+=sprintf(s+i,"%-9s ",s1);
            if (GENOFFopt) {sprintf(s1,"UseGenOff=%i", UD->PID[ps].GENOFFuse);  i+=sprintf(s+i,"%s  ",s1);}
//          sprintf(s1,"Soll=%i", iTest);                                       i+=sprintf(s+i,"%-10s  ",s1);
//          sprintf(s1,"Ist=%i", jTest);                                        i+=sprintf(s+i,"%-10s  ",s1);
            if (testext>=1) {sprintf(s1,"esum=%i", PID_esum);                   i+=sprintf(s+i,"%-10s  ",s1);}
        }
        DL(TAG(255));
        DL_CMD_FGCOLOR(0x00FF00);
        DL(COLOR_RGB(0,0,0));
        DL_CMD_BUTTON(0, 480-18, 800, 18, 18, OPT_FLAT, "");
        DL_CMD_TEXT(5, 464, 18, 0, s);
    }
}

// *****************************************************************************
void showSetupFooter(int x, int y)                                 // 20.01.2019
// *****************************************************************************
{
char s[100];
#ifdef DreiTasten
    DL(COLOR_RGB(255, 255, 255));
    DL(TAG(0x11));
    DL_CMD_BUTTON(x-60-140, y, 120, 45, 30, 0,  "Default");
    DL(TAG(0x12));
    DL(COLOR_RGB(255, 0,0));
    DL_CMD_BUTTON(x-60, y, 120, 46, 31,  0,     "X");
    DL(TAG(0x13));
    DL(COLOR_RGB(255, 255, 255));
    DL_CMD_BUTTON(x-60+140, y, 120, 46, 30, 0,  "SAVE");
    DL(TAG(0));
#else
    DL(TAG(0x12));
    DL(COLOR_RGB(255, 0,0));
    DL_CMD_BUTTON(x-10-120, y, 120, 46, 31,  0, "X");
    DL(TAG(0x13));
    DL(COLOR_RGB(255, 255, 255));
    DL_CMD_BUTTON(x+10, y, 120, 46, 30, 0,      "SAVE");
#endif
    DL(TAG(0));
    sprintf(s,"%s V%s.%02i %s * Copyright (c) %s",SetDeviceString,SetVersionString,SetVersionIntExt%100,SetVersionDateString,SetCopyrightString);
//  sprintf(s,"V%s.%02i %s * S/N=%i * Copyright (c) %s",SetVersionString,SetVersionIntExt%100,SetVersionDateString,MF40_Setup.SN,SetCopyrightString);
//  sprintf(s,"%s V%s.%02i %s * S/N=%i * Copyright (c) %s",SetDeviceString,SetVersionString,SetVersionIntExt%100,SetVersionDateString,MF40_Setup.SN,SetCopyrightString);
    DL_CMD_TEXT(x, 466, 26, OPT_CENTER, s);
}

// *****************************************************************************
int getIntendedWorkMode(int override)
// *****************************************************************************
// 10.02.2019 HR - override neu
// 20.11.2017 HR - Erstellt
// *****************************************************************************
{
    switch (override) {
        case 1: hub_state=hub_MF40; return MF40_IntRun;
        case 2: if (MF40_Setup.RAMP_Option) {hub_state=hub_Ramp; return MF40_IntRunRamp;}
                return 0;
    }
    switch(hub_state) {
        case hub_MF40:          
        case hub_Default:   return MF40_IntRun;
        case hub_RampSetup: hub_state=hub_Ramp;    
        case hub_Ramp:      return MF40_IntRunRamp;
        default:            return 0;
    }
}

// *****************************************************************************
// FT801 Display Verteiler                                            19.11.2016
// *****************************************************************************
void FT801_Display_Hub(void)
{
static int running;             // bei PowerOn=0
static int first;
static int hub_last;
static  uint16  ms,ms1,maxms;

    if (!EVE_available) return; // keine EVE vorhanden, nichts zu tun

    if ((MilliSekunden-ms1)>1023) {ms1=MilliSekunden; if (maxms) maxms--;}

    if (!running) {
        first = running = 1;
        hub_state = hub_Splash;
        TBMP->new = 1;
        GBMP->new = 1;
    }
    if (hub_state==hub_return) {
        hub_state=hub_lastMain;
    }
    if (hub_state!=hub_last) {first=1; hub_state_last=hub_last;}
    hub_last = hub_state;
    
    switch(hub_state) {
        case hub_MF40:          
        case hub_Default:       hub_lastMain=hub_state;
                                FT81X_Display_MF40(first); break;
        case hub_News:          FT81X_Display_News_MF40(first); break;
        case hub_Calculator:    FT81X_Display_Calculator(first); break;
        case hub_Ramp:          hub_lastMain=hub_state;
//ms = MilliSekunden;        
                                FT81X_Display_Ramp_MF40(first);
//ms = MilliSekunden-ms; if (ms>=maxms) maxms=ms; TESTTEST=maxms;
//ms = MilliSekunden-ms; if (!first) if (ms>maxms) maxms=ms; TESTTEST=maxms;
                                break;
        case hub_RampSetup:     hub_lastMain=hub_state;
                                FT81X_Display_RampSetup_MF40(first); break;
        case hub_UserSetup1:    FT81X_Display_UserSetup_MF40_1(first); break;    
        case hub_UserSetup2:    FT81X_Display_UserSetup_MF40_2(first); break;    
        case hub_Setup:         FT81X_Display_FactorySetup_MF40(first); break;
        case hub_Keyboard:      FT81X_Display_Keyboard_MF40(first); break;
        
        case hub_SliderDialCalc:FT801_Display_SliderDialCalc(first); break;

        case hub_Exception:     FT81X_Display_Exception(first); break;
        case hub_Splash:        FT81X_Display_Splash_MF40(first);
                                if (MF40_Setup.SplashOnly) FT81X_Display_Splash_MF40(first); 
                                if (MF40_Setup.SplashOnly) FT81X_Display_Splash_MF40(first); 
                                break;
        default:                hub_state=hub_MF40; break;
    }
    first=0;
}

/*******************************************************************************
 End of File
*/

