#ifndef FLUFFYFORTNIGHT_H
#define FLUFFYFORTNIGHT_H

/******************************************************************************
 * 
 *****************************************************************************/

// External Dependencies
#include <stdint.h>

/******************************************************************************
 * MACROS
 *****************************************************************************/
#define KILOBYTES(value) ((value) * 1024)
#define MEGABYTES(value) (KILOBYTES(value) * 1024)
#define GIGABYTES(value) (MEGABYTES(value) * 1024)

/******************************************************************************
 * CONST
 *****************************************************************************/
const int DEFAULT_GFX_BUFFER_WIDTH = 1280;
const int DEFAULT_GFX_BUFFER_HEIGHT = 720;



/******************************************************************************
 * STRUCTS
 *****************************************************************************/

struct game_Memory {
    bool            isInitialized;                  
    uint64_t        permanentStorageSize;           // size of the persistant partition
    game_State*     permanentStorage;               // persistant image
    uint64_t        tempStorageSize;                // size of the temporary partition
    void*           tempStorage;                    // temporary image
};

struct game_State {
    // win32_ReadFromDisk*     win32ReadFromDisk;      
    // win32_WriteToDisk*      win32WriteToDisk;
};

struct game_GfxBuffer {
    void*           memory;
    int             width;
    int             height;
    int             channelCount;
    int             pitch;
};


struct game_Thread {
    uint32_t*       threadId;
    void*           threadCallBack;
};
/******************************************************************************
 * External Bindings
 *****************************************************************************/
#define GAME_RENDER(name) void name(game_GfxBuffer *gfxBuffer)
#define GAME_INIT(name) void name(game_Memory *memory)
// TODO: Add additional bindings here

/******************************************************************************
 * The game_render function takes a reference to a game_GfxBuffer struct and 
 * renders the game screen to it.
 * @param gfxBuffer - The game_GfxBuffer struct to modify.
 *****************************************************************************/
typedef GAME_RENDER(game_Render);
typedef GAME_INIT(game_Init);
/******************************************************************************
 * Public Functions
 *****************************************************************************/



#endif