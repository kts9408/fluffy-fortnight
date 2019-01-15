#include "../includes/FluffyFortnight.h"

namespace {
    
    game_Memory         gameMemory = { };
    game_State          gameState;

    game_TileMap Level[4] = { 0 };

    

    // TODO: Incorporate into player struct
    float playerWidth   = 30.0f;
    float playerHeight  = 60.0f;
	 

    /**************************************************************************
    * Internal Methods 
    **************************************************************************/

   /***************************************************************************
    * Forward Declarations
    **************************************************************************/
    void renderTestGradient(game_GfxBuffer* gfxBuffer);
    inline unsigned int roundFloatToUInt(float value);
    inline int roundFloatToInt(float value);
    void drawRect(
        float x0, float y0,     // coordinates of the top left corner
        float x1, float y1,     // coordinates of the botom right corner
        game_Color* color,
        game_GfxBuffer* buffer
    );
    void drawTileMap(
        game_TileMap* map,      // 2-D array of integers representing the pallettized map
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
    
    /**************************************************************************
     * 
     *************************************************************************/
    inline int roundFloatToInt(float value) {
        return (int)(value + 0.5f);
    }
    /**************************************************************************
     * 
     *************************************************************************/
    inline unsigned int roundFloatToUInt(float value) {
        return (unsigned int)(value + 0.5f);
    }

    /**************************************************************************
     * 
     *************************************************************************/ 
    inline int truncateFloatToInt(float value) {
        return (int)(value);
    }

    /**************************************************************************
     * 
     *************************************************************************/ 
    inline unsigned int truncateFloatToUInt(float value) {
        return (unsigned int)(value);
    }

    /**************************************************************************
     * 
     *************************************************************************/
    void normalizeTilePosition(
        uint32_t gamePostition,
        float tilePosition,
        game_TileMap* map
    ) {
        uint32_t result;
        uint16_t mapPosition = gamePostition & 0xFFFF;

        if(tilePosition > 1.0f) {
            mapPosition++;
        } else if(tilePosition < 0.0f) {
            mapPosition--;
        }
        result = 


        
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
        game_TileMap* map,
        game_GfxBuffer* out 
    ) {
        game_Color color;
        game_Color highlight = { 0.75f, 0.75f, 0.75f };
        int playerTileX   = truncateFloatToInt(*(gameState.playerX) - map->offsetX) / (int)map->tileWidth;
        int playerTileY   = truncateFloatToInt(*(gameState.playerY) - map->offsetY) / (int)map->tileHeight;    

        for(int32_t j = 0; j < map->CountY; j++) {
            for(int32_t i = 0; i < map->CountX; i++) {
                switch(map->data[j * map->CountX + i]) {
                case 0: {
                    color = { 0.5f, 0.5f, 0.5f, 0.5f };
                } break;
                case 1: {
                    color = { 1.0f, 1.0f, 1.0f, 1.0f };
                } break;
                case 2: {
                    color = { 0.145f, 0.913f, 0.247f };
                } break;
                case 3: {
                    color = { 0.913f, 0.145f, 0.909f }; 
                } break;
                
                }
                
                float x0 = (float)(i * map->tileWidth);
                float y0 = (float)(j * map->tileHeight);
                float x1 = (float)(i * map->tileWidth) + map->tileWidth + 1;
                float y1 = (float)(j * map->tileWidth) + map->tileWidth + 1;
                drawRect(
                    x0, y0,
                    x1, y1,
                    &color, out
                );
            }
        }
        drawRect(
            (float)playerTileX * map->tileWidth,
            (float)playerTileY * map->tileHeight,
            (float)(playerTileX + 1) * map->tileWidth,
            (float)(playerTileY + 1) * map->tileHeight,
            &highlight, out
        );

        drawPlayer(
            *(gameState.playerX),
            *(gameState.playerY),
            out
    );
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
    void init(void* memory) {

        // TODO: Proper Stack Allocator
        gameMemory.persistentStorageSize       = MEGABYTES(64);
        gameMemory.tempStorageSize             = GIGABYTES(4);
        gameMemory.persistentStorageHead       = (uint8_t*)memory;
        gameMemory.persistentStorage           = (uint8_t*)gameMemory.persistentStorageHead;
        gameMemory.tempStorageHead             = (uint8_t*)gameMemory.persistentStorageHead + gameMemory.persistentStorageSize;
        gameMemory.tempStorage                 = (uint8_t*)gameMemory.tempStorageHead;   // point set pointer to the beginning of temp space
    


        gameState.t = (float*)(gameMemory.persistentStorage);
        *(gameState.t) = 0.0f;
        gameMemory.persistentStorage = (float*)gameMemory.persistentStorage + 1;
        gameMemory.persistentStorageSize -= sizeof(float);

        gameState.playerX = (float*)(gameMemory.persistentStorage);
        *(gameState.playerX) = 0.0f;
        gameMemory.persistentStorage = (float*)gameMemory.persistentStorage + 1;
        gameMemory.persistentStorageSize -= sizeof(float);

        gameState.playerY = (float*)(gameMemory.persistentStorage);
        *(gameState.playerY) = 0.0f;
        gameMemory.persistentStorage = (float*)gameMemory.persistentStorage + 1;
        gameMemory.persistentStorageSize -= sizeof(float);

        gameState.inputContext = (uint16_t*)(gameMemory.persistentStorage);
        *(gameState.inputContext) = 0;
        gameMemory.persistentStorage = (uint16_t*)gameMemory.persistentStorage + 1;
        gameMemory.persistentStorageSize -= sizeof(uint16_t);

        gameState.currentMap = (uint16_t*)(gameMemory.persistentStorage);
        *(gameState.currentMap) = 0;
        gameMemory.persistentStorage = (uint16_t*)gameMemory.persistentStorage + 1;
        gameMemory.persistentStorageSize -= sizeof(uint16_t);

    /* TODO: DEBUG THIS!
        gameState.Level[0] = (game_TileMap*)(gameMemory.tempStorage);
        gameMemory.tempStorage = (game_TileMap*)gameMemory.tempStorage + 1;
        gameMemory.tempStorageSize -= sizeof(game_TileMap);
        *(gameState.Level[0]) = {
            0.0f,
            0.0f,
            16,
            9,
            120,
            120,
            (uint8_t*)&TILE_DATA0
        };
        
        gameState.Level[1] = (game_TileMap*)(gameMemory.tempStorage);
        gameMemory.tempStorage = (game_TileMap*)gameMemory.tempStorage + 1;
        gameMemory.tempStorageSize -= sizeof(game_TileMap);
        *(gameState.Level[1]) = {
            0.0f,
            0.0f,
            16,
            9,
            120,
            120,
            (uint8_t*)&TILE_DATA1
        };
*/
        gameMemory.isInitialized = true;
        // TODO: implement memory management
    }



    /**************************************************************************
     * 
     *************************************************************************/ 
    bool tileCollision(
        float testX,
        float testY,
        game_TileMap* map
    ) {
        int playerTileX   = truncateFloatToInt(testX - map->offsetX) / (int)map->tileWidth;
        int playerTileY   = truncateFloatToInt(testY - map->offsetY) / (int)map->tileHeight;        
        bool result = false;
        unsigned int tileValue;
    
        if  ((playerTileX >= 0 && playerTileX < map->CountX) &&
            (playerTileY >= 0 && playerTileY < map->CountY)) {
            tileValue = map->data[playerTileY * map->CountX + playerTileX];
            // TODO: Change transition tiles to a z coordinate shift
            switch(tileValue) {
            case 0: {
                result = true;
            } break;
            case 1: {
                result = false;
            } break;
            case 2: {
                (*gameState.currentMap) = 1;
                result = true;
            } break;
            case 3: {
                (*gameState.currentMap) = 0;
                result = true;
            } break;
            }
        }

        return result;
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
    void resetControllerStateCounter(game_ControllerInput* controller) {
        for(char i = 0; i < MAX_BUTTON_COUNT; i++) {
            controller->Buttons[i].stateChangeCount = 0;
        }

    }
    /**************************************************************************
     * 
     *************************************************************************/
    void parseInput(game_ControllerInput* controller) {
        float dPlayerX = 0.0f;
        float dPlayerY = 0.0f;
        if(controller->isAnalog) {
            // TODO: Analog stick processing
        } else {
            // TODO: Digital Input Processing
            // TODO: Add player-centric movement (ie map offsets)
            if(controller->East.isDown) {
                dPlayerX += 1.0f;
            } else if (controller->West.isDown ) {
                dPlayerX -= 1.0f;
            }

            if(controller->North.isDown) {
                dPlayerY += 1.0f;
            } else if(controller->South.isDown){
                dPlayerY -= 1.0f;
            }
        }

        dPlayerX *= 15.0f;
        dPlayerY *= 15.0f;
        bool collisionCheck = tileCollision(
            *(gameState.playerX) + dPlayerX - 0.5f*playerWidth,
            *(gameState.playerY) + dPlayerY,
            &Level[*gameState.currentMap]
        ) && 
        tileCollision(
            *(gameState.playerX) + dPlayerX + 0.5f*playerWidth,
            *(gameState.playerY) + dPlayerY,
            &Level[*gameState.currentMap]
        ) &&
        tileCollision(
            *(gameState.playerX) + dPlayerX + playerWidth,
            *(gameState.playerY) + dPlayerY,
            &Level[*gameState.currentMap]
        );


        if(!collisionCheck) {
            dPlayerX = 0.0f;
            dPlayerY = 0.0f;
        }
        
        *(gameState.playerX) += dPlayerX;
        *(gameState.playerY) += dPlayerY;    
    }


}
/******************************************************************************
 * Public Methods
 *****************************************************************************/
extern "C" GAME_RENDER_GFX(renderGameGfx) {
    // TODO: Move this off of the stack and into game memory
    Level[0] = {
        0.0f,
        0.0f,
        16,
        9,
        120,
        120,
        (uint8_t*)&TILE_DATA00
    };
    Level[1] = {
        0.0f,
        0.0f,
        16,
        9,
        120,
        120,
        (uint8_t*)&TILE_DATA01
    };
    Level[2] = {
        0.0f,
        0.0f,
        16,
        9,
        120,
        120,
        (uint8_t*)&TILE_DATA10
    };
    Level[3] = {
        0.0f,
        0.0f,
        16,
        9,
        120,
        120,
        (uint8_t*)&TILE_DATA11
    };
    
    renderTestGradient(gfxBuffer);
    drawTileMap(&Level[*gameState.currentMap], gfxBuffer);

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
        // TODO: Move this off of the stack and into game memory
    Level[0] = {
        0.0f,
        0.0f,
        16,
        9,
        120,
        120,
        (uint8_t*)&TILE_DATA00
    };
    Level[1] = {
        0.0f,
        0.0f,
        16,
        9,
        120,
        120,
        (uint8_t*)&TILE_DATA01
    };
    Level[2] = {
        0.0f,
        0.0f,
        16,
        9,
        120,
        120,
        (uint8_t*)&TILE_DATA10
    };
    Level[3] = {
        0.0f,
        0.0f,
        16,
        9,
        120,
        120,
        (uint8_t*)&TILE_DATA11
    };
    parseInput(controllerInput);
    inputTest(controllerInput);
    // TODO: Move once this starts polling more than once per frame
    resetControllerStateCounter(controllerInput);
    
}
