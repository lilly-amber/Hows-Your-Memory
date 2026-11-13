#include "deck.h"
#include <algorithm>
#include <random>
#include <vector>

Deck::Deck()
{
    Reshuffle();
}

void Deck::Reshuffle()
{
    int index = 0;

    for (int pairID = 1; pairID <= 18; pairID++)
    {
        cards[index] = Card(pairID);
        index++;

        cards[index] = Card(pairID);
        index++;
    }

    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(cards, cards + 36, rng);
}

Card& Deck::GetCard(int index)
{
    return cards[index];
}

void Deck::ShuffleUnmatched()
{
    std::vector<int> slots;
    std::vector<Card> unmatched; 

    for (int i = 0; i < 36; i++)
    {
        if (!cards[i].IsMatched())
        {
            slots.push_back(i);
            unmatched.push_back(cards[i]);
        }
    }

    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(unmatched.begin(), unmatched.end(), rng);

    for (size_t i = 0; i < slots.size(); i++)
    {
        cards[slots[i]] = unmatched[i];
    }
}