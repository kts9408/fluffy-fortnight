#ifndef WIN32_FLUFFYFORTNIGHT_H
#define WIN32_FLUFFYFORTNIGHT_H

/******************************************************************************
 * 
 *****************************************************************************/

// External Dependencies
#include <Windows.h>
#include "..\includes\FluffyFortnight.h"

/******************************************************************************
 * CONST
 *****************************************************************************/


/******************************************************************************
 * STRUCTS
 *****************************************************************************/

// Struct representing a screen buffer used by the Win32 platform
struct win32_GfxBuffer {
    BITMAPINFO      info;           // struct used by Win32 API containing color and dimention data of the Device Independant Bitmap (DIB)
    void*           memory;         // memory to store raw bitmap data
    int             channelCount;   // number of color channels per pixel (RGBA)
    int             pitch;          // offset of bytes per line    
};

// Struct encapsulating the game code handle
struct win32_GameCode {
    FILETIME        dllTimeStamp;   // timestamp of last write time
    HMODULE         dllGameCode;    // game library
    game_Render*     gameRender;     // render function
    bool            isValid;        // struct initialized
};

// Struct encapsulating the dimensions of the window
struct win32_WindowDimension {
    int             width;
    int             height;
};

#endif