# C++ Chess Engine

This is a chess engine I built in C++ as a personal learning project. You can play against the engine on [Lichess](https://lichess.org/@/Grand_Mater_Bot) under the name "Grand_Mater_Bot".
(**Note**: the bot is currently **not online 24/7**.)

## Features

- Bitboard-based representation for each piece type
- Legal move generation for every piece, castling and en passant
- User input using standard algebraic notation (**g1f3**)
- Check, checkmate and stalemate detection
- Minimax algorithm with alpha-beta pruning
- Evaluation with piece-square tables and king safety
- Time management for every move
- Quiescence search for checks and captures
- Transposition tables using zobrist hashing
- UCI connection to **Lichess** for playing against other bots

## Possible updates in the future

- Improving the engine`s performance

## About the engine

The engine's search is not very fast, since the move generation uses `std::string` instead of `uint16_t`. This can lead to the engine missing simple material wins in a few moves, but it also manages to play the best move sometimes. It often gets stuck in the search when it knows it´s about to lose, and proceeds to lose on time. The overall **elo rating** should be around **1600**.

## How to Compile and Run

Make sure you have a C++ compiler installed (`g++`).

To compile and run:

```bash
cd chess-engine-in-cpp
g++ -std=c++17 main.cpp uci.cpp board.cpp zobrist.cpp tt.cpp -o engine.exe
.\engine.exe
```
