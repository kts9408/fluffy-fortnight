#ifndef WIN32_FLUFFYFORTNIGHT
#define WIN32_FLUFFYFORTNIGHT

/******************************************************************************
 * 
 *****************************************************************************/

// External Dependencies
#include <Windows.h>
#include <stdint.h>
#include "..\includes\FluffyFortnight.h"

/******************************************************************************
 * CONST
 *****************************************************************************/
const int DEFAULT_GFX_BUFFER_WIDTH = 1280;
const int DEFAULT_GFX_BUFFER_HEIGHT = 720;


/******************************************************************************
 * STRUCTS
 *****************************************************************************/

// Struct representing a screen buffer used by the Win32 platform
struct win32_GfxBuffer {
    BITMAPINFO      info;           // struct used by Win32 API containing color and dimention data of the Device Independant Bitmap (DIB)
    void*           memory;        // memory to store raw bitmap data
    int             width;          // width of the buffer
    int             height;         // height of the buffer
    int             channelCount;   // number of color channels per pixel (RGBA)
    int             pitch;          // offset of bytes per line    

};

#endif