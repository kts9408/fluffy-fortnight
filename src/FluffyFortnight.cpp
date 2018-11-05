#include "../includes/FluffyFortnight.h"

namespace {
    
    game_Memory*        gameMemory;
    game_State*         gameState;
    /**************************************************************************
    * Internal Methods 
    **************************************************************************/

   /***************************************************************************
    * Forward Declarations
    **************************************************************************/
    void renderTestGradient(game_GfxBuffer* gfxBuffer);
    void init(void* memory);
    void XAudio2TestSound(game_SoundBuffer* soundBuffer, float t);
   


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
     * Generate a test sound wave formatted for XAudio2 (Win32 Audio Engine)
     *************************************************************************/
    void XAudio2TestSound(game_SoundBuffer* soundBuffer, float t) {
        soundBuffer->isInitialized      = true;
        soundBuffer->samplesPerSecond   = 48000;
        uint32_t toneVolume             = 3000;
        int toneHz                      = 256;
        int wavePeriod                  = (soundBuffer->samplesPerSecond) / toneHz;
        soundBuffer->sampleCount        = 48000 * 2;
        uint32_t* out                   = (uint32_t*)(soundBuffer->samples);            // XAudio2 Test Sound

        // Fill the sound buffer with a sine wave.
        for(int sampleIndex = 0; sampleIndex < soundBuffer->sampleCount; sampleIndex++) {
            float sineValue         = sinf(t);
            int32_t sampleValue     = (int32_t)(sineValue * toneVolume);
            *out++                  = sampleValue;
            t                       += 2.0f * PI32 / (float)wavePeriod;
        }

    }

    /**************************************************************************
     * Initialize the Game Memory
     *************************************************************************/
    void init(void* memory) {
        gameMemory                              = {};
        gameMemory->permanentStorageSize        = MEGABYTES(64LL);
        gameMemory->tempStorageSize             = GIGABYTES(4LL);
        gameMemory->permanentStorage            = memory;
        gameMemory->permanentStorageHead        = (uint8_t*)memory;
        gameMemory->tempStorageHead             = (uint8_t*)memory + (gameMemory->permanentStorageSize);
        gameMemory->tempStorage                 = (uint8_t*)memory + (gameMemory->permanentStorageSize);   // point set pointer to the beginning of temp space
        gameState->t                            = (float*)(gameMemory->permanentStorage);
        gameMemory->permanentStorage            = (float*)(gameMemory->permanentStorage) + 1;


        
        gameMemory->isInitialized               = true;
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
    XAudio2TestSound(soundBuffer, *gameState->t);
}
