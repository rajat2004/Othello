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
using namespace std;
using namespace Desdemona;

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
        const int MAX_DEPTH = 2;
        const int BOARD_SIZE = 8;

        Turn my_turn;

        Move minimaxDecision(const OthelloBoard &board, Turn &turn);
        int minimaxValue(const OthelloBoard &board, const Turn &orig_turn, Turn &curr_turn, int depth);
        int evaluate(const OthelloBoard &board, Turn &curr_turn);
};


int MyBot::evaluate(const OthelloBoard &board, Turn &curr_turn)
{
    // return rand()%(INT_MAX);
    static vector<vector<int>> score {
            {65, -3,  6,  4,  4,  6, -3, 65},
            {-3, -29, 3,  1,  1,  3, -29,-3},
            { 6,  3,  5,  3,  3,  5,  3,  6},
            { 4,  1,  3,  1,  1,  3,  1,  4},
            { 4,  1,  3,  1,  1,  3,  1,  4},
            { 6,  3,  5,  3,  3,  5,  3,  6},
            {-3, -29, 3,  1,  1,  3, -29,-3},
            {65, -3,  6,  4,  4,  6, -3, 65},
    };

    int tot_score = 0;
    auto opp_turn = other(curr_turn);

    for(int i=0; i<BOARD_SIZE; i++) {
        for(int j=0; j<BOARD_SIZE; j++) {

            if (board.get(i, j) == curr_turn)
                tot_score += score[i][j];
            else if (board.get(i, j) == opp_turn)
                tot_score -= score[i][j];

        }
    }

    tot_score += board.getValidMoves(curr_turn).size() - board.getValidMoves(opp_turn).size();
    int pieces_count = board.getBlackCount() - board.getRedCount();
    tot_score += (curr_turn==BLACK) ? pieces_count : -1*pieces_count;

    return tot_score;
}

int MyBot::minimaxValue(const OthelloBoard &board, const Turn &orig_turn, Turn &curr_turn, int depth)
{
    if (depth == MAX_DEPTH)
        return evaluate(board, curr_turn);

    auto opp_turn = other(curr_turn);

    auto all_moves = board.getValidMoves(curr_turn);
    if (all_moves.size()==0) {
        // Skip to next turn
        return minimaxValue(board, orig_turn, opp_turn, depth+1);
    }

    bool is_orig = curr_turn==orig_turn;
    int best_val = is_orig ? INT_MIN : INT_MAX;

    for(auto &move : all_moves) {
        auto copy_board = board;
        copy_board.makeMove(curr_turn, move);

        // Now check for opponent turn
        auto val = minimaxValue(copy_board, orig_turn, opp_turn, depth+1);
        if (is_orig) {
            if (best_val < val)
                best_val = val;
        }
        else {
            if (best_val > val)
                best_val = val;
        }
    }

    return best_val;
}

Move MyBot::minimaxDecision(const OthelloBoard &board, Turn &turn)
{
    auto all_moves = board.getValidMoves(turn);
    Move best_move = Move::pass();
    int best_val = INT_MIN;
    Turn opp_turn = other(turn);

    // No valid moves exist
    if (all_moves.size()==0) {
        return Move::pass();
    }

    for (auto &move : all_moves) {
        auto copy_board = board;
        copy_board.makeMove(turn, move);

        auto eval_val = minimaxValue(copy_board, turn, opp_turn, 1);
        if (eval_val > best_val) {
            best_val = eval_val;
            best_move = move;
        }
    }

    return best_move;
}

MyBot::MyBot( Turn turn )
    : OthelloPlayer( turn ), my_turn(turn)
{
}

Move MyBot::play( const OthelloBoard& board )
{
    // auto moves = board.getValidMoves( turn );
    // int randNo = rand() % moves.size();
    // auto it = moves.begin();

    // while(randNo>0) {
    //     it++;
    //     randNo--;
    // }

    // return *it;

    return minimaxDecision(board, my_turn);
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
