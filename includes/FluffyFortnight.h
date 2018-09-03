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

struct game_GfxBuffer {
    void*           memory;
    int             width;
    int             height;
    int             channelCount;
    int             pitch;
};

/******************************************************************************
 * Public Functions
 *****************************************************************************/

/******************************************************************************
 * The game_Render function takes a reference to a game_GfxBuffer struct and 
 * renders the game screen to it.
 * @param gfxBuffer - The game_GfxBuffer struct to modify.
 *****************************************************************************/
typedef void (*game_Render)(game_GfxBuffer *gfxBuffer);
#endif