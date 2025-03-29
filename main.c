#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

//global variable

//for game sizing
#define sizeX 40
#define sizeY 23

//movement
#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80		
#define SPACE 32
#define ENTER 13

//total enemies
int totalEnemies = 0;

//gameboard
#define GBOARD_ORIGIN_X 4
#define GBOARD_ORIGIN_Y 2
#define GBOARD_WIDTH 30
#define GBOARD_HEIGHT 40

//enemies amount

#define ENEMY_COUNT 20
int enemyCount = ENEMY_COUNT;
int enemyCount_level2 = ((ENEMY_COUNT - 9) * 2) + 10;
int enemyCount_level3 = 3;

//starting point of player
int startX = GBOARD_WIDTH + 5;
int startY = GBOARD_HEIGHT - 2;

//score, life and random item counter
int score = 0;
int life = 3;
int randItem = 0;
int testItem = 0;

int winCheck = 0; //false

// Define the bullet structure
typedef struct {
	int x;
	int y;
	int isActive;
} Bullet;

// Define the Enemy structure
typedef struct {
	COORD position;
	int health;
	int isActive;
	int movement;
} Enemy;

//Define Wall for level 3 (BOSS LEVEL)
typedef struct {
	int x;
	int y;
	int isActive;
} Wall;

//NPC for Level 1 & Level 2
Enemy enemies[ENEMY_COUNT];
Enemy enemies2[ENEMY_COUNT];
Enemy enemiesCopy[10];

//Boss for Level 3
Enemy bossLGun[100];
Enemy bossRGun[100];
Enemy bossHead[100];

//NPC wall for Level 3 (cover boss head)
Enemy wall[100];



//for printing HP bar -- hard
int bossLGunHealth;
int bossRGunHealth;
int bossHeadHealth;

// Define Bullet Structure 

typedef struct {
	COORD position;
	int isActive;
	int movement;
}Item;

//Item
Item HEART[100];
Item TP[100];
Item DUMMY_ITEM[100];

Bullet heart[20];
Bullet tp[20];
Bullet dummy_item[20];

//initializing bullets Player and NPC
#define MAX_BULLETS 20
Bullet bulletsPlayer[MAX_BULLETS];
Bullet bulletsNPC[MAX_BULLETS];
Bullet bulletsNPC2[MAX_BULLETS];
Bullet bulletsBoss[MAX_BULLETS];

void SetCurrentCursorPos(int x, int y) {
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

COORD GetCurrentCursorPos(void) {
	COORD curPoint;
	CONSOLE_SCREEN_BUFFER_INFO curInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curPoint.X = curInfo.dwCursorPosition.X;
	curPoint.Y = curInfo.dwCursorPosition.Y;
	return curPoint;
}

void showItem(Item item) {
	if (item.isActive) {
		SetCurrentCursorPos(item.position.X, item.position.Y);
		printf("♥");
	}
}

void showNPC(Enemy enemy) {
	if (enemy.isActive) {
		SetCurrentCursorPos(enemy.position.X, enemy.position.Y);
		if (enemy.health == 2) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 5);
			printf("※");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		}
		else if (enemy.health == 1) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
			printf("※");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		}
		else if (enemy.health > 2 && enemy.health <= 40) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
			printf("※");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		}
		else if (enemy.health >= 60 && enemy.health <= 61) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
			printf("※");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		}
		else {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
			printf("※");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		}

	}
}
void deleteNPC(Enemy enemy) {
	SetCurrentCursorPos(enemy.position.X, enemy.position.Y);
	printf("  ");
}



void showPlayer() {
	COORD pos = GetCurrentCursorPos();
	printf("▲");
	SetCurrentCursorPos(pos.X, pos.Y);
}
void deletePlayer() {

	COORD curPos = GetCurrentCursorPos();
	printf("  ");
	SetCurrentCursorPos(curPos.X, curPos.Y);

}
void shiftLeft() {
	deletePlayer();
	startX -= 2;
	SetCurrentCursorPos(startX, startY);
	showPlayer();
}

void shiftRight() {
	deletePlayer();
	startX += 2;
	SetCurrentCursorPos(startX, startY);
	showPlayer();
}

void clearScreen() {
	int x, y;
	SetCurrentCursorPos(0, 0);
	for (x = 1; x < GBOARD_WIDTH + 100; x++) {
		for (y = 0; y <= GBOARD_HEIGHT + 5; y++) {
			printf("  ");
		}
	}
}

int RandomInRange(int lower, int upper) {
	return  (rand() % (upper - lower + 1)) + lower;
}

void mbPlayer() {
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (bulletsPlayer[i].isActive) {
			// Move the bullet up
			SetCurrentCursorPos(bulletsPlayer[i].x, bulletsPlayer[i].y);
			printf("  "); // Clear previous bullet position
			bulletsPlayer[i].y--;

			if (bulletsPlayer[i].y == GBOARD_ORIGIN_Y) {
				bulletsPlayer[i].isActive = 0; // Deactivate the bullet when it goes out of bounds
			}
			else {
				if ((i % 2) == 0) {
					SetCurrentCursorPos(bulletsPlayer[i].x, bulletsPlayer[i].y);
					printf("↑"); // Display the bullet
				}
			}
		}
	}
}

//movement bullet NPC level 1
void mbNPC() {
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (bulletsNPC[i].isActive) {
			// Move the bullet up
			SetCurrentCursorPos(bulletsNPC[i].x, bulletsNPC[i].y);
			printf("  "); // Clear previous bullet position
			bulletsNPC[i].y++;

			if (bulletsNPC[i].y == GBOARD_HEIGHT + 2) {
				bulletsNPC[i].isActive = 0; // Deactivate the bullet when it goes out of bounds
			}
			else {
				SetCurrentCursorPos(bulletsNPC[i].x, bulletsNPC[i].y);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
				printf("↓"); // Display the bullet
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			}
		}
	}
}

//movement bullet NPC level 2
void mbNPC2() {

	for (int i = 0; i < MAX_BULLETS; i++) {
		if (bulletsNPC2[i].isActive) {
			// Move the bullet up
			SetCurrentCursorPos(bulletsNPC2[i].x, bulletsNPC2[i].y);
			printf("  "); // Clear previous bullet position
			bulletsNPC2[i].y++;

			if (bulletsNPC2[i].y == GBOARD_HEIGHT + 2) {
				bulletsNPC2[i].isActive = 0; // Deactivate the bullet when it goes out of bounds

			}
			else {
				SetCurrentCursorPos(bulletsNPC2[i].x, bulletsNPC2[i].y);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
				printf("↓"); // Display the bullet
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			}
		}
	}
}

//movement bullet NPC level 3
void mbNPC3() {

	for (int i = 0; i < 8; i++) {

		//condition if PLAYER LEFTSIDED
		if (bulletsNPC[i].isActive && (startX > GBOARD_ORIGIN_X && startX < GBOARD_ORIGIN_X + GBOARD_WIDTH + 1)) {
			// Move the bullet up
			SetCurrentCursorPos(bulletsNPC[i].x, bulletsNPC[i].y);
			printf("  "); // Clear previous bullet position
			if (i < 10) {
				bulletsNPC[i].y++;
				bulletsNPC[i].x;
			}
			else {
				bulletsNPC[i].y++;
				bulletsNPC[i].x++;
			}

			if (bulletsNPC[i].y == GBOARD_HEIGHT + 2) {
				bulletsNPC[i].isActive = 0; // Deactivate the bullet when it goes out of bounds

			}
			else {
				SetCurrentCursorPos(bulletsNPC[i].x, bulletsNPC[i].y);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
				printf("o"); // Display the bullet
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			}
		}

		//condition if PLAYER RIGHTSIDED
		else if (bulletsNPC[i].isActive && (startX >= GBOARD_ORIGIN_X + GBOARD_WIDTH + 1 && startX < GBOARD_ORIGIN_X + GBOARD_WIDTH * 2)) {
			// Move the bullet up
			SetCurrentCursorPos(bulletsNPC[i].x, bulletsNPC[i].y);
			printf("  "); // Clear previous bullet position
			bulletsNPC[i].y++;
			bulletsNPC[i].x++;

			if (bulletsNPC[i].y == GBOARD_HEIGHT + 2 || bulletsNPC[i].x == GBOARD_ORIGIN_X + (GBOARD_WIDTH + 1) * 2) {
				bulletsNPC[i].isActive = 0; // Deactivate the bullet when it goes out of bounds

			}
			else {
				SetCurrentCursorPos(bulletsNPC[i].x, bulletsNPC[i].y);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
				printf("o"); // Display the bullet
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			}
		}
	}
}

void mbNPC4() {
	for (int i = 0; i < 8; i++) {
		if (bulletsNPC2[i].isActive && (startX > GBOARD_ORIGIN_X + GBOARD_WIDTH + 1 && startX < GBOARD_ORIGIN_X + GBOARD_WIDTH * 2)) {
			// Move the bullet up
			SetCurrentCursorPos(bulletsNPC2[i].x, bulletsNPC2[i].y);
			printf("  "); // Clear previous bullet position
			if (i < 10) {
				bulletsNPC2[i].y++;
				bulletsNPC2[i].x;
			}
			else {
				bulletsNPC2[i].y++;
				bulletsNPC2[i].x--;
			}

			if (bulletsNPC2[i].y == GBOARD_HEIGHT + 2) {
				bulletsNPC2[i].isActive = 0; // Deactivate the bullet when it goes out of bounds

			}
			else {
				SetCurrentCursorPos(bulletsNPC2[i].x, bulletsNPC2[i].y);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
				printf("o"); // Display the bullet
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			}
		}

		else if (bulletsNPC2[i].isActive && (startX > GBOARD_ORIGIN_X && startX <= GBOARD_ORIGIN_X + GBOARD_WIDTH + 1)) {
			// Move the bullet up
			SetCurrentCursorPos(bulletsNPC2[i].x, bulletsNPC2[i].y);
			printf("  "); // Clear previous bullet position
			bulletsNPC2[i].y++;
			bulletsNPC2[i].x--;

			if (bulletsNPC2[i].y == GBOARD_HEIGHT + 2) {
				bulletsNPC2[i].isActive = 0; // Deactivate the bullet when it goes out of bounds

			}
			else {
				SetCurrentCursorPos(bulletsNPC2[i].x, bulletsNPC2[i].y);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
				printf("o"); // Display the bullet
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			}
		}
	}
}

void mbNPC5() {

	for (int i = 0; i < MAX_BULLETS; i++) {
		if (bulletsBoss[i].isActive) {
			// Move the bullet up
			SetCurrentCursorPos(bulletsBoss[i].x, bulletsBoss[i].y);
			printf("  "); // Clear previous bullet position

			if (startX > GBOARD_ORIGIN_X && startX <= GBOARD_ORIGIN_X + GBOARD_WIDTH + 1) {
				if (i < 6) {
					bulletsBoss[i].y++;
					bulletsBoss[i].x--;
				}
				else if (i >= 6 && i < 12) {
					bulletsBoss[i].y++;
					bulletsBoss[i].x;
				}
				else {
					bulletsBoss[i].y++;
					bulletsBoss[i].x++;
				}
			}
			else if (startX > GBOARD_ORIGIN_X + GBOARD_WIDTH + 1 && startX < GBOARD_ORIGIN_X + GBOARD_WIDTH * 2) {
				if (i < 6) {
					bulletsBoss[i].y++;
					bulletsBoss[i].x++;
				}
				else if (i >= 6 && i < 12) {
					bulletsBoss[i].y++;
					bulletsBoss[i].x--;
				}
				else {
					bulletsBoss[i].y++;
					bulletsBoss[i].x;
				}
			}

			if (bulletsBoss[i].y == GBOARD_HEIGHT + 2 || bulletsBoss[i].x == GBOARD_ORIGIN_X || bulletsBoss[i].x == GBOARD_ORIGIN_X + GBOARD_WIDTH * 2 + 1) {
				bulletsBoss[i].isActive = 0; // Deactivate the bullet when it goes out of bounds
			}

			else {
				SetCurrentCursorPos(bulletsBoss[i].x, bulletsBoss[i].y);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
				printf("o"); // Display the bullet
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			}
		}
	}
}

void playerShoot() {
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (!bulletsPlayer[i].isActive) {
			bulletsPlayer[i].isActive = 1;
			bulletsPlayer[i].x = startX;
			bulletsPlayer[i].y = startY - 1;
			break;
		}
	}
}

void NPCShoot(int random_npc) { //parameter from 'movementNPC'
	for (int i = 0; i < 1; i++) {
		if (!bulletsNPC[i].isActive) {
			// Set the bullet's position and activate it
			bulletsNPC[i].x = enemies[random_npc].position.X;
			bulletsNPC[i].y = enemies[random_npc].position.Y + 1; // Adjust the Y position based on NPC's position
			bulletsNPC[i].isActive = 1;

			break;
		}
	}
}

void NPCShoot_Level2(int random_npc) { //parameter from 'movementNPC'
	int count = 0;
	for (int i = 0; i < ENEMY_COUNT - 9; i++) {

		if (!bulletsNPC[i].isActive && enemies[i].isActive == 1) {
			// Set the bullet's position and activate it
			bulletsNPC[i].x = enemies[i].position.X;
			bulletsNPC[i].y = enemies[i].position.Y + 1; // Adjust the Y position based on NPC's position - npc hilang sbb overlay
			bulletsNPC[i].isActive = 1;
			count++;
			break;
		}

		if (!bulletsNPC2[i].isActive && enemies2[i].isActive > 0) {
			// Set the bullet's position and activate it
			bulletsNPC2[i].x = enemies2[i].position.X;
			bulletsNPC2[i].y = enemies2[i].position.Y + 1; // Adjust the Y position based on NPC's position - npc hilang sbb overlay
			bulletsNPC2[i].isActive = 1;
			break;
		}
	}
}

void NPCShoot_Level3_LGun(int random_npc) { //parameter from 'movementNPC'
	int count = 0;
	for (int i = 0; i < MAX_BULLETS; i++) {

		if (!bulletsNPC[i].isActive && bossLGun[i].isActive == 1) {
			// Set the bullet's position and activate it
			bulletsNPC[i].x = bossLGun[random_npc].position.X++;
			if (bossLGun[random_npc].position.X > 5) {
				bossLGun[random_npc].position.X--;
			}
			bulletsNPC[i].y = bossLGun[random_npc].position.Y + 1; // Adjust the Y position based on NPC's position - npc hilang sbb overlay
			bulletsNPC[i].isActive = 1;
			break;
		}
	}
}

void NPCShoot_Level3_RGun(int random_npc) { //parameter from 'movementNPC'
	int count = 0;
	for (int i = 0; i < MAX_BULLETS; i++) {

		if (!bulletsNPC2[i].isActive && bossRGun[i].isActive == 1) {
			// Set the bullet's position and activate it
			bulletsNPC2[i].x = bossRGun[random_npc].position.X++;
			if (bossRGun[random_npc].position.X > 5) {
				bossRGun[random_npc].position.X--;
			}
			bulletsNPC2[i].y = bossRGun[random_npc].position.Y + 1; // Adjust the Y position based on NPC's position - npc hilang sbb overlay
			bulletsNPC2[i].isActive = 1;
			break;
		}
	}
}

void NPCShoot_Level3_Head(int random_npc) { //parameter from 'movementNPC'
	int count = 0;
	for (int i = 0; i < MAX_BULLETS; i++) {

		if (!bulletsBoss[i].isActive && bossHead[i].isActive == 1) {
			// Set the bullet's position and activate it
			bulletsBoss[i].x = bossHead[random_npc].position.X;
			bulletsBoss[i].y = bossHead[random_npc].position.Y + 1; // Adjust the Y position based on NPC's position - npc hilang sbb overlay
			bulletsBoss[i].isActive = 1;
			break;
		}
	}
}

void AddScore() {

	score += 10;
	SetCurrentCursorPos(82, 7);
	printf("%d", score);

}

int item_spawnRate = 0;

void langgar() {
	for (int i = 0; i < MAX_BULLETS; i++) {
		for (int j = 0; j < ENEMY_COUNT; j++) {

			//when the bullet hit the aliens, the points increase by 10
			// aliens will disappear
			// random item will appear after 10 successful hit

			//bullet will go away
			if ((enemies[j].isActive == 0 && bulletsPlayer->isActive && bulletsPlayer[i].x == enemies[j].position.X && bulletsPlayer[i].y == enemies[j].position.Y) ||
				(enemies[j].isActive == 0 && bulletsPlayer->isActive && bulletsPlayer[i].x == enemies[j].position.X + 1 && bulletsPlayer[i].y == enemies[j].position.Y) ||
				(enemies[j].isActive == 0 && bulletsPlayer->isActive && bulletsPlayer[i].x == enemies[j].position.X - 1 && bulletsPlayer[i].y == enemies[j].position.Y)) {
				bulletsPlayer[i].isActive = 1;

			}

			//bullet will hit the aliens
			if ((enemies[j].isActive != 0 && bulletsPlayer->isActive && bulletsPlayer[i].x == enemies[j].position.X && bulletsPlayer[i].y == enemies[j].position.Y) ||
				(enemies[j].isActive != 0 && bulletsPlayer->isActive && bulletsPlayer[i].x == enemies[j].position.X + 1 && bulletsPlayer[i].y == enemies[j].position.Y) ||
				(enemies[j].isActive != 0 && bulletsPlayer->isActive && bulletsPlayer[i].x == enemies[j].position.X - 1 && bulletsPlayer[i].y == enemies[j].position.Y)) {

				randItem += 1;
				enemies[j].isActive = 0;
				bulletsPlayer[i].isActive = 0;
				AddScore();
				enemyCount -= 1;

				if (randItem % 5 == 0) {
					SetCurrentCursorPos(84, 12);
					testItem++;
					printf("item : %d", testItem);

				}
				deleteNPC(enemies[j]);

			}
		}
	}
}

void langgar_level2() {
	for (int i = 0; i < MAX_BULLETS; i++) {


		for (int j = 0; j < (ENEMY_COUNT - 9); j++) {

			//when the bullet hit the aliens, the points increase by 10
			// aliens will disappear
			// random item will appear after 10 successful hit

			//bullet will hit the aliens #1
			if ((enemies[j].isActive == 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == enemies[j].position.X && bulletsPlayer[i].y == enemies[j].position.Y) ||
				(enemies[j].isActive == 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == enemies[j].position.X + 1 && bulletsPlayer[i].y == enemies[j].position.Y) ||
				(enemies[j].isActive == 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == enemies[j].position.X - 1 && bulletsPlayer[i].y == enemies[j].position.Y)) {
				bulletsPlayer[i].isActive = 1;

			}
			//bullet will hit the aliens
			if ((enemies[j].isActive != 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == enemies[j].position.X && bulletsPlayer[i].y == enemies[j].position.Y) ||
				(enemies[j].isActive != 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == enemies[j].position.X + 1 && bulletsPlayer[i].y == enemies[j].position.Y) ||
				(enemies[j].isActive != 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == enemies[j].position.X - 1 && bulletsPlayer[i].y == enemies[j].position.Y)) {

				randItem += 1;
				enemies[j].health -= 1;
				//if (enemies[j].health > 0) {
				//	SetCurrentCursorPos(enemies[j].position.X, enemies[j].position.Y);
				//	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
				//	printf("※");
				//	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				//}
				if (enemies[j].health == 0) {
					enemies[j].isActive = 0;
					AddScore();
					enemyCount_level2 -= 1;
				}
				bulletsPlayer[i].isActive = 0;

				if (randItem % 10 == 0) {
					SetCurrentCursorPos(84, 12);
					testItem++;
					printf("item : %d", testItem);

				}
				deleteNPC(enemies[j]);

			}
			//bullet will go away #2

			if ((enemies2[j].isActive == 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == enemies2[j].position.X && bulletsPlayer[i].y == enemies2[j].position.Y) ||
				(enemies2[j].isActive == 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == enemies2[j].position.X + 1 && bulletsPlayer[i].y == enemies2[j].position.Y) ||
				(enemies2[j].isActive == 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == enemies2[j].position.X - 1 && bulletsPlayer[i].y == enemies2[j].position.Y)) {
				bulletsPlayer[i].isActive = 1;

			}
			if ((enemies2[j].isActive != 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == enemies2[j].position.X && bulletsPlayer[i].y == enemies2[j].position.Y) ||
				(enemies2[j].isActive != 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == enemies2[j].position.X + 1 && bulletsPlayer[i].y == enemies2[j].position.Y) ||
				(enemies2[j].isActive != 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == enemies2[j].position.X - 1 && bulletsPlayer[i].y == enemies2[j].position.Y)) {

				randItem += 1;
				enemies2[j].health -= 1;
				//if (enemies2[j].health > 0) {
				//	SetCurrentCursorPos(enemies2[j].position.X, enemies2[j].position.Y);
				//	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
				//	printf("※");
				//	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				//}
				if (enemies2[j].health == 0) {
					enemies2[j].isActive = 0;
					AddScore();
					enemyCount_level2 -= 1;
				}
				bulletsPlayer[i].isActive = 0;

				if (randItem % 10 == 0) {
					SetCurrentCursorPos(84, 12);
					testItem++;
					printf("item : %d", testItem);

				}
				deleteNPC(enemies2[j]);

			}
		}


		//bullet will go away #copies
		for (int j = 0; j < 10; j++) {

			//when the bullet hit the aliens, the points increase by 10
			// aliens will disappear
			// random item will appear after 10 successful hit

			//bullet will hit the aliens #1
			if ((enemiesCopy[j].isActive == 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == enemiesCopy[j].position.X && bulletsPlayer[i].y == enemiesCopy[j].position.Y) ||
				(enemiesCopy[j].isActive == 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == enemiesCopy[j].position.X + 1 && bulletsPlayer[i].y == enemiesCopy[j].position.Y) ||
				(enemiesCopy[j].isActive == 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == enemiesCopy[j].position.X - 1 && bulletsPlayer[i].y == enemiesCopy[j].position.Y)) {
				bulletsPlayer[i].isActive = 1;

			}
			//bullet will hit the aliens
			if ((enemiesCopy[j].isActive != 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == enemiesCopy[j].position.X && bulletsPlayer[i].y == enemiesCopy[j].position.Y) ||
				(enemiesCopy[j].isActive != 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == enemiesCopy[j].position.X + 1 && bulletsPlayer[i].y == enemiesCopy[j].position.Y) ||
				(enemiesCopy[j].isActive != 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == enemiesCopy[j].position.X - 1 && bulletsPlayer[i].y == enemiesCopy[j].position.Y)) {

				randItem += 1;
				enemiesCopy[j].health -= 1;
				if (enemiesCopy[j].health == 0) {
					enemiesCopy[j].isActive = 0;
					AddScore();
					enemyCount_level2 -= 1;
				}
				bulletsPlayer[i].isActive = 0;

				if (randItem % 10 == 0) {
					SetCurrentCursorPos(84, 12);
					testItem++;
					printf("item : %d", testItem);

				}
				deleteNPC(enemiesCopy[j]);

			}
		}
	}
}


void langgar_level3() {

	item_spawnRate = 3;

	for (int i = 0; i < MAX_BULLETS; i++) {


		for (int j = 0; j <= 34; j++) {

			//bullet will hit the aliens #1
			if ((bossLGun[j].isActive == 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == bossLGun[j].position.X && bulletsPlayer[i].y == bossLGun[j].position.Y) ||
				(bossLGun[j].isActive == 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == bossLGun[j].position.X + 1 && bulletsPlayer[i].y == bossLGun[j].position.Y) ||
				(bossLGun[j].isActive == 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == bossLGun[j].position.X - 1 && bulletsPlayer[i].y == bossLGun[j].position.Y)) {
				bulletsPlayer[i].isActive = 1;

			}
			//bullet will hit the aliens
			if ((bossLGun[j].isActive != 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == bossLGun[j].position.X && bulletsPlayer[i].y == bossLGun[j].position.Y + 1) ||
				(bossLGun[j].isActive != 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == bossLGun[j].position.X + 1 && bulletsPlayer[i].y == bossLGun[j].position.Y + 1) ||
				(bossLGun[j].isActive != 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == bossLGun[j].position.X - 1 && bulletsPlayer[i].y == bossLGun[j].position.Y + 1)) {

				randItem += 1;

				bulletsPlayer[i].isActive = 0;
				SetCurrentCursorPos(bulletsPlayer[i].x, bulletsPlayer[i].y);
				printf("  ");

				for (int k = 0; k <= 34; k++) {
					bossLGun[k].health -= 1;

				}
				bossLGunHealth -= 1;
				if (bossLGun[j].health == 0) {
					for (int k = 0; k <= 34; k++) {
						bossLGun[k].isActive = 0;
						deleteNPC(bossLGun[k]);
					}

					for (int i = 0; i < 25; i++) {
						AddScore();
					}

					enemyCount_level3 -= 1;
				}

				bulletsPlayer[i].isActive = 0;

				if (randItem % item_spawnRate == 0) {
					SetCurrentCursorPos(84, 12);
					testItem++;
					printf("item : %d", testItem);
				}
			}

			//bullet will go away #2
			if ((bossRGun[j].isActive == 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == bossRGun[j].position.X && bulletsPlayer[i].y == bossRGun[j].position.Y) ||
				(bossRGun[j].isActive == 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == bossRGun[j].position.X + 1 && bulletsPlayer[i].y == bossRGun[j].position.Y) ||
				(bossRGun[j].isActive == 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == bossRGun[j].position.X - 1 && bulletsPlayer[i].y == bossRGun[j].position.Y)) {
				bulletsPlayer[i].isActive = 1;

			}
			//bullet will hit the aliens
			if ((bossRGun[j].isActive != 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == bossRGun[j].position.X && bulletsPlayer[i].y == bossRGun[j].position.Y + 1) ||
				(bossRGun[j].isActive != 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == bossRGun[j].position.X + 1 && bulletsPlayer[i].y == bossRGun[j].position.Y + 1) ||
				(bossRGun[j].isActive != 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == bossRGun[j].position.X - 1 && bulletsPlayer[i].y == bossRGun[j].position.Y + 1)) {

				randItem += 1;

				bulletsPlayer[i].isActive = 0;
				SetCurrentCursorPos(bulletsPlayer[i].x, bulletsPlayer[i].y);
				printf("  ");

				for (int k = 0; k <= 34; k++) {
					bossRGun[k].health -= 1;

				}
				bossRGunHealth -= 1;

				if (bossRGun[j].health == 0) {
					for (int k = 0; k <= 34; k++) {
						bossRGun[k].isActive = 0;
						deleteNPC(bossRGun[k]);
					}

					for (int i = 0; i < 25; i++) {
						AddScore();
					}

					enemyCount_level3 -= 1;
				}

				bulletsPlayer[i].isActive = 0;

				if (randItem % item_spawnRate == 0) {
					SetCurrentCursorPos(84, 12);
					testItem++;
					printf("item : %d", testItem);
				}
			}
		}


		for (int j = 0; j <= 39; j++) {

			//when the bullet hit the aliens, the points increase by 10
			// aliens will disappear
			// random item will appear after 10 successful hit

			//bullet will hit the aliens #1
			if ((bossHead[j].isActive == 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == bossHead[j].position.X && bulletsPlayer[i].y == bossHead[j].position.Y) ||
				(bossHead[j].isActive == 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == bossHead[j].position.X + 1 && bulletsPlayer[i].y == bossHead[j].position.Y) ||
				(bossHead[j].isActive == 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == bossHead[j].position.X - 1 && bulletsPlayer[i].y == bossHead[j].position.Y)) {
				bulletsPlayer[i].isActive = 1;

			}
			//bullet will hit the aliens
			if ((bossHead[j].isActive != 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == bossHead[j].position.X && bulletsPlayer[i].y == bossHead[j].position.Y + 1) ||
				(bossHead[j].isActive != 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == bossHead[j].position.X + 1 && bulletsPlayer[i].y == bossHead[j].position.Y + 1) ||
				(bossHead[j].isActive != 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == bossHead[j].position.X - 1 && bulletsPlayer[i].y == bossHead[j].position.Y + 1)) {

				randItem += 1;

				bulletsPlayer[i].isActive = 0;
				SetCurrentCursorPos(bulletsPlayer[i].x, bulletsPlayer[i].y);
				printf("  ");

				for (int k = 0; k <= 39; k++) {
					bossHead[k].health -= 1;

				}

				bossHeadHealth -= 1;

				if (bossHead[j].health == 0) {
					for (int k = 0; k <= 39; k++) {
						bossHead[k].isActive = 0;
						deleteNPC(bossHead[k]);
					}

					for (int i = 0; i < 50; i++) {
						AddScore();
					}

					enemyCount_level3 -= 1;
				}

				bulletsPlayer[i].isActive = 0;

				if (randItem % item_spawnRate == 0) {
					SetCurrentCursorPos(84, 12);
					testItem++;
					printf("item : %d", testItem);
				}
			}
		}

		for (int j = 0; j < 22; j++) {

			//bullet will hit the aliens
			if ((wall[j].isActive != 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == wall[j].position.X && bulletsPlayer[i].y == wall[j].position.Y + 1) ||
				(wall[j].isActive != 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == wall[j].position.X + 1 && bulletsPlayer[i].y == wall[j].position.Y + 1) ||
				(wall[j].isActive != 0 && bulletsPlayer[i].isActive == 1 && bulletsPlayer[i].x == wall[j].position.X - 1 && bulletsPlayer[i].y == wall[j].position.Y + 1)) {

				bulletsPlayer[i].isActive = 0;
				SetCurrentCursorPos(bulletsPlayer[i].x, bulletsPlayer[i].y);
				printf("  ");

			}
		}
	}
}

void addLife() { //♥ 

	if (life < 3) {
		SetCurrentCursorPos(82, 5);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
		printf("♥ ♥ ♥");
		life = 3;
	}

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

void minusLife() { //♥ 

	if (life == 2) {
		SetCurrentCursorPos(82, 5);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
		printf("♥ ♥ ♡");
	}
	else if (life == 1) {
		SetCurrentCursorPos(82, 5);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
		printf("♥ ♡ ♡");
	}
	else if (life == 0) {
		SetCurrentCursorPos(82, 5);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
		printf("♡ ♡ ♡");
	}

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}


void bulletLanggarPlayer() {
	int flag = 0;
	for (int i = 0; i < MAX_BULLETS; i++) {
		for (int j = 0; j < ENEMY_COUNT; j++) {

			//when the bullet hit the aliens, the points increase by 10
			// aliens will disappear
			// random item will appear after 10 successful hit

			//bullet will go away


			//bullet will hit the aliens
			if ((bulletsNPC[i].isActive && bulletsNPC[i].x == startX && bulletsNPC[i].y == startY - 1) ||
				(bulletsNPC[i].isActive && bulletsNPC[i].x == startX + 1 && bulletsNPC[i].y == startY - 1) ||
				(bulletsNPC[i].isActive && bulletsNPC[i].x == startX - 1 && bulletsNPC[i].y == startY - 1)) {
				bulletsNPC[i].isActive = 0;
				SetCurrentCursorPos(bulletsNPC[i].x, bulletsNPC[i].y);
				printf("  ");
				SetCurrentCursorPos(startX, startY);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
				printf("▲");
				flag++;
			}

			if ((bulletsNPC2[i].isActive && bulletsNPC2[i].x == startX && bulletsNPC2[i].y == startY - 1) ||
				(bulletsNPC2[i].isActive && bulletsNPC2[i].x == startX + 1 && bulletsNPC2[i].y == startY - 1) ||
				(bulletsNPC2[i].isActive && bulletsNPC2[i].x == startX - 1 && bulletsNPC2[i].y == startY - 1)) {
				bulletsNPC2[i].isActive = 0;
				SetCurrentCursorPos(bulletsNPC2[i].x, bulletsNPC2[i].y);
				printf("  ");
				SetCurrentCursorPos(startX, startY);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
				printf("▲");
				flag++;
			}

			if ((bulletsBoss[i].isActive && bulletsBoss[i].x == startX && bulletsBoss[i].y == startY - 1) ||
				(bulletsBoss[i].isActive && bulletsBoss[i].x == startX + 1 && bulletsBoss[i].y == startY - 1) ||
				(bulletsBoss[i].isActive && bulletsBoss[i].x == startX - 1 && bulletsBoss[i].y == startY - 1)) {
				bulletsBoss[i].isActive = 0;
				SetCurrentCursorPos(bulletsBoss[i].x, bulletsBoss[i].y);
				printf("  ");
				SetCurrentCursorPos(startX, startY);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
				printf("▲");
				flag++;
			}
		}
	}
	if (flag > 0) {
		life -= 1;
		minusLife();
	}
}

int collisionPlayerItem(Bullet item[]) {

	for (int i = 0; i <= 20; i++) {
		if ((item[i].isActive && item[i].x == startX && item[i].y == startY - 1) ||
			(item[i].isActive && item[i].x == startX + 1 && item[i].y == startY - 1) ||
			(item[i].isActive && item[i].x == startX - 1 && item[i].y == startY - 1)) {
			return 1; // Collision detected
		}
	}

	return 0; // No collision
}

void spawnItem() {
	srand(time(0));
	int xpos = 0;
	int ypos = GBOARD_ORIGIN_Y + 17;

	//change what type of item spawn

	//int spawn = RandomInRange(0, 1);
	//int spawn = 1;

	int spawn = 0;

	for (int i = 0; i < 20; i++) {
		if (heart[i].y == GBOARD_HEIGHT + 2) {
			spawn = RandomInRange(0, 1);
		}
		if (collisionPlayerItem(heart)) {
			spawn = RandomInRange(0, 1);
		}
	}

	int trackItem = testItem + 1;

	for (int i = 0; i < 20; i++) {
		xpos = HEART[i].position.X;

		if (testItem > 0 && testItem == trackItem) {

			heart[i].y = ypos;
			heart[i].x = xpos;
			heart[i].isActive = 1;

			trackItem++;
		}
	}


	for (int i = 0; i < 20; i++) {

		if (heart[i].isActive) {
			SetCurrentCursorPos(heart[i].x, heart[i].y);
			printf(" ");
			heart[i].y++;
			if (collisionPlayerItem(heart)) {
				SetCurrentCursorPos(heart[i].x, heart[i].y);
				printf(" ");
				heart[i].isActive = 0;
				testItem = 0;

				if(life < 3){
					addLife();
				}

			}
			else if (heart[i].y == GBOARD_HEIGHT + 2) {
				heart[i].isActive = 0;
				testItem = 0;
			}
			else {
				SetCurrentCursorPos(heart[i].x, heart[i].y);
				printf("H"); //♥ ♡

			}
		}
	}
}


void movementPlayer() {
	if (_kbhit()) {
		char key = _getch();

		SetCurrentCursorPos(startX, startY);


		if ((key == 65 || key == 97) && startX > GBOARD_ORIGIN_X + 3) {
			shiftLeft();

		}
		if ((key == 68 || key == 100) && startX < (GBOARD_ORIGIN_X + (GBOARD_WIDTH + 1) * 2 - 3)) {
			shiftRight();

		}
		// Check if spacebar is pressed to fire a bullet
		if (key == SPACE) { // ' ' is spacebar, 13 is Enter key
			playerShoot();
		}
	}
}

void initItem() {
	for (int i = 0; i < ENEMY_COUNT; i++) {

		HEART[i].position.X = GBOARD_ORIGIN_X + 3 + i * 3;
		HEART[i].position.Y = GBOARD_ORIGIN_Y + 15;
		HEART[i].isActive = 1;

	}
}

void initEnemies() {
	for (int i = 0; i < ENEMY_COUNT; i++) {

		if (i < 10) {
			enemies[i].position.X = GBOARD_ORIGIN_X + 3 + i * 4;
			enemies[i].position.Y = GBOARD_ORIGIN_Y + 2;
		}
		else {
			enemies[i].position.X = GBOARD_ORIGIN_X + 3 + (i - 10) * 4;
			enemies[i].position.Y = GBOARD_ORIGIN_Y + 4;
		}

		enemies[i].health = 1;
		enemies[i].isActive = 1;
		enemies[i].movement = 1;
	}
}

void initEnemies_Level2() {

	for (int i = 0; i < ENEMY_COUNT - 9; i++) {

		//enemy type "\/" semiboss ig
		if (i < 5) { // this is "\"
			enemies[i].position.X = GBOARD_ORIGIN_X + 3 + i * 2;
			enemies[i].position.Y = GBOARD_ORIGIN_Y + 2 + i;


			enemies2[i].position.X = GBOARD_ORIGIN_X + 29 + i * 2;
			enemies2[i].position.Y = GBOARD_ORIGIN_Y + 2 + i;

		}
		else if (i > 5 || i < 10) { // this is "/"
			enemies[i].position.X = GBOARD_ORIGIN_X + 3 + i * 2;
			enemies[i].position.Y = GBOARD_ORIGIN_Y + 8 - (i - 4);


			enemies2[i].position.X = GBOARD_ORIGIN_X + 29 + i * 2;
			enemies2[i].position.Y = GBOARD_ORIGIN_Y + 8 - (i - 4);
		}

		enemies[i].health = 2;
		enemies[i].isActive = 1;
		enemies[i].movement = 1;

		enemies2[i].health = 2;
		enemies2[i].isActive = 1;
		enemies2[i].movement = 1;
	}

	for (int i = 0; i < 10; i++) {
		if (i < 5) {
			enemiesCopy[i].position.X = GBOARD_ORIGIN_X + 4 + i * 4;
			enemiesCopy[i].position.Y = GBOARD_ORIGIN_Y + 14;
		}
		else {
			enemiesCopy[i].position.X = GBOARD_ORIGIN_X + 4 + (i - 5) * 4;
			enemiesCopy[i].position.Y = GBOARD_ORIGIN_Y + 16;
		}

		enemiesCopy[i].health = 2;
		enemiesCopy[i].isActive = 1;
		enemiesCopy[i].movement = 1;
	}
}

void initEnemies_Level3() {
	//GBOARD_ORIGIN_X +1 -> paling leftside
	//GBOARD_ORIGIN_X + GBOARD_WIDTH * 2 -> paling 

	//================================================================================================
	//A atas sekali
	bossLGun[0].position.X = GBOARD_ORIGIN_X + 9;
	bossLGun[0].position.Y = GBOARD_ORIGIN_Y + 1;

	bossLGun[1].position.X = GBOARD_ORIGIN_X + 7;
	bossLGun[1].position.Y = GBOARD_ORIGIN_Y + 2;

	bossLGun[2].position.X = GBOARD_ORIGIN_X + 11;
	bossLGun[2].position.Y = GBOARD_ORIGIN_Y + 2;

	// | left gelanggang tangan
	bossLGun[3].position.X = GBOARD_ORIGIN_X + 5;
	bossLGun[3].position.Y = GBOARD_ORIGIN_Y + 3;

	bossLGun[4].position.X = GBOARD_ORIGIN_X + 5;
	bossLGun[4].position.Y = GBOARD_ORIGIN_Y + 4;

	bossLGun[5].position.X = GBOARD_ORIGIN_X + 5;
	bossLGun[5].position.Y = GBOARD_ORIGIN_Y + 5;

	// | right gelanggang tangan
	bossLGun[6].position.X = GBOARD_ORIGIN_X + 13;
	bossLGun[6].position.Y = GBOARD_ORIGIN_Y + 3;

	bossLGun[7].position.X = GBOARD_ORIGIN_X + 13;
	bossLGun[7].position.Y = GBOARD_ORIGIN_Y + 4;

	bossLGun[8].position.X = GBOARD_ORIGIN_X + 13;
	bossLGun[8].position.Y = GBOARD_ORIGIN_Y + 5;

	// ㅡ at gelanggang tangan
	bossLGun[9].position.X = GBOARD_ORIGIN_X + 7;
	bossLGun[9].position.Y = GBOARD_ORIGIN_Y + 5;

	bossLGun[10].position.X = GBOARD_ORIGIN_X + 9;
	bossLGun[10].position.Y = GBOARD_ORIGIN_Y + 5;

	bossLGun[11].position.X = GBOARD_ORIGIN_X + 11;
	bossLGun[11].position.Y = GBOARD_ORIGIN_Y + 5;

	//left paw
	bossLGun[12].position.X = GBOARD_ORIGIN_X + 3;
	bossLGun[12].position.Y = GBOARD_ORIGIN_Y + 6;

	bossLGun[13].position.X = GBOARD_ORIGIN_X + 1;
	bossLGun[13].position.Y = GBOARD_ORIGIN_Y + 7;

	bossLGun[14].position.X = GBOARD_ORIGIN_X + 1;
	bossLGun[14].position.Y = GBOARD_ORIGIN_Y + 8;

	bossLGun[15].position.X = GBOARD_ORIGIN_X + 1;
	bossLGun[15].position.Y = GBOARD_ORIGIN_Y + 9;

	// ㅡ at paw
	bossLGun[16].position.X = GBOARD_ORIGIN_X + 3;
	bossLGun[16].position.Y = GBOARD_ORIGIN_Y + 10;

	bossLGun[17].position.X = GBOARD_ORIGIN_X + 5;
	bossLGun[17].position.Y = GBOARD_ORIGIN_Y + 10;
	bossLGun[18].position.X = GBOARD_ORIGIN_X + 7;
	bossLGun[18].position.Y = GBOARD_ORIGIN_Y + 10;
	bossLGun[19].position.X = GBOARD_ORIGIN_X + 9;
	bossLGun[19].position.Y = GBOARD_ORIGIN_Y + 10;
	bossLGun[20].position.X = GBOARD_ORIGIN_X + 11;
	bossLGun[20].position.Y = GBOARD_ORIGIN_Y + 10;
	bossLGun[21].position.X = GBOARD_ORIGIN_X + 13;
	bossLGun[21].position.Y = GBOARD_ORIGIN_Y + 10;
	bossLGun[22].position.X = GBOARD_ORIGIN_X + 15;
	bossLGun[22].position.Y = GBOARD_ORIGIN_Y + 10;

	//right paw
	bossLGun[23].position.X = GBOARD_ORIGIN_X + 15;
	bossLGun[23].position.Y = GBOARD_ORIGIN_Y + 6;
	bossLGun[24].position.X = GBOARD_ORIGIN_X + 17;
	bossLGun[24].position.Y = GBOARD_ORIGIN_Y + 7;
	bossLGun[25].position.X = GBOARD_ORIGIN_X + 17;
	bossLGun[25].position.Y = GBOARD_ORIGIN_Y + 8;
	bossLGun[26].position.X = GBOARD_ORIGIN_X + 17;
	bossLGun[26].position.Y = GBOARD_ORIGIN_Y + 9;

	// | inside paw left
	bossLGun[27].position.X = GBOARD_ORIGIN_X + 6;
	bossLGun[27].position.Y = GBOARD_ORIGIN_Y + 9;
	bossLGun[28].position.X = GBOARD_ORIGIN_X + 6;
	bossLGun[28].position.Y = GBOARD_ORIGIN_Y + 8;

	// | inside paw right
	bossLGun[29].position.X = GBOARD_ORIGIN_X + 12;
	bossLGun[29].position.Y = GBOARD_ORIGIN_Y + 9;
	bossLGun[30].position.X = GBOARD_ORIGIN_X + 12;
	bossLGun[30].position.Y = GBOARD_ORIGIN_Y + 8;

	//bullet comin out from
	bossLGun[31].position.X = GBOARD_ORIGIN_X + 4;
	bossLGun[31].position.Y = GBOARD_ORIGIN_Y + 11;

	bossLGun[32].position.X = GBOARD_ORIGIN_X + 8;
	bossLGun[32].position.Y = GBOARD_ORIGIN_Y + 11;

	bossLGun[33].position.X = GBOARD_ORIGIN_X + 12;
	bossLGun[33].position.Y = GBOARD_ORIGIN_Y + 11;

	bossLGun[34].position.X = GBOARD_ORIGIN_X + 15;
	bossLGun[34].position.Y = GBOARD_ORIGIN_Y + 11;
	//================================================================================================
	int r = 43;

	//A atas sekali
	bossRGun[0].position.X = GBOARD_ORIGIN_X + 9 + r;
	bossRGun[0].position.Y = GBOARD_ORIGIN_Y + 1;
	bossRGun[1].position.X = GBOARD_ORIGIN_X + 7 + r;
	bossRGun[1].position.Y = GBOARD_ORIGIN_Y + 2;
	bossRGun[2].position.X = GBOARD_ORIGIN_X + 11 + r;
	bossRGun[2].position.Y = GBOARD_ORIGIN_Y + 2;

	// | left gelanggang tangan
	bossRGun[3].position.X = GBOARD_ORIGIN_X + 5 + r;
	bossRGun[3].position.Y = GBOARD_ORIGIN_Y + 3;
	bossRGun[4].position.X = GBOARD_ORIGIN_X + 5 + r;
	bossRGun[4].position.Y = GBOARD_ORIGIN_Y + 4;
	bossRGun[5].position.X = GBOARD_ORIGIN_X + 5 + r;
	bossRGun[5].position.Y = GBOARD_ORIGIN_Y + 5;

	// | right gelanggang tangan
	bossRGun[6].position.X = GBOARD_ORIGIN_X + 13 + r;
	bossRGun[6].position.Y = GBOARD_ORIGIN_Y + 3;
	bossRGun[7].position.X = GBOARD_ORIGIN_X + 13 + r;
	bossRGun[7].position.Y = GBOARD_ORIGIN_Y + 4;
	bossRGun[8].position.X = GBOARD_ORIGIN_X + 13 + r;
	bossRGun[8].position.Y = GBOARD_ORIGIN_Y + 5;

	// ㅡ at gelanggang tangan
	bossRGun[9].position.X = GBOARD_ORIGIN_X + 7 + r;
	bossRGun[9].position.Y = GBOARD_ORIGIN_Y + 5;
	bossRGun[10].position.X = GBOARD_ORIGIN_X + 9 + r;
	bossRGun[10].position.Y = GBOARD_ORIGIN_Y + 5;
	bossRGun[11].position.X = GBOARD_ORIGIN_X + 11 + r;
	bossRGun[11].position.Y = GBOARD_ORIGIN_Y + 5;

	//left paw
	bossRGun[12].position.X = GBOARD_ORIGIN_X + 3 + r;
	bossRGun[12].position.Y = GBOARD_ORIGIN_Y + 6;
	bossRGun[13].position.X = GBOARD_ORIGIN_X + 1 + r;
	bossRGun[13].position.Y = GBOARD_ORIGIN_Y + 7;
	bossRGun[14].position.X = GBOARD_ORIGIN_X + 1 + r;
	bossRGun[14].position.Y = GBOARD_ORIGIN_Y + 8;
	bossRGun[15].position.X = GBOARD_ORIGIN_X + 1 + r;
	bossRGun[15].position.Y = GBOARD_ORIGIN_Y + 9;

	// ㅡ at paw

	bossRGun[16].position.X = GBOARD_ORIGIN_X + 3 + r;
	bossRGun[16].position.Y = GBOARD_ORIGIN_Y + 10;
	bossRGun[17].position.X = GBOARD_ORIGIN_X + 5 + r;
	bossRGun[17].position.Y = GBOARD_ORIGIN_Y + 10;
	bossRGun[18].position.X = GBOARD_ORIGIN_X + 7 + r;
	bossRGun[18].position.Y = GBOARD_ORIGIN_Y + 10;
	bossRGun[19].position.X = GBOARD_ORIGIN_X + 9 + r;
	bossRGun[19].position.Y = GBOARD_ORIGIN_Y + 10;
	bossRGun[20].position.X = GBOARD_ORIGIN_X + 11 + r;
	bossRGun[20].position.Y = GBOARD_ORIGIN_Y + 10;
	bossRGun[21].position.X = GBOARD_ORIGIN_X + 13 + r;
	bossRGun[21].position.Y = GBOARD_ORIGIN_Y + 10;
	bossRGun[22].position.X = GBOARD_ORIGIN_X + 15 + r;
	bossRGun[22].position.Y = GBOARD_ORIGIN_Y + 10;

	//right paw
	bossRGun[23].position.X = GBOARD_ORIGIN_X + 15 + r;
	bossRGun[23].position.Y = GBOARD_ORIGIN_Y + 6;
	bossRGun[24].position.X = GBOARD_ORIGIN_X + 17 + r;
	bossRGun[24].position.Y = GBOARD_ORIGIN_Y + 7;
	bossRGun[25].position.X = GBOARD_ORIGIN_X + 17 + r;
	bossRGun[25].position.Y = GBOARD_ORIGIN_Y + 8;
	bossRGun[26].position.X = GBOARD_ORIGIN_X + 17 + r;
	bossRGun[26].position.Y = GBOARD_ORIGIN_Y + 9;

	// | inside paw left
	bossRGun[27].position.X = GBOARD_ORIGIN_X + 6 + r;
	bossRGun[27].position.Y = GBOARD_ORIGIN_Y + 9;
	bossRGun[28].position.X = GBOARD_ORIGIN_X + 6 + r;
	bossRGun[28].position.Y = GBOARD_ORIGIN_Y + 8;
	// | inside paw right
	bossRGun[29].position.X = GBOARD_ORIGIN_X + 12 + r;
	bossRGun[29].position.Y = GBOARD_ORIGIN_Y + 9;
	bossRGun[30].position.X = GBOARD_ORIGIN_X + 12 + r;
	bossRGun[30].position.Y = GBOARD_ORIGIN_Y + 8;

	//bullet comin out from
	bossRGun[31].position.X = GBOARD_ORIGIN_X + 4 + r;
	bossRGun[31].position.Y = GBOARD_ORIGIN_Y + 11;
	bossRGun[32].position.X = GBOARD_ORIGIN_X + 8 + r;
	bossRGun[32].position.Y = GBOARD_ORIGIN_Y + 11;
	bossRGun[33].position.X = GBOARD_ORIGIN_X + 12 + r;
	bossRGun[33].position.Y = GBOARD_ORIGIN_Y + 11;
	bossRGun[34].position.X = GBOARD_ORIGIN_X + 15 + r;
	bossRGun[34].position.Y = GBOARD_ORIGIN_Y + 11;
	//================================================================================================

	//ear left & right
	bossHead[0].position.X = GBOARD_ORIGIN_X + 20;
	bossHead[0].position.Y = GBOARD_ORIGIN_Y + 6;

	bossHead[1].position.X = GBOARD_ORIGIN_X + 41;
	bossHead[1].position.Y = GBOARD_ORIGIN_Y + 6;

	//leftside (above eye)

	bossHead[2].position.X = GBOARD_ORIGIN_X + 22;
	bossHead[2].position.Y = GBOARD_ORIGIN_Y + 5;

	bossHead[3].position.X = GBOARD_ORIGIN_X + 22;
	bossHead[3].position.Y = GBOARD_ORIGIN_Y + 4;

	bossHead[4].position.X = GBOARD_ORIGIN_X + 24;
	bossHead[4].position.Y = GBOARD_ORIGIN_Y + 3;

	bossHead[5].position.X = GBOARD_ORIGIN_X + 26;
	bossHead[5].position.Y = GBOARD_ORIGIN_Y + 2;

	bossHead[6].position.X = GBOARD_ORIGIN_X + 28;
	bossHead[6].position.Y = GBOARD_ORIGIN_Y + 1;

	//rightside (above eye)
	bossHead[7].position.X = GBOARD_ORIGIN_X + 37;
	bossHead[7].position.Y = GBOARD_ORIGIN_Y + 3;

	bossHead[8].position.X = GBOARD_ORIGIN_X + 35;
	bossHead[8].position.Y = GBOARD_ORIGIN_Y + 2;

	bossHead[9].position.X = GBOARD_ORIGIN_X + 33;
	bossHead[9].position.Y = GBOARD_ORIGIN_Y + 1;

	bossHead[10].position.X = GBOARD_ORIGIN_X + 39;
	bossHead[10].position.Y = GBOARD_ORIGIN_Y + 5;

	bossHead[11].position.X = GBOARD_ORIGIN_X + 39;
	bossHead[11].position.Y = GBOARD_ORIGIN_Y + 4;

	//completing above head (V-shaped)
	bossHead[12].position.X = GBOARD_ORIGIN_X + 32;
	bossHead[12].position.Y = GBOARD_ORIGIN_Y + 2;

	bossHead[13].position.X = GBOARD_ORIGIN_X + 31;
	bossHead[13].position.Y = GBOARD_ORIGIN_Y + 3;

	bossHead[14].position.X = GBOARD_ORIGIN_X + 29;
	bossHead[14].position.Y = GBOARD_ORIGIN_Y + 2;

	// ㅡ below face
	bossHead[15].position.X = GBOARD_ORIGIN_X + 24;
	bossHead[15].position.Y = GBOARD_ORIGIN_Y + 9;
	bossHead[16].position.X = GBOARD_ORIGIN_X + 26;
	bossHead[16].position.Y = GBOARD_ORIGIN_Y + 10;
	bossHead[17].position.X = GBOARD_ORIGIN_X + 28;
	bossHead[17].position.Y = GBOARD_ORIGIN_Y + 10;
	bossHead[18].position.X = GBOARD_ORIGIN_X + 30;
	bossHead[18].position.Y = GBOARD_ORIGIN_Y + 10;
	bossHead[19].position.X = GBOARD_ORIGIN_X + 32;
	bossHead[19].position.Y = GBOARD_ORIGIN_Y + 10;
	bossHead[20].position.X = GBOARD_ORIGIN_X + 34;
	bossHead[20].position.Y = GBOARD_ORIGIN_Y + 10;
	bossHead[21].position.X = GBOARD_ORIGIN_X + 36;
	bossHead[21].position.Y = GBOARD_ORIGIN_Y + 10;
	bossHead[22].position.X = GBOARD_ORIGIN_X + 37;
	bossHead[22].position.Y = GBOARD_ORIGIN_Y + 9;

	//below part of face (right & left)
	bossHead[23].position.X = GBOARD_ORIGIN_X + 22;
	bossHead[23].position.Y = GBOARD_ORIGIN_Y + 7;
	bossHead[24].position.X = GBOARD_ORIGIN_X + 22;
	bossHead[24].position.Y = GBOARD_ORIGIN_Y + 8;
	bossHead[25].position.X = GBOARD_ORIGIN_X + 39;
	bossHead[25].position.Y = GBOARD_ORIGIN_Y + 7;
	bossHead[26].position.X = GBOARD_ORIGIN_X + 39;
	bossHead[26].position.Y = GBOARD_ORIGIN_Y + 8;

	//face
	//left eye
	bossHead[27].position.X = GBOARD_ORIGIN_X + 25;
	bossHead[27].position.Y = GBOARD_ORIGIN_Y + 4;

	bossHead[28].position.X = GBOARD_ORIGIN_X + 27;
	bossHead[28].position.Y = GBOARD_ORIGIN_Y + 4;

	bossHead[29].position.X = GBOARD_ORIGIN_X + 27;
	bossHead[29].position.Y = GBOARD_ORIGIN_Y + 5;

	bossHead[30].position.X = GBOARD_ORIGIN_X + 29;
	bossHead[30].position.Y = GBOARD_ORIGIN_Y + 4;

	//right eye
	bossHead[31].position.X = GBOARD_ORIGIN_X + 33;
	bossHead[31].position.Y = GBOARD_ORIGIN_Y + 4;

	bossHead[32].position.X = GBOARD_ORIGIN_X + 35;
	bossHead[32].position.Y = GBOARD_ORIGIN_Y + 4;

	bossHead[33].position.X = GBOARD_ORIGIN_X + 35;
	bossHead[33].position.Y = GBOARD_ORIGIN_Y + 5;

	bossHead[34].position.X = GBOARD_ORIGIN_X + 37;
	bossHead[34].position.Y = GBOARD_ORIGIN_Y + 4;

	//mouth
	//leftside
	bossHead[35].position.X = GBOARD_ORIGIN_X + 28;
	bossHead[35].position.Y = GBOARD_ORIGIN_Y + 9;
	bossHead[36].position.X = GBOARD_ORIGIN_X + 29;
	bossHead[36].position.Y = GBOARD_ORIGIN_Y + 8;
	//middle
	bossHead[37].position.X = GBOARD_ORIGIN_X + 31;
	bossHead[37].position.Y = GBOARD_ORIGIN_Y + 7;
	//rightside
	bossHead[38].position.X = GBOARD_ORIGIN_X + 32;
	bossHead[38].position.Y = GBOARD_ORIGIN_Y + 8;
	bossHead[39].position.X = GBOARD_ORIGIN_X + 33;
	bossHead[39].position.Y = GBOARD_ORIGIN_Y + 9;


	for (int i = 0; i <= 34; i++) {
		bossLGun[i].health = 40;
		bossLGun[i].isActive = 1;
		bossLGun[i].movement = 1;

		bossRGun[i].health = 40;
		bossRGun[i].isActive = 1;
		bossRGun[i].movement = 1;

		bossLGunHealth = bossLGun[i].health;
		bossRGunHealth = bossRGun[i].health;

	}

	for (int i = 0; i <= 39; i++) {
		bossHead[i].health = 50;
		bossHead[i].isActive = 1;
		bossHead[i].movement = 1;

		bossHeadHealth = bossHead[i].health;
	}
}

void initWall() {
	for (int i = 0; i < 22; i++) {

		if (i == 0 || i == 21) {
			wall[i].position.X = GBOARD_ORIGIN_X + 20 + i;
			wall[i].position.Y = GBOARD_ORIGIN_Y + 10;
		}
		else if (i == 1 || i == 20) {
			wall[i].position.X = GBOARD_ORIGIN_X + 20 + i;
			wall[i].position.Y = GBOARD_ORIGIN_Y + 11;
		}
		else {
			wall[i].position.X = GBOARD_ORIGIN_X + 20 + i;
			wall[i].position.Y = GBOARD_ORIGIN_Y + 12;
		}

		wall[i].isActive = 1;
		wall[i].health = 60;
	}
}

//--------------------------------------------------------------------------------------------------
int trackItemCount;

void itemDrop(int random_npc) { //parameter from 'movementNPC'
	for (int i = 0; i < 1; i++) {
		if (!heart[i].isActive) {
			// Set the bullet's position and activate it
			heart[i].x = HEART[random_npc].position.X;
			heart[i].y = HEART[random_npc].position.Y + 1; // Adjust the Y position based on NPC's position
			heart[i].isActive = 1;
			//if(heart[i].y == s)

			break;
		}

		if (!tp[i].isActive) {
			// Set the bullet's position and activate it
			tp[i].x = TP[random_npc].position.X;
			tp[i].y = TP[random_npc].position.Y + 1; // Adjust the Y position based on NPC's position
			tp[i].isActive = 1;

			break;
		}
	}
}


void movementItem() {
	srand((unsigned int)time(NULL));
	int random_item = 0;
	int count = 0;
	trackItemCount = testItem;

	if (startX > GBOARD_ORIGIN_X && startX < GBOARD_ORIGIN_X + GBOARD_WIDTH + 1) {
		random_item = RandomInRange(0, 5);
	}
	else if (startX >= GBOARD_ORIGIN_X + GBOARD_WIDTH + 1 && startX < GBOARD_ORIGIN_X + GBOARD_WIDTH * 2) {
		random_item = RandomInRange(14, 20);
	}

	for (int i = 0; i < ENEMY_COUNT; i++) {

		if (HEART[random_item].isActive == 1 && testItem > 0) {
			itemDrop(random_item);
		}
	}
}

//--------------------------------------------------------------------------------------------------

void movementNPC() {
	srand((unsigned int)time(NULL));
	int random_npc;
	int count = 0;
	int temp_Y;

	for (int i = 0; i < ENEMY_COUNT; i++) {
		deleteNPC(enemies[i]);



		if (enemies[ENEMY_COUNT - 1].position.X == (GBOARD_ORIGIN_X + (GBOARD_WIDTH + 1) * 2 - 3) || enemies[0].position.X == GBOARD_ORIGIN_X + 2) {
			enemies[i].position.Y += 2;

			enemies[i].movement *= -1;
		}

		if (enemies[0].movement == 1) {
			enemies[i].position.X++;
		}
		else if (enemies[0].movement == -1) {
			enemies[i].position.X--;
		}

		showNPC(enemies[i]);


		random_npc = (rand() % ENEMY_COUNT);
		//random_x = (rand() % (GBOARD_ORIGIN_X + (GBOARD_WIDTH + 1) * 2 - 3) - (GBOARD_ORIGIN_X + 3));
		temp_Y = enemies[ENEMY_COUNT - 1].position.Y;
		if (count == 0 && enemies[random_npc].isActive == 1) {
			NPCShoot(random_npc);
			count++;
		}
		if (count > 0 && temp_Y < enemies[ENEMY_COUNT - 1].position.Y) {
			count--;
			temp_Y = enemies[ENEMY_COUNT - 1].position.Y;
		}
	}
}

void movementNPC_Level2() {

	for (int i = 0; i < ENEMY_COUNT; i++) {
		deleteNPC(enemies[i]);
		deleteNPC(enemies2[i]);

		if (enemies[i].isActive > 0 || enemies2[i].isActive > 0) {
			NPCShoot_Level2(i);
		}

		showNPC(enemies[i]);
		showNPC(enemies2[i]);
	}

	for (int i = 0; i < 10; i++) {
		if (enemiesCopy[0].isActive == 1 || enemiesCopy[1].isActive == 1 || enemiesCopy[2].isActive == 1 ||
			enemiesCopy[3].isActive == 1 || enemiesCopy[4].isActive == 1 || enemiesCopy[5].isActive == 1 ||
			enemiesCopy[6].isActive == 1 || enemiesCopy[7].isActive == 1 || enemiesCopy[8].isActive == 1 ||
			enemiesCopy[9].isActive == 1) {

			deleteNPC(enemiesCopy[i]);

			if (enemiesCopy[9].position.X == (GBOARD_ORIGIN_X + (GBOARD_WIDTH + 1) * 2 - 3) || enemiesCopy[0].position.X == GBOARD_ORIGIN_X + 2) {
				enemiesCopy[i].position.Y += 2;

				enemiesCopy[i].movement *= -1;
			}

			if (enemiesCopy[0].movement == 1) {
				enemiesCopy[i].position.X += 1;
			}
			else if (enemiesCopy[0].movement == -1) {
				enemiesCopy[i].position.X -= 1;
			}

			showNPC(enemiesCopy[i]);

		}
	}
}

int Attack = 0;
int lowHealth = 20;
int Slow = 0;

void movementNPC_Level3() {
	srand(time(0));
	if (Attack == 0) {

		for (int i = 31; i <= 34; i++) {

			if (bossLGun[i].isActive == 1 || bossRGun[i].isActive == 1) {

				if (bossLGun[i].health <= lowHealth || bossRGun[i].health <= lowHealth) {
					Attack = 1;
				}

				if (bossLGun[i].health > lowHealth || bossRGun[i].health > lowHealth) {
					NPCShoot_Level3_LGun(RandomInRange(31, 34));
					NPCShoot_Level3_RGun(RandomInRange(31, 34));
				}


			}

		}


	}
	else if (Attack == 1) {
		//if one of the guns reached low health, other one can keep shooting
		for (int i = 31; i <= 34; i++) {

			if (bossLGun[i].isActive == 1 || bossRGun[i].isActive == 1) {

				if (bossLGun[i].health > lowHealth) {
					NPCShoot_Level3_LGun(RandomInRange(31, 34));

				}
				if (bossRGun[i].health > lowHealth) {
					NPCShoot_Level3_RGun(RandomInRange(31, 34));
				}


			}

		}

		//the low health downward attack motion
		for (int i = 0; i <= 34; i++) {

			if (bossLGun[i].isActive == 1 || bossRGun[i].isActive == 1) {

				if (bossLGun[i].position.Y <= (startY + 1) && bossLGun[i].health <= lowHealth) {
					deleteNPC(bossLGun[i]);

					bossLGun[i].position.Y += 2; //speed

					showNPC(bossLGun[i]);

				}
				else if (bossLGun[i].position.Y >= (startY + 1) && bossLGun[i].health <= lowHealth) {
					deleteNPC(bossLGun[i]);

					bossLGun[i].position.Y -= 36; //respawn the gun to top screen

					showNPC(bossLGun[i]);


				}

				if (bossRGun[i].position.Y <= (startY + 1) && bossRGun[i].health <= lowHealth) {

					deleteNPC(bossRGun[i]);

					bossRGun[i].position.Y += 2;

					showNPC(bossRGun[i]);
				}
				else if (bossRGun[i].position.Y >= (startY + 1) && bossRGun[i].health <= lowHealth) {

					deleteNPC(bossRGun[i]);

					bossRGun[i].position.Y -= 36;

					showNPC(bossRGun[i]);
				}

			}
		}


	}




	for (int i = 16; i <= 21; i++) {
		if (bossHead[i].isActive == 1) {

			NPCShoot_Level3_Head(i);

		}
	}
}



// Function to check for collision between player and NPCs
int collisionPlayerNPC() {
	for (int i = 0; i < ENEMY_COUNT; i++) {
		if ((enemies[i].isActive && enemies[i].position.X == startX && enemies[i].position.Y == startY) ||
			(enemies[i].isActive && enemies[i].position.Y == GBOARD_HEIGHT + 2)) {
			return 1; // Collision detected
		}
	}
	return 0; // No collision
}

int collisionPlayerNPC_level2() {
	for (int i = 0; i < 10; i++) {
		if ((enemiesCopy[i].isActive && enemiesCopy[i].position.X == startX && enemiesCopy[i].position.Y == startY) ||
			(enemiesCopy[i].isActive && enemiesCopy[i].position.Y == GBOARD_HEIGHT + 2)) {
			return 1; // Collision detected
		}
	}
	return 0; // No collision
}

int collisionPlayerNPC_level3() {

	for (int i = 0; i <= 34; i++) {
		if ((bossLGun[i].isActive && bossLGun[i].position.X == startX && bossLGun[i].position.Y == startY + 2) ||
			(bossLGun[i].isActive && bossLGun[i].position.X == startX + 1 && bossLGun[i].position.Y == startY + 2) ||
			(bossLGun[i].isActive && bossLGun[i].position.X == startX - 1 && bossLGun[i].position.Y == startY + 2) ||
			(bossLGun[i].isActive && bossLGun[i].position.Y == GBOARD_HEIGHT + 2)) {
			return 1; // Collision detected
		}

	}
	for (int i = 0; i <= 34; i++) {
		if ((bossRGun[i].isActive && bossRGun[i].position.X == startX && bossRGun[i].position.Y == startY + 2) ||
			(bossRGun[i].isActive && bossRGun[i].position.X == startX + 1 && bossRGun[i].position.Y == startY + 2) ||
			(bossRGun[i].isActive && bossRGun[i].position.X == startX - 1 && bossRGun[i].position.Y == startY + 2) ||
			(bossRGun[i].isActive && bossRGun[i].position.Y == GBOARD_HEIGHT + 2)) {
			return 1; // Collision detected
		}

	}


	for (int i = 0; i <= 39; i++) {
		if ((bossHead[i].isActive && bossHead[i].position.X == startX && bossHead[i].position.Y == startY) ||
			(bossHead[i].isActive && bossHead[i].position.Y == GBOARD_HEIGHT + 2)) {
			return 1; // Collision detected
		}
	}
	return 0; // No collision
}


//to remove the cursor from flickering in the screen
void RemoveCursor(void)
{
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

void drawMainMenu() {

	int x, y;

	for (x = 1; x < GBOARD_WIDTH + 1; x++) {
		SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2, GBOARD_ORIGIN_Y);
		printf("━");
	}
	for (y = 0; y <= GBOARD_HEIGHT; y++) {
		SetCurrentCursorPos(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y + y);

		if (y == GBOARD_HEIGHT)
			printf("┗");
		else if (y == 0) {
			printf("┏");
		}
		else
			printf("┃");
	}

	for (y = 0; y <= GBOARD_HEIGHT; y++) {
		SetCurrentCursorPos(GBOARD_ORIGIN_X + (GBOARD_WIDTH + 1) * 2, GBOARD_ORIGIN_Y + y);
		if (y == GBOARD_HEIGHT)
			printf("┛");
		else if (y == 0) {
			printf("┓");
		}
		else
			printf("┃");
	}

	for (x = 1; x < GBOARD_WIDTH + 1; x++) {
		SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT);
		printf("━");
	}

	SetCurrentCursorPos(32, 17);

	printf("XENOLITH");

	SetCurrentCursorPos(32, 20);

	printf("START");

	SetCurrentCursorPos(32, 22);

	printf("QUIT");

	SetCurrentCursorPos(75, 22);

	printf("UP: ▲/W");

	SetCurrentCursorPos(75, 24);

	printf("DOWN: ▼/S");

	SetCurrentCursorPos(75, 26);

	printf("SELECT: ENTER");

	SetCurrentCursorPos(28, 20);

	printf("→");

}

int gameMenu() {

	int choice = 1;
	int key;

	while (1) {

		if (!_kbhit())
			continue;
		key = _getch();
		if (key == DOWN || key == 83 || key == 115) {
			if (choice == 1) {

				SetCurrentCursorPos(28, 20);
				deletePlayer();
				choice++;
				SetCurrentCursorPos(28, 22);
				printf("→");

			}
			else {

				SetCurrentCursorPos(28, 22);
				deletePlayer();
				choice--;
				SetCurrentCursorPos(28, 20);
				printf("→");

			}
			Sleep(100);
		}
		else if (key == UP || key == 87 || key == 119) {
			deletePlayer();
			if (choice == 2) {

				SetCurrentCursorPos(28, 22);
				deletePlayer();
				choice--;
				SetCurrentCursorPos(28, 20);
				printf("→");

			}
			else {

				SetCurrentCursorPos(28, 20);
				deletePlayer();
				choice++;
				SetCurrentCursorPos(28, 22);
				printf("→");

			}
			Sleep(100);
		}
		else if (key == ENTER) {

			return choice;

		}
	}

}

void DrawGameBoard() {
	int x, y;


	for (x = 1; x < GBOARD_WIDTH + 1; x++) {
		SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2, GBOARD_ORIGIN_Y);
		printf("━");
	}
	for (y = 0; y <= GBOARD_HEIGHT; y++) {
		SetCurrentCursorPos(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y + y);

		if (y == GBOARD_HEIGHT)
			printf("┗");
		else if (y == 0) {
			printf("┏");
		}
		else
			printf("┃");
	}

	for (y = 0; y <= GBOARD_HEIGHT; y++) {
		SetCurrentCursorPos(GBOARD_ORIGIN_X + (GBOARD_WIDTH + 1) * 2, GBOARD_ORIGIN_Y + y);
		if (y == GBOARD_HEIGHT)
			printf("┛");
		else if (y == 0) {
			printf("┓");
		}
		else
			printf("┃");
	}

	for (x = 1; x < GBOARD_WIDTH + 1; x++) {
		SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT);
		printf("━");
	}


	SetCurrentCursorPos(75, 5);

	printf("HEALTH:");

	SetCurrentCursorPos(82, 5);

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);

	printf("♥ ♥ ♥"); //♥ ♡

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

	SetCurrentCursorPos(75, 7);

	printf("SCORE: 0");

	SetCurrentCursorPos(75, 20);

	printf("CONTROL");

	SetCurrentCursorPos(75, 22);

	printf("SHIFT LEFT: A");

	SetCurrentCursorPos(75, 24);

	printf("SHIFT RIGHT: D");

	SetCurrentCursorPos(75, 26);

	printf("SHOOT: SPACEBAR");

	SetCurrentCursorPos(GBOARD_WIDTH + 5, GBOARD_HEIGHT - 2);

}

//hp threshold for updating hp bar
int MAX_bossLGunHealth, MAX_bossRGunHealth, MAX_bossHeadHealth;



void DrawBossHP() {
	MAX_bossLGunHealth = bossLGunHealth;
	MAX_bossRGunHealth = bossRGunHealth;
	MAX_bossHeadHealth = bossHeadHealth;

	SetCurrentCursorPos(5, 44);
	//printf("LEFT GUN HP");
	printf("LEFT GUN HP: %2d", bossLGunHealth);

	SetCurrentCursorPos(5, 46);
	if (bossLGunHealth <= MAX_bossLGunHealth && bossLGunHealth > 35) {
		printf("[■■■■■]");
	}
	else if (bossLGunHealth >= 25 && bossLGunHealth < 35) {
		printf("[■■■■  ]");
	}
	else if (bossLGunHealth >= 15 && bossLGunHealth < 25) {
		printf("[■■■    ]");
	}
	else if (bossLGunHealth >= 5 && bossLGunHealth < 15) {
		printf("[■■      ]");
	}
	else if (bossLGunHealth > 0 && bossLGunHealth < 5) {
		printf("[■        ]");
	}
	else if (bossLGunHealth <= 0) {
		printf("[          ]");
	}

	SetCurrentCursorPos(31, 44);
	//printf("BOSS HP");
	printf(" BOSS HP: %2d", bossHeadHealth);

	SetCurrentCursorPos(25, 46);
	if (bossHeadHealth <= MAX_bossHeadHealth && bossHeadHealth > 40) {
		printf("[■■■■■■■■■■]");
	}
	else if (bossHeadHealth >= 30 && bossHeadHealth < 40) {
		printf("[■■■■■■■■    ]");
	}
	else if (bossHeadHealth >= 20 && bossHeadHealth < 30) {
		printf("[■■■■■■        ]");
	}
	else if (bossHeadHealth >= 10 && bossHeadHealth < 20) {
		printf("[■■■■            ]");
	}
	else if (bossHeadHealth > 0 && bossHeadHealth < 10) {
		printf("[■■                ]");
	}
	else if (bossHeadHealth <= 0) {
		printf("[                    ]");
	}

	SetCurrentCursorPos(55, 44);
	printf("RIGHT GUN HP: %2d", bossRGunHealth);
	//printf("RIGHT GUN HP");

	SetCurrentCursorPos(55, 46);
	if (bossRGunHealth <= MAX_bossRGunHealth && bossRGunHealth > 35) {
		printf("[■■■■■]");
	}
	else if (bossRGunHealth >= 25 && bossRGunHealth < 35) {
		printf("[■■■■  ]");
	}
	else if (bossRGunHealth >= 15 && bossRGunHealth < 25) {
		printf("[■■■    ]");
	}
	else if (bossRGunHealth >= 5 && bossRGunHealth < 15) {
		printf("[■■      ]");
	}
	else if (bossRGunHealth > 0 && bossRGunHealth < 5) {
		printf("[■        ]");
	}
	else if (bossRGunHealth <= 0) {
		printf("[          ]");
	}

	SetCurrentCursorPos(GBOARD_WIDTH + 5, GBOARD_HEIGHT - 2);
}

void DrawDifficultySelectMenu() {

	SetCurrentCursorPos(32, 20);

	printf("          ");

	SetCurrentCursorPos(32, 20);

	printf("EASY");

	SetCurrentCursorPos(32, 22);

	printf("MEDIUM");

	SetCurrentCursorPos(32, 24);

	printf("HARD");

	SetCurrentCursorPos(28, 20);

	printf("→");

}

int DifficultySelectMenu() {

	int choice = 1;
	int key;

	while (1) {

		if (!_kbhit())
			continue;
		key = _getch();
		if (key == DOWN || key == 83 || key == 115) {
			if (choice == 1) {

				SetCurrentCursorPos(28, 20);
				deletePlayer();
				choice++;
				SetCurrentCursorPos(28, 22);
				printf("→");

			}
			else if (choice == 3) {

				SetCurrentCursorPos(28, 24);
				deletePlayer();
				choice = 1;
				SetCurrentCursorPos(28, 20);
				printf("→");

			}
			else {

				SetCurrentCursorPos(28, 22);
				deletePlayer();
				choice++;
				SetCurrentCursorPos(28, 24);
				printf("→");

			}
			Sleep(100);
		}
		else if (key == UP || key == 87 || key == 119) {
			deletePlayer();
			if (choice == 2) {

				SetCurrentCursorPos(28, 22);
				deletePlayer();
				choice--;
				SetCurrentCursorPos(28, 20);
				printf("→");

			}
			else if (choice == 1) {

				SetCurrentCursorPos(28, 20);
				deletePlayer();
				choice = 3;
				SetCurrentCursorPos(28, 24);
				printf("→");

			}
			else {

				SetCurrentCursorPos(28, 24);
				deletePlayer();
				choice--;
				SetCurrentCursorPos(28, 22);
				printf("→");

			}
			Sleep(100);
		}
		else if (key == ENTER) {

			return choice;

		}

	}

}

//reset everything here
void DrawGameOverMenu() {

	startX = GBOARD_WIDTH + 5;
	startY = GBOARD_HEIGHT - 2;

	enemyCount = ENEMY_COUNT;
	enemyCount_level2 = ((ENEMY_COUNT - 9) * 2) + 10;
	enemyCount_level3 = 3;

	score = 0;
	life = 3;

	randItem = 0;
	testItem = 0;

	for (int i = 0; i < 20; i++) {
		HEART[i].isActive = 0;
		TP[i].isActive = 0;
		heart[i].isActive = 0;
		tp[i].isActive = 0;
		SetCurrentCursorPos(heart[i].x, heart[i].y);
		printf("  ");
		SetCurrentCursorPos(tp[i].x, tp[i].y);
		printf("  ");
	}


	for (int i = 0; i < MAX_BULLETS; i++) {

		bulletsNPC[i].isActive = 0;
		bulletsNPC2[i].isActive = 0;
		bulletsBoss[i].isActive = 0;

		SetCurrentCursorPos(bulletsNPC[i].x, bulletsNPC[i].y);
		printf("  ");

		SetCurrentCursorPos(bulletsNPC2[i].x, bulletsNPC2[i].y);
		printf("  ");

		SetCurrentCursorPos(bulletsBoss[i].x, bulletsBoss[i].y);
		printf("  ");

		bulletsPlayer[i].isActive = 0;

		SetCurrentCursorPos(bulletsPlayer[i].x, bulletsPlayer[i].y);
		printf("  ");
	}

	if (winCheck == -1) {
		SetCurrentCursorPos(30, 18);
		printf("You Lose!!");
		winCheck = 0;

	}
	else if (winCheck == 1) {
		SetCurrentCursorPos(30, 18);
		printf("You Win!!");
		winCheck = 0;
	}


	SetCurrentCursorPos(32, 20);

	printf("RESTART");

	SetCurrentCursorPos(32, 22);

	printf("MENU");

	SetCurrentCursorPos(75, 22);

	printf("                                          ");

	SetCurrentCursorPos(75, 22);

	printf("UP: ▲/W");

	SetCurrentCursorPos(75, 24);

	printf("                                          ");

	SetCurrentCursorPos(75, 24);

	printf("DOWN: ▼/S");

	SetCurrentCursorPos(75, 26);

	printf("                                          ");

	SetCurrentCursorPos(75, 26);

	printf("SELECT: ENTER");

	SetCurrentCursorPos(28, 20);

	printf("→");

}

int GameOverMenu() {

	int choice = 1;
	int key;

	while (1) {

		if (!_kbhit())
			continue;
		key = _getch();
		if (key == DOWN || key == 83 || key == 115) {
			if (choice == 1) {

				SetCurrentCursorPos(28, 20);
				deletePlayer();
				choice++;
				SetCurrentCursorPos(28, 22);
				printf("→");

			}
			else {

				SetCurrentCursorPos(28, 22);
				deletePlayer();
				choice--;
				SetCurrentCursorPos(28, 20);
				printf("→");

			}
			Sleep(100);
		}
		else if (key == UP || key == 87 || key == 119) {
			deletePlayer();
			if (choice == 2) {

				SetCurrentCursorPos(28, 22);
				deletePlayer();
				choice--;
				SetCurrentCursorPos(28, 20);
				printf("→");

			}
			else {

				SetCurrentCursorPos(28, 20);
				deletePlayer();
				choice++;
				SetCurrentCursorPos(28, 22);
				printf("→");

			}
			Sleep(100);
		}
		else if (key == ENTER) {

			return choice;

		}

	}

}

void Level1() {

	int choice;
	int wave = 1;

	while (1) {

		DrawGameBoard();
		showPlayer();
		initEnemies();
		initItem();
		for (int i = 0; i < ENEMY_COUNT; i++) {
			showNPC(enemies[i]);
		}

		Sleep(1000);



		while (!winCheck) {

			movementNPC();

			movementPlayer(); // ProcessKeyInput()

			mbPlayer(); // Move player bullets

			mbNPC(); //Move npc bullets

			langgar();

			bulletLanggarPlayer();

			movementItem();

			if (wave == 1) {
				spawnItem();
				wave = 2;
			}
			else if (wave == 2) {
				wave = 3;
			}
			else if (wave == 3) {
				wave = 1;
			}

			if (collisionPlayerNPC() || life == 0) {
				winCheck = -1;
			}

			if (enemyCount == 0) {
				winCheck = 1;
			}


			Sleep(50); // Adjust the speed of the game
		}

		deletePlayer();
		for (int i = 0; i < ENEMY_COUNT; i++) {
			deleteNPC(enemies[i]);
		}

		DrawGameOverMenu();

		choice = GameOverMenu();

		if (choice == 2) {

			clearScreen();

			break;

		}

		clearScreen();

	}

	return;
}

void Level2() {
	int choice;
	int wave_movement = 1;
	int wave = 1;

	while (1) {

		DrawGameBoard();
		showPlayer();
		initEnemies_Level2();
		initItem();
		for (int i = 0; i < ENEMY_COUNT; i++) {
			showNPC(enemies[i]);
			showNPC(enemiesCopy[i]);
			showNPC(enemies2[i]);
		}

		Sleep(1000);



		while (!winCheck) {
			//only movementNPC is different
			movementNPC_Level2();

			movementPlayer(); // ProcessKeyInput()

			mbPlayer(); // Move player bullets



			if (wave_movement == 1) {
				mbNPC(); //Move npc bullets
				wave_movement = 2;
			}
			else if (wave_movement == 2) {
				mbNPC2();
				wave_movement = 1;
			}

			movementItem();

			if (wave == 1) {
				spawnItem();
				wave = 2;
			}
			else if (wave == 2) {
				wave = 3;
			}
			else if (wave == 3) {
				wave = 1;
			}

			langgar_level2();

			bulletLanggarPlayer();

			if (collisionPlayerNPC_level2() || life == 0) {
				winCheck = -1;
			}

			if (enemyCount_level2 == 0) {
				winCheck = 1;
			}


			Sleep(50); // Adjust the speed of the game
		}

		deletePlayer();
		for (int i = 0; i < ENEMY_COUNT; i++) {
			deleteNPC(enemies[i]);
			deleteNPC(enemiesCopy[i]);
			deleteNPC(enemies2[i]);
		}

		DrawGameOverMenu();

		choice = GameOverMenu();

		if (choice == 2) {

			clearScreen();

			break;

		}

		clearScreen();

	}

	return;

}

void Level3() {
	int choice;
	int wave = 1;
	int check = -1;

	while (1) {

		DrawGameBoard();

		showPlayer();
		initEnemies_Level3();
		initWall();
		initItem();
		check = -1;

		DrawBossHP();

		for (int i = 0; i <= 34; i++) {
			showNPC(bossLGun[i]);
			showNPC(bossRGun[i]);
		}
		for (int i = 0; i <= 39; i++) {
			showNPC(bossHead[i]);
		}

		for (int i = 0; i < 22; i++) {
			showNPC(wall[i]);
		}

		/*
				for (int i = 0; i < 20; i++) {
			showItem(HEART[i]);
		}
		*/


		Sleep(1000);

		while (!winCheck) {

			//for updating hp bar
			MAX_bossLGunHealth = bossLGunHealth;
			MAX_bossRGunHealth = bossRGunHealth;
			MAX_bossHeadHealth = bossHeadHealth;

			DrawBossHP();

			//only movementNPC is different
			movementNPC_Level3();

			movementPlayer(); // ProcessKeyInput()

			mbPlayer(); // Move player bullets

			//if (wave == 1) {
			mbNPC3(); //Move npc bullets
		//	wave = 2;
	//	}
		//else if (wave == 2) {
			mbNPC4();
			//	wave = 1;
			//}

			if (check == 1) {
				mbNPC5();
			}

			movementItem();

			if (wave == 1) {
				spawnItem();
				wave = 2;
			}
			else if (wave == 2) {
				wave = 3;
			}
			else if (wave == 3) {
				wave = 1;
			}


			if (check == -1) {
				for (int i = 0; i <= 34; i++) {
					if (!bossLGun[i].isActive && !bossRGun[i].isActive) {

						for (int i = 0; i < 22; i++) {
							deleteNPC(wall[i]);
							wall[i].isActive = 0;
						}
						check = 1;
					}
				}
			}



			langgar_level3();

			//bulletLanggarPlayer();

			//if (collisionPlayerNPC_level3() || life == 0) {
			//	winCheck = -1;
			//}



			if (enemyCount_level3 == 0) {
				winCheck = 1;
			}

			Sleep(50); // Adjust the speed of the game
		}

		deletePlayer();
		for (int i = 0; i <= 34; i++) {
			deleteNPC(bossLGun[i]);
			deleteNPC(bossRGun[i]);
		}
		for (int i = 0; i <= 39; i++) {
			deleteNPC(bossHead[i]);
		}
		for (int i = 0; i < 22; i++) {
			deleteNPC(wall[i]);
		}

		DrawGameOverMenu();


		choice = GameOverMenu();

		if (choice == 2) {

			clearScreen();

			break;

		}

		clearScreen();

	}

	return;

}

int main() {
	HWND console = GetConsoleWindow();
	ShowWindow(console, SW_SHOWMAXIMIZED); // Maximizes the console window

	COORD screenSize = { 120, 30 }; // Adjust the screen buffer size as needed
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), screenSize);


	int choice;
	RemoveCursor();


	while (1) {
		drawMainMenu();
		choice = gameMenu();

		switch (choice) {

		case 1:

			DrawDifficultySelectMenu();

			break;

		case 2:

			clearScreen();
			return 0;

		}

		choice = DifficultySelectMenu();

		switch (choice) {

		case 1:

			clearScreen();
			Sleep(200);
			Level1();

			break;

		case 2:

			clearScreen();
			Sleep(200);
			Level2();

			break;

		case 3:

			clearScreen();
			Sleep(200);
			Level3();
			break;

		}

	}

	return 0;
}