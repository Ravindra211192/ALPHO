/*******************************************************************************
  MPLAB Harmony Project Main Source File

  Company:
    Rüffler Computer Hard+Soft
    Hans Rüffler
    Microchip Technology Inc. (Harmony Framework)
  
  File Name:
    main.c

  Summary:
    This file contains the "main" function for an MPLAB Harmony project.
    Current Harmony-Version: V2.06.00
    25.04.2022 HR

  Description:
    This file contains the "main" function for an MPLAB Harmony project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state 
    machines of all MPLAB Harmony modules in the system and it calls the 
    "SYS_Tasks" function from within a system-wide "super" loop to maintain 
    their correct operation. These two functions are implemented in 
    configuration-specific files (usually "system_init.c" and "system_tasks.c")
    in a configuration-specific folder under the "src/system_config" folder 
    within this project's top-level folder.  An MPLAB Harmony project may have
    more than one configuration, each contained within it's own folder under
    the "system_config" folder.

 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
 *** PIC-MF40 Firmware ***
 Copyright (c) 2017-2022 Hans Rüffler, 76137 Karlsruhe, Germany
 *** Harmony Framework ***
 Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "system/common/sys_module.h"   // SYS function prototypes
#include "peripheral/oc/plib_oc.h"

#include <stdio.h>
#include "PIC32.h"
#include "PIC_VAR.h"

#include "system_config.h"
#include "system_definitions.h"


// *****************************************************************************
// hier enthaltene Funktionen
// *****************************************************************************
void Timer1_MilliSekunden(uintptr_t context, uint32_t alarmCount);

void COMreceive(const SYS_MODULE_INDEX index);
void COMtransmit(const SYS_MODULE_INDEX index);
void COMerror(const SYS_MODULE_INDEX index);
void COMtx_Kick(char * cp, int len);

void COM2receive(const SYS_MODULE_INDEX index);
void COM2transmit(const SYS_MODULE_INDEX index);
void COM2error(const SYS_MODULE_INDEX index);
void COM2tx_Kick(char * cp, int len);

void InputEval(void);

int  LED_Control(int val);
void LED_Eval(void);

void DoEvents(void);

void SPI0_CS_not_active(void);
void RTC_CS_active(void);
void EEP_CS_active(void);

// *****************************************************************************
// externe Funktionen
// *****************************************************************************
void    FT801_Init(void);
int     FT801_Init_Check(void);
void    FT801_Display_Hub(void);

void    MBSL_Eval(void);
void    MBMA_Eval(void);

// *****************************************************************************
// *****************************************************************************
// ***** Timer1
// *****************************************************************************
// *****************************************************************************
DRV_HANDLE  TMR1handle;
uint ms100,ms25,ms10;       				// für 1/1024s Interrupt
int ieblock;

// *****************************************************************************
// Timer1 Interrupt mit 1024 Hz
// *****************************************************************************
void Timer1_MilliSekunden(uintptr_t context, uint32_t alarmCount)
{
    MilliSekunden++;
    MilliSekunden32++;                                      // Hinweis: läuft vollkommen parallel mit MilliSekunden
    MilliSekunden_Master++;
    MilliSekunden32_Master++;                               // Hinweis: läuft vollkommen parallel mit MilliSekunden_Master
    if (++RTC_MilliSekunden_Master>=1024) {
        RTC_MilliSekunden_Master=0;
        utctime_Master++;
    }
    if (PowerOnTimer!=0xffff) PowerOnTimer++;
    // Zehntel und Hunderstel Sekunden ermitteln, RTC_Millisekunden_999 verwalten
    switch(MilliSekunden&0x3ff) {                           // exakte 1/10s Ermittlung, synchron zu MilliSekunden. (ab 11.11.2017)
        case 102: case 204:
        case 307: case 409: case 511:
        case 614: case 716: case 818:
        case 921: case 1023:    NeueZehntelSekunde++; NeueZehntelSekunde_MF40++; break; 
    }
//  if (++ms10>=102) {ms10=0; NeueZehntelSekunde++;}        // auf 1/1000s Basis, freilaufend (ungenauer als Hauptzeit)
	if (++ms25>=41)  ms25=0; else RTC_MilliSekunden_999++;  // 0..999
	if (RTC_MilliSekunden_999>999) RTC_MilliSekunden_999=999;
	if (++ms100>=10) {ms100=0; NeueHundertstelSekunde++;}   // auf 1/1024s Basis, freilaufend (ungenauer als Hauptzeit)
    // Sekundenzeitpunkt erkennen
	if (RTC_MilliSekunden<1023) {
        RTC_MilliSekunden++;                                // Bereich 0..1023
    } else {
        time++; utctime++; oprtime++;
        RTC_MilliSekunden=RTC_MilliSekunden_999=0;
        NeueSekunde++; NeueSekunde_MF40++;
        // Statistik-Counter verarbeiten
        IntCount_SPI2_s = IntCount_SPI2-tIntCount_SPI2; tIntCount_SPI2 = IntCount_SPI2;
        Count_DISP_s = Count_DISP-tCount_DISP; tCount_DISP = Count_DISP;
        Count_InputEval_s = Count_InputEval-tCount_InputEval; tCount_InputEval = Count_InputEval;
        // -----------------------------
    }
    // -------------------------------------------------------------------
#ifdef S0IN12    
    // ab 06.06.2017 wird InputEval() hier aufgerufen (Zeitbedarf ca. 4us)
    if (ieblock) ieblock++; else InputEval();        
#endif
    // -------------------------------------------------------------------
#ifdef MF40
    MF40_MilliSekunden();
#endif
    // -------------------------------------------------------------------
}

void delay(int wielange);
// --------------------------------------------------------------------------
// Delay in Millisekunden; Bereich 0..65000ms, wielange<0 ==> (uint)wielange;
// --------------------------------------------------------------------------
void delay(int wielange)
//   *****
{
	uint16 startpoint,wl;
		startpoint=MilliSekunden;
		wl = (uint16)wielange; if (wl > 65000) wl=65000;
		while (MilliSekunden - startpoint < wl) reject_exact();
}

// --------------------------------------------------------------------------
// 0..2048s mit 1/2**20s ~ ca. 1us Auflösung
// Verwendet kein di()/ei()
// --------------------------------------------------------------------------
uint32 usVal(void)
{
uint32 tmr,nsv;
uint32 ms1,ms2;

    do {
        ms1 = MilliSekunden32;
        tmr = DRV_TMR0_CounterValueGet();   // 0..39062-1
        ms2 = MilliSekunden32;
    } while ((ms1!=ms2) || (!tmr));

    nsv = ms1<<10;
    tmr *= 1000;                            // 39062 / 38.146 (38,146484)
    nsv += tmr / 38146L;

    return nsv;
}

// --------------------------------------------------------------------------
void mus(int state) // µs Messung mit Min/Max Bildung durchführen
// --------------------------------------------------------------------------
// Ergebnis in mus_max und mus_min
// --------------------------------------------------------------------------
{
uint32 delta;
    switch(state) {
        case 1: // ----- START
            mus_startval = usVal();
            break;
        case 2: // ----- START+INIT
            mus_startval = usVal();
            mus_max=0;
            mus_min=123456789;
            break;
        case 0: // ----- STOP
            if (delta=(usVal()-mus_startval)) {   // 0-Messung übergehen
                if (delta<mus_min) mus_min=delta;
                if (delta>mus_max) mus_max=delta;
            }
            break;
        case -1:// ----- INIT
            mus_startval=mus_max=0;
            mus_min=123456789;
            break;
    }
}

// *****************************************************************************
// *****************************************************************************
// ***** USART1
// *****************************************************************************
// *****************************************************************************
DRV_HANDLE  COMhandle;
int16       COMtxlen;
char *      COMtxcp;
ulong       COMerrcnt;

// -----------------------------------------------------------------------------
// Baudrate Soll:   460800                  230400
// Baudrate Ist:    454545.454 (UxBRG=21)   232558.140 (UxBRG=42)   [BRGH=1]
//                  aktuell gewählt         bis 11.11.2016
// -----------------------------------------------------------------------------
void COMreceive(const SYS_MODULE_INDEX index)
{
#ifdef COM1_MODBUS_SLAVE
 #ifdef MBSL_VIA_COM2            
    DRV_USART_ReadByte(COMhandle);          // nur abräumen
 #else
  #ifdef MBSL_4W
    DRV_USART_ReadByte(COMhandle);          // nur abräumen
  #else
    MBSL_RX(DRV_USART_ReadByte(COMhandle));
  #endif
 #endif
#else
    TBUS_RX(DRV_USART_ReadByte(COMhandle));
#endif
    
}
// -----------------------------------------------------------------------------
void COMtransmit(const SYS_MODULE_INDEX index)
{
    if (COMtxlen) {
        DRV_USART_WriteByte(COMhandle, *(COMtxcp++));
        COMtxlen--;
    }
}
// -----------------------------------------------------------------------------
void COMerror(const SYS_MODULE_INDEX index)
{
    COMerrcnt++;
}
// -----------------------------------------------------------------------------
void COMtx_Kick(char * cp, int len)
{
    if (len>0) {
        COMtxcp = cp;
        COMtxlen = len-1;       // das erste Byte wird hier schon gesendet
        DRV_USART_WriteByte(COMhandle, *(COMtxcp++));
    }
}

// *****************************************************************************
// *****************************************************************************
// ***** USART2
// *****************************************************************************
// *****************************************************************************
DRV_HANDLE  COM2handle;
int16       COM2txlen;
char *      COM2txcp;
ulong       COM2errcnt;

// -----------------------------------------------------------------------------
// Baudrate Soll:   460800                  230400
// Baudrate Ist:    454545.454 (UxBRG=21)   232558.140 (UxBRG=42)   [BRGH=1]
//                  aktuell gewählt         bis 11.11.2016
// -----------------------------------------------------------------------------
void COM2receive(const SYS_MODULE_INDEX index)
{
#ifdef MBSL_VIA_COM2            
    MBSL_RX(DRV_USART_ReadByte(COM2handle));
#else   
  #ifdef MBSL_4W
    MBSL_RX(DRV_USART_ReadByte(COM2handle));
  #else
    MBMA_RX(DRV_USART_ReadByte(COM2handle));
  #endif
#endif
}
// -----------------------------------------------------------------------------
void COM2transmit(const SYS_MODULE_INDEX index)
{
    if (COM2txlen) {
        DRV_USART_WriteByte(COM2handle, *(COM2txcp++));
        COM2txlen--;
    }
}
// -----------------------------------------------------------------------------
void COM2error(const SYS_MODULE_INDEX index)
{
    COM2errcnt++;
}
// -----------------------------------------------------------------------------
void COM2tx_Kick(char * cp, int len)
{
    if (len>0) {
        COM2txcp = cp;
        COM2txlen = len-1;       // das erste Byte wird hier schon gesendet
        DRV_USART_WriteByte(COM2handle, *(COM2txcp++));
    }
}
// -----------------------------------------------------------------------------
int Com_Parity2Linecontrol(int parity)
{
    switch (parity) {
        default:
        case 0: return USART_8N1;
        case 1: return USART_8O1;
        case 2: return USART_8E1;
    }    
}

// *****************************************************************************
// *****************************************************************************
// ***** SPI EEP
// *****************************************************************************
// *****************************************************************************
void EEPoperationStarting(DRV_SPI_BUFFER_EVENT event, DRV_SPI_BUFFER_HANDLE bufferHandle, void * context );
void EEPoperationEnded(DRV_SPI_BUFFER_EVENT event, DRV_SPI_BUFFER_HANDLE bufferHandle, void * context );
void completeCB1(DRV_SPI_BUFFER_EVENT event, DRV_SPI_BUFFER_HANDLE bufferHandle, void * context );
void SPI0_CheckSetting(int user);
void SPI0_CS_not_active(void);
void SPI0_CS_active(void);


DRV_HANDLE  SPI_EEPhandle;
DRV_SPI_BUFFER_HANDLE EEPbufferHandle, EEPbufferHandle2;

char eepbuftx[256+16];
char eepbufrx[256+16];
volatile int EEPbusy;                   // Achtung: muss unbedingt volatile sein! (27.11.2017)
int SPI0_user;

#ifdef MF40
 #define EEP_Baudrate   10000000        // SST26VF032 104 MHZ max
 #define RTC_Baudrate    1000000        // RX-4035    1.25 MHZ max 
#else
 #define EEP_Baudrate   10000000        // 25AA02E48  10 MHZ max
#endif
const DRV_SPI_CLIENT_DATA EEPclientdata =  {EEP_Baudrate, EEPoperationStarting, EEPoperationEnded};
// Hinweis: mit Harmony wird SPI0 mit 10MHZ und DRV_SPI_CLOCK_MODE_IDLE_HIGH_EDGE_FALL (für EEP/FLASH) initialisiert
// (statt DRV_SPI_CLOCK_MODE_IDLE_LOW_EDGE_FALL), für EEP gleichwertig, für RTC wichtig, da EDGE FALL daran erkannt wird.
// Die in SPI0_CheckSetting() auskommentierte, ganz spezielle CKE=1/0 Änderung, ausgehend von DRV_SPI_CLOCK_MODE_IDLE_LOW_EDGE_FALL kann damit entfallen!

#define EEP_user    0       // Default user
#define RTC_user    1

// -----------------------------------------------------------------------------
void SPI0_CheckSetting(int user)// SPI0 Setting entsprechend user checken und ggf. ändern
// -----------------------------------------------------------------------------
// 16.11.2017 : Umstellung auf direkte Baudrate-Einstellung, da die Vorgehensweise mit DRV_SPI_ClientConfigure()
//              einfach instabil ist! Aufgefallen ist dies im News-Fenster mit eingeschaltetem FLASH-Test.
//              Dadurch wird ständig zwischen RTC (Uhrzeitanzeige) und FLASH-Lesen hin- und hergewechselt.
//              Nach einigen Sekunden (meistens um die 60 Sekunden) hängt sich das System auf, der Watchdog schlägt zu.
//              Ein delay() nach DRV_SPI_ClientConfigure() hat zwar einiges gebracht, aber ist einfach nicht die Lösung.
// 13.11.2017 : delay() eingebaut (s.u.!)
// -----------------------------------------------------------------------------
{
#ifdef MF40
    if (user!=SPI0_user) {
//!!SPI0_CS_not_active();        
        switch(user) {
            case EEP_user:  PLIB_SPI_BaudRateSet( SPI_ID_1 , SYS_CLK_PeripheralFrequencyGet(CLK_BUS_PERIPHERAL_2), 10000000 );
//                          DRV_SPI_ClientConfigure(SPI_EEPhandle, &EEPclientdata);
                            break;
            case RTC_user:  PLIB_SPI_BaudRateSet( SPI_ID_1 , SYS_CLK_PeripheralFrequencyGet(CLK_BUS_PERIPHERAL_2), 1000000 );
//                          DRV_SPI_ClientConfigure(SPI_EEPhandle, &RTCclientdata);
                            break;
            default: return;        // nichts zu tun
        }
//      delay(2);         // ist merkwürdigerweise nach DRV_SPI_ClientConfigure() notwendig! Ansonsten kann zu Hängern in DRV_SPI_BufferAddWriteRead2() kommen!
        SPI0_user = user;
    }
#else
    SPI0_user = EEP_user;
#endif
}

// -----------------------------------------------------------------------------
void SPI0_CS_not_active(void)   // De-Aktivieren aller SPI0-CS
// -----------------------------------------------------------------------------
{
    SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_9);  // EEP-CS inaktiv (RD.9=1)
#ifdef MF40
    SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_3);// RTC-CS inaktiv (RB.3=0)
#endif
}
// -----------------------------------------------------------------------------
void SPI0_CS_active(void)       // Aktivieren des entsprechenden CS
// -----------------------------------------------------------------------------
{
    switch(SPI0_user) {
        case EEP_user:
            SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_9);// EEP-CS AKTIV (RD.9=0)
            break;
#ifdef MF40
        case RTC_user:
            SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_3);  // RTC-CS AKTIV (RB.3=1)
            break;
#endif
    }
}

// -----------------------------------------------------------------------------
void EEPoperationStarting(DRV_SPI_BUFFER_EVENT event, DRV_SPI_BUFFER_HANDLE bufferHandle, void * context )
// -----------------------------------------------------------------------------
{
    SPI0_CS_active();
}
// -----------------------------------------------------------------------------
void EEPoperationEnded(DRV_SPI_BUFFER_EVENT event, DRV_SPI_BUFFER_HANDLE bufferHandle, void * context )
// -----------------------------------------------------------------------------
{
    SPI0_CS_not_active();
}
// -----------------------------------------------------------------------------
void completeCB1(DRV_SPI_BUFFER_EVENT event, DRV_SPI_BUFFER_HANDLE bufferHandle, void * context )
// -----------------------------------------------------------------------------
{
//  SPI0_CS_not_active();
    EEPbusy = 0;
}

// -----------------------------------------------------------------------------
// Lese len Bytes aus dem EEPROM ab Adresse adr
// return: len, wenn ok. Ansonsten 0.
// -----------------------------------------------------------------------------
int EEPreadn(char * dp, uint16 adr, int len)
{
int i, eep_n;
char * rp;
uint16 ms;
//  for (i=0;i<sizeof(eepbufrx);i++) eepbufrx[i]=0xEE;
    if (len>256) return 0;
    SPI0_CheckSetting(EEP_user);
    i = 0; 
//  eepbuftx[i++]=0b00000101;               // READ STATUS CMD
    eepbuftx[i++]=0b00000011;               // READ BYTE CMD
    eepbuftx[i++]=adr;                      // ADDRESS
    eep_n = i+len;
    rp = eepbufrx + i;
    
    EEPbusy=1;
    EEPbufferHandle = DRV_SPI_BufferAddWriteRead2( SPI_EEPhandle, eepbuftx, eep_n, eepbufrx, eep_n, completeCB1, NULL, &EEPbufferHandle2 );
//  EVEbufferHandle = DRV_SPI_BufferAddWriteRead2( SPI_EVEhandle, eepbuftx, eep_n, eepbufrx, eep_n, completeCB2, NULL, &EVEbufferHandle2 );
    if(EEPbufferHandle2 != DRV_SPI_BUFFER_HANDLE_INVALID ) { /*...*/ }
    ms=MilliSekunden;
    while (EEPbusy) {                       // Warten bis Interrupt EEPbusy=0 setzt, danach gilt: alle relevanten SPI0-CS inaktiv
        if ((MilliSekunden-ms)>50) {Count_EEPto++; break;}
    }
    for (i=0;i<len;i++) *(dp++) = *(rp++);
    return len;
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void EEPwriteb(uint16 adr, uint16 val)
{
int i, eep_n;
uint16 ms;
    SPI0_CheckSetting(EEP_user);
    i = 0; 
    eepbuftx[i++]=0b00000110;               // WREN
    eep_n = i;
    EEPbusy=1;
    EEPbufferHandle = DRV_SPI_BufferAddWriteRead2( SPI_EEPhandle, eepbuftx, eep_n, eepbufrx, eep_n, completeCB1, NULL, &EEPbufferHandle2 );
    if(EEPbufferHandle2 != DRV_SPI_BUFFER_HANDLE_INVALID ) { /*...*/ }
    while (EEPbusy) ;                       // Warten bis Interrupt EEPbusy=0 setzt, danach gilt: alle relevanten SPI0-CS inaktiv

    i = 0; 
    eepbuftx[i++]=0b00000010;               // WRITE BYTE CMD
    eepbuftx[i++]=adr;                      // ADDRESS
    eepbuftx[i++]=val;                      // VALUE
    eep_n = i;
    EEPbusy=1;
    EEPbufferHandle = DRV_SPI_BufferAddWriteRead2( SPI_EEPhandle, eepbuftx, eep_n, eepbufrx, eep_n, completeCB1, NULL, &EEPbufferHandle2 );
    if(EEPbufferHandle2 != DRV_SPI_BUFFER_HANDLE_INVALID ) { /*...*/ }
    ms=MilliSekunden;
    while (EEPbusy) {                       // Warten bis Interrupt EEPbusy=0 setzt, danach gilt: alle relevanten SPI0-CS inaktiv
        if ((MilliSekunden-ms)>50) {Count_EEPto++; break;}
    }
}

// -----------------------------------------------------------------------------
// Lese len Bytes aus dem FLASH ab Adresse adr (24-Bit). len: 256 Bytes max.
// return: len, wenn ok. Ansonsten 0.
// -----------------------------------------------------------------------------
int FLASHreadn_page(char * dp, uint32 adr, int len)
{
int i, eep_n;
char * rp;
uint16 ms;
//  for (i=0;i<sizeof(eepbufrx);i++) eepbufrx[i]=0xEE;
    if (len>256) return 0;                  // wegen eepbufrx
    SPI0_CheckSetting(EEP_user);
    i = 0; 
    eepbuftx[i++]=0b00000011;               // READ CMD
    eepbuftx[i++]=adr>>16;                  // ADDRESS B23..B16
    eepbuftx[i++]=(adr>>8)&0xff;            // ADDRESS B15..B8
    eepbuftx[i++]=adr&0xff;                 // ADDRESS B7..B0
    eep_n = i+len;
    rp = eepbufrx + i;
    
    EEPbusy=1;
    EEPbufferHandle = DRV_SPI_BufferAddWriteRead2( SPI_EEPhandle, eepbuftx, eep_n, eepbufrx, eep_n, completeCB1, NULL, &EEPbufferHandle2 );
//  EVEbufferHandle = DRV_SPI_BufferAddWriteRead2( SPI_EVEhandle, eepbuftx, eep_n, eepbufrx, eep_n, completeCB2, NULL, &EVEbufferHandle2 );
    if(EEPbufferHandle2 != DRV_SPI_BUFFER_HANDLE_INVALID ) { /*...*/ }

    ms=MilliSekunden;
    while (EEPbusy) {                       // Warten bis Interrupt EEPbusy=0 setzt, danach gilt: alle relevanten SPI0-CS inaktiv
        if ((MilliSekunden-ms)>50) {Count_EEPto++; break;}
    }
    for (i=0;i<len;i++) *(dp++) = *(rp++);
    return len;
}
// *****************************************************************************
int FLASHreadn(char * sp, uint32 adr, int len)  // len: begrenzt nur von sp Größe
// *****************************************************************************
{
int i, r=0;
    while (i=len) {
        if (i>256) i=256;
        r+=FLASHreadn_page(sp, adr, i);
        if (!r) return r;
        adr+=i; len-=i; sp+=i;
    }
    return r;
}
// -----------------------------------------------------------------------------
// Entsperren aller Write-Protect Bits bis zum nächsten Reset 
// -----------------------------------------------------------------------------
void FLASHwriteULBPR(void)
{
int i,j, eep_n;
    SPI0_CheckSetting(EEP_user);
    i = 0; 
    eepbuftx[i++]=0b00000110;               // WREN
    eep_n = i;
    EEPbusy=1;
    EEPbufferHandle = DRV_SPI_BufferAddWriteRead2( SPI_EEPhandle, eepbuftx, eep_n, eepbufrx, eep_n, completeCB1, NULL, &EEPbufferHandle2 );
    if(EEPbufferHandle2 != DRV_SPI_BUFFER_HANDLE_INVALID ) { /*...*/ }
    while (EEPbusy) ;                       // Warten bis Interrupt EEPbusy=0 setzt, danach gilt: alle relevanten SPI0-CS inaktiv

    i = 0; 
    eepbuftx[i++]=0x98;                    // ULBPR
    eep_n = i;
    EEPbusy=1;
    EEPbufferHandle = DRV_SPI_BufferAddWriteRead2( SPI_EEPhandle, eepbuftx, eep_n, eepbufrx, eep_n, completeCB1, NULL, &EEPbufferHandle2 );
    if(EEPbufferHandle2 != DRV_SPI_BUFFER_HANDLE_INVALID ) { /*...*/ }
    while (EEPbusy) ;                       // Warten bis Interrupt EEPbusy=0 setzt, danach gilt: alle relevanten SPI0-CS inaktiv
}
// -----------------------------------------------------------------------------
// len : -1 Sector-Erase (4KB), -2 Block-Erase    // len: max. 256 Bytes, rotiert im 8-Bit Raum
// -----------------------------------------------------------------------------
void FLASHwriten_page(char *sp, uint32 adr, int len)
{
int i,j, eep_n;
int fdelay=0;
uint16 ms;
static int unprotected=0;
    if (len>256) return;                    // Page==256 Bytes, bei Überlappen --> Rotieren im 8-Bit Raum
    if (!unprotected) {
        FLASHwriteULBPR();
        unprotected=1;
    }
    SPI0_CheckSetting(EEP_user);
    i = 0; 
    eepbuftx[i++]=0b00000110;               // WREN
    eep_n = i;
    EEPbusy=1;
    EEPbufferHandle = DRV_SPI_BufferAddWriteRead2( SPI_EEPhandle, eepbuftx, eep_n, eepbufrx, eep_n, completeCB1, NULL, &EEPbufferHandle2 );
    if(EEPbufferHandle2 != DRV_SPI_BUFFER_HANDLE_INVALID ) { /*...*/ }
    while (EEPbusy) ;                       // Warten bis Interrupt EEPbusy=0 setzt, danach gilt: alle relevanten SPI0-CS inaktiv

    i = 0;
    switch(len) {
        default:if (len<1) return;          // nichts zu tun
                eepbuftx[i++]=0b00000010;   // PAGE WRITE CMD
                fdelay=(len>150)?3:2; break;
        case Sector_Erase:
                if (sp[0]) return;          // sp muss auf Leerstring zeigen         
                eepbuftx[i++]=0x20;         // SECTOR ERASE CMD
                fdelay=26; len=0; break;
        case Block_Erase:
                if (sp[0]) return;          // sp muss auf Leerstring zeigen         
                eepbuftx[i++]=0xD8;         // BLOCK ERASE CMD
                fdelay=26; len=0; break;
    }
    eepbuftx[i++]=adr>>16;                  // ADDRESS B23..B16
    eepbuftx[i++]=(adr>>8)&0xff;            // ADDRESS B15..B8
    eepbuftx[i++]=adr&0xff;                 // ADDRESS B7..B0
    for(j=0;j<len;j++) eepbuftx[i++]=*(sp++);// Daten anhängen
    eep_n = i;
    EEPbusy=1;
    EEPbufferHandle = DRV_SPI_BufferAddWriteRead2( SPI_EEPhandle, eepbuftx, eep_n, eepbufrx, eep_n, completeCB1, NULL, &EEPbufferHandle2 );
    if(EEPbufferHandle2 != DRV_SPI_BUFFER_HANDLE_INVALID ) { /*...*/ }
    ms=MilliSekunden;
    while (EEPbusy) {                       // Warten bis Interrupt EEPbusy=0 setzt, danach gilt: alle relevanten SPI0-CS inaktiv
        if ((MilliSekunden-ms)>50) {Count_EEPto++; break;}
    }
    if (fdelay) delay(fdelay);
}
// *****************************************************************************
void FLASHwriten(char * sp, uint32 adr, int len)    // len: nur von FLASH Größe beschränkt
// *****************************************************************************
{
int i;
    FLASHwriten_page("", adr&0xfffff000, Sector_Erase);  // 4096 Byte Sector Adresse
    while (i=len) {
        if (adr&0xff) {i=256-(adr&0xff);}
        else          {if (i>256) i=256;}
        FLASHwriten_page(sp, adr, i);
        adr+=i; len-=i; sp+=i;
    }
}


// -----------------------------------------------------------------------------
// Lese len Bytes aus dem RTC RX-4035LC ab Adresse adr
// return: len, wenn ok. Ansonsten 0.
// -----------------------------------------------------------------------------
int RTCreadn(char * dp, uint16 adr, int len)
{
int i, eep_n;
char * rp;
uint16 ms;
    if (len>16) return 0;
    SPI0_CheckSetting(RTC_user);
    i = 0; 
    eepbuftx[i++]=((adr&0b1111)<<4)+0b0100; // B7..B4:Register address, B3..B0=4-->Burst Read
    eep_n = i+len;
    rp = eepbufrx + i;
    
    EEPbusy=1;
    EEPbufferHandle = DRV_SPI_BufferAddWriteRead2( SPI_EEPhandle, eepbuftx, eep_n, eepbufrx, eep_n, completeCB1, NULL, &EEPbufferHandle2 );
    if(EEPbufferHandle2 != DRV_SPI_BUFFER_HANDLE_INVALID ) { /*...*/ }

    ms=MilliSekunden;
    while (EEPbusy) {                       // Warten bis Interrupt EEPbusy=0 setzt, danach gilt: alle relevanten SPI0-CS inaktiv
        if ((MilliSekunden-ms)>50) {Count_EEPto++; break;}
    }
    for (i=0;i<len;i++) *(dp++) = *(rp++);
    return len;
}

// -----------------------------------------------------------------------------
// Schreibe ein Byte in das RTC RX-4035LC Register adr (0..15)
// -----------------------------------------------------------------------------
void RTCwriteb(uint16 adr, uint16 val)
{
int i, eep_n;
uint16 ms;
    SPI0_CheckSetting(RTC_user);
    i = 0; 
    eepbuftx[i++]=((adr&0b1111)<<4)+0b0000; // B7..B4:Register address, B3..B0=0-->Burst Write
    eepbuftx[i++]=val;                      // VALUE
    eep_n = i;
    EEPbusy=1;
    EEPbufferHandle = DRV_SPI_BufferAddWriteRead2( SPI_EEPhandle, eepbuftx, eep_n, eepbufrx, eep_n, completeCB1, NULL, &EEPbufferHandle2 );
    if(EEPbufferHandle2 != DRV_SPI_BUFFER_HANDLE_INVALID ) { /*...*/ }
    ms=MilliSekunden;
    while (EEPbusy) {                       // Warten bis Interrupt EEPbusy=0 setzt, danach gilt: alle relevanten SPI0-CS inaktiv
        if ((MilliSekunden-ms)>50) {Count_EEPto++; break;}
    }
}

// -----------------------------------------------------------------------------
// Schreibe len Bytes in die RTC RX-4035LC Register ab adr (0..15)
// -----------------------------------------------------------------------------
void RTCwriten(char *sp, uint16 adr, int len)
{
int i,j, eep_n;
uint16 ms;
    if (len>16) return;
    SPI0_CheckSetting(RTC_user);
    i = 0; 
    eepbuftx[i++]=((adr&0b1111)<<4)+0b0000;  // B7..B4:Register address, B3..B0=0-->Burst Write
    for(j=0;j<len;j++) eepbuftx[i++]=*(sp++);// Daten anhängen
    eep_n = i;
    EEPbusy=1;
    EEPbufferHandle = DRV_SPI_BufferAddWriteRead2( SPI_EEPhandle, eepbuftx, eep_n, eepbufrx, eep_n, completeCB1, NULL, &EEPbufferHandle2 );
    if(EEPbufferHandle2 != DRV_SPI_BUFFER_HANDLE_INVALID ) { /*...*/ }
    ms=MilliSekunden;
    while (EEPbusy) {                       // Warten bis Interrupt EEPbusy=0 setzt, danach gilt: alle relevanten SPI0-CS inaktiv
        if ((MilliSekunden-ms)>50) {Count_EEPto++; break;}
    }
}

// *****************************************************************************
// *****************************************************************************
// ***** SPI EEP
// *****************************************************************************
// *****************************************************************************
DRV_HANDLE  SPI_EVEhandle;

DRV_SPI_BUFFER_HANDLE EVEbufferHandle, EVEbufferHandle2;
char EVEbuftx[1024+16];
char EVEbufrx[1024+16];
volatile int EVEbusy;                       // sicherheitshalber auch volatile (27.11.2017), funktioniert aber auch ohne...

void EVE_CS(int state);
// -----------------------------------------------------------------------------
// EVE_CS# Steuerung : state --> direkter Pin-Wert (0:selected, 1:not selected)
// -----------------------------------------------------------------------------
void EVE_CS(int state)
{
    if (state) {
        SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_8);
    } else {
        SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_8);
    }
}

void EVEoperationStarting(DRV_SPI_BUFFER_EVENT event, DRV_SPI_BUFFER_HANDLE bufferHandle, void * context );
// -----------------------------------------------------------------------------
void EVEoperationStarting(DRV_SPI_BUFFER_EVENT event, DRV_SPI_BUFFER_HANDLE bufferHandle, void * context )
{
    EVE_CS(0);
}
void EVEoperationEnded(DRV_SPI_BUFFER_EVENT event, DRV_SPI_BUFFER_HANDLE bufferHandle, void * context );
// -----------------------------------------------------------------------------
void EVEoperationEnded(DRV_SPI_BUFFER_EVENT event, DRV_SPI_BUFFER_HANDLE bufferHandle, void * context )
{
    EVE_CS(1);
}
void completeCB2(DRV_SPI_BUFFER_EVENT event, DRV_SPI_BUFFER_HANDLE bufferHandle, void * context );
// -----------------------------------------------------------------------------
void completeCB2(DRV_SPI_BUFFER_EVENT event, DRV_SPI_BUFFER_HANDLE bufferHandle, void * context )
{
//  EVE_CS(1);
    EVEbusy = 0;
}


void EVEreadn(uint32 adr, uchar * dp, int len);
// -----------------------------------------------------------------------------
// Lese len Bytes ab Adresse adr 
// -----------------------------------------------------------------------------
void EVEreadn(uint32 adr, uchar * dp, int len)
{
int i;
 // EVE_CS(0);                              // CS aktiv (=0)
    EVEbuftx[0] = 0b00000000 + ((adr>>16)&0x3f);
    EVEbuftx[1] = ((adr>>8)&0xff);
    EVEbuftx[2] = (adr&0xff);
    EVEbuftx[3] = 0;                        // Dummy-Byte (Zuweisung =0 eigentlich nicht notwendig) 
    EVEbusy=1;
    EVEbufferHandle = DRV_SPI_BufferAddWriteRead2( SPI_EVEhandle, EVEbuftx, 4, EVEbufrx, len+4, completeCB2, NULL, &EVEbufferHandle2 );
    if(EVEbufferHandle2 != DRV_SPI_BUFFER_HANDLE_INVALID ) { /*...*/ }
    while (EVEbusy) {                       // Warten bis Interrupt EVEbusy=0 setzt, danach gilt: EVE_CS(1)
        DoEvents();                         // Achtung, wird nie aufgerufen! SPI in Harmony nicht effizient implementiert
    }
    for(i=4;i<len+4;i++) {
        *(dp++) = EVEbufrx[i];
    }
}

void EVEwriten(uchar * sp, int len);
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void EVEwriten(uchar * sp, int len)
{
uint32 us,usr;  //!!

// EVE_CS(0);                              // CS aktiv (=0)
    EVEbusy=1;
us = usVal();   //!!
    EVEbufferHandle = DRV_SPI_BufferAddWriteRead2( SPI_EVEhandle, sp, len, EVEbufrx, 0, completeCB2, NULL, &EVEbufferHandle2 );
//if ((usr=(usVal()-us))>Count_DoEvents) Count_DoEvents=usr;    
    if(EVEbufferHandle2 != DRV_SPI_BUFFER_HANDLE_INVALID ) { /*...*/ }
    while (EVEbusy) {                       // Warten bis Interrupt EVEbusy=0 setzt, danach gilt: EVE_CS(1)
        DoEvents();                         // Achtung, wird nie aufgerufen! SPI in Harmony nicht effizient implementiert
    }
}

int EVE_busy(void);
// -----------------------------------------------------Erstellt:16.10.2018-HR--
// Achtung sinnlos, da der Zustand von EVEbusy bereits während
// DRV_SPI_BufferAddWriteRead2() wieder auf 0 gesetzt wird! Harmony :(
// -----------------------------------------------------------------------------
int EVE_busy(void)
{
    return EVEbusy;
}

// *****************************************************************************
// Steuerung des EVE-PD# Ausganges entsprechend val
// *****************************************************************************
int EVE_PD_Control(int val)
{
    if (val>0) {
        SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_2);
    } else {
        SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_2);
    }
}


// *****************************************************************************
// *****************************************************************************
// ***** IMPULS-ERFASSUNG S0IN12
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// Impulszählung, wird bei Erkennung der entsprechenden Flanke aufgerufen
// i : Kanal 0..11
// *****************************************************************************
void PulseCount(int i)
{
    di();
        CI[i].pulsecount++;
        CI[i].period_last = CI[i].period;
        CI[i].period = 0;
    ei();
}

uint16_t ie_ms_last;
// *****************************************************************************
// Auswertung der Eingänge
// Aufruf erfolgt permanent von main(), die Auswertung erfolgt jedoch
// nur jede Millisekunde (1/1024s))
// Die Erkennung und teilweise die Nomenklatur wurde vom C166 der U1601 über-
// nommen, dort war der Assembler-Code mit C Analogien versehen.
// Hinweis: ab 06.06.2017 wird InputEval() direkt aus Timer1_MilliSekunden()
// aufgerufen. Zeitbedarf: ca. 4us
//
// 19.10.2016 HR : erstellt
// *****************************************************************************
void InputEval(void)
{
    int i;
    uint16 ms, delta_ms;
    
    // Prüfung, ob in aktueller Millisekunde bereits eine Auswertung erfolgte
    if ((ms=MilliSekunden) == ie_ms_last) return;    // bereits durchgeführt --> nichts tun
//mus(1);
    Count_InputEval++; //!!
    delta_ms = ms - ie_ms_last;
    ie_ms_last = ms;
    for (i=0;i<maxS0;i++) {
        if (CI[i].period<maxPeriod) CI[i].period++;
        if (CI[i].ain) {
            if (!(inputs & (1<<i))) {
                CI[i].act++;
                if (CI[i].act >= CI[i].apuls) {
                    CI[i].ain=0;
                    CI[i].act=0;
                    if (!CI[i].aflanke) PulseCount(i);	// --__
                }
            } else {
                CI[i].act=0;
            }
        } else {
            if (inputs & (1<<i)) {
                CI[i].act++;
                if (CI[i].act >= CI[i].apuls) {
                    CI[i].ain=1;
                    CI[i].act=0;
                    if (CI[i].aflanke) PulseCount(i);	// __--
                }
            } else {
                CI[i].act=0;
            }
        }        
    }
//mus(0); // Ergebnis: mus_min=3 us, mus_max=4 us (ohne Interrupt-Anteil, kurz nach mus(-1) gemessen)
}
// *****************************************************************************
//
// *****************************************************************************
void InputEvalBlock(int state)
{
    if (state) {
        // Blockieren von Änderungen durch InputEval()
        ieblock=1;
    } else {
        // Aufhebung der Blockierung von Änderungen durch InputEval()
        di();
        if (ieblock>1) {    
            // Messung am 11.06.2017: ieblock>1 tritt im Schnitt ca. alle 3s auf, teilweise aber auch 2 mal pro Sekunde
            if (ieblock>=3) Count_DoEvents++;   // tritt nicht auf
            InputEval();
        }
        ieblock=0;
        ei();
    }
}

// *****************************************************************************
// Steuerung der LED :  val>0 --> LED=1=ON, val==0 --> LED=0=OFF
//                      val<0 --> return LED
// *****************************************************************************
int LED_Control(int val)
{
    if (val<0) return LED;
    if (val) {
#ifndef MF40
        SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_3);
#endif
        LED = 1;
    } else {
#ifndef MF40
        SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_3);
#endif
        LED = 0;
    }
}

// *****************************************************************************
// Auswertung und Ansteuerung der LED
// *****************************************************************************
void LED_Eval(void)
{
int led;
uint16 ms500;
    if (LEDovrTO) {
        led = (LEDovr & ((uint32)1<<((MilliSekunden_Master&0x7ff)>>6))) ? 1:0;
    } else {
        if (MasterConnectedTO) {
            led = 1;
        } else {
            if (MilliSekunden_Master&B8) { // 0,5 Hz
    //      if (MilliSekunden_Master&B9) { // 1 Hz
                led = 1;
            } else {
                led = 0;
            }
        }        
    }
    LED_Control(led);
}

// *****************************************************************************
// Damit Wartezeit effektiv genutzt werden kann und auch muss, wird bei längerem
// Warten DoEvents() aufgerufen (man erinnert sich an alte VisualBasic Zeiten).
// Hinweis: da das SPI Handling so "umständlich" im Harmony Framework 
// implementiert (Interrupt-driven), kann DoEvents() gar nicht verwendet werden.
// DMA und EnhancedBuffer funktionieren nicht, Polling noch sinnloser.
// InputEval() wird daher ab dem 06.06.2017 stets von Timer1_MilliSekunden()
// aufgerufen.
// *****************************************************************************
void DoEvents(void)
{
//    if (PowerOnTimer>4000) while(1) {
//        watchdog();
//        OUT_RESET_Control((MilliSekunden&B8)?1:0);
//    }
    if (useDoEvents>0) {    // nur optional wegen Testphase
        Count_DoEvents++;
    }
}

//const DRV_SPI_CLIENT_DATA EVEclientdata_slow = {0, EVEoperationStarting, EVEoperationEnded};

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
//
// Hinweise:
// - Compiler-Option XC32 (Global Options) -- xc32-gcc, Additional options:
//   -- KEINE --
//   Hinweis: -fno-short-double nicht verwenden, da dadurch alle Floats 
//            long double sind (ab XC32 V1.44). Vor V1.44 war die Verwendung
//            von long double eigentlich nicht möglich.
// - Zeichen-Codierung: Default war: ISO 8859-1, umgestellt auf Windows 1252
//    Beide Zeichensätze sind West-Europäisch, unterscheiden sich angeblich in
//    der Nutzung von Steuerzeichen/Druckbaren Zeichen im 
//    Bereich 0..0x1f und 0x80..0x9f
// *****************************************************************************
// *****************************************************************************
int main ( void )
{
char c;
int i, j;
uint16 pv;
DRV_SPI_CLIENT_DATA EVEclientdata;
uint32 ui;
int ns_eve=0;
int com1_tbaud;
int com1_tparity;
//int ITs;

    /* Initialize all MPLAB Harmony modules, including application(s). */
    SYS_Initialize ( NULL );

    for(i=0;i<maxS0;i++) {
        CI[i].apuls=12;
        CI[i].aflanke=1;
    }
    
    // ----- UART COM-1 -----
    COMhandle = DRV_USART_Open(DRV_USART_INDEX_0, DRV_IO_INTENT_EXCLUSIVE);
    if (DRV_HANDLE_INVALID == COMhandle) { }    // Unable to open the driver. May be the driver is not initialized or the initialization is not complete.
#ifdef COM1_MODBUS_SLAVE    
    com1_tbaud = Com1_Baudrate = COM1_Baudrate_Default;
    DRV_USART_BaudSet(COMhandle, Com1_Baudrate);// Harmony Setting: 460800
    com1_tparity = Com1_Parity = 0;             // 0-->8N1
    DRV_USART_LineControlSet(COMhandle, Com_Parity2Linecontrol(Com1_Parity));
#endif
    DRV_USART_ByteReceiveCallbackSet(DRV_USART_INDEX_0, COMreceive);
    DRV_USART_ByteTransmitCallbackSet(DRV_USART_INDEX_0, COMtransmit);
    DRV_USART_ByteErrorCallbackSet(DRV_USART_INDEX_0, COMerror);
    
    // ----- UART COM-2 -----
#ifdef COM2_MODBUS_MASTER    
    COM2handle = DRV_USART_Open(DRV_USART_INDEX_1, DRV_IO_INTENT_EXCLUSIVE);
    if (DRV_HANDLE_INVALID == COM2handle) { }    // Unable to open the driver. May be the driver is not initialized or the initialization is not complete.
    DRV_USART_BaudSet(COM2handle, COM2_Baudrate_Default);       // Harmony Setting: 115200
    DRV_USART_ByteReceiveCallbackSet(DRV_USART_INDEX_1, COM2receive);
    DRV_USART_ByteTransmitCallbackSet(DRV_USART_INDEX_1, COM2transmit);
    DRV_USART_ByteErrorCallbackSet(DRV_USART_INDEX_1, COM2error);
#endif
    // ----- TIMER 1 : Free running with 1024 Hz @ 40MHZ PBCLK, Prescaler=1, Period = 39062 
    TMR1handle = DRV_TMR_Open(DRV_TMR_INDEX_0, DRV_IO_INTENT_EXCLUSIVE);
    if (DRV_HANDLE_INVALID == TMR1handle) { }    // Unable to open the driver. May be the driver is not initialized or the initialization is not complete.
    DRV_TMR_AlarmRegister(TMR1handle, 39062, true, 0, Timer1_MilliSekunden);    // 40000 --> 1000Hz, 39062 --> 1024Hz
    DRV_TMR_AlarmEnable(TMR1handle, true);
    DRV_TMR_Start(TMR1handle);

    // ----- SPI 0
    SPI0_CS_not_active();               // alle CS von SPI0 deaktivieren
    SPI_EEPhandle = DRV_SPI_Open(DRV_SPI_INDEX_0, DRV_IO_INTENT_READWRITE);
    DRV_SPI_ClientConfigure(SPI_EEPhandle, &EEPclientdata);
    SPI0_user=EEP_user;
    // Read MAC48
    EEPreadn(MAC48,0xFA,6);
        
    // ----- SPI 1
    EVE_CS(1);
    SPI_EVEhandle = DRV_SPI_Open(DRV_SPI_INDEX_1, DRV_IO_INTENT_READWRITE);
    EVEclientdata.baudRate = 10000000;  // EVE-Init mit 10 MHZ SPI Clock
//  EVEclientdata.baudRate = 0;         // unchanged
    EVEclientdata.operationStarting = EVEoperationStarting;
    EVEclientdata.operationEnded = EVEoperationEnded;
    DRV_SPI_ClientConfigure(SPI_EVEhandle, &EVEclientdata);
#ifdef USE_EVE                          // enthält Firmware überhaupt Display-Routinen?
    FT801_Init();                       // hier wird EVE_available ermittelt
//  EVEclientdata.baudRate = 14000000;  // nach Init kann EVE mit höherer Baudrate verwendet werden
    EVEclientdata.baudRate = 0;         // unchanged
    DRV_SPI_ClientConfigure(SPI_EVEhandle, &EVEclientdata);
#endif        

#ifdef MF40
    MF40_Init();
#endif
    
#ifdef USE_WATCHDOG    
    SYS_WDT_Enable(false);          // enable Watchdog in Non-Window Mode
#endif    
    // -------------------------------------------------------------------------
    // Zeitmessungen mus_min / mus_maxdurchgeführt am 06.06.2017 (mit Display)
    // Bitte beachten: Die mus_min Zeiten liegen meist bei 1 us, und zwar genau
    // bei den Teilen, die nur unter bestimmten Bedingungen etwas tun.
    // mus_max wird erst nach >20s bestimmt, dadurch sind dann die Interrupt-
    // zeiten für UART und TIMER mit enthalten (nicht jedoch SPI, außer bei den
    // Teilen, die SPI verwenden); Summe ca. 85 us, hauptsächlich vom TIMER.
    // -------------------------------------------------------------------------
    while ( true )
    {
        
//if (IN_TASTER) {
//    if (!ITs) {COMtx_Kick("A9", 2); ITs=1;}
//} else ITs=0;        
        
        /* Maintain state machines of all polled MPLAB Harmony modules. */
//mus(1);
        SYS_Tasks ( );
//mus(0); // Ergebnis: mus_min=1 us, mus_max=95 us
        inputs = S0inputs;
        dilsw = TBUSrotsw;                          // 0..15

        if (EVE_available) {
            if (dilsw != 0xf) {                     // ROTSWITCH==F --> nur Display
                tbus_ADR = dilsw + '0';             // '0', '1', '2', ...
            } else {
                tbus_ADR = 0;                       // off
            }                                       // SODIL1+2 --> ON2/ON1:
#ifdef MF40
            tbus_ADR2 = 0;  // off
#else        
            tbus_ADR2 = (TypeCodeInputs & 3) + 'D'; // 0='D', 1='E', 2='F', 3='G'
#endif
        } else {
            tbus_ADR = dilsw + '0';                 // '0', '1', '2', ...
            tbus_ADR2 = 0;                          // off
        }

        // ********** Baudrate+Parity prüfen und ggf. anpassen **********
        if (Com1_Baudrate!=com1_tbaud) {
            com1_tbaud = Com1_Baudrate;
            DRV_USART_BaudSet(COMhandle, Com1_Baudrate);       // Harmony Setting: 460800
        }
        if (Com1_Parity!=com1_tparity) {
            com1_tparity = Com1_Parity;
            DRV_USART_LineControlSet(COMhandle, Com_Parity2Linecontrol(Com1_Parity));
        }
        // ********** S0-Eingänge auswerten **********
#ifdef S0IN12
//mus(1);
//      InputEval();    // nicht mehr hier, wird direkt vom MilliSekunden-Interrupt aufgerufen
//mus(0); // Ergebnis: mus_min=1 us, mus_max=95 us
#endif
        // ********** MF40-Evaluation ******************************************
#ifdef MF40
        MF40_Eval();
#endif
        // ********** LED evaluieren und ausgeben ******************************
//mus(1);
        LED_Eval();
//mus(0); // Ergebnis: mus_min=1 us, mus_max=93 us
        
        // ********** TBUS Empfang bearbeiten **********************************
//mus(1);
#ifndef COM1_MODBUS_SLAVE
        TBUS_Eval();
#endif        
//mus(0); // Ergebnis: mus_min=1 us, mus_max=1322 us

        // ********** MODBUS-SLAVE Empfang bearbeiten **************************
#ifdef COM1_MODBUS_SLAVE
        MBSL_Eval();
#endif        

        // ********** MODBUS-MASTER Empfang bearbeiten *************************
#ifdef COM2_MODBUS_MASTER
        MBMA_Eval();
#endif        
        
        // ********** EVE Display **********************************************
#ifdef USE_EVE        
        if (EVE_available) {
            if (EVE_Com_Error) {
                EVE_Com_Error_Cnt++;
                EVE_Com_Error=0;
                FT801_Init();
            }
            if (ns_eve) {
                ns_eve=0;
                if (FT801_Init_Check()) {
                    EVE_Check_Error_Cnt++;
                    FT801_Init();
                }
            }
//mus(1);
            FT801_Display_Hub();
//mus(0); // Ergebnis: mus_min=71 us, mus_max=9824 us (@14MHZ/40MHZ SPI Clock/PBCLK)
        }
#endif        
        
        // ********** Neue Zehntel-Sekunde **********
        if (NeueZehntelSekunde) {
            NeueZehntelSekunde=0;
        }
        // ********** Neue Sekunde **********
        counter++; counter_s++;
        if (NeueSekunde) {
            NeueSekunde=0;
            if (MasterConnectedTO) MasterConnectedTO--;
            if (LEDovrTO) LEDovrTO--;
            if (SysOpen) SysOpen--;
            counts_per_second = counter_s;
            counter_s = 0;
            ns_eve++;
        }

        // Reset auslösen?
        if (doResetRequest==590) while(1);
        watchdog();
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}

/*******************************************************************************
 End of File
*/

