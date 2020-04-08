#include <cstdlib>
#include <time.h>
#include <algorithm>

#include "poker.h"

Cards::Poker::Poker(long balance, int seed) : balance(balance)
{
    if (seed == 0)
        srand(time(NULL));
    else
        srand(seed);
    cards = std::vector<Cards::Card>();
    usedCards = std::vector<int>();
    stakeIndex = 0;
}

void Cards::Poker::PlayNextRound()
{
    DrawNewHand();
    PrintCurrentHand();
}

void Cards::Poker::SetStakes()
{
    stakeIndex = 0;
    stakes = std::vector<int>(6);
    stakes[0] = 1;
    stakes[1] = 5;
    stakes[2] = 10;
    stakes[3] = 20;
    stakes[4] = 50;
    stakes[5] = 100;
}

void Cards::Poker::DrawNewHand()
{
    usedCards.clear();
    cards.clear();
    for (int i = 0; i < 5; i++)
    {
        int cardValue = NextInt();
        while (true)
        {
            auto hasAppeared = std::find(usedCards.begin(), usedCards.end(), cardValue);
            if (hasAppeared == usedCards.end())
                break;
            cardValue++;
            cardValue = cardValue % 52;
        }
        usedCards.push_back(cardValue);
        cards.push_back(ConvertIntToCard(cardValue));
    }
}

int Cards::Poker::NextInt()
{
    return rand() % 52;
}

Cards::Card Cards::Poker::ConvertIntToCard(int i)
{
    int value = (i % 13) + 2;
    int color = i / 13;
    Cards::Suits suit = static_cast<Cards::Suits>(color);
    return Cards::Card(suit, value);
}

void Cards::Poker::PrintCurrentHand()
{
    auto cardsStrings = std::vector<std::vector<std::string>>(cards.size());
    auto printeableVector = std::vector<std::string>();
    auto drawCardDeck = Cards::Card::GetDrawCardDeck();
    for (int i = 0; i < cards.size(); i++)
    {
        cardsStrings[i] = cards[i].GetCardDeck();
    }
    for (int i = 0; i < cardsStrings[0].size(); i++)
    {
        std::string line;
        for (int j = 0; j < cardsStrings.size(); j++)
        {
            line += cardsStrings[j][i];
            line += "  ";            
        }
        line += drawCardDeck[i];
        printeableVector.push_back(line);
    }

    //print lines
    for (int i = 0; i < printeableVector.size(); i++)
    {
        std::cout << printeableVector[i] << std::endl;
    }
}