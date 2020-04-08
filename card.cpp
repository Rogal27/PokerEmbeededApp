#include "card.h"

Cards::Card::Card(Suits _suit, int _value): suit(_suit), value(_value)
{
    SetCardDeck();
}

std::vector<std::string> Cards::Card::GetCardDeck()
{
    return CardDeck;
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
    CardDeck = std::vector<std::string>(9);
    CardDeck[0] = "┌─────────┐";  
    CardDeck[1] = "│" + GetValueSymbolUp() + "       │";
    CardDeck[2] = "│  " + symbol + " " + symbol + " " + symbol + "  │";
    CardDeck[3] = "│   " + symbol + " " + symbol + "   │";
    CardDeck[4] = CardDeck[2];
    CardDeck[5] = CardDeck[3];
    CardDeck[6] = CardDeck[2];
    CardDeck[7] = "│       " + GetValueSymbolDown() + "│";
    CardDeck[8] = "└─────────┘";
}