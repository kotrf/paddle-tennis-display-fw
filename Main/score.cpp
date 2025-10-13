#include <score.h>

Score::Score()
{
	reset();
}

void Score::reset() {
	playerScore[0] = {0};
	playerScore[1] = {0};
	numOfSet = 0;
	advantage = false;
	deuce = false;
	serveFirst = true;
}

void Score::pointToFirst() {
	if (playerScore[0].scoreGame[numOfSet] < 3) {
		++playerScore[0].scoreGame[numOfSet];
		deuce = (playerScore[0].scoreGame[numOfSet] == 3 && playerScore[1].scoreGame[numOfSet] == 3);

	}
}

void Score::pointToSecond() {
}
