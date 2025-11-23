# C++ Chess Engine

This is a chess engine I built in C++ as a personal learning project. You can play against the engine on [Lichess](https://lichess.org/@/Grand_Mater_Bot) under the name **"Grand_Mater_Bot"**.
(**Note**: the bot is currently **not online 24/7**.)

## Features

- **Bitboard-based** representation for each piece type
- Legal move generation for every piece, castling and en passant
- Check, checkmate and stalemate detection
- **Minimax** algorithm with **alpha-beta** pruning
- Evaluation with **piece-square tables**, **king safety** and **pawn structure**
- Time management for every move
- **Quiescence search** for captures
- **Transposition tables** using **zobrist** hashing
- **UCI** connection to **Lichess** for playing against other bots

## Possible updates in the future

- Switching to **uint16_t** based moves instead of **std::string**. 

## How to Compile and Run

Make sure you have a C++ compiler installed (`g++`).

To compile and run:

```bash
cd chess-engine-in-cpp
g++ -std=c++17 main.cpp uci.cpp board.cpp zobrist.cpp tt.cpp -o engine.exe
.\engine.exe
```
