#pragma once
#include <vector>
#include <map>

using namespace std;

namespace Menu
{
	class CommandLineInterface
	{
		public:
			map<string, bool> flags;
			string command;
			vector<int> params;

			CommandLineInterface(int argc, char* argv[])
			{
				size_t paramIndex = 2;     // indeks opcji
				string options(argv[1]);   // ³añcuch znaków opcji

				// je¿eli u¿ytkownik wybiera jakieœ opcje
				if (options[0] == '-')
				{
					paramIndex = 3;

					for (size_t i = 1; i < options.length(); i++)
					{
						switch (options[i])
						{
							case 'v':
								flags["verbose"] = true;
								break;
						}
					}
				}

				command = argv[paramIndex - 1];

				for (; paramIndex < argc; paramIndex++)
				{
					params.push_back(atoi(argv[paramIndex]));
				}
			}
	};

	void Help();
	int RunCommand(CommandLineInterface& cli);
	int BenchmarkCommand(CommandLineInterface& cli);
}