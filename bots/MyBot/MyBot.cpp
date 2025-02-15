/*
* @file botTemplate.cpp
* @author Arun Tejasvi Chaganty <arunchaganty@gmail.com>
* @date 2010-02-04
* Template for users to create their own bots
*/

#include "Othello.h"
#include "OthelloBoard.h"
#include "OthelloPlayer.h"
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <list>
#include <climits>
#include <chrono>

using namespace std;
using namespace Desdemona;

using millis = std::chrono::milliseconds;
using std::chrono::duration_cast;
using std::chrono::steady_clock;

class MyBot: public OthelloPlayer
{
    public:
        /**
         * Initialisation routines here
         * This could do anything from open up a cache of "best moves" to
         * spawning a background processing thread.
         */
        MyBot( Turn turn );

        /**
         * Play something
         */
        virtual Move play( const OthelloBoard& board );
    private:
        const int MAX_DEPTH = 4;
        const int BOARD_SIZE = 8;

        const int POSITIONWEIGHT = 5;
        const int MOBILITYWEIGHT = 15;

        Turn orig_turn;

        Move minimaxDecision(const OthelloBoard &board, Turn &turn);
        int minimaxValue(const OthelloBoard &board, Turn &curr_turn, int depth, int alpha, int beta);
        int evaluate(const OthelloBoard &board, Turn &curr_turn);
};


int MyBot::evaluate(const OthelloBoard &board, Turn &curr_turn)
{
    static vector<vector<int>> score {
            {50, -1,  5,  2,  2,  5, -1, 50},
            {-1, -10, 1,  1,  1,  1, -10,-1},
            { 5,  1,  1,  1,  1,  1,  1,  5},
            { 2,  1,  1,  0,  0,  1,  1,  2},
            { 2,  1,  1,  0,  0,  1,  1,  2},
            { 5,  1,  1,  1,  1,  1,  1,  5},
            {-1, -10, 1,  1,  1,  1, -10,-1},
            {50, -1,  5,  2,  2,  5, -1, 50},
    };

    int pos_score = 0, tot_score = 0;
    auto opp_turn = other(curr_turn);

    for(int i=0; i<BOARD_SIZE; i++) {
        for(int j=0; j<BOARD_SIZE; j++) {

            if (board.get(i, j) == curr_turn)
                pos_score += score[i][j];
            else if (board.get(i, j) == opp_turn)
                pos_score -= score[i][j];

        }
    }

    int mob_score = board.getValidMoves(curr_turn).size() - board.getValidMoves(opp_turn).size();
    int pieces_count = board.getBlackCount() - board.getRedCount();
    int pieces_score = (curr_turn==BLACK) ? pieces_count : -1*pieces_count;

    tot_score = POSITIONWEIGHT*pos_score + MOBILITYWEIGHT*mob_score + pieces_score;

    return tot_score;
}

int MyBot::minimaxValue(const OthelloBoard &board, Turn &curr_turn, int depth, int alpha, int beta)
{
    if (depth == MAX_DEPTH)
        return evaluate(board, curr_turn);

    auto opp_turn = other(curr_turn);

    auto all_moves = board.getValidMoves(curr_turn);
    if (all_moves.size()==0) {
        // Skip to next turn
        return minimaxValue(board, opp_turn, depth+1, alpha, beta);
    }

    bool is_orig = curr_turn==orig_turn;
    int best_val = is_orig ? INT_MIN : INT_MAX;

    for(auto &move : all_moves) {
        auto copy_board = board;
        copy_board.makeMove(curr_turn, move);

        // Now check for opponent turn
        auto val = minimaxValue(copy_board, opp_turn, depth+1, alpha, beta);
        if (is_orig) {
            if (best_val < val)
                best_val = val;

            alpha = max(alpha, best_val);
            if (beta <= alpha)
                break;
        }
        else {
            if (best_val > val)
                best_val = val;

            beta = min(beta, best_val);
            if (beta <= alpha)
                break;
        }
    }

    return best_val;
}

Move MyBot::minimaxDecision(const OthelloBoard &board, Turn &turn)
{
    // auto t_seq_1 = steady_clock::now();

    auto all_moves = board.getValidMoves(turn);
    Move best_move = Move::pass();
    int best_val = INT_MIN;
    Turn opp_turn = other(turn);

    // No valid moves exist
    if (all_moves.size()==0) {
        return Move::pass();
    }

    for (const auto &move : all_moves) {
        auto copy_board = board;
        copy_board.makeMove(turn, move);

        auto eval_val = minimaxValue(copy_board, opp_turn, 1, INT_MIN, INT_MAX);
        if (eval_val > best_val) {
            best_val = eval_val;
            best_move = move;
        }
    }

    // auto t_seq_2 = steady_clock::now();
    // auto time1 = duration_cast<millis>(t_seq_2 - t_seq_1).count();
    // cout << "Time: " << time1 << " milliseconds\n";

    return board.validateMove(turn, best_move) ? best_move : Move::pass();
}

MyBot::MyBot( Turn turn )
    : OthelloPlayer( turn ), orig_turn(turn)
{
}

Move MyBot::play( const OthelloBoard& board )
{
    return minimaxDecision(board, orig_turn);
}

// The following lines are _very_ important to create a bot module for Desdemona

extern "C" {
    OthelloPlayer* createBot( Turn turn )
    {
        return new MyBot( turn );
    }

    void destroyBot( OthelloPlayer* bot )
    {
        delete bot;
    }
}
