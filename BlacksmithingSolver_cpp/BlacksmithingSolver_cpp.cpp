#include <iostream>
#include <vector>
#include <random>
#include <stack>
#include <string>
#include <chrono>
#include <map>

//import Blacksmithing;
//import BlacksmithingSolver;
//import Blacksmithing:Game;
//import Blacksmithing:Solver;
import Blacksmithing;

std::random_device rd;
std::mt19937_64 eng(rd());
std::uniform_int_distribution<unsigned long long> distr;


unsigned long long board_hash_maps[36][10];
unsigned long long current_piece_hash_maps[36][10];
int completion_attempts{ 0 };
//std::vector<long long> seen_hashes{};
std::map<long long, int> seen_hashes{};


std::stack<BlacksmithingGame::Square*> played_square_stack;
long long transpositions{ 0 };


bool Backtrack(BlacksmithingGame::Board* board,const int& non_empty_squares,const BlacksmithingGame::Square* sq) {
    //If every non empty square has been hit board is completed
    if (!non_empty_squares) {
        return true;
    }
    //std::cout << "Starting " << sq->piece_type << " at (" << sq->row << "," << sq->col << ") - " <<board->board_hash << std::endl;
    //remove sq current piece hash
    board->board_hash ^= current_piece_hash_maps[sq->id][sq->piece_id];
    for (BlacksmithingGame::Square* targ : sq->target_squares) { 
        
        if (targ->hits_left) {
            //adjust hash here to new current piece and board hash
            //to do
            //remove targ board hash value from hash
            board->board_hash ^= board_hash_maps[targ->id][targ->piece_id];
            //add targ current piece hash
            board->board_hash ^= current_piece_hash_maps[targ->id][targ->piece_id];
            //std::cout << "Attempting " << targ->piece_type << " at (" << targ->row << "," << targ->col << ") - " << board->board_hash << std::endl;
            //target hits left needs to be 0 or another square may try to hit it again
            targ->hits_left = 0;

            //hash has already been seen and failed, dont need to check again
            /*if (std::find(seen_hashes.begin(), seen_hashes.end(), board->board_hash) == seen_hashes.end()) {
                transpositions++;
            }*/
            if (non_empty_squares<4 &&  seen_hashes.contains(board->board_hash)) {
                //std::cout << board->board_hash << " - " << seen_hashes[board->board_hash] << std::endl;
                transpositions++;
                seen_hashes[board->board_hash]++;
                //std::cout << "Terminating hash - " << board->board_hash << " : visited " << seen_hashes.at(board->board_hash) << "times : Total transpositions found - " << transpositions << std::endl;
            }
            //hash not seen yet, check to see if it works
            else {
                if (Backtrack(board, non_empty_squares - 1, targ)) {
                    played_square_stack.push(targ);
                    return true;
                }
                if (non_empty_squares < 4)
                {
                    seen_hashes.emplace(board->board_hash, 0);
                }
            }
            //reset hash if false
            //readd targ hash value to hash
            board->board_hash ^= board_hash_maps[targ->id][targ->piece_id];
            //remove targ current piece hash
            board->board_hash ^= current_piece_hash_maps[targ->id][targ->piece_id];

            //reset target state
            //seen_hashes.push_back(board->board_hash);
            targ->hits_left = 1;
        }
    }
    //add back sq current piece hash
    board->board_hash ^= current_piece_hash_maps[sq->id][sq->piece_id];
    completion_attempts++;
    
    return false;
}

//Get to choose initial square
bool InitBacktracking(BlacksmithingGame::Board& board) {
    for (int i = 0; i < 36; i++) {
        if (board.board[i].hits_left) {
            played_square_stack.push(&board.board[i]);
            board.board[i].hits_left = 0;
            if (Backtrack(&board,board.starting_pieces, &board.board[i])) {
                return true;
            }
            board.board[i].hits_left = 1;
            played_square_stack.pop();
        }
    }


    return false;
}

//Starting from a specified last square hit
bool InitBacktracking(BlacksmithingGame::Board& board, BlacksmithingGame::Square& init_square) {
    return Backtrack(&board,board.starting_pieces, &init_square);
}

int main()
{
    //Initialize board hashes with random longs
    for (int i = 0; i < 36; i++) {
        for (int j = 0; j < 10; j++) {
            board_hash_maps[i][j] = distr(eng);
            current_piece_hash_maps[i][j] = distr(eng);
        }
    }

    //board is input rowwise starting top left. empty squares should be given x
    std::string board_string{"3k2j1bk1kk1kx1141xrx22kr21r3krq14k1r"};
    char starting_piece_type{ 'k' };

    int starting_row{ 0 };
    int starting_col{ 0 };

    BlacksmithingGame::Board game{ board_string };
    BlacksmithingGame::Square starting_square(6 * starting_row + starting_col, starting_piece_type);
    
    
    starting_square.AssignTargetSquares(game.board);

    starting_square.PrintSquare();

    //set intial board hash
    game.board_hash ^= current_piece_hash_maps[starting_square.id][starting_square.piece_id];
    for (int i = 0; i < 36; i++) {
        game.board_hash ^= board_hash_maps[i][game.board[i].piece_id];
        game.board[i].square_hash = board_hash_maps[i][game.board[i].piece_id];
    }

    std::cout << "Starting hash: " << game.board_hash << std::endl;
    //std::cout<<game.PrintBoard();

    auto clock_start = std::chrono::steady_clock::now();


    bool backtrack_result = InitBacktracking(game, starting_square);

    auto clock_end = std::chrono::steady_clock::now();

    std::cout << "Ending hash: " << game.board_hash << std::endl;
    std::cout << "Finished in " << std::chrono::duration_cast<std::chrono::milliseconds>(clock_end - clock_start).count() << "ms" << std::endl;
    std::cout << "Transpositions observed: " << transpositions << std::endl;
    std::cout << "Attempts made: " << completion_attempts << std::endl;
    std::cout << "~"<<1000*(completion_attempts / std::chrono::duration_cast<std::chrono::milliseconds>(clock_end - clock_start).count()) << " attempts/s" << std::endl;
    
    if (backtrack_result) {
        std::cout << "Board completed after " << completion_attempts << " attempts" << std::endl;
        std::cout << played_square_stack.size() << std::endl;
        std::string out_string{};
        while (played_square_stack.size()) {
            out_string = out_string + played_square_stack.top()->GetCoords()+"\n" ;
            played_square_stack.pop();
        }
        std::cout << out_string << std::endl;
    }
}
