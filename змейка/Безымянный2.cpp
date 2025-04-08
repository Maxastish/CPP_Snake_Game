#include <iostream>
#include <conio.h>
#include <ctime>
#include <cstdlib>
#include "console.h"
#include <string>
#include <vector>
using namespace std;
TConsole console;



struct Field{
	int width;
	int height;
	int x;
	int y;
};

enum Direction{
	sdUp, sdDown, sdLeft, sdRight
};

enum Key{
	kNone, kUp, kDown, kLeft, kRight, kEsc, kSpace, kEnter
};

Key getKey(){
	if(console.KeyPressed()){
		unsigned char k = getch();
		switch(k){
			case 0x20: return kSpace;
			case 0x0D: return kEnter;
			case 0x1B: return kEsc;
			case 0xE0:
				k = getch();
				switch(k){
					case 0x48: return kUp;
					case 0x50: return kDown;
					case 0x4B: return kLeft;
					case 0x4D: return kRight;
					default: return kNone;
				}
			default: return kNone;
		}
	}
}

void initField(Field &f){
	f.width = 78; // >20         10 78
	f.height = 22; // >= 3         5 22
	f.x = 0;
	f.y = 1;
}

void showField(Field &f){
	#define COLOR_YELLOW 14
	console.TextColor(COLOR_YELLOW);
	for (int i = f.width+1; i >= 0; i--){
		console.GotoXY(i+f.x,0+f.y);
		cout << "#";
		//Sleep(100);
	}
	for (int i = f.height+1; i >= 0; i--){
		console.GotoXY(0+f.x,i+f.y);
		cout << "#";
		//Sleep(100);
	}
	for (int i = f.width+1; i >= 0; i--){
		console.GotoXY(i+f.x,f.height+1+f.y);
		cout << "#";
		//Sleep(100);
	}
	for (int i = f.height+1; i >= 0; i--){
		console.GotoXY(f.width+1+f.x,i+f.y);
		cout << "#";
		//Sleep(100);
	}
}

//положения игры
enum State{
	gsPlay,
	gsPause,
	gsGameover,
	gsExit
};

//очки
typedef int Score;
typedef int Count;
typedef int Flag;

//еда
struct Point{
	int x;
	int y;
};
typedef Point Food;
typedef Point Telep;
typedef Point SuperFood;

//змея
struct Snake{
	Point body [1000];
	Direction direction;
	int length;
};

struct Game{
	Telep telep;
	Telep telep2;
	Food food;
	SuperFood superFood;
	Snake snake;
	Score score;
	State state;
	Field field;
	Count count;
	Flag flag;
};

//init змеи
void initSnake (Snake &s, Field &f){
	s.length = 4;
	//увеличивается длина на 1
	Point start;
	start.x = (f.width - s.length) / 2;
	start.y = ((f.height)/2);
	for (int i = 0; i < s.length; i++){
		s.body[i].x = start.x + i;
		s.body[i].y = start.y;
	}
	s.direction = sdUp;
}

//SuperFood
void generateSuperFoodCoord(SuperFood &fd, Field &f){
	fd.x = rand() % (f.width);
	fd.y = rand() % (f.height);
}

bool isRightSuperFoodCoord(SuperFood &fd, Snake &s, Food &f, Telep &t){
	for (int i = 0; i < s.length; i++){
		if ((fd.x == s.body[i].x && fd.y == s.body[i].y) || (fd.x == f.x && fd.y == f.y) || (fd.x == t.x && fd.y == t.y)){
			return false;
		}
	}
	return true;
}

void createSuperFood (SuperFood &fd, Snake &s, Field &f, Food &g, Telep &t){
	do {
		generateSuperFoodCoord (fd, f);
	}while (not isRightSuperFoodCoord (fd, s, g, t));
}

void showSuperFood(SuperFood &fd, Field &f, Count &c){
	console.TextColor(COLOR_WHITE);
	if (c == 1){
		console.GotoXY(fd.x + f.x + 1, fd.y + f.y + 1);
		cout << "Я";
	}
}
//
//food
void generateFoodCoord(Food &fd, Field &f){
	fd.x = rand() % (f.width);
	fd.y = rand() % (f.height);
}

bool isRightFoodCoord(Food &fd, Snake &s, SuperFood &sf, Telep &t){
	for (int i = 0; i < s.length; i++){ //body
		if ((fd.x == s.body[i].x && fd.y == s.body[i].y) || (fd.x == sf.x && fd.y == sf.y) || (fd.x == t.x && fd.y == t.y)){
			return false;
		}
	}
	return true;
}

void createFood (Food &fd, Snake &s, Field &f, Count &c, SuperFood &sf, Flag &fl, Telep &t){
	do {
		generateFoodCoord (fd, f);
		c = rand () % 2;
		if ((c == 1) and (fl == 0)){
			createSuperFood(sf, s, f, fd, t);
			showSuperFood(sf, f, c);
			fl = 1;
		}
		//cout << c;
	}while (not isRightFoodCoord (fd, s, sf, t));

}

//teleport
void generateTeleportCoord(Telep &fd, Field &f){
	fd.x = rand() % (f.width-1);
	fd.y = rand() % (f.height-1);
}

bool isRightTeleportCoord(Telep &fd, Snake &s){
	for (int i = 0; i < s.length; i++){
		if (fd.x == s.body[i].x && fd.y == s.body[i].y){
			return false;
		}
	}
	return true;
}

void createTeleport (Telep &fd, Snake &s, Field &f){
	do {
		generateTeleportCoord (fd, f);
	}while (not isRightTeleportCoord (fd, s));

}
//

void initGame (Game &g){
	//setlocale (LC_ALL, "rus");
	srand(time(0));
	g.score = 0;
	g.count = 0;
	g.flag = 0;
	g.state = gsPause;
	initField(g.field);
	initSnake(g.snake, g.field);
	createFood(g.food, g.snake, g.field, g.count, g.superFood, g.flag, g.telep);
	createSuperFood(g.superFood, g.snake, g.field, g.food, g.telep);
	createTeleport(g.telep, g.snake, g.field);
	createTeleport(g.telep2, g.snake, g.field);
}

//score
void showScore(Score &sc, Field &f, Snake &s){
	//initGame(g, s, f);
	console.TextColor(COLOR_LIGHTGREEN);
	console.GotoXY(f.x, -1+f.y);
	//console.GotoXY(/*f.width+1+*/f.width/3 + f.x, -1+f.y);
	cout << "Your Score: " << sc;
}

//food
void showFood(Food &fd, Field &f){
	console.TextColor(COLOR_LIGHTRED);
	console.GotoXY(fd.x + f.x + 1, fd.y + f.y + 1);
	cout << "*";
}

void showSnake(Snake &s, Field &f){
	console.TextColor(COLOR_WHITE);
	console.GotoXY(s.body[0].x + f.x + 1, s.body[0].y + f.y + 1);
	cout << "Е";
	for (int i = 1; i < s.length; i++){
		console.GotoXY(s.body[i].x + f.x + 1, s.body[i].y + f.y + 1);
		cout << "О";
	}
}

void showTelep(Telep &fd, Field &f){
	console.TextColor(COLOR_LIGHTBLUE);
	console.GotoXY(fd.x + f.x + 1, fd.y + f.y + 1);
	cout << "*";
}

void showTelep2(Telep &fd, Field &f){
	console.TextColor(COLOR_LIGHTGREEN);
	console.GotoXY(fd.x + f.x + 1, fd.y + f.y + 1);
	cout << "*";
}

void turnSnake(Snake &s, Direction dir){
	switch (dir){
		case sdLeft: if(s.direction == sdRight) return;
		break;

		case sdRight: if(s.direction == sdLeft) return;
		break;

		case sdUp: if(s.direction == sdDown) return;
		break;

		case sdDown: if(s.direction == sdUp) return;
		break;
	}
	s.direction = dir;
}

void moveSnake(Snake &s){
	for (int i = s.length - 1; i > 0; i--){
		s.body[i].x = s.body[i-1].x;
		s.body[i].y = s.body[i-1].y;
	}
	switch (s.direction){
		case sdLeft: s.body[0].x--;
		break;

		case sdRight: s.body[0].x++;
		break;

		case sdUp: s.body[0].y--;
		break;

		case sdDown: s.body[0].y++;
		break;
	}
}

void hideSnake(Snake &s, Field &f){
	console.GotoXY(s.body[s.length-1].x + f.x+1, s.body[s.length-1].y + f.y+1);
	cout << " ";
}

void hideTeleport(Snake &s, Field &f, Telep &t){
	console.GotoXY(t.x + f.x + 1, t.y + f.y + 1);
	cout << " ";
}

bool isOnWall(Field &f, Snake &s){
	//                нижняя                     верхняя                    правая                       левая
	return (s.body[0].y == + f.height) || (s.body[0].y == -1) || (s.body[0].x ==  + f.width) || (s.body[0].x == - 1);
}

bool isOnSelf(Snake &s){
	for (int i = 3; i < s.length; i++){
		if ((s.body[0].x == s.body[i].x) && (s.body[0].y == s.body[i].y)) return true;
	}
	return false;
}

bool isOnFood(Snake &s, Food & fd){
	if ((s.body[0].x == fd.x) && (s.body[0].y == fd.y)) return true;
	else return false;
}

bool isOnSuperFood(Snake &s, SuperFood & fd){
	if ((s.body[0].x == fd.x) && (s.body[0].y == fd.y)) return true;
	else return false;
}

bool isOnTeleport(Snake &s, Telep & fd){
	if ((s.body[0].x == fd.x) && (s.body[0].y == fd.y)) return true;
	else return false;
}

void growSnake(Snake &s){
	s.length++;
	s.body[s.length-1].x=s.body[s.length-2].x;
	s.body[s.length-1].y=s.body[s.length-2].y;
}

void cutSnake(Snake &s, Field &f){
	hideSnake(s, f);
	s.length--;
}

bool isGameOver(Game &g){
	return(isOnWall(g.field, g.snake)) or (isOnSelf(g.snake));
}

int showGameOver(Game &SnakeGame){
	console.GotoXY((SnakeGame.field.width - SnakeGame.snake.length) / 2 - 2 + SnakeGame.field.x, ((SnakeGame.field.height)/2) + SnakeGame.field.y);
	cout << "GAME OVER" << endl;
	console.GotoXY((SnakeGame.field.width - SnakeGame.snake.length) / 2 - 6 + SnakeGame.field.x, ((SnakeGame.field.height)/2)+1 + SnakeGame.field.y);
	cout << "Length of snake: " << SnakeGame.snake.length << endl;
	console.GotoXY((SnakeGame.field.width - SnakeGame.snake.length) / 2 - 6 + SnakeGame.field.x, ((SnakeGame.field.height)/2)+2 + SnakeGame.field.y);
	cout << "Points of food: " << SnakeGame.score << endl;
}

/*void showSettings(){
    action = showSettings();
}*/

typedef void(*MenuAction)();
//MenuAction ma;

struct MenuItem{
	string text;
	MenuAction action;
};


struct Menu{
	string caption;
	vector <MenuItem> items;
	int activeItem;
	int size;
	int count;
};

void initMenu(Menu & m, string caption){
	m.caption = caption;
	m.activeItem = -1;
};

void addMenuItem(Menu & m, string text, MenuAction & ma){
	MenuItem mi;
	mi.text = text;
	mi.action = ma;
	m.items.push_back(mi);
	m.count = m.items.size();
};

void prevMenuItem(Menu &m ){
	m.activeItem--;
};

void nextMenuItem(Menu &m){
	m.activeItem++;
};

void showMenu(Menu &m){
	console.TextBackground(COLOR_DARKGRAY);
	console.TextColor(COLOR_WHITE);
	console.GotoXY(36, 9);
	cout << "  MENU  ";
	Menu menu;
	MenuAction ma;
	addMenuItem(menu, "Play game", ma);
	addMenuItem(menu, "Settings", ma);
	addMenuItem(menu, "Exit", ma);
	//console.GotoXY(33, 11);
	//cout << "  Play  game  ";
};

int runGame(Game &SnakeGame){
	Menu menu;
	showMenu(menu);
	console.TextBackground(COLOR_BLACK);
	getch();
	system("cls");
	showField(SnakeGame.field);
	showFood(SnakeGame.food, SnakeGame.field);
	//showSuperFood(SnakeGame.superFood, SnakeGame.field, SnakeGame.count);
	showScore(SnakeGame.score, SnakeGame.field, SnakeGame.snake);
	showTelep(SnakeGame.telep, SnakeGame.field);
	showTelep2(SnakeGame.telep2, SnakeGame.field);

	while (SnakeGame.state != gsExit){
		Key key = getKey();
		switch (SnakeGame.state){
			case gsPlay:
				switch(key){
					case kUp: turnSnake(SnakeGame.snake, sdUp);
					break;

					case kDown: turnSnake(SnakeGame.snake, sdDown);
					break;

					case kLeft: turnSnake(SnakeGame.snake, sdLeft);
					break;

					case kRight: turnSnake(SnakeGame.snake, sdRight);
					break;

					case kSpace: SnakeGame.state = gsPause;
					break;

					case kEsc: SnakeGame.state = gsExit;
					break;
				}

				moveSnake(SnakeGame.snake);

				if (isOnFood(SnakeGame.snake, SnakeGame.food)){
					//count++;
					//count = rand () % 10;
					SnakeGame.score++;
					growSnake(SnakeGame.snake);
					createFood(SnakeGame.food, SnakeGame.snake, SnakeGame.field, SnakeGame.count, SnakeGame.superFood, SnakeGame.flag, SnakeGame.telep);
					showScore(SnakeGame.score, SnakeGame.field, SnakeGame.snake);
					showFood(SnakeGame.food, SnakeGame.field);
				}

				if (isOnSuperFood(SnakeGame.snake, SnakeGame.superFood)){
					SnakeGame.flag = 0;
					SnakeGame.score+=3;
					if (SnakeGame.snake.length > 1){
						cutSnake(SnakeGame.snake, SnakeGame.field);
					}
					//count = 0;
					showScore(SnakeGame.score, SnakeGame.field, SnakeGame.snake);
				}

				if (isOnTeleport(SnakeGame.snake, SnakeGame.telep)){
					hideSnake(SnakeGame.snake, SnakeGame.field);
					Sleep(50);
					console.TextColor(COLOR_WHITE);
					SnakeGame.snake.body[0].x = SnakeGame.telep2.x;
					SnakeGame.snake.body[0].y = SnakeGame.telep2.y;
					showSnake(SnakeGame.snake, SnakeGame.field);

					hideTeleport(SnakeGame.snake, SnakeGame.field, SnakeGame.telep);
					createTeleport(SnakeGame.telep, SnakeGame.snake, SnakeGame.field);
					createTeleport(SnakeGame.telep2, SnakeGame.snake, SnakeGame.field);
					showTelep(SnakeGame.telep, SnakeGame.field);
				}

				if (isGameOver(SnakeGame)) SnakeGame.state = gsGameover;

				showSnake(SnakeGame.snake, SnakeGame.field);
				showTelep2(SnakeGame.telep2, SnakeGame.field);
				Sleep(100);
				hideSnake(SnakeGame.snake, SnakeGame.field);
				break;

			case gsPause:
				if ((key == kSpace) || (key == kEnter)) SnakeGame.state = gsPlay;
				break;

			case gsGameover:
				showGameOver(SnakeGame);
				if ((key == kEnter) || (key == kEsc)) SnakeGame.state = gsExit;
				break;
		}
	}
}

void playSnake(){
	Game SnakeGame;
	initGame(SnakeGame);

	runGame(SnakeGame);
}



int main(){

	playSnake();

	return 0;
}


