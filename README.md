 # ♦️ Blackjack Bot ♦️

This repository contains a simple C++ blackjack bot designed for command-line interaction. It offers the ability to run the game, as well as conduct profit benchmarks.

## Usage
```bash
blackjack [OPTIONS] [COMMAND] [ARGUMENTS]
```
## Options
- `-v`: Enable verbose output

## Commands
- `run <Money> <Bet> <Rounds>`: Run the game with specified money, bet amount and number of rounds to play.
- `benchmark <Money> <Bet> <Iterations> <Rounds_vector>`: Conduct profit benchmarks with specified money, bet amount, a given number of iterations, and an optional vector of round numbers for more detailed analysis.

## Arguments
- `Money`: Starting money for the game or profit benchmarking.
- `Bet`: Amount of money to bet per hand.
- `Rounds`: Number of rounds to play in the game.
- `Iterations`: Number of times to conduct the profit benchmarks. (Optional when running the game)
- `Rounds_vector`: Vector of round numbers for more detailed profit analysis during the benchmarking process. (Optional when conducting profit benchmarks)

## Learn more
[Stanford CS221](https://stanford-cs221.github.io/spring2020-extra/lectures/games1.pdf)
