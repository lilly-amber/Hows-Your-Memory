#include "game.h"
#include "constants.h"
#include <iostream>
#include <string>
#include <cstring>
#include <cmath>

Game::Game()
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        "How's Your Memory?",
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0
    );

    running = true;
    renderer = SDL_CreateRenderer(window, NULL);

    selectedRow = -1;
    selectedColumn = -1;

    mouseX = 0;
    mouseY = 0;

    showShuffleMessage = false;
    shuffleMessageStart = 0;

    firstCard = -1;
    secondCard = -1;

    waitingToFlip = false;
    flipTime = 0;

    totalErrors = 0;

    startTime = SDL_GetTicks();
    finalTime = 0;
    matchedPairs = 0;
    gameWon = false;

    elapsedSeconds = 0;

    state = GameState::Start;
    previewing = false;
    previewStart = 0;

    backTexture = nullptr;

    winStamp = 0;

    matchPending = false;

    for (int i = 0; i < 18; i++)
    {
        faceTextures[i] = nullptr;
    }

    LoadAssets();
}

Game::~Game()
{
    if (backTexture != nullptr)
    {
        SDL_DestroyTexture(backTexture);
    }

    for (int i = 0; i < 18; i++)
    {
        if (faceTextures[i] != nullptr)
        {
            SDL_DestroyTexture(faceTextures[i]);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

SDL_Texture* Game::LoadTexture(const char* path)
{
    SDL_Surface* surface = SDL_LoadBMP(path);

    if (surface == nullptr)
    {
        std::cout << "Could not load " << path
            << ": " << SDL_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_DestroySurface(surface);

    if (texture == nullptr)
    {
        std::cout << "Could not create texture for " << path
            << ": " << SDL_GetError() << std::endl;
    }

    return texture;
}

void Game::LoadAssets()
{
    backTexture = LoadTexture("assets/back.bmp");

    for (int i = 0; i < 18; i++)
    {
        std::string path = "assets/card" + std::to_string(i + 1) + ".bmp";
        faceTextures[i] = LoadTexture(path.c_str());
    }
}

void Game::Run()
{
    while (running)
    {
        Input();
        Update();
        Render();
    }
}

void Game::Input()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
        {
            running = false;
        }

        if (state == GameState::Start)
        {
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN
                || event.type == SDL_EVENT_KEY_DOWN)
            {
                StartGame();
            }

            continue;
        }

        if (state == GameState::Won)
        {
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN
                || event.type == SDL_EVENT_KEY_DOWN)
            {
                StartGame();
            }

            continue;
        }

        if (waitingToFlip || previewing || matchPending)
        {
            continue;
        }

        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
        {
            mouseX = (int)event.button.x;
            mouseY = (int)event.button.y;

            float localX = event.button.x - GRID_START_X;
            float localY = event.button.y - GRID_START_Y;

            if (localX < 0 || localY < 0)
            {
                continue;
            }

            float stepX = CARD_WIDTH + CARD_SPACING;
            float stepY = CARD_HEIGHT + CARD_SPACING;

            selectedColumn = (int)(localX / stepX);
            selectedRow = (int)(localY / stepY);

            if (selectedColumn >= GRID_COLUMNS || selectedRow >= GRID_ROWS)
            {
                continue;
            }

            if (fmodf(localX, stepX) > CARD_WIDTH
                || fmodf(localY, stepY) > CARD_HEIGHT)
            {
                continue;
            }

            int cardIndex =
                (selectedRow * GRID_COLUMNS)
                + selectedColumn;

            if (cardIndex < 0 || cardIndex >= 36)
            {
                continue;
            }

            if (deck.GetCard(cardIndex).IsMatched())
            {
                continue;
            }

            if (firstCard == -1)
            {
                firstCard = cardIndex;

                deck.GetCard(firstCard).Flip();
            }
            else
            {
                if (cardIndex == firstCard)
                {
                    continue;
                }

                if (deck.GetCard(cardIndex).IsFaceUp())
                {
                    continue;
                }

                secondCard = cardIndex;

                deck.GetCard(secondCard).Flip();
            }

            if (firstCard != -1 && secondCard != -1)
            {
                int firstPairID =
                    deck.GetCard(firstCard).GetPairID();

                int secondPairID =
                    deck.GetCard(secondCard).GetPairID();

                if (firstPairID == secondPairID)
                {
                    matchPending = true;
                    flipTime = SDL_GetTicks();

                    std::cout << "MATCH!" << std::endl;
                }
                else
                {
                    waitingToFlip = true;

                    flipTime = SDL_GetTicks();

                    totalErrors++;

                    std::cout
                        << "NOT A MATCH!" << totalErrors
                        << std::endl;
                }

            }

            std::cout
                << "Row: " << selectedRow
                << " Column: " << selectedColumn
                << " Card Index: " << cardIndex
                << std::endl;
        }
    }
}

void Game::Update()
{
    if (state == GameState::Start)
    {
        elapsedSeconds = 0;
        return;
    }

    if (state == GameState::Won)
    {
        return;
    }

    if (gameWon && SDL_GetTicks())
    {
        state = GameState::Won;
        return;
    }

    if (previewing)
    {
        if (SDL_GetTicks() - previewStart >= previewDuration)
        {
            for (int i = 0; i < GRID_ROWS * GRID_COLUMNS; i++)
            {
                if (!deck.GetCard(i).IsMatched() && deck.GetCard(i).IsFaceUp())
                {
                    deck.GetCard(i).Flip();
                }
            }

            previewing = false;

            if (!shuffleReveal)
            {
                startTime = SDL_GetTicks();
            }

            shuffleReveal = false;
        }
    }

    if (waitingToFlip)
    {
        if (SDL_GetTicks() - flipTime > 1000)
        {
            deck.GetCard(firstCard).Flip();
            deck.GetCard(secondCard).Flip();

            firstCard = -1;
            secondCard = -1;

            if (totalErrors % 10 == 0)
            {
                deck.ShuffleUnmatched();
                std::cout << "SHUFFLED!" << std::endl;

                for (int i = 0; i < GRID_ROWS * GRID_COLUMNS; i++)
                {
                    if (!deck.GetCard(i).IsMatched() && !deck.GetCard(i).IsFaceUp())
                    {
                        deck.GetCard(i).Flip();
                    }
                }

                previewing = true;
                shuffleReveal = true;
                previewStart = SDL_GetTicks();
                previewDuration = SHUFFLE_REVEAL_MS;
            }

            waitingToFlip = false;
        }
    }

    if (matchPending)
    {
        if (SDL_GetTicks() - flipTime >= MATCH_REVEAL_MS)
        {
            deck.GetCard(firstCard).SetMatched(true);
            deck.GetCard(secondCard).SetMatched(true);

            firstCard = -1;
            secondCard = -1;

            matchedPairs++;

            if (matchedPairs == 18)
            {
                gameWon = true;
                finalTime = (SDL_GetTicks() - startTime) / 1000;
                state = GameState::Won;
            }

            matchPending = false;
        }
    }

    if (showShuffleMessage
        && SDL_GetTicks() - shuffleMessageStart > SHUFFLE_MESSAGE_MS)
    {
        showShuffleMessage = false;
    }

    if (previewing)
    {
        elapsedSeconds = 0;
    }

    else if (gameWon)
    {
        elapsedSeconds = finalTime;
    }
    else
    {
        elapsedSeconds = (SDL_GetTicks() - startTime) / 1000;
    }
}

void Game::Render()
{
    SDL_SetRenderDrawColor(renderer, 0, 200, 200, 255);
    SDL_RenderClear(renderer);

    if (state == GameState::Start)
    {
        DrawTextCentered("How's Your Memory?", 250.0f);
        DrawTextCentered("Memorise and pair the cards.", 350.0f);
        DrawTextCentered("Every 10 mistakes shuffles the hidden cards.", 390.0f);
        DrawTextCentered("Click to start", 480.0f);

        SDL_RenderPresent(renderer);
        return;
    }

    if (state == GameState::Won)
    {
        std::string timeLine = "Time: " + std::to_string(finalTime) + "s";
        std::string errorLine = "Total errors: " + std::to_string(totalErrors);

        DrawTextCentered("You win!", 250.0f);
        DrawTextCentered(timeLine.c_str(), 350.0f);
        DrawTextCentered(errorLine.c_str(), 390.0f);
        DrawTextCentered("Click to play again", 480.0f);

        SDL_RenderPresent(renderer);
        return;
    }

    for (int row = 0; row < GRID_ROWS; row++)
    {
        for (int column = 0; column < GRID_COLUMNS; column++)
        {
            int cardIndex =
                (row * GRID_COLUMNS)
                + column;

            SDL_FRect card =
            {
                GRID_START_X + (column * (CARD_WIDTH + CARD_SPACING)),
                GRID_START_Y + (row * (CARD_HEIGHT + CARD_SPACING)),
                CARD_WIDTH,
                CARD_HEIGHT
            };

            Card& currentCard = deck.GetCard(cardIndex);

            if (currentCard.IsMatched())
            {
                continue;
            }

            SDL_Texture* texture = nullptr;

            if (currentCard.IsMatched() || currentCard.IsFaceUp())
            {
                texture = faceTextures[currentCard.GetPairID() - 1];
            }
            else
            {
                texture = backTexture;
            }

            if (texture != nullptr)
            {
                SDL_RenderTexture(renderer, texture, NULL, &card);
            }

            if (currentCard.IsMatched())
            {
                continue;
            }
        }
    }

    int remaining = 10 - (totalErrors % 10);

    if (waitingToFlip && totalErrors > 0 && totalErrors % 10 == 0)
    {
        remaining = 0;
    }

    std::string timeText = "Time: " + std::to_string(elapsedSeconds) + "s";
    std::string errorText = "Total Errors: " + std::to_string(totalErrors);
    std::string shuffleText = "Shuffle in: " + std::to_string(remaining);

        DrawText(timeText.c_str(), 60.0f, 850.0f);
        DrawText(errorText.c_str(), 240.0f, 850.0f);
        DrawText(shuffleText.c_str(), 530.0f, 850.0f);

    if (previewing && !shuffleReveal)
    {
        Uint64 elapsed = SDL_GetTicks() - previewStart;
        int secondsLeft = (int)((PREVIEW_TIME_MS - elapsed) / 1000) + 1;

        std::string previewText = "Memorise the cards! " + std::to_string(secondsLeft);
        DrawText(previewText.c_str(), 230.0f, 15.0f);
    }

    if (showShuffleMessage)
    {
        DrawTextCentered("Shuffled!", 15.0f);
    }

    SDL_RenderPresent(renderer);
}

void Game::DrawText(const char* text, float x, float y)
{
    const float scale = 2.0f;

    SDL_SetRenderScale(renderer, scale, scale);
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderDebugText(renderer, x / scale, y / scale, text);
    SDL_SetRenderScale(renderer, 1.0f, 1.0f);
}

void Game::StartGame()
{
    deck.Reshuffle();
    state = GameState::Playing;

    totalErrors = 0;
    matchedPairs = 0;
    gameWon = false;
    firstCard = -1;
    secondCard = -1;
    waitingToFlip = false;
    showShuffleMessage = false;
    shuffleReveal = false;
    previewDuration = PREVIEW_TIME_MS;
    matchPending = false;

    for (int i = 0; i < GRID_ROWS * GRID_COLUMNS; i++)
    {
        if (!deck.GetCard(i).IsFaceUp())
        {
            deck.GetCard(i).Flip();
        }
    }

    previewing = true;
    previewStart = SDL_GetTicks();
}

void Game::DrawTextCentered(const char* text, float y)
{
    const float charWidth = 16.0f;
    float width = strlen(text) * charWidth;

    DrawText(text, (WINDOW_WIDTH - width) / 2.0f, y);
}