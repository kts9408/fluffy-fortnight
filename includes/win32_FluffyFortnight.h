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

// XInput Support
#include <XInput.h>
#define XINPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
typedef XINPUT_SET_STATE(xinput_set_state);     // function pointer support
#define XInputSetState _XInputSetState      // redefine api function name to pointer
#define XINPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState)
typedef XINPUT_GET_STATE(xinput_get_state);
#define XInputGetState _XInputGetState


#include "..\includes\FluffyFortnight.h"


/******************************************************************************
 * CONST
 *****************************************************************************/
const uint32_t XINPUT_BITMASKS[] = {
    XINPUT_GAMEPAD_Y,
    XINPUT_GAMEPAD_A,
    XINPUT_GAMEPAD_X,
    XINPUT_GAMEPAD_B,
    XINPUT_GAMEPAD_START,
    XINPUT_GAMEPAD_BACK,
    XINPUT_GAMEPAD_LEFT_SHOULDER,
    XINPUT_GAMEPAD_RIGHT_SHOULDER,
    XINPUT_GAMEPAD_DPAD_UP,
    XINPUT_GAMEPAD_DPAD_DOWN,
    XINPUT_GAMEPAD_DPAD_LEFT,
    XINPUT_GAMEPAD_DPAD_RIGHT
};


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
    game_RenderAudio*   gameRenderAudio;
    game_Init*          gameInit;       // initalize memory function
    game_Update*        gameUpdate;
    bool                isValid;        // struct initialized
};

// Struct encapsulating the dimensions of the window
struct win32_WindowDimension {
    int     width;
    int     height;
};

struct win32_XInputCode {
    xinput_set_state*       _XInputSetState;
    xinput_get_state*       _XInputGetState;
};

typedef struct ThreadContext {
    int     PlaceHolder;
} ThreadContext;

/******************************************************************************
 * External Bindings - For Prototyping Purposes
 *****************************************************************************/

/******************************************************************************
 * Public Functions
 *****************************************************************************/
#if PROTOTYPE == 1
typedef struct DebugReadFileResult {
    uint32_t    ContentSize;
    void*       Contents;
} DebugReadFileResult;

#define DEBUG_FREE_FILE_MEMORY(name) void name(ThreadContext* thread, void* memory)


#define DEBUG_READ_FROM_DISK(name) uint16_t name(ThreadContext* thread, char* filename, DebugReadFileResult* out)
typedef uint16_t (*Win32_ReadFromDisk)(ThreadContext* thread, char* filename, DebugReadFileResult* out);
#define DEBUG_WRITE_TO_DISK(name) bool name(ThreadContext* thread, char* filename, uint32_t memorySize, void* memory)

#endif

#endif