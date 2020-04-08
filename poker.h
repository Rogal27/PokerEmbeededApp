#ifndef POKER_H
#define POKER_H

#include "card.h"

namespace Cards
{
    enum class WinType
    {
        None,
        Pair,
        TwoPair,
        ThreeOfAKind,
        Straight,
        Flush,
        FullHouse,
        FourOfAKind,
        StraightFlush,
        RoyalFlush
    };

    enum class State
    {
        StakeChoose,
        CardChoose,
        Result
    };

    class Poker
    {
    public:
        Poker(long balance, int seed = 0);

        void PlayNextRound();
        void ChangeCards(bool shouldBeChanged[5]);
    private:
        void InitStakes();
        void DrawNewHand();
        void DrawChangedCards(bool shouldBeChanged[5]);

        int NextInt();
        Card ConvertIntToCard(int i);
        void PrintCurrentHand(bool drawButton);

        WinType CalculateResult();
        int CalculateTockenGain(WinType type);
        void PrintResult(WinType type, int wonTokens);

        bool CheckCardsVectorSize();

        bool CheckRoyalFlush();
        bool CheckStraightFlush();
        bool CheckFourOfAKind();
        bool CheckFullHouse();
        bool CheckFlush();
        bool CheckStraight();
        bool CheckThreeOfAKind();
        bool CheckTwoPair();
        bool CheckPair();
        
        int CountCardsWithEqualValue(int value);
        int CountCardsWithEqualSuit(Suits suit);
        int CountCardsValueSum();

        std::vector<Card> cards;
        std::vector<int> usedCards;
        std::vector<int> stakes;
        int stakeIndex;
        long balance;
        State state;
    };
} // namespace Cards

#endif // POKER_H