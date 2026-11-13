#pragma once
#include "card.h"

class Deck
{
private:
    Card cards[36];

public:
    Deck();

    Card& GetCard(int index);
    void ShuffleUnmatched();
    void Reshuffle();
};