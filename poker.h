#ifndef POKER_H
#define POKER_H

#include "card.h"

namespace Cards
{
    class Poker
    {
    public:
        Poker(long balance, int seed = 0);

        void PlayNextRound();
    private:
        void SetStakes();
        void DrawNewHand();
        int NextInt();
        Card ConvertIntToCard(int i);
        void PrintCurrentHand();

        std::vector<Card> cards;
        std::vector<int> usedCards;
        std::vector<int> stakes;
        int stakeIndex;
        long balance;
    };
} // namespace Cards

#endif // POKER_H