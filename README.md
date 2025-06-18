# C++ Chess Engine

This is a chess engine I'm currently building in C++ as a personal learning project.

## Current Features

- Bitboard-based representation for each piece type
- Legal move generation for knights and pawns
- User input using standard algebraic notation (e.g. g1f3)
- Capturing the opponent’s knight
- Turn switching between White and Black

## Upcoming Features

- Legal move generation for bishops, rooks, queen, and king
- Check, checkmate, and stalemate detection
- Minimax algorithm with a basic evaluation function
- UCI connection to Lichess for playing against other bots

## How to Compile and Run

Make sure you have a C++ compiler installed (e.g. g++).

To compile and run:

```bash
cd chess-engine-in-cpp
g++ -std=c++17 main.cpp -o main.exe
main.exe
```

## About This Project

This is my first project that I gave myself for my future goal of becoming a software engineer. Even though C++ is not commonly used in software engineering I thought a chess engine would be a good way to start coding serious projects.