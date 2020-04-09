#ifndef POKER_H
#define POKER_H

#include "card.h"

struct gpiod_line;

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
    Poker(long balance, struct gpiod_line **lines, int linesCount, int seed = 0);

    ~Poker();

    void PlayNextRound();
    bool ChangeCards(bool shouldBeChanged[5]);
    void DrawSelectPanel(bool shouldBeChanged[5], int currentSelect);
    void SetStake(int stakeIndex);
    void SetStakeWithLED(int stakeIndex);
    void ShowStakePrompt();

private:
    void InitStakes();
    void DrawNewHand();
    void DrawChangedCards(bool shouldBeChanged[5]);
    void DrawSelectPanel(bool shouldBeChanged[5], int currentSelect, bool isFirstTimeDraw);
    void ShowStakePrompt(bool isFirstTimeDraw);

    int NextInt();
    Card ConvertIntToCard(int i);
    void PrintCurrentHand(bool drawButton);

    WinType CalculateResult();
    int CalculateTockenGain(WinType type);
    void PrintResult(WinType type, int wonTokens);
    void FlashLEDsResult(WinType type);

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

    int CountSelectedCards(bool shouldBeChanged[5]);

    void SetLEDValue(struct gpiod_line *line, int value);
    void CleardLEDs();
    void FlashLED();
    void FlashLEDs();
    void LightFirstNLeds(int n);

    std::vector<Card> cards;
    std::vector<int> usedCards;
    std::vector<int> stakes;
    int stakeIndex;
    long balance;
    State state;
    struct gpiod_line **lines;
    int linesCount;
    bool hasSelectedAllCards;
};
} // namespace Cards

#endif // POKER_H