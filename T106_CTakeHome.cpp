#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include <windows.h>
#include<conio.h>
FILE *f;

int counter = 0;
int initialEnemyCount, enemySpawnCount, heroHealthPoint;
int totalBullet = 0;
int bulletFace;
int bulletLife[1000][2];
int bulletPos[1000][4];
int xplayer;
int yplayer;
char map[100][100];
int score = 0;
int health = 2;
int minSize = 5;
int macSize = 10;
char name[100];
int difficulty;

struct player{
	char name[100];
	int difficulty;
	int score;
}players[100];

struct bullet{
	int xPos;
	int yPos;
	char direction;
}bullets[1000];

struct Room{
	//posX posY = posisi yang mau dibagi
	int width, height, splitX, splitY, split; //split 0: vertical, 1:horizontal
	Room *left, *right;
}*root;

//bikin room baru (2)
Room *createRoom(int width, int height, int splitX, int splitY){
	Room* newRoom = (Room*)malloc(sizeof(Room));
	newRoom->width = width;
	newRoom->height = height;
	newRoom->splitX = splitX;
	newRoom->splitY = splitY;
	newRoom->left = newRoom->right = NULL;
	
	return newRoom;
}


int randomNum(int min, int max){
	return rand() % (max-min + 1) + min;
}

Room* randomRoom(Room* room){
	if(room->left != NULL){
		int rand = randomNum(0, 1);
		if(rand == 0)
		return randomRoom(room->left);
	}
	return (room->right);
}


void split1(Room *room);

void roomSplit(Room *parent, int min, int max, int split){
	int newSize = 0, secondSize = 0;
	
	//horizontal
	if(split){
		//bagi ruangan
		newSize = parent->height * randomNum(min, max) / 10;
		secondSize = parent->height - newSize;
	}else{
		//vertical
		newSize = parent->width * randomNum(min, max) / 10;
		secondSize = parent->width - newSize;
	}
//	printf("%d\n", parent->splitX);
//	printf("%d\n", parent->splitY);
//	printf("%d\n", parent->width);
//	printf("%d\n", parent->height);
//	getchar();
	//kalo salah satu ada yang kurang dari minSize, itung ulang
	if(newSize - 1 < minSize || secondSize - 2 < minSize){
		roomSplit(parent, min + 1, max -1, split);
		return;
	}
	
	//ukuran diatas minim size
	else{
		if(split){
			//horizontal
			parent->left = createRoom(parent->width, newSize, parent->splitX, parent->splitY);
			parent->right = createRoom(parent->width, secondSize, parent->splitX, parent->splitY+newSize);
		}else{
			parent->left = createRoom(newSize, parent->height, parent->splitX, parent->splitY);
			parent->right = createRoom(secondSize, parent->height, parent->splitX + newSize, parent->splitY);
		}
	}
	
	split1(parent->left);
	split1(parent->right);
}

// (3)
void split1(Room *room){
	
	int randNum = randomNum(0, 1);
	// 0 = vertical, 1 = horizontal
	//cek vertical, apakah bisa dibagi lagi
	if(randNum == 0 && room->width > 2 * minSize + 2){
		room->split = 0;
		roomSplit(room, 1, 9, 0);
		return;
	}
	//split horizontal
	else if(randNum == 1 && room->height > 2 * minSize + 2){
		room->split = 1;
		roomSplit(room, 1, 9, 1);
		return;
	}
	
	//buat mastiin aja barangkali masih bisa dibagi secara vertical atau horizontal
	if(room->width > 2 * minSize + 2){
		room->split = 0;
		roomSplit(room, 1,9,0);
		return;
	}if(room->height > 2 * minSize + 2){
		room->split = 1;
		roomSplit(room, 1, 9, 1);
		return;
	}
}

void BSP(Room *parent){
	int doorSizeNow = 0;
	int doorSize = randomNum(2, 4);
	int doorIndex = 0;
	if(parent->left == NULL)return;
	//print horizontal
	if(parent->split){
		doorIndex = randomNum(parent->splitX + 1, parent->width + parent->splitX - doorSize);
		for(int i = parent->splitX + 1; i < parent->width + parent->splitX + 1&& i < 30; i++){
			if(i >= doorIndex && doorSizeNow < doorSize){
				map[parent->right->splitY][i] = ' ';
				doorSizeNow++;
				continue;
			}
			map[parent->right->splitY][i] = '#';
			
		}
	}
	//print vertical
	else{
		doorIndex = randomNum(parent->splitY + 1, parent->height + parent->splitY - doorSize);
		for(int i = parent->splitY + 1; i < parent->height + parent->splitY + 1 && i < 30; i++){
			if(i >= doorIndex && doorSizeNow < doorSize){
				map[i][parent->right->splitX] = ' ';
				doorSizeNow++;
				continue;
			}
			map[i][parent->right->splitX] = '#';
		}
	}
	
	BSP(parent->left);
	BSP(parent->right);
}

void clearScreen(){
    COORD cursorPosition;
    cursorPosition.X = 0;
    cursorPosition.Y = 0;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
}

void print_main_menu(){
	printf("Untitled Shooter Game Main Menu\n");	
	printf("================================\n");
	printf("1. Play Game\n");
	printf("2. Highscore\n");
	printf("3. Help\n");
	printf("4. Exit\n");
	printf("Input [1-4]: ");	
}

void help_menu(){

		system("cls");
		printf("Untitled Shooter Game Help\n");
		printf("============================\n");
		printf("How To Play:\n");
		printf("You play as an untitled hero that needs to survive as long as possible against the enemy horde.\n");
		printf("The hero is able to shoot the enemy to kill them in one shot. On the other hand, the enemy is\n");
		printf("able to melee strike the hero, reducing his health in the process but also killing the enemy.\n");
		printf("For each random amount of time, a health pack will appear on the map. This health pack can be\n");
		printf("picked up by the hero to restore their health.\n");
		puts("");
		printf("Score:\n");
		printf("When the hero is eventually slain, a score will be calculated. The calculated score can be\n");
		printf("viewed again in the Highscore menu.\n");
		puts("");
		printf("Controls:\n");
		printf("W: Move Up\n");
		printf("S: Move Down\n");
		printf("A: Move Left\n");
		printf("D: Move Right\n");
		puts("");
		printf("K: Shoot\n");
		puts("");
		printf("ESC: Exit from current game session\n");
		getchar();
}

void read_file(){
	f = fopen("highscore.csv","r");
	
	while(!feof(f)){
		fscanf(f, "%[^,],%d,%d\n", players[counter].name, &players[counter].difficulty, &players[counter].score);
		counter++;
		fflush(stdin);
	}
	fclose(f);
}

void write_file(){
	f = fopen("highscore.csv", "a");
	fprintf(f, "%s,%d,%d\n", players[counter].name, players[counter].difficulty, players[counter].score);
	fclose(f);
}
void swap(struct player *a, struct player *b){
	struct player temp = *a;
	*a = *b;
	*b = temp;	
}

int partition(player *arr, int low, int high){
	player pivot = arr[high];
	int x = low - 1;
	
	for(int i = low; i < high; i++){
		if(arr[i].score < pivot.score){
			x++;
			swap(&arr[x], &arr[high]);
		}else if(arr[i].score == arr[high].score){
			if(arr[i].difficulty < arr[high].difficulty){
				x++;
				swap(&arr[x], &arr[high]);
			}
		}
	}
	x++;
	swap(&arr[x], &arr[high]);
	return x;
}

void quick_sort(player *arr, int low, int high){
	if(low < high){
		int index = partition(arr,low,high);
		quick_sort(arr,low,high-1);
		quick_sort(arr,low+1, high);
	}
}

void print_leaderboard(){
	system("cls");
	printf("Untitled Shooter Highscores\n");
	printf("===============================\n");
	printf("Name       Difficulty     Score\n");
	for(int i = 0; i < counter; i++){
		printf("%-10s %-15d%-10d\n", players[i].name, players[i].difficulty, players[i].score);
	}
	getchar();
}

void setDifficulty(int difficulty){
	if(difficulty == 1){
		initialEnemyCount = 5;
		enemySpawnCount = 2;
		heroHealthPoint = 6;
	}else if(difficulty == 2){
		initialEnemyCount = 7;
		enemySpawnCount = 3;
		heroHealthPoint = 4;
	}else if(difficulty == 3){
		initialEnemyCount = 9;
		enemySpawnCount = 4;
		heroHealthPoint = 2;
	}
}

void generateMap(){
	root = createRoom(30, 30, 0, 0);
	split1(root);
	for(int i = 0; i < 30; i++){
		for(int j = 0; j < 30; j++){
			if(i == 0 || i == 29 || j == 0 || j == 29){
				map[i][j] = '#';
			}else{
				map[i][j] = ' ';
			}
		}
	}
	BSP(root);
	for(int i = 0; i < 30; i++){
		for(int j = 0; j < 30; j++){
			if(i == 0 || i == 29 || j == 0 || j == 29){
				map[i][j] = '#';
			}
		}
	}
}

clock_t healthTimer = clock() + 12000;

void spawnHealth(){
	
	if(clock() > healthTimer){
		int x,y;
		
		do{
			x = rand()%30;
			y = rand()%30;
			if(map[y][x] == ' ') break;
		} while(true);
		map[x][y] = 'H';
		healthTimer = clock() + 12000;
	}
}

void printMap(){
	int score = 0;
	int health = 0;
	for(int i = 0; i < 30; i++){
		printf("%s\n", map[i]);
	}
}

clock_t enemyTimer = clock() + 6000;

void enemySpawn(){
	if(clock() > enemyTimer){
		int x,y;
		
		do{
			x = rand()%30;
			y = rand()%30;
			if(map[y][x] == ' ') break;
		} while(true);
		
		map[x][y] = '@';
		enemyTimer = clock() + 6000;
	}
}



void printMapBulletNew(){

	for(int i = 0; i < 30; i++){
		for(int j = 0; j < 30; j++){
			
			int replace = 0;
			
			for(int k = 0; k < totalBullet; k++){
				if(bullets[k].xPos == j && bullets[k].yPos == i){
					printf("*");
					replace = 1;
				}
			}
			if(replace == 0){
				printf("%c", map[i][j]);	
			}
//			replace = 0;
		}
		puts("");
	}
	printf("Score: %d\n", score);
	printf("Health: %d\n", health);
}


clock_t bulletDelay = clock() + 50;

void deleteBullet(int n){
		for(int i = 0; i < totalBullet; i++){
			bullets[i] = bullets[i+1];	
		}
		totalBullet--;
}

void deleteEnemy(int x, int y){
	map[y][x] = ' ';
}
void collision(){
	for(int i = 0; i < totalBullet; i++){
		if(map[bullets[i].yPos][bullets[i].xPos] == '#'){
			deleteBullet(i);
		}
		if(map[bullets[i].yPos][bullets[i].xPos] == '@'){
			map[bullets[i].yPos][bullets[i].xPos] = ' ';
			deleteBullet(i);
			score += 50;
//			deleteEnemy(bullets[i].xPos, bullets[i].yPos);
		}
	}
}

void bulletUpdateNew(){
	if(clock() > bulletDelay){
		for(int i = 0; i < totalBullet; i++){
			
			if(bullets[i].xPos == 1 || bullets[i].xPos == 28 || bullets[i].yPos == 1 || bullets[i].yPos == 28){
				deleteBullet(i);
			}
//			if(map[i][j] == )
			
			if(bullets[i].direction == '^'){
				bullets[i].yPos--;
			}
			if(bullets[i].direction == '<'){
				bullets[i].xPos--;
			}
			if(bullets[i].direction == 'v'){
				bullets[i].yPos++;
			}
			if(bullets[i].direction == '>'){
				bullets[i].xPos++;
			}
		}
		bulletDelay = clock() + 50;
	}
}
void bulletUpdate(){

    if(clock() > bulletDelay){
        for (int i = 0; i < totalBullet; i++){
            
            if(bulletPos[i][1] == 1 || bulletPos[i][1] == 28 || bulletPos[i][0] == 1 || bulletPos[i][0] == 28) {
                bulletLife[i][0] = 1;
            }
            
            if(bulletLife[i][0] == 0){
                if(bulletLife[i][1] == 0){
                    bulletPos[i][1]--;
                }
                else if(bulletLife[i][1] == 1){
                    bulletPos[i][0]--;
                }
                else if(bulletLife[i][1] == 2){
                    bulletPos[i][1]++;
                }
                else if(bulletLife[i][1] == 3){
                    bulletPos[i][0]++;
                }
            }  
        }
        bulletDelay = clock() + 50;
    }
}

clock_t movecd = clock() + 100;
clock_t bulletcd = clock() + 50;
void game(){
	generateMap();
	printMap();
	xplayer = (rand()%28) + 1;
	yplayer = (rand()%28) + 1;
	
	int xenemy;
	int yenemy;
	health = 2;
	
	for(int i = 0; i < enemySpawnCount; i++){
		do{
			xenemy = (rand()%28) + 1;
			yenemy = (rand()%28) + 1;	
		}while(map[yenemy][xenemy] != ' ');
		map[yenemy][xenemy] = '@';
	}
	
	system("cls");
	do{
		clearScreen();
		collision();
		spawnHealth();
		enemySpawn();
		bulletUpdateNew();
		printMapBulletNew();
		
		if(clock() > movecd){
			if(kbhit()){
				char input = getch();
				switch(input){
					case 'w':
//						if(yplayer > 1){
//							map[yplayer][xplayer] = 'H';
//							yplayer--;	
//							map[yplayer][xplayer] = '^';
//							bulletFace = 0;
//							health++;
//						}
						if(map[yplayer - 1][xplayer] == 'H')health += 1;
						if(map[yplayer - 1][xplayer] == '@')health -= 1;
						
						if(map[yplayer - 1][xplayer] == ' ' || map[yplayer - 1][xplayer] == '@' || map[yplayer - 1][xplayer] == 'H'){
							map[yplayer][xplayer] = ' ';
							yplayer--;	
							map[yplayer][xplayer] = '^';
							bulletFace = 0;
							collision();
						}
						collision();
					break;
					case 'a':
						if(map[yplayer][xplayer - 1] == 'H')health += 1;
						if(map[yplayer][xplayer - 1] == '@')health -= 1;
						
						if(map[yplayer][xplayer - 1] == ' ' || map[yplayer][xplayer - 1] == '@' || map[yplayer][xplayer - 1] == 'H'){
							map[yplayer][xplayer] = ' ';
							collision();
							xplayer--;	
							map[yplayer][xplayer] = '<';
							bulletFace = 1;
						}
					break;
					case 's':
						if(map[yplayer + 1][xplayer] == 'H')health += 1;
						if(map[yplayer + 1][xplayer] == '@')health -= 1;
						
						if(map[yplayer + 1][xplayer] == ' ' || map[yplayer + 1][xplayer] == '@' || map[yplayer + 1][xplayer] == 'H'){
							map[yplayer][xplayer] = ' ';
							collision();
							yplayer++;	
							map[yplayer][xplayer] = 'v';
							bulletFace = 2;
						}
					break;
					case 'd':
						if(map[yplayer][xplayer + 1] == 'H')health += 1;
						if(map[yplayer][xplayer + 1] == '@')health -= 1;
						
						if(map[yplayer][xplayer + 1] == ' ' || map[yplayer][xplayer + 1] == '@' || map[yplayer][xplayer + 1] == 'H'){
							map[yplayer][xplayer] = ' ';
							collision();
							xplayer++;	
							map[yplayer][xplayer] = '>';
							bulletFace = 3;
						}
					break;
					case 'k':
						bullets[totalBullet].xPos = xplayer;
						bullets[totalBullet].yPos = yplayer;
						bullets[totalBullet].direction = map[yplayer][xplayer];
						totalBullet++;
						
////						bulletPos[totalBullet][0] = xplayer; //kanan kiri 
////						bulletPos[totalBullet][1] = yplayer; //atas bawah
////						bulletLife[totalBullet][0] = 0;
////						bulletLife[totalBullet][1] = bulletFace;
////						totalBullet++;
						break;
				}
			}
		}

		if(clock() > bulletcd){
			if(kbhit()){
				char input = getch();
				switch(input){
					case 'k':
						bullets[totalBullet].xPos = xplayer;
						bullets[totalBullet].yPos = yplayer;
						bullets[totalBullet].direction = map[yplayer][xplayer];
						totalBullet++;
						break;
				}
			}
		}
		
		if(health == 0){
			counter++;
			strcpy(players[counter].name, name);
			players[counter].difficulty = difficulty;
			players[counter].score = score;	
			write_file();		
		}
	}while(health != 0);
	counter++;
}

int main(){
	int input;
	
	do{
		system("cls");
		read_file();
		print_main_menu();
		scanf("%d", &input); getchar();
		
		switch(input){
			case 1: 
					do{
						system("cls");
						printf("Input Hero's name [Must be between 3 and 10 characters (inclusve)]: ");
						scanf("%s", name);
					}while(strlen(name) < 3 || strlen(name) > 10);
					
					do{
						printf("Input difficulty [Must be between 1 and 3(inclusive)]: ");
						scanf("%d", &difficulty);
					}while(difficulty < 1 || difficulty > 3);
					
					setDifficulty(difficulty);
					game();
					
				break;
			case 2:
				quick_sort(players, 0, counter-1);
				print_leaderboard();
				getchar();
				break;
			case 3:
				help_menu();
				break;
			case 4:
				break;
		}
	}while(input != 4);
	
	return 0;
}
