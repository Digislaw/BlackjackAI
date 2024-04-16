#include <iostream>
#include "Menu.h"
#include "Blackjack.h"

using namespace std;

namespace Menu
{
	void Help()
	{
		cout << "Usage: blackjack [OPTIONS] [COMMAND] [ARGUMENTS]" << endl << endl
			<< "Options:" << endl
			<< "-v \t enable verbose output" << endl << endl
			<< "Commands:" << endl
			<< "run [Money] [Bet] [Rounds] \t\t\t\t Run the game" << endl
			<< "benchmark [Money] [Bet] [Iterations] [Rounds vector] \t The profit benchmark" << endl 
			<< "help \t\t\t\t\t\t\t Display commands & arguments" << endl << endl
			<< "Arguments:" << endl
			<< "Money \t\t Starting money" << endl
			<< "Bet \t\t The amount of money to bet" << endl
			<< "Rounds \t\t Number of rounds to play" << endl
			<< "Iterations \t Number of benchmark iterations" << endl;
	}

	int RunCommand(CommandLineInterface& cli)
	{
		if (cli.params.size() < 3)
		{
			return EXIT_FAILURE;
		}

		int balance = Simulate(cli.params[0], cli.params[1], cli.params[2], cli.flags["verbose"]);

		if (!cli.flags["verbose"])
		{
			cout << "Money: " << balance << endl;
		}

		return EXIT_SUCCESS;
	}

	int BenchmarkCommand(CommandLineInterface& cli)
	{
		if (cli.params.size() < 3)
		{
			return EXIT_FAILURE;
		}

		vector<unsigned int> rounds;
		for (size_t n = 3; n < cli.params.size(); n++)
		{
			rounds.push_back(cli.params[n]);
		}

		Benchmark(cli.params[0], cli.params[1], rounds, cli.params[2]);
		return EXIT_SUCCESS;
	}
}