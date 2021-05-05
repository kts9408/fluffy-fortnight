#include "../includes/FluffyFortnight.h"

namespace {
    
    game_Memory         gameMemory = { };
    game_State          gameState;

    // TODO: Move Stack Allocation into Game Memory
    game_TileMap        Map = { 0 };
    game_TilePage       tempMap[9];
    game_TilePage*      tilePageFrame;

    // TODO: Incorporate this into a struct.
    float playerX = 0.0f;
    float playerY = 0.0f;

    // TODO: Incorporate this into the scaling components of a transform matrix
    float unitsPerTile = 30.0f;
    float pixelsPerUnit = 30.0f;
    

    // TODO: Incorporate into player struct
    float playerWidth   = 30.0f;
    float playerHeight  = 30.0f;
    
	 

    /**************************************************************************
    * Internal Methods 
    **************************************************************************/

   /***************************************************************************
    * Forward Declarations
    **************************************************************************/

    void drawRect(
        float x0, float y0,     // coordinates of the top left corner
        float x1, float y1,     // coordinates of the botom right corner
        game_Color* color,
        game_GfxBuffer* buffer
    );
    void drawTilePage(
        game_TileMap* page,      // 2-D array of integers representing the pallettized map
        game_GfxBuffer* out     // the graphics buffer to write to.
    );
    void drawPlayer(
        float playerX,
        float playerY,
        game_GfxBuffer* out
    );

    void fixedDrawPlayer(
        game_GfxBuffer* out
    );
    void init(game_Memory* memory);
    void XAudio2TestSound(game_SoundBuffer* soundBuffer);
    void inputTest(game_ControllerInput* controller);
    void parseInput(game_ControllerInput* controller);
    void parseInput(
        game_ControllerInput* controller,
        game_WorkingPosition* playerPosition,
        game_TileMap* map
    );


    bool tileCollision(
        int testTileX,
        int testTileY,
        game_WorkingPosition* playerPosition,
        game_TileMap* map
    );
   


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
     * This function takes in the Player Position (in UNITS) and returns the
     * Tile Index and the fractional sub-tile position.
     *************************************************************************/ 
    inline void calculateTilePosition(
        float playerPositionIn,     // Player Position in Units (Input)
        float unitPerTileIn,        // Number of Units Per Tile (Input)
        int &tileIndexOut,          // Tile Index (Output)
        float &subTilePositionOut   // Fractional sub-tile Position (Output)
    ) {
        tileIndexOut = (int)(playerPositionIn / unitPerTileIn);
        subTilePositionOut = (playerPositionIn / unitPerTileIn);
    }

    /**************************************************************************
     * This function takes in an unpacked WorkingPosition struct and returns a
     * packed UnifiedPosition struct containing the coordinates.
     *************************************************************************/ 
    inline void packWorkingPosition(
        game_WorkingPosition* in,
        game_UnifiedPosition* out
    ) {
        out->TileMapX = ((uint32_t)in->TileMapX) << MAP_LOCATION_SHIFT | in->TilePageX;    
        out->TileMapY = ((uint32_t)in->TileMapY) << MAP_LOCATION_SHIFT | in->TilePageX;
    

        out->TileX = in->TileX;
        out->TileY = in->TileY;
    }
        
    /**************************************************************************
     * This function takes in a packed UnifiedPosition struct and returns an 
     * unpacked WorkingPosition struct containing the proper sub-locations.
     *************************************************************************/
    inline void unpackUnifiedPosition(
        game_UnifiedPosition* in,
        game_WorkingPosition* out
    ) {
        out->TileMapX = (uint16_t)(in->TileMapX >> MAP_LOCATION_SHIFT);
        out->TileMapY = (uint16_t)(in->TileMapY >> MAP_LOCATION_SHIFT);

        out->TilePageX = (uint16_t)in->TileMapX & MAP_LOCATION_MASK;
        out->TilePageY = (uint16_t)in->TileMapY & MAP_LOCATION_MASK;

        out->TileX = in->TileX;
        out->TileY = in->TileY;
    }

    /**************************************************************************
     * Draw the Player in the center of a given buffer.
     * TODO:
     * -implement layers
     * -implement sprites/models
     *************************************************************************/ 
    void fixedDrawPlayer(
        game_GfxBuffer* out
    ) {
        game_Color color = { 0.0f, 0.25f, 1.0f, 1.0f };     // TEMPORARY VALUE
        float x0 = (out->width - playerWidth) / 2.0f;
        float x1 = (out->width + playerWidth) / 2.0f;
        float y0 = (out->height - playerHeight) / 2.0f;
        float y1 = (out->height + playerHeight) / 2.0f;
        drawRect(
            x0, y0,
            x1, y1,
            &color, out
        );

    }
    
    /**************************************************************************
     * Draw the Player at a specific coordinate of a given buffer.
     * TODO:
     * -implement layers
     * -implement sprites/models
     *************************************************************************/ 
    void drawPlayer(
        float playerX,
        float playerY,
        game_GfxBuffer* out
    ) {


    }

    /**************************************************************************
     * Draw a TilePage to the graphics buffer.
     * TODO:
     * -implement layers
     * -implement sprites/decals
     * -implement different preset layouts
     *************************************************************************/
    void drawTilePage(
        game_TilePage* page,
        float tileWidth,
        float tileHeight,
        game_PlayerPosition* playerPosition,
        game_GfxBuffer* out 
    ) {
        game_Color color;
        game_Color highlight = { 0.75f, 0.75f, 0.75f };     // TEMPORARY VALUE

        for(int32_t j = 0; j < page->height; j++) {
            for(int32_t i = 0; i < page->width; i++) {
                switch(page->data[j * page->width + i]) {
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
        drawRect(
            (float)playerPosition->TilePageX * tileWidth,
            (float)playerPosition->TilePageY * tileHeight,
            (float)(playerPosition->TilePageX + 1) * tileWidth,
            (float)(playerPosition->TilePageY + 1) * tileHeight,
            &highlight, out
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
            (roundFloatToUInt(color->red * 255.0f) << 0x10) |
            (roundFloatToUInt(color->green * 255.0f) << 0x8) |
            (roundFloatToUInt(color->blue * 255.0f) << 0x0)
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
            float sineValue         = Sinf(t);
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

        // Initialize the UnifiedPosition struct representing the player's 
        // position on the TileMap
        gameState.playerPosition = (game_UnifiedPosition*)(gameMemory.persistentStorage);
        gameState.playerPosition->TileX = 0.0f;
        gameState.playerPosition->TileMapX = 3;
        gameState.playerPosition->TileY = 0.0f;
        gameState.playerPosition->TileMapY = 3;
        gameMemory.persistentStorage = (game_UnifiedPosition*)gameMemory.persistentStorage + 1;
        gameMemory.persistentStorageSize -= sizeof(game_UnifiedPosition);

        gameState.inputContext = (uint16_t*)(gameMemory.persistentStorage);
        *(gameState.inputContext) = 0;
        gameMemory.persistentStorage = (uint16_t*)gameMemory.persistentStorage + 1;
        gameMemory.persistentStorageSize -= sizeof(uint16_t);

        gameState.currentMap = (uint16_t*)(gameMemory.persistentStorage);
        *(gameState.currentMap) = 0;
        gameMemory.persistentStorage = (uint16_t*)gameMemory.persistentStorage + 1;
        gameMemory.persistentStorageSize -= sizeof(uint16_t);

        gameMemory.isInitialized = true;
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
    void resetControllerStateCounter(game_ControllerInput* controller) {
        for(char i = 0; i < MAX_BUTTON_COUNT; i++) {
            controller->Buttons[i].stateChangeCount = 0;
        }

    }
    /**************************************************************************
     * 
     *************************************************************************/
    void parseInput(
        game_ControllerInput* controller,
        game_PlayerPosition* playerPosition,
        game_TileMap* map
    ) {
        float dPlayerX = 0.0f;
        float dPlayerY = 0.0f;

        float testX = 0.0f;
        float testY = 0.0f;
        int tileIndexX = 0;
        int tileIndexY = 0;
        float tileY = 0.0f;
        float tileX = 0.0f;
        if(controller->isAnalog) {
            // TODO: Analog stick processing
        } else {
            // TODO: Digital Input Processing
            // TODO: Add player-centric movement (ie map offsets)
            controller->East.isDown = true;
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

        // calculate test coordinates in units for collision detection.
        testX = playerPosition->PositionX + dPlayerX + 0.5f * playerWidth; //center the test point horizontally
        testY = playerPosition->PositionY + dPlayerY;

        // calculate tile locations
        calculateTilePosition(
            testX,
            unitsPerTile,
            tileIndexX,
            tileX
        );

        calculateTilePosition(
            testY,
            unitsPerTile,
            tileIndexY,
            tileY
        );      
        
        bool collisionCheck = tileCollision(
            tileIndexX,
            tileIndexY,
            playerPosition,
            map
        ) && 
        tileCollision(
            tileIndexX,
            tileIndexY,
            playerPosition,
            map
        ) &&
        tileCollision(
            roundFloatToInt((playerPosition->TileX) + dPlayerX + playerWidth),
            roundFloatToInt((playerPosition->TileY) + dPlayerY),
            playerPosition,
            map
        );


        if(!collisionCheck) {
            dPlayerX = 0.0f;
            dPlayerY = 0.0f;
        }
        
        playerPosition->TileX += dPlayerX;
        playerPosition->TileY += dPlayerY;    
    }

    /**************************************************************************
     * 
     *************************************************************************/ 
    void renderGameGfx(
        game_GfxBuffer* gfxBuffer,
        game_WorkingPosition* playerPosition,
        game_TileMap* map
    ) {  
        //renderTestGradient(gfxBuffer);
        tilePageFrame = &(map->data[playerPosition->TileMapY * map->width + playerPosition->TileMapX]);
        
        /*
        getTilePageFrame(
            playerPosition,
            &Map,
            tilePageFrame
        );
        */
       
        drawTilePage(
            tilePageFrame,
            map->tileWidth,
            map->tileWidth,
            playerPosition,
            gfxBuffer
        );
        
        //drawPlayer(gfxBuffer);
        fixedDrawPlayer(gfxBuffer);

    }

/**************************************************************************
 * Check a coordinate inside a TilePage to determine if the Tile is
 * passable
 *************************************************************************/ 
bool tileCollision(
    int testTileX,
    int testTileY,
    game_PlayerPosition* playerPosition,
    game_TileMap* map
) {
    int pageIndex = (testTileX * map->width) + playerPosition->PositionX;
    game_TilePage* page = &map->data[pageIndex];
    int tileIndex = (testTileY * page->height) + playerPosition->PositionY;
    // game_Tile* tile = &page->data[tileIndex];

    bool result = false;
    uint8_t tileValue;

    // check test tile X isn't off the current Tile Page
    if(testTileX < 0) {
        page = &map->data[pageIndex - 1];
        testTileX = page->width + testTileX;
    } else if(testTileX > page->width) {
        page = &map->data[pageIndex + 1];
        testTileX = page->width + testTileX % page->width;
    }

    // check test tile Y isn't off the current Tile Page
    if(testTileY < 0) {
        page = &map->data[pageIndex - map->width];
        testTileY = page->height + (testTileY * page->width);
    } else if(testTileX > page->height) {
        page = &map->data[pageIndex + map->width];
        testTileY = page->height + (testTileY % page->height) * map->width;
    }

    tileValue = page->data[testTileY * page->width + testTileX];
    switch(tileValue) {
        case 0: {
            result = true;
        } break;
        case 1: {
            result = false;
        } break;
    }

    return result;
}

/**************************************************************************
 * This function takes in a sub-tile position unit vector component, 
 * normalizes it and updates the pagePosition accordingly.
 *************************************************************************/
inline float normalizeTilePosition(
    float tilePosition,
    uint16_t* pagePosition
) {
    float result = tilePosition;    // initialize return value

    if(result > 1.0f) {    // if sub-tile position is on next tile
        (*pagePosition)++;
        result--;
    } else if(result < 0.0f) {  // if sub-tile position is on the previous tile
        (*pagePosition)--;
        result--;
    }
    return result;
}

/**************************************************************************
 * 
 *************************************************************************/ 
bool worldTileCollision(
    float testX,
    float testY,
    float tileWidth,
    float tileHeight,
    game_WorkingPosition* playerPosition,
    game_TileMap* map
) {
    int pageIndex = (playerPosition->TileMapY * map->width) + playerPosition->TileMapX;
    game_TilePage* page = &map->data[pageIndex];
    int tileIndex = (playerPosition->TilePageY * page->width) + playerPosition->TilePageX;
    // game_Tile* tile = &page->data[tileIndex];

    int playerTileX   = truncateFloatToInt(testX) / (int)tileWidth;
    int playerTileY   = truncateFloatToInt(testY) / (int)tileHeight;        
    bool result = false;
    uint8_t tileValue;

    /*if  ((playerTileX >= 0 && playerTileX < page->width) &&
        (playerTileY >= 0 && playerTileY < page->height)) {
    */
        
    if (playerTileX < 0) {
        page = &map->data[(playerPosition->TileMapY * map->width) + playerPosition->TileMapX - 1];
        playerTileX = page->width + playerTileX;
    } else if (playerTileX > page->width) {
        page = &map->data[(playerPosition->TileMapY * map->width) + playerPosition->TileMapX + 1];
        playerTileX = playerTileX - page->width;
    } else if (playerTileY < 0) {
        page = &map->data[map->width * (playerPosition->TileMapY - 1) + playerPosition->TileMapX];
        playerTileY = page->height + playerTileY;
    } else if (playerTileY > page->height) {
        page = &map->data[map->width * (playerPosition->TileMapY + 1) + playerPosition->TileMapX];
        playerTileY = playerTileY - page->height;
    }

    tileValue = page->data[playerTileY * page->width + playerTileX];
    // TODO: Change transition tiles to a z coordinate shift
    switch(tileValue) {
        case 0: {
            result = true;
        } break;
        case 1: {
            result = false;
        } break;
    }

    return result;
}

/**************************************************************************
 * Calculates the TilePosition in meters instead of tiles.
 *************************************************************************/
void setUnitTileCoordinates(
    game_TilePage* page,
    float tileWidth,
    float tileHeight,
    game_WorkingPosition* out
) {
    out->UnitX = tileWidth * out->TileX;
    out->UnitY = tileHeight * out->TileY;
}

/**************************************************************************
 * Load in only the pages around the player.
 * TODO: make more dynamic and flexible with inconsistent map dimensions.
 * - Make this dynamic based off of render speed (ie not paging in a new
 *   TilePageFrame every frame)
 *************************************************************************/
void getTilePageFrame(
    game_WorkingPosition* playerPosition,
    game_TileMap* map,
    game_TilePage** out
) {
    out[4] = &(map->data[playerPosition->TileMapY * map->width + playerPosition->TileMapX]);
    
    int pageFrameWidth = TILE_PAGE_FRAME_WIDTH;
    int j = 0;
    for(
        int y = playerPosition->TileMapY - 1;
        y <= playerPosition->TileMapY + 1;
        y++
    ) {
        int i = 0;
        for(
            int x = playerPosition->TileMapX - 1;
            x <= playerPosition->TileMapX + 1;
            x++
        ) {
            out[j * pageFrameWidth + i] = &(map->data[map->width * y + x]);
            i++;
        }
        j++;
    }

}

}
/******************************************************************************
 * Public Methods
 *****************************************************************************/


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
    game_WorkingPosition playerPosition {
        0,
        0,
        0,
        0,
        0.0,
        0.0,
        0.0,
        0.0
    };

   // DEBUG MAP INITIALIZATION
   // TODO: Move this to Heap allocation
    Map.width = 2;
    Map.height = 2;
    Map.tileHeight = 90.0f;
    Map.tileWidth = 90.0f;
    Map.style = 0;
    tempMap[0] = {
        16,
        9,
        (uint8_t*)&TILE_DATA00
    };
    tempMap[1] = {
        16,
        9,
        (uint8_t*)&TILE_DATA01
    };
    tempMap[2] = {
        16,
        9,
        (uint8_t*)&TILE_DATA02
    };
    tempMap[3] = {
        16,
        9,
        (uint8_t*)&TILE_DATA10
    };
    tempMap[4] = {
        16,
        9,
        (uint8_t*)&TILE_DATA11
    };
    tempMap[5] = {
        16,
        9,
        (uint8_t*)&TILE_DATA12
    };
    tempMap[6] = {
        16,
        9,
        (uint8_t*)&TILE_DATA20
    };
    tempMap[7] = {
        16,
        9,
        (uint8_t*)&TILE_DATA21
    };
    tempMap[8] = {
        16,
        9,
        (uint8_t*)&TILE_DATA22
    };

    Map.data = tempMap;

/*
    unpackUnifiedPosition(
        gameState.playerPosition,
        &playerPosition
    );
*/
    parseInput(
        controllerInput,
        &playerPosition,
        &Map
    );
    inputTest(controllerInput);
    renderGameGfx(
        gfxBuffer,
        &playerPosition,
        &Map
    );
    // TODO: Move once this starts polling more than once per frame
    resetControllerStateCounter(controllerInput);
    
}

 