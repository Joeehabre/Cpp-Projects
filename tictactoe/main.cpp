#include <array>
#include <iostream>
#include <limits>
#include <string>

class TicTacToe {
    std::array<char, 9> board_{};
    int moves_made_ = 0;

    static constexpr int WIN_LINES[8][3] = {
        {0,1,2},{3,4,5},{6,7,8},   // rows
        {0,3,6},{1,4,7},{2,5,8},   // cols
        {0,4,8},{2,4,6}            // diagonals
    };

    char check_win() const {
        for (auto& w : WIN_LINES)
            if (board_[w[0]] != ' ' &&
                board_[w[0]] == board_[w[1]] &&
                board_[w[1]] == board_[w[2]])
                return board_[w[0]];
        return ' ';
    }

    // Alpha-beta minimax.  Depth is used to prefer faster wins.
    int minimax(bool is_ai, int depth, int alpha, int beta) const {
        char winner = check_win();
        if (winner == 'O') return 10 - depth;
        if (winner == 'X') return depth - 10;
        if (moves_made_ == 9) return 0;

        // const_cast is safe here — we restore the square before returning.
        auto& self = const_cast<TicTacToe&>(*this);

        if (is_ai) {
            int best = std::numeric_limits<int>::min();
            for (int i = 0; i < 9; ++i) {
                if (board_[i] != ' ') continue;
                self.board_[i] = 'O';
                ++self.moves_made_;
                best = std::max(best, minimax(false, depth + 1, alpha, beta));
                self.board_[i] = ' ';
                --self.moves_made_;
                alpha = std::max(alpha, best);
                if (beta <= alpha) break;
            }
            return best;
        } else {
            int best = std::numeric_limits<int>::max();
            for (int i = 0; i < 9; ++i) {
                if (board_[i] != ' ') continue;
                self.board_[i] = 'X';
                ++self.moves_made_;
                best = std::min(best, minimax(true, depth + 1, alpha, beta));
                self.board_[i] = ' ';
                --self.moves_made_;
                beta = std::min(beta, best);
                if (beta <= alpha) break;
            }
            return best;
        }
    }

    void ai_move() {
        int best_val = std::numeric_limits<int>::min();
        int best_sq  = -1;
        for (int i = 0; i < 9; ++i) {
            if (board_[i] != ' ') continue;
            board_[i] = 'O';
            ++moves_made_;
            int val = minimax(false, 0, std::numeric_limits<int>::min(),
                                         std::numeric_limits<int>::max());
            board_[i] = ' ';
            --moves_made_;
            if (val > best_val) { best_val = val; best_sq = i; }
        }
        board_[best_sq] = 'O';
        ++moves_made_;
    }

    void display() const {
        std::cout << "\n";
        for (int i = 0; i < 9; ++i) {
            std::cout << " " << board_[i] << " ";
            if (i % 3 != 2)  std::cout << "|";
            if (i % 3 == 2 && i != 8) std::cout << "\n-----------\n";
        }
        std::cout << "\n\n";
    }

    // Returns 'X', 'O', 'D' (draw), or '\0' (game ongoing).
    char game_over() const {
        char w = check_win();
        if (w != ' ') return w;
        if (moves_made_ == 9) return 'D';
        return '\0';
    }

public:
    TicTacToe() { board_.fill(' '); }

    void play() {
        std::cout << "Board positions:\n"
                     " 1 | 2 | 3 \n"
                     "-----------\n"
                     " 4 | 5 | 6 \n"
                     "-----------\n"
                     " 7 | 8 | 9 \n\n"
                     "You are X, AI is O.\n";

        while (true) {
            // --- Human turn ---
            display();
            if (char result = game_over(); result != '\0') {
                if      (result == 'X') std::cout << "You win!\n";
                else if (result == 'O') std::cout << "AI wins!\n";
                else                    std::cout << "Draw!\n";
                return;
            }

            int pos;
            std::cout << "Enter position (1-9): ";
            while (true) {
                if (std::cin >> pos) {
                    if (pos >= 1 && pos <= 9 && board_[pos - 1] == ' ') break;
                } else {
                    if (std::cin.eof()) return;
                    std::cin.clear();
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid — enter a free square (1-9): ";
            }
            board_[pos - 1] = 'X';
            ++moves_made_;

            // --- Check after human ---
            if (char result = game_over(); result != '\0') {
                display();
                if      (result == 'X') std::cout << "You win!\n";
                else if (result == 'O') std::cout << "AI wins!\n";
                else                    std::cout << "Draw!\n";
                return;
            }

            // --- AI turn ---
            std::cout << "AI is thinking...\n";
            ai_move();
        }
    }
};

int main() {
    char again = 'y';
    while (again == 'y' || again == 'Y') {
        TicTacToe game;
        game.play();
        std::cout << "\nPlay again? (y/n): ";
        std::cin >> again;
    }
    std::cout << "Thanks for playing!\n";
    return 0;
}
