#include <iostream>
#include "Menu.h"
#include "Blackjack.h"

using namespace std;

// Symuluj N rozgrywek
int Simulate(int money, int bet, unsigned int rounds, bool verbose)
{
	int current;
	char sign;

	for (unsigned int i = 0; i < rounds; i++)
	{
		Blackjack game(bet);
		current = game.Play(verbose);
		money += current;
		sign = current >= 0 ? '+' : '\0';

		if (verbose)
		{
			cout << "Money: " << money << 
				" (" << sign << current << ")" << endl;
		}
	}

	return money;
}

// Wyznacz średni stan konta N rozgrywek
float AverageMoney(int money, int bet, unsigned int rounds, unsigned int benchmarkIterations)
{
	float avg = 0.0f;

	for (unsigned int i = 0; i < benchmarkIterations; i++)
	{
		avg += Simulate(money, bet, rounds, false);
	}

	return avg / benchmarkIterations;
}

// Benchmark średniego zysku
void Benchmark(int money, int bet, const vector<unsigned int>& roundsVector, unsigned int benchmarkIterations)
{
	cout << "Rounds \t Profit" << endl;

	for (const auto& rounds : roundsVector)
	{
		cout << rounds << "\t" << AverageMoney(money, bet, rounds, benchmarkIterations) - money << endl;
	}
}

int main(int argc, char* argv[])
{
	int exitCode = 0;

	if (argc < 2)
	{
		Menu::Help();
		return EXIT_SUCCESS;
	}

	Menu::CommandLineInterface cli(argc, argv);

	if (cli.command == "run")
	{
		exitCode = Menu::RunCommand(cli);
	}
	else if (cli.command == "benchmark")
	{
		exitCode = Menu::BenchmarkCommand(cli);
	}
	else
	{
		Menu::Help();
	}

	// wystąpił błąd - na przykład niepełna lista parametrów
	if (exitCode)
	{
		Menu::Help();
	}

	return EXIT_SUCCESS;
}