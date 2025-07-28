# C++ Chess Engine

This is a chess engine I'm currently building in C++ as a personal learning project. You can play against the engine on Lichess under the name "Grand_Mater_Bot".

## Features

- Bitboard-based representation for each piece type
- Legal move generation for every piece, castling and en passant
- User input using standard algebraic notation (e.g. g1f3)
- Capturing the opponent’s pieces
- Turn switching between White and Black
- Check detection
- Checkmate and stalemate detection
- Minimax algorithm with evaluation function
- Time management for every move
- Quiescence search for checks and captures
- Transposition tables using zobrist hashing
- UCI connection to Lichess for playing against other bots

## Left to do

- Improving the engine`s performance

## How to Compile and Run

Make sure you have a C++ compiler installed (e.g. g++).

To compile and run:

```bash
cd chess-engine-in-cpp
g++ -std=c++17 main.cpp uci.cpp board.cpp -o engine.exe
.\engine.exe
```

## About This Project

This is my first project that I gave myself. Even though C++ is not commonly used in software engineering I thought a chess engine would be a good way to start coding serious projects.

