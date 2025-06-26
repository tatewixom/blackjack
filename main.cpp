#include <algorithm> // for std::shuffle
#include <array>
#include <cassert>
#include <iostream>
#include "Random.h"

struct Card
{
  enum Rank
  {
    rank_ace,
    rank_2,
    rank_3,
    rank_4,
    rank_5,
    rank_6,
    rank_7,
    rank_8,
    rank_9,
    rank_10,
    rank_jack,
    rank_queen,
    rank_king,

    max_ranks
  };

  enum Suit
  {
    suit_club,
    suit_diamond,
    suit_heart,
    suit_spade,

    max_suits
  };

  static constexpr std::array allRanks{ rank_ace, rank_2, rank_3, rank_4, rank_5, rank_6, rank_7, rank_8, rank_9, rank_10, rank_jack, rank_queen, rank_king };
  static constexpr std::array allSuits{ suit_club, suit_diamond, suit_heart, suit_spade };

  Rank rank{};
  Suit suit{};

  friend std::ostream& operator<<(std::ostream& out, const Card& card)
  {
    static constexpr std::array ranks{ 'A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K' };
    static constexpr std::array suits{ 'C', 'D', 'H', 'S' };

    out << ranks[card.rank] << suits[card.suit];
    return out;
  }

  int value() const
  {
    static constexpr std::array rankValues{ 11, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10 };
    return rankValues[rank];
  }
};

class Deck
{
private:
  std::array<Card, 52> m_cards{};
  std::size_t m_nextCardIndex{ 0 };

public:
  Deck()
  {
    std::size_t count{ 0 };
    for (auto suit : Card::allSuits)
      for (auto rank : Card::allRanks)
        m_cards[count++] = Card{ rank, suit };
  }

  void shuffle()
  {
    std::shuffle(m_cards.begin(), m_cards.end(), Random::mt);
    m_nextCardIndex = 0;
  }

  Card dealCard()
  {
    assert(m_nextCardIndex != 52 && "Deck::dealCard ran out of cards");
    return m_cards[m_nextCardIndex++];
  }
};

class Entity
{
public:
  Entity()
  {
    m_hand.reserve(7);
  }

  void addCard(const Card& card)
  {
    m_hand.push_back(card);
  }
  
  std::vector<Card> getHand() const
  {
    return m_hand;
  }

  void setScore(int var)
  {
    m_score = var;
  }

  int getScore() const
  {
    return m_score;
  }

  void reset()
  {
    m_hand.clear();
    m_score = 0;
  }

private:
  std::vector<Card> m_hand{};
  int m_score{};
};

void dealCard(Entity& entity, Deck& deck, int cardNum=1)
{
  for (std::size_t index{ 0 }; index < cardNum; index++)
    entity.addCard(deck.dealCard());

  int score{};
  for (const auto& i : entity.getHand())
    score += static_cast<int>(i.value());
  entity.setScore(score);
}

void displayScores(Entity& dealer, Entity& player)
{
  std::cout << "Dealer: ";
  for (const auto& i : dealer.getHand())
    std::cout << i << ' ';
  std::cout << '\n';

  std::cout << "Score: " << dealer.getScore() << "\n\n";

  //displaying player
  std::cout << "Player: ";
  for (const auto& i : player.getHand())
    std::cout << i << ' ';
  std::cout << '\n';

  std::cout << "Score: " << player.getScore() << "\n\n";
}

char getInput()
{
  std::cout << "Hit or stand? [h/s]: ";

  while (true)
  {
    std::string input{};
    std::getline(std::cin >> std::ws, input);

    if (input.size() > static_cast<std::size_t>(1))
    {
      std::cout << "Invalid input. Try again: ";
      continue;
    }

    if ((input[0] == 'h') || (input[0] == 's'))
    {
      return input[0];
    }
    else
    {
      std::cout << "Invalid input. Try again: ";
      continue;
    }
  }
}

int finalDeal(Entity& dealer, Deck& deck)
{
  while (dealer.getScore() < 17)
  {
    dealCard(dealer, deck);
  }

  return dealer.getScore();
}

void reset(Entity& dealer, Entity& player, Deck& deck)
{
  dealer.reset();
  player.reset();

  deck.shuffle();
}

int main()
{
  //initializing
  Deck deck{};

  Entity dealer{};
  Entity player{};

  while (true)
  {
    //shuffling deck
    deck.shuffle();

    //dealing
    dealCard(dealer, deck);

    dealCard(player, deck, 2);

    //displaying dealer
    displayScores(dealer, player);

    while (true)
    {
      if (player.getScore() == 21)
      {
        finalDeal(dealer, deck);

        if (dealer.getScore() == 21)
          std::cout << "Draw!\n";
        else
          std::cout << "Player wins!\n";

        break;
      }
      else
      {
        char input(getInput());
        if (input == 'h')
        {
          dealCard(player, deck);

          if (player.getScore() > 21)
          {
            displayScores(dealer, player);

            std::cout << "Dealer wins!\n";
            break;
          }
          else if (player.getScore() == 21)
          {
            int outcome{ finalDeal(dealer, deck) };

            displayScores(dealer, player);

            if (outcome > 21)
              std::cout << "Player wins!" << '\n';
            else if (outcome < player.getScore())
              std::cout << "Player wins!\n";
            else
              std::cout << "Draw!\n";

            break;
          }
          else
            displayScores(dealer, player);
        }
        else if (input == 's')
        {
          while (dealer.getScore() < 17)
          {
            dealCard(dealer, deck);
          }

          displayScores(dealer, player);

          if (dealer.getScore() > 21)
            std::cout << "Player wins!\n";
          else if (dealer.getScore() > player.getScore())
            std::cout << "Dealer wins!\n";
          else if (dealer.getScore() < player.getScore())
            std::cout << "Player wins!\n";
          else
            std::cout << "Draw!\n";

          break;
        }
        else
        {
          std::cerr << "ERROR::MAIN.CPP::FUNCTION_NAME::CHAR_NOT_EQUAL_TO_CHOICES\n";
        }
      }
    }
      std::cout << "\nAnother round? [y/n]: ";
      char input{};
      while (true)
      {
        std::cin >> input;

        if (std::cin.fail() || (!std::cin.eof() && std::cin.peek() != '\n'))
        {
          std::cin.clear();
          std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
          std::cout << "Invalid input. Try again: ";
          continue;
        }

        if (input == 'y' || input == 'n')
        {
          break;
        }
        else
        {
          std::cout << "Invalid input. Try Again: ";
          continue;
        }
      }
    
    if (input == 'y')
      reset(dealer, player, deck);
    else
      break;
  }

  return 0;
}