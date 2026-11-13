#pragma once
#include <SDL3/SDL.h>
#include "deck.h"

enum class GameState
{
    Start,
    Playing,
    Won
};

class Game
{
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* LoadTexture(const char* path);
    SDL_Texture* faceTextures[18];
    SDL_Texture* backTexture;

    Deck deck;

    Uint64 flipTime;
    Uint64 startTime;
    Uint64 finalTime;
    Uint64 elapsedSeconds;
    Uint64 previewStart;
    Uint64 shuffleMessageStart;
    Uint64 previewDuration;
    Uint64 winStamp;

    GameState state;

    bool running;
    bool waitingToFlip;
    bool gameWon;
    bool previewing;
    bool showShuffleMessage;
    bool shuffleReveal;
    bool matchPending;

    int mouseX;
    int mouseY;
    int selectedRow;
    int selectedColumn;
    int firstCard;
    int secondCard;
    int totalErrors;
    int matchedPairs;

    void Input();
    void Update();
    void Render();
    void DrawText(const char* text, float x, float y);
    void StartGame();
    void DrawTextCentered(const char* text, float y);
    void LoadAssets();

public:
    Game();
    ~Game();

    void Run();
};