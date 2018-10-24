#ifndef FLUFFYFORTNIGHT_H
#define FLUFFYFORTNIGHT_H
/******************************************************************************
 * PROTOTYPE
 * 1    - Prototype build
 * 0    - Release build
 *****************************************************************************/
#define PROTOTYPE 1

/******************************************************************************
 * 
 *****************************************************************************/

// External Dependencies
#include <stdint.h>

/******************************************************************************
 * MACROS
 *****************************************************************************/
#define KILOBYTES(value) ((value) * 1024LL)
#define MEGABYTES(value) (KILOBYTES(value) * 1024LL)
#define GIGABYTES(value) (MEGABYTES(value) * 1024LL)
#define ASSERT(exp) \
    if(!(exp)) { *(uint32_t *)0 = 0; }

/******************************************************************************
 * CONST
 *****************************************************************************/
const int DEFAULT_GFX_BUFFER_WIDTH = 1920;
const int DEFAULT_GFX_BUFFER_HEIGHT = 1080;



/******************************************************************************
 * STRUCTS
 *****************************************************************************/
struct game_State {
    void*     win32ReadFromDisk;      
    void*     win32WriteToDisk;
};

struct game_Memory {
    bool            isInitialized;                  
    uint64_t        permanentStorageSize;           // size of the persistant partition
    game_State*     permanentStorage;               // persistant image i.e. the game state
    uint64_t        tempStorageSize;                // size of the temporary partition
    void*           tempStorage;                    // temporary image
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
// PROTOTYPING CODE
#if PROTOTYPE
    void* win32_ReadFromDisk(char* filename);
    bool win32_WriteToDisk(char* filename, uint32_t memorySize, void* memory);
    void win32_FreeFileMemory(void* memory);
#endif

#define GAME_RENDER(name) void name(game_GfxBuffer *gfxBuffer)
#define GAME_INIT(name) void name(void *memory)
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