#include <vector>
#include <ranges>
#include <string>
#include <map>

export module Blacksmithing;



namespace BlacksmithingGame {
	
	export class Square {
	private:
		std::map<char, int> _piece_id_map{ {'x',0},{'1',1},{'2',2},{'3',3},{'4',4},{'r',5},{'b',6},{'q',7},{'k',8},{'j',9} };

		void AssignNumericType(std::vector<Square>& board) {
			int square_val = int(piece_type) - 48;
			
			//Piece has hits left, not this piece, and distance check is valid
			auto res = board | std::views::filter([this, &square_val](Square& sq) {
				return sq.hits_left > 0 && sq.id != this->id &&
				((sq.row == this->row && std::abs(sq.col - this->col) == square_val)
					|| (sq.col == this->col && std::abs(sq.row - this->row) == square_val)
					|| (std::abs(sq.row - this->row) == square_val && std::abs(sq.col - this->col) == square_val)

					);
					});

			for (Square& r : res) {
				target_squares.push_back(&r);
			}
		}

		void AssignRookMoves(std::vector<Square>& board) {
			//Piece has hits left, not this piece, edge piece, and on same row or col
			auto res = board | std::views::filter([this](Square& sq) {
				return sq.hits_left > 0 && sq.id != this->id && ((sq.row == this->row && (sq.col == 0 || sq.col==5)) || (sq.col == this->col && (sq.row==0||sq.row==5)));
				});

			for (Square& r : res) {
				target_squares.push_back(&r);
			}
		}

		void AssignBishopMoves(std::vector<Square>& board) {
			//Target squares row and col are same distance from this square's row and col ignoring sign and is edge piece
			auto res = board | std::views::filter([this](Square& sq) {
				return sq.hits_left > 0 && sq.id!=this->id && sq.is_edge &&  ((sq.row - this->row) == (sq.col - this->col) || (sq.row - this->row) == (this->col - sq.col));
				});

			for (Square& r : res) {
				target_squares.push_back(&r);
			}

		}

		void AssignKnightMoves(std::vector<Square>& board) {
			//2 in one direction 1 in another
			auto res = board | std::views::filter([this](Square& sq) {
				return sq.hits_left > 0 && sq.id != this->id &&
				(
					(std::abs(sq.row - this->row) == 2 && std::abs(sq.col - this->col) == 1)
					|| (std::abs(sq.row - this->row) == 1 && std::abs(sq.col - this->col) == 2)
				);
				});

			for (Square& r : res) {
				target_squares.push_back(&r);
			}
		}

	public:
		std::vector<Square*> target_squares;
		unsigned long square_hash{ 0 };//hash of square based on location and type. will leave to user to set if wanted
		int hits_left{ 0};
		int id{ 0 };
		int row{ 0 };
		int col{ 0 };
		int piece_id{ 0 };
		char piece_type{ 'x' };
		bool is_edge{ false };

		void AssignTargetSquares(std::vector<Square>& board) {
			if (piece_type == '1' || piece_type == '2' || piece_type == '3' || piece_type == '4') {
				this->AssignNumericType(board);
			}
			else if (piece_type == 'r') {
				this->AssignRookMoves(board);
			}
			else if (piece_type == 'b') {
				this->AssignBishopMoves(board);
			}
			else if (piece_type == 'q') {
				this->AssignRookMoves(board);
				this->AssignBishopMoves(board);
			}
			else if (piece_type == 'k') {
				this->AssignKnightMoves(board);
			}
			else if (piece_type == 'j') {
				//jug can hit every square
				for (Square& s : board) {
					//dont add squares that have no hits left at init
					if (s.hits_left && s.id	!= this->id) {
						target_squares.push_back(&s);
					}
				}
			}
		}

		Square(int id,char piece_type) :id(id), row(id / 6), col(id % 6),piece_type(piece_type),piece_id(_piece_id_map[piece_type]) {
			if (piece_type != 'x') {
				hits_left = 1;
			}
			is_edge = row == 0 || row == 5 || col == 0 || col == 5;
		}
		/*
		Square(const Square& sq) =default;
		}

		Square operator = default;
		

		Square(const Square&& sq) = default;

		Square operator = (const Square&& sq) = default;


		~Square() = default;
		*/
		std::string PrintSquare() {
			std::string out_string{this->piece_type};
			out_string += "-" + std::to_string(this->hits_left) + " " + this->GetCoords() + " --> ";
			for (Square* sq : target_squares) {
				out_string += sq->GetCoords() + " ";

			}

			return out_string;

			/*std::cout << this->piece_type << "-"<<this->hits_left << " (" << std::to_string(row) << "," << std::to_string(col) << ") --> ";
			for (Square* sq : target_squares) {
				std::cout<< "(" << sq->row << "," << sq->col << ") ";
			}
			std::cout << "\n\n";*/

			
		}

		std::string GetCoords() {
			return "(" + std::to_string(row) + "," + std::to_string(col) + ")";
		}
	};


	export class Board {
	public:
		unsigned long board_hash{ 0 };
		int starting_pieces{ 0 };
		std::vector<Square> board;
		Square active_square{-1,'j'};

		Board(std::string board_string) {
			//should probably check to ensure size is 36 but it w/e
			for (int i = 0; i < board_string.size(); i++) {
				board.emplace_back(i, board_string[i]);
				if (board_string[i] != 'x') {
					starting_pieces++;
				}
			}

			if (board.size() < 36) {
				for (int i = board.size(); i < 36; i++) {
					board.emplace_back(i, 'x');
				}
			}

			//Assign possible child squares for all squares
			for (Square& sq : this->board) {
				sq.AssignTargetSquares(this->board);
			}

		}

		std::string PrintBoard() {
			std::string out_string{};
			for (Square& sq : board) {
				sq.PrintSquare();
				out_string += sq.PrintSquare()+ "\n\n";
			}
			return out_string;
		}

		//need copy constructor, = operator and destructor and the rvalue shit
		//need play_move eventually too but its fine for now
	};
}