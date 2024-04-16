#pragma once
#include <string>
#include <random>

using namespace std;

const size_t DECK_SIZE = 13;   // liczba kart bez uwzgl�dniania kolor�w
const unsigned int DEALER_TARGET_VALUE = 17;   // krupier pobiera karty do osi�gni�cia tej liczby

// Karta
struct Card
{
	unsigned int value;   // warto�� karty w punktach
	size_t count;         // liczba kart tego typu w talii
	string symbol;        // symbol karty (2-10, J, Q, K, A)
};

// Stan gry wykorzystywany przez Expectimax
struct GameState
{
	float score;  // bie��ca liczba punkt�w
	bool stand;   // czy zako�czono wykonywanie ruch�w
	int bet;      // zak�ad
};

// Mo�liwe ruchy
enum class Move
{
	NONE,
	STAND,
	HIT,
	DOUBLE_DOWN
};

class Player
{
	vector<string> cards;    // przypisane do gracza karty w postaci symboli
	string name;

public:
	unsigned int score = 0;	 // suma punkt�w
	bool stop = false;       // czy gracz zako�czy� swoje ruchy

	explicit Player(const string& name) : name(name) {}

	// Wydruk kart
	void PrintCards()
	{
		cout << name << ": ";

		for (const auto& card : cards)
		{
			cout << card << " ";
		}

		cout << " (" << score << ")" << endl;
	}

	// Dodaj kart�
	void AddCard(const Card& card)
	{
		cards.push_back(card.symbol);
		score += card.value;
	}

	// Dodaj ukryt� kart� (krupier)
	void AddSecret()
	{
		cards.push_back("?");
	}

	// Ods�o� ukryt� kart�
	void ShowSecretCard(const Card& card)
	{
		cards[0] = card.symbol;
		score += card.value;
	}

	// Liczba kart
	size_t CardsNum() const
	{
		return cards.size();
	}
};

class Blackjack
{
	// generowanie liczb losowych
	random_device rd;
	mt19937 rng;
	uniform_int_distribution<int> distr;

	// gra
	vector<Card> cards;
	Player dealer;	// krupier
	Player player;	// gracz AI
	int bet;  // aktualny zak�ad
	Card secretDealerCard;  // ukryta karta krupiera
	int money;
	bool verbose;  // aktywuje dodatkowe informacje dla cel�w dydaktycznych
	bool doubleDownPossible = true;

	// �rednia warto�� karty, przyjmuj�c asa jako 1
	const float averageValue = (1.0 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10 + 10 + 10 + 10) / DECK_SIZE;

	// Pobierz kart�
	Card Pick()
	{
		size_t i = distr(rng);

		while (cards[i].count == 0)
		{
			i = distr(rng);
		}

		cards[i].count--;
		return cards[i];
	}

	// Nast�pny stan
	GameState NextState(float score, Move move, int bet)
	{
		GameState state{};
		state.bet = bet;

		switch (move)
		{
			case Move::STAND:
				state.score = score;
				state.stand = true;
				break;

			case Move::HIT:
				state.score = score + averageValue;
				state.stand = false;
				break;

			case Move::DOUBLE_DOWN:
				state.score = score + averageValue;
				state.stand = true;
				state.bet *= 2;
				break;
		}

		return state;
	}

	// W�z�y "szansy"
	float ChanceNode(GameState state, float dealerScore, bool isMaximizingPlayer)
	{
		GameState newState;
		pair<float, Move> child;

		vector<Move> possibleMoves = { Move::STAND, Move::HIT };
		float newScore = 0;

		for (const auto& move : possibleMoves)
		{
			newState = NextState(state.score, move, state.bet);
			child = Expectimax(newState, newState.bet, dealerScore, false);
			newScore += child.first / possibleMoves.size();
		}

		return newScore;
	}

	// W�z�y max
	pair<float, Move> MaxNode(GameState state, float dealerScore, bool isMaximizingPlayer)
	{
		GameState newState;
		pair<float, Move> child;
		pair<float, Move> result;

		vector<Move> possibleMoves = { Move::STAND, Move::HIT };

		if (doubleDownPossible)
		{
			possibleMoves.push_back(Move::DOUBLE_DOWN);
		}

		result.first = INT_MIN;

		for (const auto& move : possibleMoves)
		{
			newState = NextState(state.score, move, state.bet);
			child = Expectimax(newState, newState.bet, dealerScore, false);

			if (child.first > result.first)
			{
				result.first = child.first;
				result.second = move;
			}
		}

		if (verbose)
		{
			cout << endl << "Expected dealer's score: " << dealerScore
				 << endl << "Utility value (expected change): " << result.first
				 << endl;
		}

		return result;
	}

	// Algorytm expectimax
	pair<float, Move> Expectimax(GameState state, int bet, float dealerScore, bool isMaximizingPlayer)
	{

		if (state.stand || state.score > 21)
		{
			if (state.score > 21)
			{
				return { -bet, Move::NONE };
			}
			else if (dealerScore > 21)
			{
				return { bet, Move::NONE };
			}
			else if (dealerScore > state.score)
			{
				return { -bet, Move::NONE };
			}
			else if (state.score > dealerScore)
			{
				return { bet, Move::NONE };
			}
			else
			{
				return { 0, Move::NONE };
			}

		}

		if (isMaximizingPlayer)
		{
			return MaxNode(state, dealerScore, isMaximizingPlayer);
		}
		else
		{
			return { ChanceNode(state, dealerScore, isMaximizingPlayer), Move::NONE };
		}
	}

	// Ruch krupiera
	void DealerMove()
	{
		if (!dealer.stop)
		{
			dealer.AddCard(Pick());

			if (dealer.score + secretDealerCard.value >= DEALER_TARGET_VALUE)
			{
				dealer.stop = true;
			}
		}
	}

	// Ruch gracza SI
	void PlayerMove()
	{
		// szacowanie punkt�w krupiera
		float expectedDealerScore = dealer.score;

		while (expectedDealerScore < DEALER_TARGET_VALUE)
		{
			expectedDealerScore += averageValue;
		}

		GameState state{};
		state.score = player.score;
		state.stand = player.stop;
		state.bet = bet;

		pair<float, Move> action = Expectimax(state, bet, expectedDealerScore, true);

		switch (action.second)
		{
			case Move::STAND:
				player.stop = true;
				if (verbose) cout << "Move: STAND" << endl << endl;
				break;

			case Move::HIT:
				player.AddCard(Pick());
				if (verbose) cout << "Move: HIT" << endl << endl;
				break;

			case Move::DOUBLE_DOWN:
				player.AddCard(Pick());
				player.stop = true;
				bet *= 2;
				if (verbose) cout << "Move: DOUBLE DOWN" << endl << endl;
				break;
		}

		doubleDownPossible = false; // wykonano ju� ruch
	}

public:

	Blackjack(int bet, size_t decksNum = 4) : rng(rd()), distr(0, DECK_SIZE - 1),
		cards(DECK_SIZE), dealer("D"), player("P"), bet(bet), money(money)
	{
		size_t count = 4 * decksNum;
		string symbols[] = { "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A" };
		int values[] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 1 };

		for (size_t i = 0; i < DECK_SIZE; i++)
		{
			cards[i].value = values[i];
			cards[i].count = count;
			cards[i].symbol = symbols[i];
		}
	}

	Blackjack(const Blackjack&) = delete;
	Blackjack(Blackjack&&) = delete;

	// Zagraj jedn� rund�
	int Play(bool verboseOutput = true)
	{
		verbose = verboseOutput;

		secretDealerCard = Pick();
		dealer.AddSecret();
		dealer.AddCard(Pick());

		player.AddCard(Pick());
		player.AddCard(Pick());

		if (verbose)
		{
			dealer.PrintCards();
			player.PrintCards();
		}

		while (!player.stop || !dealer.stop)
		{
			DealerMove();
			PlayerMove();

			if (verbose)
			{
				dealer.PrintCards();
				player.PrintCards();
			}

			if (player.score > 21)
			{
				player.stop = true;
			}
		}

		dealer.ShowSecretCard(secretDealerCard);

		if (verbose)
		{
			dealer.PrintCards();
			player.PrintCards();
		}

		if (player.score > 21)
		{
			return -bet;
		}
		else if (dealer.score <= 21 && dealer.score > player.score)
		{
			return -bet;
		}
		else if (dealer.score == player.score)
		{
			return 0;
		}
		else if (player.score == 21 && player.CardsNum() == 2)
		{
			return 2 * bet;
		}

		return bet;
	}
};


// Symuluj N rozgrywek
int Simulate(int money, int bet, unsigned int rounds, bool verbose);

// Wyznacz �redni stan konta N rozgrywek
float AverageMoney(int money, int bet, unsigned int rounds, unsigned int benchmarkIterations);

// Benchmark �redniego zysku
void Benchmark(int money, int bet, const vector<unsigned int>& roundsVector, unsigned int benchmarkIterations);
