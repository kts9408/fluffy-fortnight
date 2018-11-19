#ifndef WIN32_FLUFFYFORTNIGHT_H
#define WIN32_FLUFFYFORTNIGHT_H

/******************************************************************************
 * 
 *****************************************************************************/

// External Dependencies
#include <Windows.h>

// XAudio2 Support
#include <XAudio2.h>
#define XAUDIO2_CREATE(name) HRESULT WINAPI name(IXAudio2 **ppXAudio2, UINT32 Flags, XAUDIO2_PROCESSOR XAudio2Processor)
typedef XAUDIO2_CREATE(xaudio_Create);

#include "..\includes\FluffyFortnight.h"

/******************************************************************************
 * CONST
 *****************************************************************************/
#define     DELAY_THREAD_START 0x00000004

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

// Strut representing XAudio2
struct win32_SoundEngine {
    WAVEFORMATEX                    waveFormat;
    XAUDIO2_BUFFER                  soundBuffer;    // buffer used by audio engine
    IXAudio2*                       xAudio;         // Pointer to audio Engine
    IXAudio2MasteringVoice*         masterVoice;    // Pointer to master voice
    IXAudio2SourceVoice*            sourceVoice;
};

// Struct encapsulating the game code handle
struct win32_GameCode {
    FILETIME            dllTimeStamp;   // timestamp of last write time
    HMODULE             dllGameCode;    // game library
    game_RenderGfx*     gameRenderGfx;     // render function
    game_RenderAudio*   gameRenderAudio;
    game_Init*          gameInit;       // initalize memory function
    bool                isValid;        // struct initialized
};

// Struct encapsulating the dimensions of the window
struct win32_WindowDimension {
    int             width;
    int             height;
};


/******************************************************************************
 * External Bindings - For Prototyping Purposes
 *****************************************************************************/

/******************************************************************************
 * Public Functions
 *****************************************************************************/

#endif