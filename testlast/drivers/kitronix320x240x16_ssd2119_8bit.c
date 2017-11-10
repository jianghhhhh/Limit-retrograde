//*****************************************************************************
//
// kitronix320x240x16_ssd2119_8bit.c - Display driver for the Kitronix
//                                     K350QVG-V1-F TFT display with an SSD2119
//                                     controller.  This version assumes an
//                                     8080-8bit interface between the micro
//                                     and display (PS3-0 = 0011b).
//
// This is part of revision 5228 of the DK-LM3S9B92.
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup display_api
//! @{
//
//*****************************************************************************

#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/epi.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "grlib/grlib.h"
#include "drivers/kitronix320x240x16_ssd2119_8bit.h"
#include "drivers/set_pinout.h"

//*****************************************************************************
//
// This driver operates in four different screen orientations.  They are:
//
// * Portrait - The screen is taller than it is wide, and the flex connector is
//              on the left of the display.  This is selected by defining
//              PORTRAIT.
//
// * Landscape - The screen is wider than it is tall, and the flex connector is
//               on the bottom of the display.  This is selected by defining
//               LANDSCAPE.
//
// * Portrait flip - The screen is taller than it is wide, and the flex
//                   connector is on the right of the display.  This is
//                   selected by defining PORTRAIT_FLIP.
//
// * Landscape flip - The screen is wider than it is tall, and the flex
//                    connector is on the top of the display.  This is
//                    selected by defining LANDSCAPE_FLIP.
//
// These can also be imagined in terms of screen rotation; if portrait mode is
// 0 degrees of screen rotation, landscape is 90 degrees of counter-clockwise
// rotation, portrait flip is 180 degrees of rotation, and landscape flip is
// 270 degress of counter-clockwise rotation.
//
// If no screen orientation is selected, "landscape flip" mode will be used.
//
//*****************************************************************************
#if ! defined(PORTRAIT) && ! defined(PORTRAIT_FLIP) && \
    ! defined(LANDSCAPE) && ! defined(LANDSCAPE_FLIP)
#define PORTRAIT
#endif

//*****************************************************************************
//
// Various definitions controlling coordinate space mapping and drawing
// direction in the four supported orientations.
//
//*****************************************************************************
#ifdef PORTRAIT
#define HORIZ_DIRECTION 0x28
#define VERT_DIRECTION 0x20
#define MAPPED_X(x, y) (239 - (y))
#define MAPPED_Y(x, y) (x)
#endif


#ifdef LANDSCAPE
#define HORIZ_DIRECTION 0x00
#define VERT_DIRECTION  0x08
#define MAPPED_X(x, y) (239 - (x))
#define MAPPED_Y(x, y) (319 - (y))
#endif
#ifdef PORTRAIT_FLIP
#define HORIZ_DIRECTION 0x18
#define VERT_DIRECTION 0x10
#define MAPPED_X(x, y) (y)
#define MAPPED_Y(x, y) (319 - (x))
#endif
#ifdef LANDSCAPE_FLIP
#define HORIZ_DIRECTION 0x30
#define VERT_DIRECTION  0x38
#define MAPPED_X(x, y) (x)
#define MAPPED_Y(x, y) (y)
#endif

//*****************************************************************************
//
// Defines for the pins that are used to communicate with the SSD2119.
//
//*****************************************************************************

#define LCD_DATAH_PINS          0xFF
#define LCD_DATAH_PERIPH        SYSCTL_PERIPH_GPIOD
#define LCD_DATAH_BASE          GPIO_PORTD_BASE

//
// LCD control line GPIO definitions.
//
#define LCD_RST_PERIPH          SYSCTL_PERIPH_GPIOB
#define LCD_RST_BASE            GPIO_PORTB_BASE
#define LCD_RST_PIN             GPIO_PIN_7
#define LCD_RD_PERIPH           SYSCTL_PERIPH_GPIOB
#define LCD_RD_BASE             GPIO_PORTB_BASE
#define LCD_RD_PIN              GPIO_PIN_5
#define LCD_WR_PERIPH           SYSCTL_PERIPH_GPIOH
#define LCD_WR_BASE             GPIO_PORTH_BASE
#define LCD_WR_PIN              GPIO_PIN_6
#define LCD_DC_PERIPH           SYSCTL_PERIPH_GPIOH
#define LCD_DC_BASE             GPIO_PORTH_BASE
#define LCD_DC_PIN              GPIO_PIN_7

//*****************************************************************************
//
// Backlight control GPIO used with the Flash/SRAM/LCD daughter board.
//
//*****************************************************************************
#define LCD_BACKLIGHT_BASE      GPIO_PORTE_BASE
#define LCD_BACKLIGHT_PIN       GPIO_PIN_2

//*****************************************************************************
//
// Macro used to set the LCD data bus in preparation for writing a byte to the
// device.
//
//*****************************************************************************
#define SET_LCD_DATA(ucByte)                                                  \
{                                                                             \
    HWREG(LCD_DATAH_BASE + GPIO_O_DATA + (LCD_DATAH_PINS << 2)) = (ucByte);   \
}

//*****************************************************************************
//
// Various internal SD2119 registers name labels
//
//*****************************************************************************
#define S6D0154_DRIVER_OPT_CTRL_REG   0x01
#define S6D0154_DRV_WAVEFORM_CTRL_REG 0x02
#define S6D0154_ENTRY_MODE_REG        0x03
#define S6D0154_DISPLAY_CTRL_REG      0x07
#define S6D0154_BLANK_PERIOD_CTRL_REG 0X08
#define S6D0154_FRAME_CYCLE_CTRL_REG  0x0B
#define S6D0154_EXTERNAL_IF_CTRL_REG  0x0C
#define S6D0154_MODE_REG              0x0E
#define S6D0154_START_OSC_REG         0x0F
#define S6D0154_PWR_CTRL_1_REG        0x10
#define S6D0154_PWR_CTRL_2_REG        0x11
#define S6D0154_PWR_CTRL_3_REG        0x12
#define S6D0154_PWR_CTRL_4_REG        0x13
#define S6D0154_PWR_CTRL_5_REG        0x14
#define S6D0154_VCI_RECYCLING_REG     0x15
#define S6D0154_H_WINDOW_START_REG    0x37
#define S6D0154_H_WINDOW_END_REG      0x36
#define S6D0154_V_WINDOW_START_REG    0x39
#define S6D0154_V_WINDOW_END_REG      0x38
#define S6D0154_H_RAM_ADDR_REG        0x20
#define S6D0154_V_RAM_ADDR_REG        0x21
#define S6D0154_RAM_DATA_REG          0x22
#define S6D0154_GAMMA_CTRL_1_REG      0x50
#define S6D0154_GAMMA_CTRL_2_REG      0x51
#define S6D0154_GAMMA_CTRL_3_REG      0x52
#define S6D0154_GAMMA_CTRL_4_REG      0x53
#define S6D0154_GAMMA_CTRL_5_REG      0x54
#define S6D0154_GAMMA_CTRL_6_REG      0x55
#define S6D0154_GAMMA_CTRL_7_REG      0x56
#define S6D0154_GAMMA_CTRL_8_REG      0x57
#define S6D0154_GAMMA_CTRL_9_REG      0x58
#define S6D0154_GAMMA_CTRL_10_REG     0x59

#define ENTRY_MODE_DEFAULT 0x1028
#define MAKE_ENTRY_MODE(x) ((ENTRY_MODE_DEFAULT & 0xFF00) | (x))

//*****************************************************************************
//
// The dimensions of the LCD panel.
//
//*****************************************************************************
#define LCD_VERTICAL_MAX 240
#define LCD_HORIZONTAL_MAX 320

//*****************************************************************************
//
// Translates a 24-bit RGB color to a display driver-specific color.
//
// \param c is the 24-bit RGB color.  The least-significant byte is the blue
// channel, the next byte is the green channel, and the third byte is the red
// channel.
//
// This macro translates a 24-bit RGB color into a value that can be written
// into the display's frame buffer in order to reproduce that color, or the
// closest possible approximation of that color.
//
// \return Returns the display-driver specific color.
//
//*****************************************************************************
#define DPYCOLORTRANSLATE(c)    ((((c) & 0x00f80000) >> 8) |               \
                                 (((c) & 0x0000fc00) >> 5) |               \
                                 (((c) & 0x000000f8) >> 3))

//*****************************************************************************
//
// Function pointer types for low level LCD controller access functions.
//
//*****************************************************************************
typedef void (*pfnWriteData)(unsigned short usData);
typedef void (*pfnWriteCommand)(unsigned char ucData);

//*****************************************************************************
//
// Function pointers for low level LCD controller access functions.
//
//*****************************************************************************

static void WriteDataGPIO(unsigned short usData);
static void WriteCommandGPIO(unsigned char ucData);

pfnWriteData WriteData = WriteDataGPIO;
pfnWriteCommand WriteCommand = WriteCommandGPIO;

//*****************************************************************************
//
// Writes a data word to the SSD2119.  This function implements the basic GPIO
// interface to the LCD display.
//
//*****************************************************************************
static void
WriteDataGPIO(unsigned short usData)
{
    //
    // Write the most significant byte of the data to the bus.
    //
    SET_LCD_DATA(usData >> 8);

    //
    // Assert the write enable signal.  We need to do this 3 times to ensure
    // that we don't violate the timing requirements for the display (when
    // running with a 50MHz system clock).
    //
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = 0;
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = 0;
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = 0;

    //
    // Deassert the write enable signal.
    //
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = LCD_WR_PIN;
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = LCD_WR_PIN;
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = LCD_WR_PIN;

    //
    // Write the least significant byte of the data to the bus.
    //
    SET_LCD_DATA(usData);

    //
    // Assert the write enable signal.
    //
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = 0;
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = 0;
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = 0;

    //
    // Deassert the write enable signal.  WR needs to be high for at least
    // 50nS and back-to-back inlined calls to this function could just,
    // conceivably violate this so add one access to pad the time a bit.
    //
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = LCD_WR_PIN;
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = LCD_WR_PIN;
}

//*****************************************************************************
//
// Writes a data word to the SSD2119 via the EPI interface as wired when using
// the lm3s9b96 development kit SRAM/flash daughter board.
//
//*****************************************************************************
static void
WriteDataEPI(unsigned short usData)
{
    HWREGB(LCD_DATA_PORT) = usData >> 8;
    HWREGB(LCD_DATA_PORT) = usData;
}

//*****************************************************************************
//
// Writes a command word to the SSD2119 via the EPI interface as wired when
// using the lm3s9b96 development kit SRAM/flash daughter board.
//
//*****************************************************************************
static void
WriteCommandEPI(unsigned char ucData)
{
    HWREGB(LCD_COMMAND_PORT) = 0;
    HWREGB(LCD_COMMAND_PORT) = ucData;
}

//*****************************************************************************
//
// Writes a command to the SSD2119.  This function implements the basic GPIO
// interface to the LCD display.
//
//*****************************************************************************
static void
WriteCommandGPIO(unsigned char ucData)
{

    //
    // Write the most significant byte of the data to the bus. This is always
    // 0 since commands are no more than 8 bits currently.
    //
    SET_LCD_DATA(0);

    //
    // Assert DC
    //
    HWREG(LCD_DC_BASE + GPIO_O_DATA + (LCD_DC_PIN << 2)) = 0;

    //
    // Assert the write enable signal.  Do this twice to 3 times to slow things
    // down a bit.
    //
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = 0;
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = 0;
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = 0;

    //
    // Deassert the write enable signal. We need to leave WR high for at least
    // 50nS so, again, stick in a dummy write to pad the timing.
    //
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = LCD_WR_PIN;
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = LCD_WR_PIN;

    //
    // Write the least significant byte of the data to the bus.
    //
    SET_LCD_DATA(ucData);

    //
    // Assert the write enable signal.  Again, do this twice to ensure
    // we meet the timing spec.
    //
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = 0;
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = 0;

    //
    // Deassert the write enable signal.
    //
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = LCD_WR_PIN;

    //
    // Set the DC signal high, indicating that following writes are data.  There
    // is no need to pad the timing here since we won't violate the 50nS rule
    // even if this function is inlined and it or WriteData are called
    // immediately after we exit.
    //
    HWREG(LCD_DC_BASE + GPIO_O_DATA + (LCD_DC_PIN << 2)) = LCD_DC_PIN;
}

//*****************************************************************************
//
// Initializes the pins required for the GPIO-based LCD interface.
//
// This function configures the GPIO pins used to control the LCD display
// when the basic GPIO interface is in use.  On exit, the LCD controller
// has been reset and is ready to receive command and data writes.
//
// \return None.
//
//*****************************************************************************
static void
InitGPIOLCDInterface(unsigned long ulClockMS)
{
    //
    // Convert the PB7/NMI pin into a GPIO pin.  This requires the use of the
    // GPIO lock since changing the state of the pin is otherwise disabled.
    //
    HWREG(GPIO_PORTB_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY_DD;
    HWREG(GPIO_PORTB_BASE + GPIO_O_CR) = 0x80;

    //
    // Make PB7 an output.
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_7);

    //
    // Clear the commit register, effectively locking access to registers
    // controlling the PB7 configuration.
    //
    HWREG(GPIO_PORTB_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY_DD;
    HWREG(GPIO_PORTB_BASE + GPIO_O_CR) = 0x00;

    //
    // Configure the pins that connect to the LCD as GPIO outputs.
    //
    GPIOPinTypeGPIOOutput(LCD_DATAH_BASE, LCD_DATAH_PINS);
    GPIOPinTypeGPIOOutput(LCD_DC_BASE, LCD_DC_PIN);
    GPIOPinTypeGPIOOutput(LCD_RD_BASE, LCD_RD_PIN);
    GPIOPinTypeGPIOOutput(LCD_WR_BASE, LCD_WR_PIN);
    GPIOPinTypeGPIOOutput(LCD_RST_BASE, LCD_RST_PIN);

    //
    // Set the LCD control pins to their default values.  This also asserts the
    // LCD reset signal.
    //
    GPIOPinWrite(LCD_DATAH_BASE, LCD_DATAH_PINS, 0x00);
    GPIOPinWrite(LCD_DC_BASE, LCD_DC_PIN, 0x00);
    GPIOPinWrite(LCD_RD_BASE, LCD_RD_PIN, LCD_RD_PIN);
    GPIOPinWrite(LCD_WR_BASE, LCD_WR_PIN, LCD_WR_PIN);
    GPIOPinWrite(LCD_RST_BASE, LCD_RST_PIN, 0x00);

    //
    // Delay for 1ms.
    //
    SysCtlDelay(ulClockMS);

    //
    // Deassert the LCD reset signal.
    //
    GPIOPinWrite(LCD_RST_BASE, LCD_RST_PIN, LCD_RST_PIN);

    //
    // Delay for 1ms while the LCD comes out of reset.
    //
    SysCtlDelay(ulClockMS);
}

//*****************************************************************************
//
// Initialize the low level LCD communication when the SRAM/Flash daughter
// board is connected.
//
// This function sets up the low level LCD access function pointers correctly
// when the SRAM/Flash daughter board is present.  In this case, communication
// with the LCD panel is via the EPI rather than via the basic GPIO interface.
// On exit, the LCD controller has been reset and is ready to receive command
// and data writes.
//
// \return None.
//
//*****************************************************************************
static void
InitSRAMDaughterLCDInterface(unsigned long ulClockMS)
{
    //
    // Set the low level access function pointers so that the EPI interface is
    // used to access the LCD controller.
    //
    WriteData = WriteDataEPI;
    WriteCommand = WriteCommandEPI;

    //
    // Asserts the LCD reset signal.
    //
    HWREGB(LCD_CONTROL_CLR_REG) = LCD_CONTROL_NRESET;

    //
    // Delay for 1ms.
    //
    SysCtlDelay(ulClockMS);

    //
    // Deassert the LCD reset signal
    //
    HWREGB(LCD_CONTROL_SET_REG) = LCD_CONTROL_NRESET;

    //
    // Delay for 1ms while the LCD comes out of reset.
    //
    SysCtlDelay(ulClockMS);

    //
    // Turn on the backlight.
    //
    GPIOPinTypeGPIOOutput(LCD_BACKLIGHT_BASE, LCD_BACKLIGHT_PIN);
    GPIOPinWrite(LCD_BACKLIGHT_BASE, LCD_BACKLIGHT_PIN, LCD_BACKLIGHT_PIN);
}

//*****************************************************************************
//
//! Initializes the display driver.
//!
//! This function initializes the SSD2119 display controller on the panel,
//! preparing it to display data.
//!
//! \return None.
//
//*****************************************************************************
void
Kitronix320x240x16_SSD2119Init(void)
{
    unsigned long ulClockMS, ulCount;

    //
    // Get the current processor clock frequency.
    //
    ulClockMS = SysCtlClockGet() / (3 * 1000);

    //
    // Perform low level interface initialization depending upon how the LCD
    // is connected to the Stellaris microcontroller.  This varies depending
    // upon the daughter board connected it is possible that a daughter board
    // can drive the LCD directly rather than via the basic GPIO interface.
    //
    switch(g_eDaughterType)
    {
        case DAUGHTER_NONE:
        default:
        {
            //
            // Initialize the GPIOs used to interface to the LCD controller.
            //
            InitGPIOLCDInterface(ulClockMS);
            break;
        }

        case DAUGHTER_SRAM_FLASH:
        {
            //
            // Initialize the display via the EPI interface to the LCD.
            //
            InitSRAMDaughterLCDInterface(ulClockMS);
            break;
        }
    }
    
    // POWER ON SEQUENCE
    WriteCommand(S6D0154_PWR_CTRL_2_REG);
    WriteData(0x001C);
    WriteCommand(S6D0154_PWR_CTRL_3_REG);
    WriteData(0x1212);
    WriteCommand(S6D0154_PWR_CTRL_4_REG);
    WriteData(0x0041);
    WriteCommand(S6D0154_PWR_CTRL_5_REG);
    WriteData(0x426A);
    WriteCommand(S6D0154_PWR_CTRL_1_REG);
    WriteData(0x0C00);
    SysCtlDelay(20 * ulClockMS);
    WriteCommand(S6D0154_PWR_CTRL_2_REG);
    WriteData(0x011C);
    SysCtlDelay(10 * ulClockMS);
    WriteCommand(S6D0154_PWR_CTRL_2_REG);
    WriteData(0x011C);
    SysCtlDelay(10 * ulClockMS);
    WriteCommand(S6D0154_PWR_CTRL_2_REG);
    WriteData(0x031C);
    SysCtlDelay(10 * ulClockMS);
    WriteCommand(S6D0154_PWR_CTRL_2_REG);
    WriteData(0x071C);
    SysCtlDelay(10 * ulClockMS);
    WriteCommand(S6D0154_PWR_CTRL_2_REG);
    WriteData(0x0F1C);
    SysCtlDelay(20 * ulClockMS);
    WriteCommand(S6D0154_PWR_CTRL_2_REG);
    WriteData(0x0F3C);
    SysCtlDelay(30 * ulClockMS);
    
    // OTHER MODE SET
    WriteCommand(S6D0154_DRIVER_OPT_CTRL_REG);
    WriteData(0x0128);
    WriteCommand(S6D0154_DRV_WAVEFORM_CTRL_REG);
    WriteData(0x0100);
    WriteCommand(S6D0154_ENTRY_MODE_REG);
    WriteData(0x1028);
    WriteCommand(S6D0154_BLANK_PERIOD_CTRL_REG);
    WriteData(0x0808);
    WriteCommand(S6D0154_FRAME_CYCLE_CTRL_REG);
    WriteData(0x1105); 
    WriteCommand(S6D0154_EXTERNAL_IF_CTRL_REG);
    WriteData(0x0000);
    WriteCommand(S6D0154_MODE_REG);
    WriteData(0x0200);
    WriteCommand(S6D0154_START_OSC_REG);
    WriteData(0x1801);
    WriteCommand(S6D0154_VCI_RECYCLING_REG);
    WriteData(0x0020); 
    
    // Gamma set
    WriteCommand(S6D0154_GAMMA_CTRL_1_REG);
    WriteData(0x0101);
    WriteCommand(S6D0154_GAMMA_CTRL_2_REG);
    WriteData(0x0903);
    WriteCommand(S6D0154_GAMMA_CTRL_3_REG);
    WriteData(0x0E0E);
    WriteCommand(S6D0154_GAMMA_CTRL_4_REG);
    WriteData(0x0001);
    WriteCommand(S6D0154_GAMMA_CTRL_5_REG);
    WriteData(0x0101);
    WriteCommand(S6D0154_GAMMA_CTRL_6_REG);
    WriteData(0x0D06);
    WriteCommand(S6D0154_GAMMA_CTRL_7_REG);
    WriteData(0x0E0E);
    WriteCommand(S6D0154_GAMMA_CTRL_8_REG);
    WriteData(0x0100);
    WriteCommand(S6D0154_GAMMA_CTRL_9_REG);
    WriteData(0x081F);
    WriteCommand(S6D0154_GAMMA_CTRL_10_REG);
    WriteData(0x0200);

    // Display on sequence
    WriteCommand(S6D0154_DISPLAY_CTRL_REG);
    WriteData(0x0010);
    WriteCommand(S6D0154_DISPLAY_CTRL_REG);
    WriteData(0x0012);
    SysCtlDelay(10 * ulClockMS);
    WriteCommand(S6D0154_DISPLAY_CTRL_REG);
    WriteData(0x0012);
    WriteCommand(S6D0154_DISPLAY_CTRL_REG);
    WriteData(0x0013);
    SysCtlDelay(10 * ulClockMS);
    WriteCommand(S6D0154_DISPLAY_CTRL_REG);
    WriteData(0x0017);

    
    WriteCommand(S6D0154_H_WINDOW_END_REG);
    WriteData(0x00ef);
    WriteCommand(S6D0154_H_WINDOW_START_REG);
    WriteData(0x0000);

    WriteCommand(S6D0154_V_WINDOW_END_REG);
    WriteData(0x013f);
    WriteCommand(S6D0154_V_WINDOW_START_REG);
    WriteData(0x0000);

    WriteCommand(S6D0154_H_RAM_ADDR_REG);
    WriteData(0x00ef);
    WriteCommand(S6D0154_V_RAM_ADDR_REG);
    WriteData(0x0000);

    WriteCommand(S6D0154_RAM_DATA_REG);

    for(ulCount = 0; ulCount < (320 * 240); ulCount++)
    {
        WriteData(0x0000);
    }
}

//*****************************************************************************
//
//! Draws a pixel on the screen.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX is the X coordinate of the pixel.
//! \param lY is the Y coordinate of the pixel.
//! \param ulValue is the color of the pixel.
//!
//! This function sets the given pixel to a particular color.  The coordinates
//! of the pixel are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
static void
Kitronix240x320x16_SD60154PixelDraw(void *pvDisplayData, long lX, long lY,
                                   unsigned long ulValue)
{
 
    //
    // Set the X address of the display cursor.
    //
    WriteCommand(S6D0154_H_RAM_ADDR_REG);
    WriteData(MAPPED_X(lX, lY));

    //
    // Set the Y address of the display cursor.
    //
    WriteCommand(S6D0154_V_RAM_ADDR_REG);
    WriteData(MAPPED_Y(lX, lY));

    //
    // Write the pixel value.
    //
    WriteCommand(S6D0154_RAM_DATA_REG);
    WriteData(ulValue);

}

//*****************************************************************************
//
//! Draws a horizontal sequence of pixels on the screen.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX is the X coordinate of the first pixel.
//! \param lY is the Y coordinate of the first pixel.
//! \param lX0 is sub-pixel offset within the pixel data, which is valid for 1
//! or 4 bit per pixel formats.
//! \param lCount is the number of pixels to draw.
//! \param lBPP is the number of bits per pixel; must be 1, 4, or 8.
//! \param pucData is a pointer to the pixel data.  For 1 and 4 bit per pixel
//! formats, the most significant bit(s) represent the left-most pixel.
//! \param pucPalette is a pointer to the palette used to draw the pixels.
//!
//! This function draws a horizontal sequence of pixels on the screen, using
//! the supplied palette.  For 1 bit per pixel format, the palette contains
//! pre-translated colors; for 4 and 8 bit per pixel formats, the palette
//! contains 24-bit RGB values that must be translated before being written to
//! the display.
//!
//! \return None.
//
//*****************************************************************************
static void
Kitronix240x320x16_SD60154PixelDrawMultiple(void *pvDisplayData, long lX,
                                           long lY, long lX0, long lCount,
                                           long lBPP,
                                           const unsigned char *pucData,
                                           const unsigned char *pucPalette)
{
    unsigned long ulByte;

    //
    // Set the cursor increment to left to right, followed by top to bottom.
    //
    WriteCommand(S6D0154_ENTRY_MODE_REG);
    WriteData(MAKE_ENTRY_MODE(HORIZ_DIRECTION));

    //
    // Set the starting X address of the display cursor.
    //
    WriteCommand(S6D0154_H_RAM_ADDR_REG);
    WriteData(MAPPED_X(lX, lY));

    //
    // Set the Y address of the display cursor.
    //
    WriteCommand(S6D0154_V_RAM_ADDR_REG);
    WriteData(MAPPED_Y(lX, lY));

    //
    // Write the data RAM write command.
    //
    WriteCommand(S6D0154_RAM_DATA_REG);

    //
    // Determine how to interpret the pixel data based on the number of bits
    // per pixel.
    //
    switch(lBPP)
    {
        //
        // The pixel data is in 1 bit per pixel format.
        //
        case 1:
        {
            //
            // Loop while there are more pixels to draw.
            //
            while(lCount)
            {
                //
                // Get the next byte of image data.
                //
                ulByte = *pucData++;

                //
                // Loop through the pixels in this byte of image data.
                //
                for(; (lX0 < 8) && lCount; lX0++, lCount--)
                {
                    //
                    // Draw this pixel in the appropriate color.
                    //
                    WriteData(((unsigned long *)pucPalette)[(ulByte >>
                                                             (7 - lX0)) & 1]);
                }

                //
                // Start at the beginning of the next byte of image data.
                //
                lX0 = 0;
            }

            //
            // The image data has been drawn.
            //
            break;
        }

        //
        // The pixel data is in 4 bit per pixel format.
        //
        case 4:
        {
            //
            // Loop while there are more pixels to draw.  "Duff's device" is
            // used to jump into the middle of the loop if the first nibble of
            // the pixel data should not be used.  Duff's device makes use of
            // the fact that a case statement is legal anywhere within a
            // sub-block of a switch statement.  See
            // http://en.wikipedia.org/wiki/Duff's_device for detailed
            // information about Duff's device.
            //
            switch(lX0 & 1)
            {
                case 0:
                    while(lCount)
                    {
                        //
                        // Get the upper nibble of the next byte of pixel data
                        // and extract the corresponding entry from the
                        // palette.
                        //
                        ulByte = (*pucData >> 4) * 3;
                        ulByte = (*(unsigned long *)(pucPalette + ulByte) &
                                  0x00ffffff);

                        //
                        // Translate this palette entry and write it to the
                        // screen.
                        //
                        WriteData(DPYCOLORTRANSLATE(ulByte));

                        //
                        // Decrement the count of pixels to draw.
                        //
                        lCount--;

                        //
                        // See if there is another pixel to draw.
                        //
                        if(lCount)
                        {
                case 1:
                            //
                            // Get the lower nibble of the next byte of pixel
                            // data and extract the corresponding entry from
                            // the palette.
                            //
                            ulByte = (*pucData++ & 15) * 3;
                            ulByte = (*(unsigned long *)(pucPalette + ulByte) &
                                      0x00ffffff);

                            //
                            // Translate this palette entry and write it to the
                            // screen.
                            //
                            WriteData(DPYCOLORTRANSLATE(ulByte));

                            //
                            // Decrement the count of pixels to draw.
                            //
                            lCount--;
                        }
                    }
            }

            //
            // The image data has been drawn.
            //
            break;
        }

        //
        // The pixel data is in 8 bit per pixel format.
        //
        case 8:
        {
            //
            // Loop while there are more pixels to draw.
            //
            while(lCount--)
            {
                //
                // Get the next byte of pixel data and extract the
                // corresponding entry from the palette.
                //
                ulByte = *pucData++ * 3;
                ulByte = *(unsigned long *)(pucPalette + ulByte) & 0x00ffffff;

                //
                // Translate this palette entry and write it to the screen.
                //
                WriteData(DPYCOLORTRANSLATE(ulByte));
            }

            //
            // The image data has been drawn.
            //
            break;
        }

        //
        // We are being passed data in the display's native format.  Merely
        // write it directly to the display.  This is a special case which is
        // not used by the graphics library but which is helpful to
        // applications which may want to handle, for example, JPEG images.
        //
        case 16:
        {
            unsigned short usByte;

            //
            // Loop while there are more pixels to draw.
            //
            while(lCount--)
            {
                //
                // Get the next byte of pixel data and extract the
                // corresponding entry from the palette.
                //
                usByte = *((unsigned short *)pucData);
                pucData += 2;

                //
                // Translate this palette entry and write it to the screen.
                //
                WriteData(usByte);
            }
        }
    }
}

//*****************************************************************************
//
//! Draws a horizontal line.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX1 is the X coordinate of the start of the line.
//! \param lX2 is the X coordinate of the end of the line.
//! \param lY is the Y coordinate of the line.
//! \param ulValue is the color of the line.
//!
//! This function draws a horizontal line on the display.  The coordinates of
//! the line are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
static void
Kitronix240x320x16_SD60154LineDrawH(void *pvDisplayData, long lX1, long lX2,
                                   long lY, unsigned long ulValue)
{

    //
    // Set the cursor increment to left to right, followed by top to bottom.
    //
    WriteCommand(S6D0154_ENTRY_MODE_REG);
    WriteData(MAKE_ENTRY_MODE(HORIZ_DIRECTION));

    //
    // Set the starting X address of the display cursor.
    //
  
    WriteCommand(S6D0154_H_RAM_ADDR_REG);
    WriteData(MAPPED_X(lX1, lY));

    //
    // Set the Y address of the display cursor.
    //
    WriteCommand(S6D0154_V_RAM_ADDR_REG);
    WriteData(MAPPED_Y(lX1, lY));

    //
    // Write the data RAM write command.
    //
    WriteCommand(S6D0154_RAM_DATA_REG);

    //
    // Loop through the pixels of this horizontal line.
    //
    while(lX1++ <= lX2)
    {
        //
        // Write the pixel value.
        //
        WriteData(ulValue);
    }

}

//*****************************************************************************
//
//! Draws a vertical line.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX is the X coordinate of the line.
//! \param lY1 is the Y coordinate of the start of the line.
//! \param lY2 is the Y coordinate of the end of the line.
//! \param ulValue is the color of the line.
//!
//! This function draws a vertical line on the display.  The coordinates of the
//! line are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
static void
Kitronix240x320x16_SD60154LineDrawV(void *pvDisplayData, long lX, long lY1,
                                   long lY2, unsigned long ulValue)
{
  
    //
    // Set the cursor increment to top to bottom, followed by left to right.
    //
    WriteCommand(S6D0154_ENTRY_MODE_REG);
    WriteData(MAKE_ENTRY_MODE(VERT_DIRECTION));

    //
    // Set the X address of the display cursor.
    //
    WriteCommand(S6D0154_H_RAM_ADDR_REG);
    WriteData(MAPPED_X(lX, lY1));

    //
    // Set the starting Y address of the display cursor.
    //
    WriteCommand(S6D0154_V_RAM_ADDR_REG);
    WriteData(MAPPED_Y(lX, lY1));

    //
    // Write the data RAM write command.
    //
    WriteCommand(S6D0154_RAM_DATA_REG);

    //
    // Loop through the pixels of this vertical line.
    //
    while(lY1++ <= lY2)
    {
        //
        // Write the pixel value.
        //
        WriteData(ulValue);
    }
  
}

//*****************************************************************************
//
//! Fills a rectangle.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param pRect is a pointer to the structure describing the rectangle.
//! \param ulValue is the color of the rectangle.
//!
//! This function fills a rectangle on the display.  The coordinates of the
//! rectangle are assumed to be within the extents of the display, and the
//! rectangle specification is fully inclusive (in other words, both sXMin and
//! sXMax are drawn, along with sYMin and sYMax).
//!
//! \return None.
//
//*****************************************************************************
static void
Kitronix240x320x16_SD60154RectFill(void *pvDisplayData, const tRectangle *pRect,
                                  unsigned long ulValue)
{
  
    long lCount;

    //
    // Write the Y extents of the rectangle.
    //
    WriteCommand(S6D0154_ENTRY_MODE_REG);
    WriteData(MAKE_ENTRY_MODE(HORIZ_DIRECTION));

    //
    // Write the X extents of the rectangle.
    //
    WriteCommand(S6D0154_H_WINDOW_START_REG);
#if (defined PORTRAIT) || (defined LANDSCAPE)
    WriteData(MAPPED_X(pRect->sXMax, pRect->sYMax));
#else
    WriteData(MAPPED_X(pRect->sXMin, pRect->sYMin));
#endif

    WriteCommand(S6D0154_H_WINDOW_END_REG);
#if (defined PORTRAIT) || (defined LANDSCAPE)
    WriteData(MAPPED_X(pRect->sXMin, pRect->sYMin));
#else
    WriteData(MAPPED_X(pRect->sXMax, pRect->sYMax));
#endif

    //
    // Write the Y extents of the rectangle
    //
    WriteCommand(S6D0154_V_WINDOW_START_REG);
#if (defined LANDSCAPE_FLIP) || (defined PORTRAIT)
    WriteData(MAPPED_Y(pRect->sXMin, pRect->sYMin));
#else
    WriteData(MAPPED_Y(pRect->sXMax, pRect->sYMax));
#endif
     WriteCommand(S6D0154_V_WINDOW_END_REG);
#if (defined LANDSCAPE_FLIP) || (defined PORTRAIT)
    WriteData(MAPPED_Y(pRect->sXMax, pRect->sYMax));
#else
    WriteData(MAPPED_Y(pRect->sXMin, pRect->sYMin));
#endif
    
    //
    // Set the display cursor to the upper left of the rectangle (in application
    // coordinate space).
    //
    WriteCommand(S6D0154_H_RAM_ADDR_REG);
    WriteData(MAPPED_X(pRect->sXMin, pRect->sYMin));

    WriteCommand(S6D0154_V_RAM_ADDR_REG);
    WriteData(MAPPED_Y(pRect->sXMin, pRect->sYMin));

    //
    // Tell the controller we are about to write data into its RAM.
    //
    WriteCommand(S6D0154_RAM_DATA_REG);

    //
    // Loop through the pixels of this filled rectangle.
    //
    for(lCount = ((pRect->sXMax - pRect->sXMin + 1) *
                  (pRect->sYMax - pRect->sYMin + 1)); lCount >= 0; lCount--)
    {
        //
        // Write the pixel value.
        //
        WriteData(ulValue);
    }

    //
    // Reset the X extents to the entire screen.
    //
    WriteCommand(S6D0154_H_WINDOW_END_REG);
    WriteData(0x00ef);
    WriteCommand(S6D0154_H_WINDOW_START_REG);
    WriteData(0x0000);

    //
    // Reset the Y extent to the full screen
    //
    WriteCommand(S6D0154_V_WINDOW_END_REG);
    WriteData(0x013f);
    WriteCommand(S6D0154_V_WINDOW_START_REG);
    WriteData(0x0000);

}

//*****************************************************************************
//
//! Translates a 24-bit RGB color to a display driver-specific color.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param ulValue is the 24-bit RGB color.  The least-significant byte is the
//! blue channel, the next byte is the green channel, and the third byte is the
//! red channel.
//!
//! This function translates a 24-bit RGB color into a value that can be
//! written into the display's frame buffer in order to reproduce that color,
//! or the closest possible approximation of that color.
//!
//! \return Returns the display-driver specific color.
//
//*****************************************************************************
static unsigned long
Kitronix240x320x16_SD60154ColorTranslate(void *pvDisplayData,
                                        unsigned long ulValue)
{
    //
    // Translate from a 24-bit RGB color to a 5-6-5 RGB color.
    //
    return(DPYCOLORTRANSLATE(ulValue));
}

//*****************************************************************************
//
//! Flushes any cached drawing operations.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//!
//! This functions flushes any cached drawing operations to the display.  This
//! is useful when a local frame buffer is used for drawing operations, and the
//! flush would copy the local frame buffer to the display.  For the SSD2119
//! driver, the flush is a no operation.
//!
//! \return None.
//
//*****************************************************************************
static void
Kitronix240x320x16_SD60154Flush(void *pvDisplayData)
{
    //
    // There is nothing to be done.
    //
}

//*****************************************************************************
//
//! The display structure that describes the driver for the Kitronix
//! K350QVG-V1-F TFT panel with an SSD2119 controller.
//
//*****************************************************************************
const tDisplay g_sKitronix320x240x16_SSD2119 =
{
    sizeof(tDisplay),
    0,
#if defined(PORTRAIT) || defined(PORTRAIT_FLIP)
    320,
    240,
#else
    240,
    320,
#endif
    Kitronix240x320x16_SD60154PixelDraw,
    Kitronix240x320x16_SD60154PixelDrawMultiple,
    Kitronix240x320x16_SD60154LineDrawH,
    Kitronix240x320x16_SD60154LineDrawV,
    Kitronix240x320x16_SD60154RectFill,
    Kitronix240x320x16_SD60154ColorTranslate,
    Kitronix240x320x16_SD60154Flush
};

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
