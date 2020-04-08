#include "card.h"

Cards::Card::Card(Suits _suit, int _value) : suit(_suit), value(_value)
{
    SetCardDeck();
}

std::vector<std::string> Cards::Card::GetCardDeck()
{
    return cardDeck;
}

std::vector<std::string> Cards::Card::GetDrawCardDeck()
{
    auto drawCardDeck = std::vector<std::string>(9);
    drawCardDeck[0] = " ";
    drawCardDeck[1] = " ";
    drawCardDeck[2] = "┌────────┐";
    drawCardDeck[3] = "│        │";
    drawCardDeck[4] = "│  Draw  │";
    drawCardDeck[5] = "│        │";
    drawCardDeck[6] = "└────────┘";
    drawCardDeck[7] = " ";
    drawCardDeck[8] = " ";
    return drawCardDeck;
}

std::string Cards::Card::GetSuitSymbol()
{
    switch (suit)
    {
    case Suits::Club:
        return "\u2663";
    case Suits::Diamond:
        return "\u2666";
    case Suits::Heart:
        return "\u2665";
    case Suits::Spade:
        return "\u2660";
    default:
        break;
    }
    return " ";
}

std::string Cards::Card::GetValueSymbolUp()
{
    if (value < 2)
        return "  ";
    if (value < 10)
        return std::to_string(value) + " ";
    if (value == 10)
        return std::to_string(value);
    if (value == 11)
        return "J ";
    if (value == 12)
        return "Q ";
    if (value == 13)
        return "K ";
    if (value == 14)
        return "A ";
    return "  ";
}

std::string Cards::Card::GetValueSymbolDown()
{
    if (value < 2)
        return "  ";
    if (value < 10)
        return " " + std::to_string(value);
    if (value == 10)
        return std::to_string(value);
    if (value == 11)
        return " J";
    if (value == 12)
        return " Q";
    if (value == 13)
        return " K";
    if (value == 14)
        return " A";
    return "  ";
}

void Cards::Card::SetCardDeck()
{
    std::string symbol = GetSuitSymbol();
    cardDeck = std::vector<std::string>(9);
    cardDeck[0] = "┌─────────┐";
    cardDeck[1] = "│" + GetValueSymbolUp() + "       │";
    cardDeck[2] = "│  " + symbol + " " + symbol + " " + symbol + "  │";
    cardDeck[3] = "│   " + symbol + " " + symbol + "   │";
    cardDeck[4] = cardDeck[2];
    cardDeck[5] = cardDeck[3];
    cardDeck[6] = cardDeck[2];
    cardDeck[7] = "│       " + GetValueSymbolDown() + "│";
    cardDeck[8] = "└─────────┘";
}