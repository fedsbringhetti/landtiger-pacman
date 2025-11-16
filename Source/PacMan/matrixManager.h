#ifndef MATRIXMANAGER_H
#define MATRIXMANAGER_H

typedef struct {
	int x;
	int y;
	int lives;
	int score;
	int checkScore;
	int direction;
	int gameStatus;
	int timerValue;
} Player;

typedef struct {
	int x;
	int y;
	int direction;
	int num;
	int enemy;
	int ghostDifficult;
} Ghost;

typedef struct Node {
	int x, y;      // Coordinates
	int g, h, f;   // g = cost from start, h = heuristic, f = g + h
	struct Node *parent; // Pointer to parent node for backtracking
} Node;

void resetPlayerPos(int x, int y, Player *pl);
void resetGhostPos(int x, int y, int num, Ghost *gh);
void testAstar(Player *pl, Ghost *gh);
void drawPacMan(int x, int y);
void drawGhost(int x, int y, int color);
void initPlayerPos(int x, int y, Player *pl);
void initGhostPos(int x, int y, int num, Ghost *gh);
void clearPos(int x, int y);
void clearPosGhost(int x, int y);
void movePlayer(Player *pl);
void moveGhost(Ghost *gh, int oldX, int oldY, int num);
void updateDirection(Player *pl, int dir);
void updateDirectionGhost(Ghost *gh);
void livesManager(Player *pl, int addRemove);
void displayTimer(Player *pl);
void manageGameStatus(Player *pl);
int a_star_algorithm_escape(int pacmanX, int pacmanY, int ghostX, int ghostY, int *nextX, int *nextY);
int a_star_algorithm(int pacmanX, int pacmanY, int ghostX, int ghostY, int *nextX, int *nextY);

#endif