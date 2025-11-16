#include "matrixManager.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "GLCD/GLCD.h"
#include "RIT/RIT.h"
#include "timer.h"
#include "../music/music.h"

extern Player pl;
extern Ghost ghRed;
extern int pacmanMatrix[10][10];
extern int ghostMatrix[10][10];
extern int mapMatrix[26][24];
extern int mapMatrixGhost[26][24];
extern int manageGhostStatus;
extern int waitRespawnGhost;

int nextY, nextX;

void drawPacMan(int x, int y) {
	int dx,dy;
	for(dy=0;dy<10;dy++) {
		for(dx=0;dx<10;dx++) {
			if(pacmanMatrix[dy][dx] == 1) {
				LCD_SetPoint(x*10+dx, y*10+dy, Yellow);
			}
		}
	}	
}

void drawGhost(int x, int y, int color) {
	int dx,dy;
	for(dy=0;dy<10;dy++) {
		for(dx=0;dx<10;dx++) {
			if(ghostMatrix[dy][dx] == 1) {
				if (color==1){
					LCD_SetPoint(x*10+dx, y*10+dy, Red);
				} else if (color==2){
					LCD_SetPoint(x*10+dx, y*10+dy, Blue2);
				} else if (color==3){
					LCD_SetPoint(x*10+dx, y*10+dy, Green);
				} else if (color==4){
					LCD_SetPoint(x*10+dx, y*10+dy, White);
				}
			}
		}
	}
}

void initPlayerPos(int x, int y, Player *pl) {
	pl->x=x;
	pl->y=y;
	pl->gameStatus=PAUSE;
	pl->timerValue=60;
}

void resetPlayerPos(int x, int y, Player *pl) {
	pl->x=x;
	pl->y=y;
	pl->direction=0;
	updateMatrix(x,y,PACMAN);
	drawPacMan(x,y+3);
}

void initGhostPos(int x, int y, int num, Ghost *gh) {
	gh->x=x;
	gh->y=y;
	gh->num=num;
	gh->enemy=1;
}

void resetGhostPos(int x, int y, int num, Ghost *gh) {
	gh->x=x;
	gh->y=y;
	gh->enemy=1;
	moveGhost(gh,0,0,500);
}

void clearPos(int x, int y) {
	int dy, dx;
	mapMatrix[y-3][x]=0;
	for (dy = 0; dy < 10; dy++) {
			for (dx = 0; dx < 10; dx++) {
					LCD_SetPoint(x*10+dx, y*10+dy, Black);
			}
	}
}

void clearPosGhost(int x, int y) {
	int dy, dx;
	mapMatrixGhost[y-3][x]=0;
	for (dy = 0; dy < 10; dy++) {
			for (dx = 0; dx < 10; dx++) {
					LCD_SetPoint(x*10+dx, y*10+dy, Black);
			}
	}
}

void clearLivesPos(int x) {
	int dx,dy;
	for (dy = 0; dy < 10; dy++) {
			for (dx = 0; dx < 10; dx++) {
					LCD_SetPoint(x*10+dx, 30*10+dy, Black);
			}
	}
}

int returnYOff(Player *pl) {
	return pl->y+3;
}

void displayScore(Player *pl) {
	char scoreStr[10];  // Buffer to hold the string representation of the score
	sprintf(scoreStr, "%d", pl->score);  // Convert int score to a string
	GUI_Text(160, 13, (uint8_t *)scoreStr, White, Black);  // Pass the string to GUI_Text
}

void displayTimer(Player *pl) {
	int timerValue = pl->timerValue-=1;
	char timerStr[10];  // Buffer to hold the string representation of the score
	if(timerValue<10 && timerValue!=0) {
		sprintf(timerStr, "0%d", timerValue);  // Convert int score to a string
	} else if (timerValue==0) {
		pl->gameStatus=LOSE;
		sprintf(timerStr, "0%d", timerValue);
		manageGameStatus(pl);
	}	else {
		sprintf(timerStr, "%d", timerValue);  // Convert int score to a string
	}
	GUI_Text(40, 13, (uint8_t *)timerStr, White, Black);  // Pass the string to GUI_Text
}

void pointManager (Player *pl, int points) {
	pl->score+=points;
	if (pl->score - pl->checkScore >= 1000) {
		pl->checkScore+=1000;
		livesManager(pl,1);
	} 
	if (pl->score==2640) {
		pl->gameStatus=WIN;
		manageGameStatus(pl);
	}
	displayScore(pl);
}

int currentNote2 = 0;
int ticks2 = 0;

void updateGhDifficult(int score) {
	ghRed.ghostDifficult = score / (2640 / 4);      // Divide into 4 steps (0, 1, 2, 3)
}

//moving instruction:
//1 bottom, 2 left, 3 up, 4 right
void movePlayer(Player *pl) {
	if (pl->direction!=0 && pl->gameStatus==START) {
		int itemFound;
		itemFound=checkItems(pl);
		if(itemFound != WALL) {
			// Update the matrix with the new position
			updateMatrix(pl->x, pl->y, PACMAN);

			// Draw PacMan at the new position
			drawPacMan(pl->x, returnYOff(pl));
			if(itemFound == STANDARDPILL) {
				pointManager(pl,10);
			} else if (itemFound == POWERPILL) {
				pointManager(pl,50);
				ghRed.enemy=0;
				manageGhostStatus=1;
			}
			if (itemFound == STANDARDPILL || itemFound == POWERPILL) {
				updateGhDifficult(pl->score);
				if(!isNotePlaying()) {
					++ticks2;
					if(ticks2 == UPTICKS)
					{
						ticks2 = 0;
						playNote(pill_eaten_melody[currentNote2++]);
					}
				}
				
				if(currentNote2 == (sizeof(pill_eaten_melody) / sizeof(pill_eaten_melody[0]))) {
					currentNote2=0;
				}
			}
		}	else {
			pl->direction=0;
		}
	}
	if (pl->gameStatus==START && mapMatrixGhost[pl->y][pl->x] == REDGHOST && ghRed.enemy == 1) {
		clearPos(pl->x,pl->y+3);
		clearPosGhost(ghRed.x,ghRed.y+3);
		resetPlayerPos(11,16,pl);
		resetGhostPos(10,12,REDGHOST,&ghRed);
		livesManager(pl,0);
	} else if (pl->gameStatus==START && mapMatrixGhost[pl->y][pl->x] == REDGHOST && ghRed.enemy == 0) {
		clearPosGhost(ghRed.x,ghRed.y+3);
		drawPacMan(pl->x,pl->y+3);
		waitRespawnGhost=1;
		pointManager(pl,100);
	}
}

void moveGhost(Ghost *gh, int oldX, int oldY, int num) {
	int dx,dy;
	oldY=oldY+3;
		// Update the matrix with the new position
		updateMatrixGhost(gh->x, gh->y, 500);

		// Draw PacMan at the new position
		if (num==GREENGHOST) {
			drawGhost(gh->x, gh->y+3, 3);
		} else if (num==REDGHOST) {
			drawGhost(gh->x, gh->y+3, 1);
		} else if (num==WHITEGHOST) {
			drawGhost(gh->x, gh->y+3, 4);
		} else if (num==BLUEGHOST) {
			drawGhost(gh->x, gh->y+3, 2);
		}
		if (mapMatrix[oldY-3][oldX]==STANDARDPILL) {
			for (dy = -1; dy < 1; dy++) {
				for (dx = -1; dx < 1; dx++) {
					LCD_SetPoint(oldX*10+5+dx, oldY*10+5+dy, Green);
				}
			}
		} else if (mapMatrix[oldY-3][oldX]==POWERPILL) {
			for (dy = -3; dy < 3; dy++) {
				for (dx = -3; dx < 3; dx++) {
					LCD_SetPoint(oldX*10+5+dy, oldY*10+5+dx, Red);
				}
			}
		}
}

int globalMoveSeed=0;
void updateDirection(Player *pl, int dir) {
	if(pl->gameStatus==START) {
		pl->direction=dir;
	}
	globalMoveSeed++;
}

void updateDirectionGhost(Ghost *gh) {
	gh->direction=getRandomNumber(1,4);
}

void livesManager(Player *pl, int addRemove) {
	if (addRemove==1){
		pl->lives+=1;
		drawPacMan(pl->lives, 30);
	} else if (addRemove==0){
		clearLivesPos(pl->lives);
		pl->lives-=1;
		if (pl->lives<1) {
			pl->gameStatus=LOSE;
			manageGameStatus(pl);
		}
	}
}

void manageGameStatus(Player *pl) {
	if (pl->gameStatus==PAUSE) {
		enable_timer(0);
		//enable_timer(1);
		enable_timer(2);
		pl->gameStatus=START;
		removeMessage();
	} else if (pl->gameStatus==START || pl->gameStatus==WIN || pl->gameStatus==LOSE) {
		disable_timer(0);
		//disable_timer(1);
		disable_timer(2);
		if (pl->gameStatus==START) {
			pl->gameStatus=PAUSE;
			GUI_Text(90, 160, (uint8_t *) " PAUSE ", Red, Green);
		} else if (pl->gameStatus==WIN) {
			GUI_Text(100, 160, (uint8_t *) " WIN ", Red, Green);
			disable_timer(0);
			//disable_timer(3);
		} else if (pl->gameStatus==LOSE) {
			GUI_Text(70, 160, (uint8_t *) " GAME OVER ", Red, Green);
			disable_timer(0);
			//disable_timer(3);
		}
	}
}

void testAstar(Player *pl, Ghost *gh) {	
	int pacmanX = pl->x;
	int pacmanY = pl->y;
	
	int ghostX = gh->x;
	int ghostY = gh->y;
	
	int oldX,oldY;
	
	if (gh->enemy == 1) {
		if (a_star_algorithm(pacmanX, pacmanY, ghostX, ghostY, &nextX, &nextY)) {			
			if (nextX != -1 && nextY != -1) {
				//updateMatrixGhost(nextX, nextY, REDGHOST);
				gh->x=nextX;
				gh->y=nextY;
				clearPosGhost(ghostX, ghostY+3);
				//drawGhost(nextX, nextY+3, 1);
				moveGhost(gh, ghostX, ghostY, 500);
			}
		}
	} else if (gh->enemy == 0) {
		if (a_star_algorithm_escape(pacmanX, pacmanY, ghostX, ghostY, &nextX, &nextY)) {
			if (nextX != -1 && nextY != -1) {
				//updateMatrixGhost(nextX, nextY, REDGHOST);
				gh->x=nextX;
				gh->y=nextY;
				clearPosGhost(ghostX, ghostY+3);
				//drawGhost(nextX, nextY+3, 2);
				moveGhost(gh, ghostX, ghostY, 501);
			}
		}
	}
}

// A* Algorithm function for Pac-Man ghost movement

Node openList[1000], closedList[1000];
Node *path;
Node current;

// Initialize a global variable to store the current path of the ghost
Node* ghostPath = NULL;
int ghostPathIndex = 0;

int a_star_algorithm(int pacmanX, int pacmanY, int ghostX, int ghostY, int *nextX, int *nextY) {
    int i,j,openCount=0,closedCount=0;

    // Direction vectors for moving up, right, down, left
    int dx[] = {0, 1, 0, -1};
    int dy[] = {1, 0, -1, 0};

    // Initialize the open list with the ghost's current position
    openList[0] = (Node){ghostX, ghostY, 0, 0, 0, NULL};
    openCount = 1;

    // Variable to store the first step
    int firstStepX = -1, firstStepY = -1;

    // A* Algorithm loop
    while (openCount > 0) {
        // Find the node with the lowest f cost in the open list
        int lowestFIndex = 0;
        for (i = 1; i < openCount; i++) {
            if (openList[i].f < openList[lowestFIndex].f) {
                lowestFIndex = i;
            }
        }

        // Get the current node
        current = openList[lowestFIndex];

        // If the current node is Pac-Man's position, reconstruct the path
        if (current.x == pacmanX && current.y == pacmanY) {
            path = &current;

            // Backtrack to the parent node to find the next step
            while (path->parent != NULL) {
                firstStepX = path->x; // Store the first step's coordinates
                firstStepY = path->y;
                path = path->parent;

                // Break when we reach the ghost's initial position
                if (path->x == ghostX && path->y == ghostY) break;
            }

            *nextX = firstStepX; // Return the first step coordinates
            *nextY = firstStepY;
            return 1; // Path found
        }

        // Remove the current node from the open list
        for (i = lowestFIndex; i < openCount - 1; i++) {
            openList[i] = openList[i + 1];
        }
        openCount--;

        // Add the current node to the closed list
        closedList[closedCount++] = current;

        // Explore neighbors (up, right, down, left)
        for (i = 0; i < 4; i++) {
            int nx = current.x + dx[i];
            int ny = current.y + dy[i];

            // Skip out-of-bounds or wall positions
            if (nx < 0 || ny < 0 || nx >= 24 || ny >= 26) continue;
            if (mapMatrix[ny][nx] == 1) continue; // 1 represents a wall

            // Check if the neighbor is in the closed list
            int inClosed = 0;
            for (j = 0; j < closedCount; j++) {
                if (closedList[j].x == nx && closedList[j].y == ny) {
                    inClosed = 1;
                    break;
                }
            }
            if (inClosed) continue;

            // Calculate costs for the neighbor
            int g = current.g + 1; // Cost from start to this node
            int h = abs(nx - pacmanX) + abs(ny - pacmanY); // Manhattan distance heuristic
            int f = g + h; // Total cost

            // Check if the neighbor is in the open list
            int inOpen = -1;
            for (j = 0; j < openCount; j++) {
                if (openList[j].x == nx && openList[j].y == ny) {
                    inOpen = j;
                    break;
                }
            }

            // If the neighbor is not in the open list, add it
            if (inOpen == -1) {
                openList[openCount++] = (Node){nx, ny, g, h, f, &closedList[closedCount - 1]};
            }
            // If the neighbor is in the open list with a higher cost, update it
            else if (g < openList[inOpen].g) {
                openList[inOpen].g = g;
                openList[inOpen].f = f;
                openList[inOpen].parent = &closedList[closedCount - 1];
            }
        }
    }

    // If no path is found, return 0
    return 0;
}

int a_star_algorithm_escape(int pacmanX, int pacmanY, int ghostX, int ghostY, int *nextX, int *nextY) {
    int i, j, openCount = 0, closedCount = 0;

    // Direction vectors for moving up, right, down, left
    int dx[] = {0, 1, 0, -1};
    int dy[] = {1, 0, -1, 0};

    // Initialize the open list with the ghost's current position
    openList[0] = (Node){ghostX, ghostY, 0, 0, 0, NULL};
    openCount = 1;

    // Variable to store the first step
    int pathFound = 0;
    Node *path = NULL;
    Node *lastNode = NULL;

    // A* Algorithm loop
    while (openCount > 0) {
        // Find the node with the lowest f cost in the open list
        int lowestFIndex = 0;
        for (i = 1; i < openCount; i++) {
            if (openList[i].f < openList[lowestFIndex].f) {
                lowestFIndex = i;
            }
        }

        // Get the current node
        current = openList[lowestFIndex];

        // If the current node is Pac-Man's position, reconstruct the path
        if (current.x == pacmanX && current.y == pacmanY) {
            path = &current; // The path ends at Pac-Man
            pathFound = 1;
            break;
        }

        // Remove the current node from the open list
        for (i = lowestFIndex; i < openCount - 1; i++) {
            openList[i] = openList[i + 1];
        }
        openCount--;

        // Add the current node to the closed list
        closedList[closedCount++] = current;

        // Explore neighbors (up, right, down, left)
        for (i = 0; i < 4; i++) {
            int nx = current.x + dx[i];
            int ny = current.y + dy[i];

            // Skip out-of-bounds or wall positions
            if (nx < 0 || ny < 0 || nx >= 24 || ny >= 26) continue;
            if (mapMatrix[ny][nx] == 1) continue; // 1 represents a wall

            // Check if the neighbor is in the closed list
            int inClosed = 0;
            for (j = 0; j < closedCount; j++) {
                if (closedList[j].x == nx && closedList[j].y == ny) {
                    inClosed = 1;
                    break;
                }
            }
            if (inClosed) continue;

            // Calculate costs for the neighbor
            int g = current.g + 1; // Cost from start to this node
            int h = abs(nx - pacmanX) + abs(ny - pacmanY); // Manhattan distance heuristic
            int f = g + h; // Total cost

            // Check if the neighbor is in the open list
            int inOpen = -1;
            for (j = 0; j < openCount; j++) {
                if (openList[j].x == nx && openList[j].y == ny) {
                    inOpen = j;
                    break;
                }
            }

            // If the neighbor is not in the open list, add it
            if (inOpen == -1) {
                openList[openCount++] = (Node){nx, ny, g, h, f, &closedList[closedCount - 1]};
            }
            // If the neighbor is in the open list with a higher cost, update it
            else if (g < openList[inOpen].g) {
                openList[inOpen].g = g;
                openList[inOpen].f = f;
                openList[inOpen].parent = &closedList[closedCount - 1];
            }
        }
    }

    // If no path is found, return 0
    if (!pathFound) return 0;

    // Reconstruct the path in reverse
    lastNode = path;
    Node *secondLastNode = NULL;

    // Traverse back until we reach the ghost's starting position
    while (lastNode->parent != NULL) {
        secondLastNode = lastNode; // Keep track of the previous step
        lastNode = lastNode->parent;

        // Stop when we reach the ghost's starting position
        if (lastNode->x == ghostX && lastNode->y == ghostY) break;
    }

    // Attempt to move in the opposite direction
    if (secondLastNode != NULL) {
        int oppositeX = ghostX - (secondLastNode->x - ghostX);
        int oppositeY = ghostY - (secondLastNode->y - ghostY);

        // Validate the opposite position
        if (oppositeX >= 0 && oppositeY >= 0 && oppositeX < 24 && oppositeY < 26 &&
            mapMatrix[oppositeY][oppositeX] != 1) { // Ensure it's not a wall
            *nextX = oppositeX;
            *nextY = oppositeY;
            return 1; // Escape path found
        }
    }

    // If the opposite position is invalid, explore alternatives
    int bestDist = -1;
    int bestX = ghostX, bestY = ghostY;
    for (i = 0; i < 4; i++) {
        int nx = ghostX + dx[i];
        int ny = ghostY + dy[i];

        // Skip out-of-bounds or wall positions
        if (nx < 0 || ny < 0 || nx >= 24 || ny >= 26) continue;
        if (mapMatrix[ny][nx] == 1) continue; // 1 represents a wall

        // Calculate the distance from Pac-Man
        int dist = abs(nx - pacmanX) + abs(ny - pacmanY);

        // Choose the farthest valid position from Pac-Man
        if (dist > bestDist) {
            bestDist = dist;
            bestX = nx;
            bestY = ny;
        }
    }

    // Return the best escape position
    *nextX = bestX;
    *nextY = bestY;
    return 1; // Path calculated successfully
}
