#ifndef FLUFFYFORTNIGHT_H
#define FLUFFYFORTNIGHT_H

/******************************************************************************
 * 
 *****************************************************************************/

// External Dependencies
#include <stdint.h>


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
    uint16_t        cursorX;
    uint16_t        cursorY;
    
    uint64_t        permanentStorageSize;
    void*           permanentStorage;
    uint64_t        tempStorageSize;
    void*           tempStorage;

    // TODO: add handles for file I/O
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
// TODO: Add additional bindings here

/******************************************************************************
 * The game_render function takes a reference to a game_GfxBuffer struct and 
 * renders the game screen to it.
 * @param gfxBuffer - The game_GfxBuffer struct to modify.
 *****************************************************************************/
typedef GAME_RENDER(game_render);

/******************************************************************************
 * Public Functions
 *****************************************************************************/



#endif