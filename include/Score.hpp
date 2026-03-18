#ifndef SCORE_HPP
#define SCORE_HPP


class Score {
private:
    int score;

public:
    Score(int s);
    int getScore() const;
    void increase(int amount);
    void reset();
};


#endif // SCORE_HPP