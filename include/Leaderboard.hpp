#ifndef LEADERBOARD_HPP
#define LEADERBOARD_HPP


class Leaderboard {
public:
    Leaderboard();

    // TODO list of scores

    void addScore(int playerId, int score);

    int top(int K); // return top K scores
    void reset(int playerId);
};


#endif // LEADERBOARD_HPP