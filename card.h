#include <iostream>
#include <string>
#include <vector>

namespace Cards
{
    enum class Suits
    {
        Club,//Trefl
        Diamond,
        Heart,
        Spade//Pik
    };

    class Card
    {    
    public:
        Card(Suits suit, int value);

        std::vector<std::string> GetCardDeck();
    private:
        std::string GetSuitSymbol();
        std::string GetValueSymbolUp();
        std::string GetValueSymbolDown();
        void SetCardDeck();

        Suits suit;
        int value;
        std::vector<std::string> CardDeck;
    };
}