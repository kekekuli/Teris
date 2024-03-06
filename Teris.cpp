
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>
#define BLOCK_BODY_SOLID	"■"
#define BLOCK_BODY_HOLLOW	"□"
#define MAP_WIDTH 12
#define MAP_HEIGH 22
#define SCORE_X 13
#define SCORE_Y 8
#define __N__ 10000
#define __T__ 1000
#define MOVE_LEFT (-1)
#define MOVE_RIGHT 1

struct Block {
	//起始坐标和形状
	int posX, posY, shape, angle;
	/*
	*二进制表示图形
	*	0 0 0 0
	*	0 1 0 0
	*	0 1 0 0
	*	0 1 1 0
	* 可表示为 0000 0100 0100 0110
	*/
	unsigned int binary;


};
//各种已经压缩过的旋转形状
const unsigned int WholeBlocks[7][4] = {
	{17476, 15, 17476, 15},
	{1094, 116, 1570, 46},
	{102, 102, 102, 102},
	{198, 612, 198, 612},
	{54, 1122, 54, 1122},
	{78, 2248, 228, 610},
	{550, 71, 1604, 226}
};

int map[MAP_HEIGH + 5][MAP_WIDTH + 5] = { 0 };
int score = 0, key_v = 0, dlay = 500;
bool isGameOver = false;

//init
void    DrawMap();
void    HideCursor();
void    SolidWindowShape();
void    Init();
void	ShowInfoOfBlock(const Block&);
bool	ClearTheLastLine();
//tools
void    gotoxy(int, int);
int     GetBinOfI(const Block&, int);
void	PrintSingle(int, int);
void	ClearSingle(int, int);
void	FindCoord(const Block&, int, int&, int&);
//blocks
Block	CreateBlock();
void	PrintBlock(const Block&);
void	ClearBlock(const Block&);
bool	DroppingBlock(Block&);
void	BlockToMap(const Block&);
void	SpinBlock(Block&);
void	MoveBlock(Block&, int direction);
int main() {

	Init();
	while (!isGameOver) {
		Block item = CreateBlock();

		do {
			dlay = 500;
			//Block temp = item;
			PrintBlock(item);

			int start = clock();
			do {
				if (kbhit()) {
					key_v = (int)(getch());
					if (key_v < 97) { key_v += 32; }
				}
				else key_v = -1;

				switch (key_v) {
				case 'w':
					ClearBlock(item);
					SpinBlock(item);
					//temp = item;
					PrintBlock(item);
					break;
				case 'a':
					ClearBlock(item);
					MoveBlock(item, MOVE_LEFT);		
					//temp = item;
					PrintBlock(item);
					break;
				case 'd':
					ClearBlock(item);
					MoveBlock(item, MOVE_RIGHT);	
					//temp = item;
					PrintBlock(item);
					break;
				case 's':
					dlay = 0;						
					break;
				default:							break;
				}
				ShowInfoOfBlock(item);
			} while ((clock() - start) < dlay);
			ClearBlock(item);
		} while (DroppingBlock(item));
		//判断一下是否死掉了
		int x, y;
		for (int i = 0; i < 16; i++) {
			FindCoord(item, i, x, y);
			if (y < 1 && (GetBinOfI(item, i) == 1)) {
				isGameOver = true;
				break;
			}
		}

		BlockToMap(item);
		while (ClearTheLastLine());
		DrawMap();
		gotoxy(13, 13);
		printf("score : %d ", score);
	}
	system("cls");
	gotoxy(0, 0);
	printf("You died.\nGot %d score\n", score);
	getchar();
	return 0;
}
bool ClearTheLastLine() {
	bool flag = true;
	int i = 0, j = 0;
	for (i = MAP_HEIGH - 2; i >= 1; i--) {
		flag = true;
		for (j = 1; j <= MAP_WIDTH - 2; j++)
			if (map[i][j] == 0)
				flag = false;
		if (flag)
			break;
	}

	if (flag) {
		score++;
		for (; i >= 1; i--) {
			for (j = 1; j <= MAP_WIDTH - 2; j++) {
				if (i == 1)
					map[i][j] = 0;
				else map[i][j] = map[i - 1][j];
			}
		}
	}
	return flag;
}
void MoveBlock(Block &block, int direction) {
	Block temp = block;
	temp.posX += direction;

	int x, y;
	bool flag = true;
	for (int i = 0; i < 16; i++) {
		FindCoord(temp, i, x, y);

		if ((GetBinOfI(temp, i) == 1)) {
			if (x <= 0 || x >= MAP_WIDTH - 1 ||
				y <= 0 || y >= MAP_HEIGH - 1 ||
				((map[y][x] == 1)))
				flag = false;
		}
	}
	if (flag)
		block = temp;

}
void SpinBlock(Block& block) {
	Block temp = block;
	temp.angle = (temp.angle + 1) % 4;
	temp.binary = WholeBlocks[temp.shape][temp.angle];

	int x, y;
	bool flag = true;
	for (int i = 0; i < 16; i++) {
		FindCoord(temp, i, x, y);

		if ((GetBinOfI(temp, i) == 1)) {
			if (x <= 0 || x >= MAP_WIDTH - 1 ||
				y <= 0 || y >= MAP_HEIGH - 1 ||
				((map[y][x] == 1)))
				flag = false;
		}
	}

	if (flag)
		block = temp;
}
void BlockToMap(const Block& block) {
	int x, y;
	for (int i = 0; i < 16; i++) {
		FindCoord(block, i, x, y);
		if (x >= 1 && x <= MAP_WIDTH - 2 && y >= 1 && y <= MAP_HEIGH - 2 && 
			(GetBinOfI(block, i) == 1))
			map[y][x] = 1;
	}
	//DrawMap();
}

bool DroppingBlock(Block& block) {
	int x, y;
	bool flag = true;
	for (int i = 0; i < 16; i++) {
		FindCoord(block, i, x, y);
		if (x >= 1 && x <= MAP_WIDTH - 2 &&
			y >= 1 && y <= MAP_HEIGH - 2 &&
			map[y + 1][x] == 1 && GetBinOfI(block, i))
			flag = false;
	}
	if (flag) block.posY++;
	//PrintBlock(block);
	/*-------------------------------------------------------*/
	//key_v = Getkey(0, 500);
	/*-------------------------------------------------------*/
	//ClearBlock(block);
	

	return flag;
}
void ClearBlock(const Block& block) {
	int x, y;
	for (int i = 0; i < 16; i++) {
		FindCoord(block, i, x, y);
		if(GetBinOfI(block, i))
			ClearSingle(x, y);
	}
}
void PrintBlock(const Block &block) {
	int x, y;
	for (int i = 0; i < 16; i++) {
		FindCoord(block, i, x, y);
		if (GetBinOfI(block, i))
			PrintSingle(x, y);
	}
}
void ShowInfoOfBlock(const Block& block) {
	gotoxy(13, 1);	printf("posX:%d ", block.posX);
	gotoxy(13, 2);	printf("posY:%d ", block.posY);
	gotoxy(13, 3);	printf("shape:%d", block.shape);
	gotoxy(13, 4);	printf("angle:%d", block.angle);
	gotoxy(13, 5);	printf("binary:%d ", block.binary);

	int posX = 13, posY = 6;
	for (int i = 15; i >= 0; i--) {
		posY = 6 + (15 - i) / 4;
		gotoxy(posX + (15 - i) % 4, posY);
		if (GetBinOfI(block, i) == 1)
			printf(BLOCK_BODY_SOLID);
		else printf(BLOCK_BODY_HOLLOW);
	}
}
void FindCoord(const Block &block, int i, int& x, int& y) {
	x = block.posX + 3, y = block.posY + 3;
	x -= i % 4;
	while (i - 4 >= 0) {
		i -= 4;
		y--;
	}
}
void ClearSingle(int x, int y) {
	gotoxy(x, y);
	if (x >= 1 && x <= MAP_WIDTH - 2 &&
		y >= 1 && y <= MAP_HEIGH - 2)
		printf(BLOCK_BODY_HOLLOW);
}
void PrintSingle(int x, int y) {
	gotoxy(x, y);
	if (x >= 1 && x <= MAP_WIDTH - 2 &&
		y >= 1 && y <= MAP_HEIGH - 2)
		printf(BLOCK_BODY_SOLID);
}

int GetBinOfI(const Block& block, int i) {
	return (((block.binary >> i) & 1) == 1);
}
Block CreateBlock() {
	Block ans;

	
	//ans.posX = rand() % MAP_WIDTH;
	//ans.posY = rand() % MAP_HEIGH;

	ans.posX = MAP_WIDTH / 2 - 2;
	ans.posY = -3;
	ans.shape = rand() % 7;
	ans.angle = rand() % 4;
	ans.binary = WholeBlocks[ans.shape][ans.angle];

	return ans;
}

void gotoxy(int x, int y) {
	COORD coord;
	coord.Y = y;
	coord.X = x * 2;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
void Init() {
	system("mode con cols=40 lines=22");
	HideCursor();
	SolidWindowShape();
	for (int i = 0; i < MAP_HEIGH; i++)
		for (int j = 0; j < MAP_WIDTH; j++)
			if (i == 0 || i == MAP_HEIGH - 1 ||
				j == 0 || j == MAP_WIDTH - 1)
				map[i][j] = 1;
	DrawMap();

	gotoxy(13, 13);
	printf("score : %d ", score);
	srand((unsigned)time(NULL));
}
void HideCursor() {
	CONSOLE_CURSOR_INFO curo;
	curo.bVisible = FALSE;
	curo.dwSize = sizeof(curo);
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorInfo(handle, &curo);
}
void SolidWindowShape() {

	SetWindowLongPtrA(

		GetConsoleWindow(),

		GWL_STYLE,

		GetWindowLongPtrA(GetConsoleWindow(), GWL_STYLE)

		& ~WS_SIZEBOX & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX

	);
}
void DrawMap() {
	for (int i = 0; i < MAP_HEIGH; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			gotoxy(j, i);
			map[i][j] == 1 ? printf(BLOCK_BODY_SOLID) : printf(BLOCK_BODY_HOLLOW);
		}
		//if (i != MAP_HEIGH - 1) printf("\n");
	}
}

