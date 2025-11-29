#include <iostream>
#include <string>
#include <bitset>
#include <sstream>
#include "Board.h"
#include <chrono>
#include "zobrist.h"
#include "move.h"
#include <cstring>

struct Board;
Board board;

extern int gameHistory [2][64][64];
extern std::vector<uint64_t> repHistory;

bool makeMove(Move move, Board& board);
Move findBestMove(const Board& board, int depth);
Move parseMove(const std::string& moveStr, const Board& board);
std::string moveToUCI(Move m);

void uciLoop() {
    std::string line;

    board.whiteToMove = true;

    while (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "uci") {
            std::cout << "id name GrandMater\n";
            std::cout << "id author ognjen-simic\n";
            std::cout << "uciok\n";
        }
        else if (token == "isready") {
            std::cout << "readyok\n";
        }
        else if (token == "ucinewgame") {
            board.whitePawns = 0x000000000000FF00ULL;
            board.whiteRooks = (1ULL << 0) | (1ULL << 7);
            board.whiteKnights = (1ULL << 1) | (1ULL << 6);
            board.whiteBishops = (1ULL << 2) | (1ULL << 5);
            board.whiteQueen = (1ULL << 3);
            board.whiteKing = (1ULL << 4);

            board.blackPawns = 0x00FF000000000000ULL;
            board.blackRooks = (1ULL << 56) | (1ULL << 63);
            board.blackKnights = (1ULL << 57) | (1ULL << 62);
            board.blackBishops = (1ULL << 58) | (1ULL << 61);
            board.blackQueen = (1ULL << 59);
            board.blackKing = (1ULL << 60);

            board.whiteToMove = true;

            board.whiteCanCastleKingside = true;
            board.whiteCanCastleQueenside = true;
            board.blackCanCastleKingside = true;
            board.blackCanCastleQueenside = true;
            board.whiteCastled = false;
            board.blackCastled = false;
            board.en_passant = -1;
            board.hash = Zobrist::computeHash(board);
            repHistory.clear();
            repHistory.push_back(board.hash);
            memset(gameHistory, 0, sizeof(gameHistory));
        }
        else if (token == "position") {
            std::string next;
            iss >> next;

            if (next == "startpos") {
                board.whitePawns = 0x000000000000FF00ULL;
                board.whiteRooks = (1ULL << 0) | (1ULL << 7);
                board.whiteKnights = (1ULL << 1) | (1ULL << 6);
                board.whiteBishops = (1ULL << 2) | (1ULL << 5);
                board.whiteQueen = (1ULL << 3);
                board.whiteKing = (1ULL << 4);

                board.blackPawns = 0x00FF000000000000ULL;
                board.blackRooks = (1ULL << 56) | (1ULL << 63);
                board.blackKnights = (1ULL << 57) | (1ULL << 62);
                board.blackBishops = (1ULL << 58) | (1ULL << 61);
                board.blackQueen = (1ULL << 59);
                board.blackKing = (1ULL << 60);

                board.whiteToMove = true;

                board.whiteCanCastleKingside = true;
                board.whiteCanCastleQueenside = true;
                board.blackCanCastleKingside = true;
                board.blackCanCastleQueenside = true;
                board.whiteCastled = false;
                board.blackCastled = false;
                board.en_passant = -1;
                board.hash = Zobrist::computeHash(board);
                repHistory.clear();
                repHistory.push_back(board.hash);
            }

            std::string word;
            if (iss >> word && word == "moves") {
                std::string move;
                while (iss >> move) {
                    Move m = parseMove(move, board);
                    if (m != 0)
                    {
                        makeMove(m, board);
                        repHistory.push_back(board.hash);
                    }
                }
            }
        }
        else if (token == "go") {
            int timeLeft = 5000;
            std::string sub;

            while (iss >> sub) {
                if (sub == "wtime" && board.whiteToMove) {
                    iss >> timeLeft;
                } else if (sub == "btime" && !board.whiteToMove) {
                    iss >> timeLeft;
                }
            }

            int timeToMove = timeLeft / 30;
            timeToMove = std::max(100, timeToMove);

            Move bestMove = findBestMove(board, timeToMove);
            std::cout << "bestmove " << moveToUCI(bestMove) << "\n" << std::flush;
        }
        else if (token == "quit") {
            break;
        }
    }
}