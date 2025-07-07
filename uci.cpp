#include <iostream>
#include <string>
#include <bitset>
#include <sstream>

bool makeMove(
    const std::string& move,
    std::bitset<64>& whiteKnights, std::bitset<64>& blackKnights,
    std::bitset<64>& whitePawns, std::bitset<64>& blackPawns,
    std::bitset<64>& whiteRooks, std::bitset<64>& blackRooks,
    std::bitset<64>& whiteBishops, std::bitset<64>& blackBishops,
    std::bitset<64>& whiteQueen, std::bitset<64>& blackQueen,
    std::bitset<64>& whiteKing, std::bitset<64>& blackKing,
    std::bitset<64>& whitePieces, std::bitset<64>& blackPieces,
    bool whiteToMove,
    std::bitset<64>& allPieces,
    std::bitset<64>& ownPieces);

std::bitset<64> getWhitePieces(const std::bitset<64>& whitePawns,
                               const std::bitset<64>& whiteRooks,
                               const std::bitset<64>& whiteKnights,
                               const std::bitset<64>& whiteBishops,
                               const std::bitset<64>& whiteQueen,
                               const std::bitset<64>& whiteKing);

std::bitset<64> getBlackPieces(const std::bitset<64>& blackPawns,
                               const std::bitset<64>& blackRooks,
                               const std::bitset<64>& blackKnights,
                               const std::bitset<64>& blackBishops,
                               const std::bitset<64>& blackQueen,
                               const std::bitset<64>& blackKing);

std::bitset<64> getAllPieces(const std::bitset<64>& whitePawns,
                             const std::bitset<64>& whiteRooks,
                             const std::bitset<64>& whiteKnights,
                             const std::bitset<64>& whiteBishops,
                             const std::bitset<64>& whiteQueen,
                             const std::bitset<64>& whiteKing,
                             const std::bitset<64>& blackPawns,
                             const std::bitset<64>& blackRooks,
                             const std::bitset<64>& blackKnights,
                             const std::bitset<64>& blackBishops,
                             const std::bitset<64>& blackQueen,
                             const std::bitset<64>& blackKing);

void uciLoop() {
    std::string line;

    std::bitset<64> whitePawns(0x000000000000FF00ULL);
    std::bitset<64> whiteRooks((1ULL << 0) | (1ULL << 7));
    std::bitset<64> whiteKnights((1ULL << 1) | (1ULL << 6));
    std::bitset<64> whiteBishops((1ULL << 2) | (1ULL << 5));
    std::bitset<64> whiteQueen(1ULL << 3);
    std::bitset<64> whiteKing(1ULL << 4);

    std::bitset<64> blackPawns(0x00FF000000000000ULL);
    std::bitset<64> blackRooks((1ULL << 56) | (1ULL << 63));
    std::bitset<64> blackKnights((1ULL << 57) | (1ULL << 62));
    std::bitset<64> blackBishops((1ULL << 58) | (1ULL << 61));
    std::bitset<64> blackQueen(1ULL << 59);
    std::bitset<64> blackKing(1ULL << 60);

    bool whiteToMove = true;

    while (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "uci") {
            std::cout << "id name MyChessEngine\n";
            std::cout << "id author ognjen-simic\n";
            std::cout << "uciok\n";
        }
        else if (token == "isready") {
            std::cout << "readyok\n";
        }
        else if (token == "ucinewgame") {
            whitePawns = 0x000000000000FF00ULL;
            whiteRooks = (1ULL << 0) | (1ULL << 7);
            whiteKnights = (1ULL << 1) | (1ULL << 6);
            whiteBishops = (1ULL << 2) | (1ULL << 5);
            whiteQueen = (1ULL << 3);
            whiteKing = (1ULL << 4);

            blackPawns = 0x00FF000000000000ULL;
            blackRooks = (1ULL << 56) | (1ULL << 63);
            blackKnights = (1ULL << 57) | (1ULL << 62);
            blackBishops = (1ULL << 58) | (1ULL << 61);
            blackQueen = (1ULL << 59);
            blackKing = (1ULL << 60);

            whiteToMove = true;
        }
        else if (token == "position") {
            std::string next;
            iss >> next;

            if (next == "startpos") {
                whitePawns = 0x000000000000FF00ULL;
                whiteRooks = (1ULL << 0) | (1ULL << 7);
                whiteKnights = (1ULL << 1) | (1ULL << 6);
                whiteBishops = (1ULL << 2) | (1ULL << 5);
                whiteQueen = (1ULL << 3);
                whiteKing = (1ULL << 4);

                blackPawns = 0x00FF000000000000ULL;
                blackRooks = (1ULL << 56) | (1ULL << 63);
                blackKnights = (1ULL << 57) | (1ULL << 62);
                blackBishops = (1ULL << 58) | (1ULL << 61);
                blackQueen = (1ULL << 59);
                blackKing = (1ULL << 60);

                std::string moves;
                iss >> moves;
                std::string move;
                while (iss >> move) {
                    std::bitset<64> whitePieces = getWhitePieces(whitePawns, whiteRooks, whiteKnights, whiteBishops, whiteQueen, whiteKing);
                    std::bitset<64> blackPieces = getBlackPieces(blackPawns, blackRooks, blackKnights, blackBishops, blackQueen, blackKing);
                    std::bitset<64> allPieces = getAllPieces(whitePawns, whiteRooks, whiteKnights, whiteBishops, whiteQueen, whiteKing,
                                                             blackPawns, blackRooks, blackKnights, blackBishops, blackQueen, blackKing);
                    std::bitset<64> ownPieces = whiteToMove ? whitePieces : blackPieces;

                    makeMove(move, whiteKnights, blackKnights, whitePawns, blackPawns,
                             whiteRooks, blackRooks, whiteBishops, blackBishops,
                             whiteQueen, blackQueen, whiteKing, blackKing,
                             whitePieces, blackPieces, whiteToMove, allPieces, ownPieces);

                    whiteToMove = !whiteToMove;
                }
            }
        }
        else if (token == "go") {
            if (whiteToMove) {
                std::cout << "bestmove e2e4\n";
            } else {
                std::cout << "bestmove e7e5\n";
            }
        }
        else if (token == "quit") {
            break;
        }
    }
}