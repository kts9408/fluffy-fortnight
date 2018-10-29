#include "../includes/FluffyFortnight.h"

namespace {
    
    game_Memory*        gameMemory;
    /**************************************************************************
    * Internal Methods 
    **************************************************************************/

   /***************************************************************************
    * Forward Declarations
    **************************************************************************/
    void renderTestGradient(game_GfxBuffer* gfxBuffer);
    void init(void* memory);
    


   // End of Forward Declarations

    /**************************************************************************
     * Draws a Test gradent to the Graphics Buffer
     *************************************************************************/
    void renderTestGradient(game_GfxBuffer* gfxBuffer) {
        int height = gfxBuffer->height;
        int width = gfxBuffer->width;
        int pitch = gfxBuffer->pitch;

        uint8_t* row = (uint8_t*)gfxBuffer->memory;         // point the row counter to the beginning of the buffer

        for(int j = 0; j < height; ++j) {
            uint32_t* pixel = (uint32_t*)row;               // point the pixel counter at the beginning of the row
            for(int i = 0; i < width; ++i) {
                uint8_t blue = (uint8_t)(i);                // set the B channel to the x coordinate
                uint8_t green = (uint8_t)(j);               // set the G channel to the y coordinate
                *pixel++ = ((green << 0x10) | blue);        // set the pixel data bitwise shift the R and A channels (not used)
            }

            row += pitch;                                   // point the row counter to the next row
        }

    }
    /**************************************************************************
     * Initialize the Game Memory
     *************************************************************************/
    void game_Init(void* memory) {
        gameMemory = (game_Memory*)memory;
        gameMemory->permanentStorageSize = MEGABYTES(64LL);
        gameMemory->tempStorageSize = GIGABYTES((uint64_t)4);
        gameMemory->permanentStorage = {};                      // initialize state
        gameMemory->tempStorage = (uint8_t*) + MEGABYTES(64);   // point set pointer to the beginning of temp space
        gameMemory->isInitialized = true;
        // TODO: implement memory management
    }

}
/******************************************************************************
 * Public Methods
 *****************************************************************************/
extern "C" GAME_RENDER_GFX(renderGameGfx) {
    renderTestGradient(gfxBuffer);                          // renders a test gradient
}

extern "C" GAME_INIT(initGame) {
    init(memory);
}

extern "C" GAME_RENDER_AUDIO(renderGameAudio) {
    
}
