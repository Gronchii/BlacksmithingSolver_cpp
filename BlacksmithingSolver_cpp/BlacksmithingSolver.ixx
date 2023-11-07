module;
#include <string>
#include <stack>
#include <memory>


export module Blacksmithing:Solver;
import :Game;

namespace BlacksmithingSolver
{
	using shared_square = std::shared_ptr<BlacksmithingGame::Square>;

	export class Solver
	{
	private:
		BlacksmithingGame::Board _game_board;
		std::string _board_string;
		std::stack<shared_square> played_square_stack;
		unsigned int _num_solns;
		unsigned int _num_attempts;
		bool _end_on_finish;

		bool Backtrack(const int& non_empty_squares, const BlacksmithingGame::Square* sq)
		{
			//If every non empty square has been hit board is completed
			if (!non_empty_squares) {
				_num_solns++;
				return _end_on_finish;
			}
			//std::cout << "Starting " << sq->piece_type << " at (" << sq->row << "," << sq->col << ") - " <<board->board_hash << std::endl;

			for (BlacksmithingGame::Square* targ : sq->target_squares) {

				if (targ->hits_left) {
					//target hits left needs to be 0 or another square may try to hit it again
					targ->hits_left = 0;
					if (Backtrack(non_empty_squares - 1, targ)) {
						//played_square_stack.push(std::make_shared<BlacksmithingGame::Square>(targ));
						played_square_stack.push(std::shared_ptr<BlacksmithingGame::Square>(targ));
						return true;
					}

					//reset target state
					targ->hits_left = 1;
				}
			}
			_num_attempts++;
			return false;
		}

		int FindMaxPossibleSolutionLength()
		{
			//should apply heuristics to board pieces in attempt to determine max soln length possible
			//check for more than 1 square with no valid children
			//check for more than 2 squares with 1 child that go to the same child square. if 2 squares have same only child the path could be parent -> child -> parent -> solution found
			//	if 2 squares check to see if either parent is a child of the child, if neither is then a full solution won't be possible
			//could eventually check for pathing to make sure each square can reach each other square somehow
			//	i.e. board may be subdivided with no way to cross from 1 group to another. max soln would be the largest group
			int output = _game_board.starting_pieces;

			return output;
		}

		bool InitiateBacktrack()
		{
			for (int i = 0; i < 36; i++) {
				if (_game_board.board[i].hits_left) {
					played_square_stack.push(std::make_shared<BlacksmithingGame::Square>(_game_board.board[i]));
					_game_board.board[i].hits_left = 0;
					/*if (Backtrack(_game_board.starting_pieces, &_game_board.board[i])) {
						return true;
					}*/
					_game_board.board[i].hits_left = 1;
					played_square_stack.pop();
				}
			}
			return false;
		}

		bool InitiateBacktrack(shared_square sq)
		{
			return false;
			//return Backtrack(_game_board.starting_pieces, sq.get());
		}


	public:
		Solver(std::string board_string) :
			_board_string(board_string),
			_game_board(BlacksmithingGame::Board{ _board_string })
		{}

		std::stack<shared_square> FindSolution() {
			_end_on_finish = true;
			_num_attempts = 0;
			_num_solns = 0;

			return played_square_stack;
		}

		std::stack<shared_square> FindSolution(BlacksmithingGame::Square& starting_square) {
			_end_on_finish = true;
			_num_attempts = 0;
			_num_solns = 0;

			return played_square_stack;
		}

		//return soln/attempts pair instead?
		unsigned int FindNumSolns() {
			_num_attempts = 0;
			_num_solns = 0;
			_end_on_finish = false;

			return _num_solns;
		}

		unsigned int FindNumSolns(BlacksmithingGame::Square& starting_square) {
			_num_attempts = 0;
			_num_solns = 0;
			_end_on_finish = false;

			return _num_solns;
		}



	};
}
