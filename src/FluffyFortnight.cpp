#include "../includes/FluffyFortnight.h"

namespace {
    
    game_Memory*        gameMemory;
    game_State          gameState;
    /**************************************************************************
    * Internal Methods 
    **************************************************************************/

   /***************************************************************************
    * Forward Declarations
    **************************************************************************/
    void renderTestGradient(game_GfxBuffer* gfxBuffer);
    inline uint32_t roundFloatToUInt(float value);
    inline int32_t roundFloatToInt(float value);
    void drawRect(
        float x0, float y0,     // coordinates of the top left corner
        float x1, float y1,     // coordinates of the botom right corner
        game_Color* color,
        game_GfxBuffer* buffer
    );
    void drawTileMap(
        uint8_t map[][16],      // 2-D array of integers representing the pallettized map
        game_GfxBuffer* out     // the graphics buffer to write to.
    );
    void drawPlayer(
        float x,
        float y,
        game_GfxBuffer* out
    );
    void init(game_Memory* memory);
    void XAudio2TestSound(game_SoundBuffer* soundBuffer);
    void inputTest(game_ControllerInput* controller);
    void parseInput(game_ControllerInput* controller);
   


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
     * 
     *************************************************************************/
    inline int32_t roundFloatToInt(float value) {
        return (int32_t)(value + 0.5f);
    }
    /**************************************************************************
     * 
     *************************************************************************/
    inline uint32_t roundFloatToUInt(float value) {
        return (uint32_t)(value + 0.5f);
    }

    /**************************************************************************
     * 
     *************************************************************************/ 
    void drawPlayer(
        float x,
        float y,
        game_GfxBuffer* out
    ) {
        game_Color color = { 0.0f, 0.25f, 1.0f, 1.0f };
        
        float playerWidth     = 30.0f;
        float playerHeight    = 60.0f;
        drawRect(
            x, y,
            (x + playerWidth),
            (y + playerHeight),
            &color, out
        );

    }

    /**************************************************************************
     * 
     *************************************************************************/
    void drawTileMap(
        uint8_t map[][16],
        game_GfxBuffer* out 
    ) {
        game_Color color;
        int32_t sizeI = 16;
        int32_t sizeJ = 9;
        int32_t tileWidth = 120;
        int32_t tileHeight = 120;
        for(int32_t j = 0; j < sizeJ; j++) {
            for(int32_t i = 0; i < sizeI; i++) {
                switch(map[j][i]) {
                case 0: {
                    color = { 0.5f, 0.5f, 0.5f, 0.5f };
                } break;
                case 1: {
                    color = { 1.0f, 1.0f, 1.0f, 1.0f };
                } break;
                
                }
                float x0 = (float)(i * tileWidth);
                float y0 = (float)(j * tileHeight);
                float x1 = (float)(i * tileWidth) + tileWidth + 1;
                float y1 = (float)(j * tileWidth) + tileWidth + 1;
                drawRect(
                    x0, y0,
                    x1, y1,
                    &color, out
                );
            }
        }
    }

    /**************************************************************************
     * 
     *************************************************************************/
    void drawRect(
        float x0, float y0,     // coordinates of the top left corner
        float x1, float y1,     // coordinates of the botom right corner
        game_Color* color,
        game_GfxBuffer* buffer
    ) {
        // cast color to int values
        uint32_t pixelColor = (uint32_t)(
            (roundFloatToUInt(color->red * 255.0f) << 16) |
            (roundFloatToUInt(color->green * 255.0f) << 8) |
            (roundFloatToUInt(color->blue * 255.0f) << 0)
        );
        // cast coordinates to int values
        int32_t intX0  = roundFloatToInt(x0);
        int32_t intX1  = roundFloatToInt(x1);
        int32_t intY0  = roundFloatToInt(y0);
        int32_t intY1  = roundFloatToInt(y1);
        // force the edges of the screen
        intX0 = (intX0 < 0) ? 0 : intX0;
        intX1 = (intX1 > buffer->width) ? buffer->width : intX1;
        intY0 = (intY0 < 0) ? 0 : intY0;
        intY1 = (intY1 > buffer->height) ? buffer->height : intY1;

        uint8_t* row = (uint8_t*)buffer->memory;
        row += intY0 * buffer->pitch;
        row += intX0 * buffer->channelCount;
        /**
         * uint8_t* row    = (
            ((uint8_t*)buffer->memory) +
            (intX0 * buffer->channelCount +
            intY0 * buffer->pitch)
        );
        **/

        for(int j = intY0; j < intY1; j++) {
            uint32_t* pixel     = (uint32_t*)row;
            for(int i = intX0; i < intX1; i++) {
                *pixel++ = pixelColor;
            }
            row += buffer->pitch;
        }
    }

    /**************************************************************************
     * Generate a test sound wave formatted for XAudio2 (Win32 Audio Engine)
     *************************************************************************/
    void XAudio2TestSound(game_SoundBuffer* soundBuffer) {   
        uint16_t toneVolume         = 3000;
        int toneHz                  = 256;
        int wavePeriod              = (soundBuffer->samplesPerSecond) / toneHz;
        uint16_t* out               = (uint16_t*)(soundBuffer->samples);            // XAudio2 Test Sound
        float t                     = 0.0f;

        // Fill the sound buffer with a sine wave.
        for(int i = 0; i < soundBuffer->sampleCount; i++) {
            float sineValue         = sinf(t);
            int16_t sampleValue     = (int16_t)(sineValue * toneVolume);
            *out++ = sampleValue;
			*out++ = sampleValue;
            t += 2.0f * PI32 / (float)wavePeriod;
        }

    }

    /**************************************************************************
     * Initialize the Game Memory
     *************************************************************************/
    void init(game_Memory* memory) {


        // TODO: Proper Stack Allocator
        gameMemory                              = {};
        gameMemory->persistentStorageSize       = MEGABYTES(64);
        gameMemory->tempStorageSize             = GIGABYTES(4);
        gameMemory->persistentStorage           = (uint8_t*)memory;
        gameMemory->persistentStorageHead       = (uint8_t*)memory;
        gameMemory->tempStorageHead             = (uint8_t*)memory + (gameMemory->persistentStorageSize);
        gameMemory->tempStorage                 = (uint8_t*)memory + (gameMemory->persistentStorageSize);   // point set pointer to the beginning of temp space
    


        gameState.t = (float*)(gameMemory->persistentStorage);
        *(gameState.t) = 0.0f;
        ++*(float*)(gameMemory->persistentStorage);
        gameMemory->persistentStorageSize -= sizeof(float*);

        gameState.playerX = (float*)(gameMemory->persistentStorage);
        *(gameState.playerX) = 0.0f;
        ++*(float*)(gameMemory->persistentStorage);
        gameMemory->persistentStorageSize -= sizeof(float*);

        gameState.playerY = (float*)(gameMemory->persistentStorage);
        *(gameState.playerY) = 0.0f;
        ++*(float*)(gameMemory->persistentStorage);
        gameMemory->persistentStorageSize -= sizeof(float*);

        gameState.inputContext = (uint16_t*)(gameMemory->persistentStorage);
        *(gameState.inputContext) = 0;
        ++*(uint16_t*)(gameMemory->persistentStorage);
        gameMemory->persistentStorageSize -= sizeof(uint16_t*);

        //gameMemory->persistentStorage            = (float*)(gameMemory->persistentStorage);
        


        
        gameMemory->isInitialized               = true;
        // TODO: implement memory management
    }
    /**************************************************************************
     * 
     *************************************************************************/ 
    void inputTest(game_ControllerInput* controller) {
        if(controller->isAnalog) {
            // TODO: Analog stick processing
        } else {
            // TODO: D-Pad Processing
        }

    }
    /**************************************************************************
     * 
     *************************************************************************/
    void parseInput(game_ControllerInput* controller) {
        if(controller->isAnalog) {
            // TODO: Analog stick processing
        } else {
            // TODO: Digital Input Processing
            for(int i = 0; i < MAX_BUTTON_COUNT; i++) {

            }
        }
        
    }

}
/******************************************************************************
 * Public Methods
 *****************************************************************************/
extern "C" GAME_RENDER_GFX(renderGameGfx) {
    renderTestGradient(gfxBuffer);
    drawTileMap(LEVEL1, gfxBuffer);
    drawPlayer(
        *(gameState.playerX),
        *(gameState.playerY),
        gfxBuffer
    );
}

/******************************************************************************
 * 
 *****************************************************************************/ 
extern "C" GAME_INIT(initGame) {
    init(memory);
}

/******************************************************************************
 * 
 *****************************************************************************/
extern "C" GAME_RENDER_AUDIO(renderGameAudio) {
    XAudio2TestSound(soundBuffer);
}

/******************************************************************************
 * 
 *****************************************************************************/
extern "C" GAME_UPDATE(updateGame) {
    parseInput(controllerInput);
    inputTest(controllerInput);
}
