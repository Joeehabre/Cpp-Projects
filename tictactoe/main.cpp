#include <iostream>
#include <vector>
#include <limits>

class TicTacToe {
    std::vector<char> board;
public:
    TicTacToe(): board(9, ' ') {}
    void display() {
        std::cout << "\\n";
        for (int i=0;i<9;i++) {
            std::cout << " " << board[i] << " ";
            if (i%3 != 2) std::cout << "|";
            if (i%3==2 && i!=8) std::cout << "\\n-----------\\n";
        }
        std::cout << "\\n\\n";
    }
    bool movesLeft() {
        for(char c: board) if(c==' ') return true;
        return false;
    }
    char checkWin() {
        int wins[8][3] = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};
        for(auto &w: wins){
            if(board[w[0]]!=' ' && board[w[0]]==board[w[1]] && board[w[1]]==board[w[2]])
                return board[w[0]];
        }
        return ' ';
    }
    int minimax(bool isAI) {
        char winner = checkWin();
        if(winner=='O') return 10;
        if(winner=='X') return -10;
        if(!movesLeft()) return 0;

        int best = isAI ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
        for(int i=0;i<9;i++) if(board[i]==' ') {
            board[i] = isAI ? 'O' : 'X';
            int val = minimax(!isAI);
            board[i] = ' ';
            best = isAI ? std::max(best,val) : std::min(best,val);
        }
        return best;
    }
    void aiMove() {
        int bestVal = std::numeric_limits<int>::min(), move = -1;
        for(int i=0;i<9;i++) if(board[i]==' ') {
            board[i] = 'O';
            int val = minimax(false);
            board[i] = ' ';
            if(val>bestVal){ move=i; bestVal=val; }
        }
        board[move]='O';
    }
    void play() {
        std::cout << "You are X, AI is O\\n";
        while(true) {
            display();
            if(checkWin()=='O'){ std::cout << "AI wins!\\n"; break; }
            if(!movesLeft()){ std::cout << "Draw!\\n"; break; }

            int pos;
            std::cout << "Enter position (1-9): ";
            std::cin >> pos;
            if(pos<1||pos>9||board[pos-1]!=' '){ std::cout << "Invalid move!\\n"; continue; }
            board[pos-1]='X';

            if(checkWin()=='X'){ display(); std::cout << "You win!\\n"; break; }
            if(!movesLeft()){ display(); std::cout << "Draw!\\n"; break; }

            aiMove();
        }
    }
};

int main() {
    TicTacToe game;
    game.play();
    return 0;
}
