#pragma once

class Card
{
private:
    int pairID;
    bool faceUp;
    bool matched;

public:
    Card();
    Card(int id);

    int GetPairID();

    bool IsFaceUp();
    bool IsMatched();

    void Flip();
    void SetMatched(bool state);

};