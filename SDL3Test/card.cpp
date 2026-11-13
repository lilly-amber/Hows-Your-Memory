#include "card.h"

Card::Card()
{
    pairID = 0;
    faceUp = false;
    matched = false;
}

Card::Card(int id)
{
    pairID = id;
    faceUp = false;
    matched = false;
}

int Card::GetPairID()
{
    return pairID;
}

bool Card::IsFaceUp()
{
    return faceUp;
}

bool Card::IsMatched()
{
    return matched;
}

void Card::Flip()
{
    faceUp = !faceUp;
}

void Card::SetMatched(bool state)
{
    matched = state;
}