/*
 * Count score of a game
 */

#ifndef __SCORE_H_
#define __SCORE_H_

class Score {
public:
	struct PlayerScore {
		unsigned int scoreGame[3];
		unsigned int scoreSet;
	};

private:
	PlayerScore playerScore[2];
	unsigned int numOfSet = 0;
	bool advantage;
	bool deuce;
	bool serveFirst;

public:
	Score();
	void reset();
	void pointToFirst();
	void pointToSecond();
};

#endif // __SCORE_H_
