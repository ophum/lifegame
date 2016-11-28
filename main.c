#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <curses.h>
#include <time.h>

#define Width 1280
#define Height 800

#define WindowWidth 80
#define WindowHeight 23

#define Cell '.'
#define None ' '

#define fps 100000000 / 48

struct{
	int o_x, o_y;	// 左上の座標
	int width, height;	// 表示する大きさ
}Window;

int game_end;

// キー操作、左上の座標を変更、終了フラグ
// w 上へ, a 左へ, s 下へ, d 右へ
void key(){
	char c;
	c = getchar_unlocked();
	if(c == 'a') Window.o_x--;
	if(c == 'd') Window.o_x++;
	if(c == 'w') Window.o_y--;
	if(c == 's') Window.o_y++;
	if(c == 'q') game_end = 1;
	
	if(Window.o_x < 0) Window.o_x = 0;
	if(Window.o_x > Width) Window.o_x = Width;

	
	if(Window.o_y < 0) Window.o_y = 0;
	if(Window.o_y > Height) Window.o_y = Height;
}

void disp(){
	struct timespec treq, trem;
	treq.tv_sec = (time_t)0;
	treq.tv_nsec = fps;
	
	// lifegame
	int board[Height][Width] = { 0 };
	int tmp_board[Height][Width] = { 0 };
	int i, j, k, cnt = 0;
	
	// Cell初期配置
	for(i = 0; i < Height; i++){
		for(j = 0; j < Width; j++){
			board[i][j] = ((rand() % 100) % 2 ? 1 : 0);
		}
	}
	
	// ゲームループ
	while(!game_end){
		printf("\033[1;1H");
		// 面倒なので一番外は無視
		// 1111
		// 1001
		// 1111
		// ^この場合1は見ないで0の部分のみ判定
		for(i = 1; i < Height-1; i++){
			for(j = 1; j < Width-1; j++){
				// 周りの生存Cellの数を数える
				k = board[i][j-1] + board[i][j+1]
					+ board[i-1][j-1] + board[i-1][j] + board[i-1][j+1] 
					+ board[i+1][j-1] + board[i+1][j] + board[i+1][j+1];

				if(k == 3){
					tmp_board[i][j] = 1;
				}else if(board[i][j] && k == 2){
				}else {
					tmp_board[i][j] = 0;
				}
			}
		}
		// tmp からコピー
		for(i = 0; i < Height; i++){
			for(j = 0; j < Width; j++){
				board[i][j] = tmp_board[i][j];
			}
		}
		// 表示
		for(i = 0; i < Window.height; i++){
			for(j = 0; j < Window.width; j++){
				printf("\033[%d;%dH", i, j);
				if(board[Window.o_y + i][Window.o_x + j]){
					printf("\033[43m%c", Cell);
				}else {
					printf("\033[47m%c", None);
				}
				printf("\033[49m");
			}
		}
		printf("\033[23;1H");
		printf("cnt: %d, O(x, y)= (%04d, %04d) end:%d", cnt++, Window.o_x, Window.o_y, game_end);
		// ちょっとタンマ！
		nanosleep(&treq, &trem);
		
	}
	
}
int main(){

	pthread_t key_thread;
	pthread_t disp_thread;
	int disp_ret;
	game_end = 0;
	
	initscr();	// lifegame
	Window.o_x = Window.o_y = 0;
	Window.width = WindowWidth;
	Window.height = WindowHeight;

	disp_ret = pthread_create(&disp_thread, NULL, (void *)disp, NULL);
	
	// lifegame
	printf("\033[2J");
	
	noecho();	// lifegame
	while(!game_end){
		key();
	}
	
	// lifegame
	printf("\033[2J");
	echo();
	endwin();
	pthread_join(disp_thread, NULL);

	return 0;
}
