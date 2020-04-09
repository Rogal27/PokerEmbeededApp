#include <cstdlib>
#include <time.h>
#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <gpiod.h>

#include "poker.h"

Cards::Poker::Poker(long balance, struct gpiod_line **lines, int linesCount, int seed) : balance(balance), state(Cards::State::StakeChoose), lines(lines), linesCount(linesCount)
{
    if (seed == 0)
        srand(time(NULL));
    else
        srand(seed);
    cards = std::vector<Cards::Card>();
    usedCards = std::vector<int>();
    InitStakes();
    if (balance < 10)
        balance = 10;
    std::cout << "Welcome to simple poker!" << std::endl;
    CleardLEDs();
    FlashLEDs();
    std::cout << "Your current balance is " << balance << " tokens." << std::endl;
    ShowStakePrompt(true);
    SetStakeWithLED(stakeIndex);
}

void Cards::Poker::PlayNextRound()
{
    if (state != Cards::State::StakeChoose)
    {
        std::cout << "You cannot call this method!" << std::endl;
        return;
    }
    CleardLEDs();
    state = Cards::State::CardChoose;
    if (balance < stakes[stakeIndex])
    {
        std::cout << "You do not have enough tokens to play :(" << std::endl;
        return;
    }
    std::cout << "Select cards to replace them. (Move with left and right button, select with middle)" << std::endl;
    balance -= stakes[stakeIndex];
    DrawNewHand();
    PrintCurrentHand(true);
    bool shouldBeChanged[5] = {false, false, false, false, false};
    DrawSelectPanel(shouldBeChanged, 0, true);
}

bool Cards::Poker::ChangeCards(bool shouldBeChanged[5])
{
    if (state != Cards::State::CardChoose)
    {
        std::cout << "You cannot call this method!" << std::endl;
        return false;
    }
    int cardsCount = 0;
    for (int i = 0; i < 5; i++)
    {
        if (shouldBeChanged[i] == true)
            cardsCount++;
    }
    if (cardsCount == 5)
    {
        std::cout << "You cannot change 5 cards!" << std::endl;
        return false;
    }
    state = Cards::State::Result;
    std::cout << "Cards after change" << std::endl;
    DrawChangedCards(shouldBeChanged);
    PrintCurrentHand(false);

    auto result = CalculateResult();
    int profit = CalculateTockenGain(result);
    PrintResult(result, profit);
    FlashLEDsResult(result);
    balance += profit;
    std::cout << "Your current balance is " << balance << " tokens." << std::endl;
    CleardLEDs();
    std::cout << "Do you want to play again? (Press middle button to play again)" << std::endl;
    state = Cards::State::StakeChoose;
    return true;
}

void Cards::Poker::DrawSelectPanel(bool shouldBeChanged[5], int currentSelect)
{
    if (state != Cards::State::CardChoose)
    {
        std::cout << "You cannot call this method!" << std::endl;
        return;
    }

    std::cout << "\x1b[1A"
              << "\x1b[1A"
              << "\x1b[1A";

    int count = CountSelectedCards(shouldBeChanged);
    LightFirstNLeds(count);
    DrawSelectPanel(shouldBeChanged, currentSelect, true);
}

void Cards::Poker::ShowStakePrompt()
{
    if (state != Cards::State::StakeChoose)
    {
        std::cout << "You cannot call this method!" << std::endl;
        return;
    }
    std::cout << "\x1b[1A";
    ShowStakePrompt(true);
}

void Cards::Poker::SetStake(int stakeIndex)
{
    if (stakeIndex < 0)
        stakeIndex = 0;
    if (stakeIndex >= stakes.size())
        stakeIndex = stakes.size() - 1;
    this->stakeIndex = stakeIndex;
}

void Cards::Poker::SetStakeWithLED(int stakeIndex)
{
    if (stakeIndex < 0)
        stakeIndex = 0;
    if (stakeIndex >= stakes.size())
        stakeIndex = stakes.size() - 1;
    this->stakeIndex = stakeIndex;

    LightFirstNLeds(stakeIndex);
}

void Cards::Poker::InitStakes()
{
    stakeIndex = 0;
    stakes = std::vector<int>(4);
    stakes[0] = 1;
    stakes[1] = 5;
    stakes[2] = 25;
    stakes[3] = 50;
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

void Cards::Poker::DrawChangedCards(bool shouldBeChanged[5])
{
    for (int i = 0; i < 5; i++)
    {
        if (shouldBeChanged[i] == false)
            continue;
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
        cards[i] = ConvertIntToCard(cardValue);
    }
}

void Cards::Poker::DrawSelectPanel(bool shouldBeChanged[5], int currentSelect, bool isFirstTimeDraw)
{
    if (state != Cards::State::CardChoose)
    {
        std::cout << "You cannot call this method!" << std::endl;
        return;
    }

    for (int i = 0; i < 5; i++)
    {
        std::cout << "┌─────────┐"
                  << "  ";
    }
    std::cout << "┌────────┐" << std::endl;
    for (int i = 0; i < 5; i++)
    {
        if (shouldBeChanged[i] == false && currentSelect != i)
            std::cout << "│         │"
                      << "  ";
        if (shouldBeChanged[i] == false && currentSelect == i)
            std::cout << "│   ░░░   │"
                      << "  ";
        if (shouldBeChanged[i] == true && currentSelect != i)
            std::cout << "│░░░░░░░░░│"
                      << "  ";
        if (shouldBeChanged[i] == true && currentSelect == i)
            std::cout << "│░░░   ░░░│"
                      << "  ";
    }
    if (currentSelect == 5)
        std::cout << "│  ░░░░  │" << std::endl;
    else
        std::cout << "│        │" << std::endl;
    for (int i = 0; i < 5; i++)
    {
        std::cout << "└─────────┘"
                  << "  ";
    }
    std::cout << "└────────┘" << std::endl;
}

void Cards::Poker::ShowStakePrompt(bool isFirstTimeDraw)
{
    if (state != Cards::State::StakeChoose)
    {
        std::cout << "You cannot call this method!" << std::endl;
        return;
    }
    std::cout << "Your current stake is: " << stakes[stakeIndex] << ". (Press left or right button to adjust, middle to play)" << std::endl;
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

void Cards::Poker::PrintCurrentHand(bool drawButton)
{
    auto cardsStrings = std::vector<std::vector<std::string>>();
    auto printeableVector = std::vector<std::string>();
    auto drawCardDeck = Cards::Card::GetDrawCardDeck();
    for (int i = 0; i < cards.size(); i++)
    {
        cardsStrings.push_back(cards[i].GetCardDeck());
    }
    for (int i = 0; i < cardsStrings[0].size(); i++)
    {
        std::string line;
        for (int j = 0; j < cardsStrings.size(); j++)
        {
            line += cardsStrings[j][i];
            line += "  ";
        }
        if (drawButton == true)
            line += drawCardDeck[i];
        printeableVector.push_back(line);
    }

    //print lines
    for (int i = 0; i < printeableVector.size(); i++)
    {
        std::cout << printeableVector[i] << std::endl;
    }
}

Cards::WinType Cards::Poker::CalculateResult()
{
    if (CheckCardsVectorSize() == false)
        return Cards::WinType::None;
    if (CheckRoyalFlush())
        return Cards::WinType::RoyalFlush;
    if (CheckStraightFlush())
        return Cards::WinType::StraightFlush;
    if (CheckFourOfAKind())
        return Cards::WinType::FourOfAKind;
    if (CheckFullHouse())
        return Cards::WinType::FullHouse;
    if (CheckFlush())
        return Cards::WinType::Flush;
    if (CheckStraight())
        return Cards::WinType::Straight;
    if (CheckThreeOfAKind())
        return Cards::WinType::ThreeOfAKind;
    if (CheckTwoPair())
        return Cards::WinType::TwoPair;
    if (CheckPair())
        return Cards::WinType::Pair;
    return Cards::WinType::None;
}

int Cards::Poker::CalculateTockenGain(Cards::WinType type)
{
    int stake = stakes[stakeIndex];
    switch (type)
    {
    case Cards::WinType::None:
        return 0;
    case Cards::WinType::Pair:
        return stake;
    case Cards::WinType::TwoPair:
        return 3 * stake;
    case Cards::WinType::ThreeOfAKind:
        return 5 * stake;
    case Cards::WinType::Straight:
        return 7 * stake;
    case Cards::WinType::Flush:
        return 9 * stake;
    case Cards::WinType::FullHouse:
        return 12 * stake;
    case Cards::WinType::FourOfAKind:
        return 50 * stake;
    case Cards::WinType::StraightFlush:
        return 200 * stake;
    case Cards::WinType::RoyalFlush:
        return 500 * stake;
    default:
        break;
    }
    return 0;
}

void Cards::Poker::PrintResult(Cards::WinType type, int wonTokens)
{
    switch (type)
    {
    case Cards::WinType::None:
        std::cout << "Nothing." << std::endl;
        return;
    case Cards::WinType::Pair:
        std::cout << "Pair.";
        break;
    case Cards::WinType::TwoPair:
        std::cout << "Two Pairs.";
        break;
    case Cards::WinType::ThreeOfAKind:
        std::cout << "Three of a kind.";
        break;
    case Cards::WinType::Straight:
        std::cout << "Straight.";
        break;
    case Cards::WinType::Flush:
        std::cout << "Flush.";
        break;
    case Cards::WinType::FullHouse:
        std::cout << "Full House.";
        break;
    case Cards::WinType::FourOfAKind:
        std::cout << "Four of a Kind.";
        break;
    case Cards::WinType::StraightFlush:
        std::cout << "Straight Flush.";
        break;
    case Cards::WinType::RoyalFlush:
        std::cout << "Royal Flush!!!";
        break;
    default:
        return;
    }
    std::cout << " You have won " << wonTokens;
    if (wonTokens == 1)
        std::cout << " token." << std::endl;
    else
        std::cout << " tokens." << std::endl;
}

void Cards::Poker::FlashLEDsResult(Cards::WinType type)
{
    switch (type)
    {
    case Cards::WinType::Flush:
    case Cards::WinType::FullHouse:
    case Cards::WinType::FourOfAKind:
    case Cards::WinType::StraightFlush:
    case Cards::WinType::RoyalFlush:
        FlashLEDs();
        break;
    default:
        return;
    }
}

bool Cards::Poker::CheckCardsVectorSize()
{
    if (cards.size() != 5)
    {
        std::cout << "Wrong cards vector size!" << std::endl;
        return false;
    }
    return true;
}

bool Cards::Poker::CheckRoyalFlush()
{
    if (CheckStraightFlush() == true)
    {
        int sum = CountCardsValueSum();
        if (sum == 60)
            return true;
    }
    return false;
}

bool Cards::Poker::CheckStraightFlush()
{
    if (CheckFlush() == false)
        return false;
    return CheckStraight();
}

bool Cards::Poker::CheckFourOfAKind()
{
    int count;
    for (int i = 0; i < 2; i++)
    {
        count = CountCardsWithEqualValue(cards[i].GetValue());
        if (count == 4)
            return true;
    }
    return false;
}

bool Cards::Poker::CheckFullHouse()
{
    std::sort(cards.begin(), cards.end());

    if (cards[0].GetValue() == cards[1].GetValue() && cards[3].GetValue() == cards[4].GetValue())
    {
        if (cards[1].GetValue() == cards[2].GetValue() || cards[2].GetValue() == cards[3].GetValue())
            return true;
    }
    return false;
}

bool Cards::Poker::CheckFlush()
{
    int sum = CountCardsWithEqualSuit(cards[0].GetSuit());
    return sum == 5;
}

bool Cards::Poker::CheckStraight()
{
    std::sort(cards.begin(), cards.end());

    int min = cards[0].GetValue();
    int max = cards[4].GetValue();
    if (max - min != 4)
        return false;

    for (int i = 1; i < 5; i++)
    {
        if (cards[i].GetValue() != cards[i - 1].GetValue() + 1)
            return false;
    }

    return true;
}

bool Cards::Poker::CheckThreeOfAKind()
{
    int count;
    for (int i = 0; i < 3; i++)
    {
        count = CountCardsWithEqualValue(cards[i].GetValue());
        if (count == 3)
            return true;
    }
    return false;
}

bool Cards::Poker::CheckTwoPair()
{
    int count;
    int doubles_count = 0;
    for (int i = 0; i < cards.size(); i++)
    {
        count = CountCardsWithEqualValue(cards[i].GetValue());
        if (count == 2)
            doubles_count++;
    }
    return doubles_count == 4;
}

bool Cards::Poker::CheckPair()
{
    int count;
    for (int i = 0; i < cards.size(); i++)
    {
        count = CountCardsWithEqualValue(cards[i].GetValue());
        if (count == 2)
        {
            if (cards[i].GetValue() > 10)
                return true;
        }
    }
    return false;
}

int Cards::Poker::CountCardsWithEqualValue(int value)
{
    int cardCount = 0;
    for (int i = 0; i < cards.size(); i++)
    {
        if (value == cards[i].GetValue())
            cardCount++;
    }
    return cardCount;
}

int Cards::Poker::CountCardsWithEqualSuit(Cards::Suits suit)
{
    int cardCount = 0;
    for (int i = 0; i < cards.size(); i++)
    {
        if (suit == cards[i].GetSuit())
            cardCount++;
    }
    return cardCount;
}

int Cards::Poker::CountCardsValueSum()
{
    int sum = std::accumulate(cards.begin(), cards.end(), 0, [](const int &sum, const Cards::Card &card) { return sum + card.GetValue(); });
    return sum;
}

int Cards::Poker::CountSelectedCards(bool shouldBeChanged[5])
{
    int count = 0;
    for (int i = 0; i < 5; i++)
    {
        if (shouldBeChanged[i] == true)
            count++;
    }
    return count;
}

void Cards::Poker::SetLEDValue(struct gpiod_line *line, int value)
{
    int ret = gpiod_line_set_value(line, value);
    if (ret < 0)
    {
        throw std::runtime_error("LED ERROR");
    }
}

void Cards::Poker::CleardLEDs()
{
    for (int i = 0; i < linesCount; i++)
    {
        SetLEDValue(lines[i], 0);
    }    
}

void Cards::Poker::FlashLEDs()
{
    struct timespec sleep_time = {0, 200 * 1000000};
    for (int k = 0; k < 3; k++)
    {
        for (int i = 0; i < linesCount; i++)
        {
            SetLEDValue(lines[i], 1);
        }
        nanosleep(&sleep_time, NULL);
        for (int i = 0; i < linesCount; i++)
        {
            SetLEDValue(lines[i], 0);
        }
        nanosleep(&sleep_time, NULL);
    }
}

void Cards::Poker::LightFirstNLeds(int n)
{
    int min = std::min(n, linesCount);

    for (int i = min; i < linesCount; i++)
    {
        SetLEDValue(lines[i], 0);
    }

    for (int i = 0; i < min; i++)
    {
        SetLEDValue(lines[i], 1);
    }
}