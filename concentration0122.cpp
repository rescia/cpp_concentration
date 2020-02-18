#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

const char suit_mark[] = {"HDSC"};
const char rank_mark[14][8] = {"", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
int player_score = 0, com_score = 0;
int use_memory, mode, next_card1 = 99, next_card2 = 99;
int memory[100][100];//5*5
int cnt[100] = {0};//5

const int ROW = 4;
const int COL = 4;
const int max_clear = 16;
int sum_clear = 0;

typedef struct {
	int rank;//1-13
	int suit;//0-3
	int visible;//0, 1
	int clear;//0, 1
	int touched;//0, 1
} Card;

void make_deck_concentration(Card deck[]);
void shuffle_cards(Card cards[], const int n);
void draw_score();
void turn_player(Card mycard[]);
void turn_com(Card mycard[]);
int check_input(Card mycard[], const int input1, const int input2);
int check_touched(Card mycard[], const int input1, const int input2);
void draw_field(const Card mycard[]);
void open_card(Card mycard[], const int input1, const int input2);
void memory_card(const int address, const int num);
void delete_memory(const int address, const int num);
void next_card(const Card mycard[]);
void judge(Card mycard[], const int input1_1, const int input1_2, const int input2_1, const int input2_2, const int target);
//void check_card(const Card mycard[]);
void add_score(const int target, const int score);
void judge_win();
void window_size();
void flush();
void check_memory();


//13行のプロンプトウィンドウサイズ
int main() {
	//window_size();
	Card mycard[100];
	int info;
	make_deck_concentration(mycard);
	shuffle_cards(mycard, 15);
	//難易度を選択してもらう
	printf("難易度を選択してください   2=Hard  1=Normal  0=Easy\n");
	scanf("%d", &mode);
	printf("難易度は%dです", mode);
	printf("Game Start.\n");
	while(1) {
		if(sum_clear >= max_clear) break;
		draw_score();
		draw_field(mycard);
		printf("turn_player start.\n");
		turn_player(mycard);//playerのターン
		if(sum_clear >= max_clear) break;
		printf("Next turn is computer. OK?   [1]OK.\n");
		while(1) {
			scanf("%d", &info);
			if(info == 1) {
				//printf("\n\n");
				flush();
				break;
			}
		}
		if(mode) next_card(mycard);//1, 2なら通す
		//printf("next_card1_%d  next_card2_%d\n", next_card1, next_card2);
		printf("turn_com start.\n");
		turn_com(mycard);//computerのターン
		printf("Next turn is you. OK?   [1]OK.\n");
		while(1) {
			scanf("%d", &info);
			if(info == 1) {
				flush();
				break;
			}
		}
	}
	printf("All cards disappeared.\n");
	draw_score();
	judge_win();
	printf("Finish the game.\n");

	// check_card(mycard);
	return 0;
}

void turn_player(Card mycard[]) {
	int input1_1, input1_2, input2_1, input2_2;
	printf("[1st] select a card.\n");
	while(1) {
		scanf("%d %d", &input1_1, &input1_2);
		if(check_input(mycard, input1_1, input1_2)){
			break;
		} else {//選んだカードが選択できないとき
			printf("There is no card there. Please choose it once again.\n");
		}
	}
	open_card(mycard, input1_1, input1_2);
	draw_field(mycard);
	printf("[2nd] select a card.\n");
	while(1) {
		scanf("%d %d", &input2_1, &input2_2);
		if(check_input(mycard, input2_1, input2_2)){
			break;
		} else {//選んだカードが選択できないとき
			printf("There is no card there. Please choose it once again.\n");
		}
	}
	open_card(mycard, input2_1, input2_2);
	draw_field(mycard);
	judge(mycard, input1_1, input1_2, input2_1, input2_2, 1);
	return;
}

void turn_com(Card mycard[]) {
	//次にめくるカード候補があれば、それを選択する。そうでなければランダムで決定する。
	int input1_1, input1_2, input2_1, input2_2;
	if(next_card1 != 99 && next_card1 != 98 && mycard[next_card1].clear == 0 && mycard[next_card2].clear == 0) {
		input1_1 = next_card1 / 4;
		input1_2 = next_card1 % 4;
		input2_1 = next_card2 / 4;
		input2_2 = next_card2 % 4;
	} else {
		while(1) {
			input1_1 = rand() % ROW;
			input1_2 = rand() % COL;
			if(check_input(mycard, input1_1, input1_2) == 1 && check_touched(mycard, input1_1, input1_2) == 1) break;
		}
		while(1) {
			input2_1 = rand() % ROW;
			input2_2 = rand() % COL;
			if(input1_1 == input2_1 && input1_2 == input2_2) continue;
			if(check_input(mycard, input2_1, input2_2) == 1) break;
		}
	}
	printf("computer chose this number :%d %d %d %d\n", input1_1, input1_2, input2_1, input2_2);
	open_card(mycard, input1_1, input1_2);
	open_card(mycard, input2_1, input2_2);
	draw_field(mycard);
	judge(mycard, input1_1, input1_2, input2_1, input2_2, 0);
	return;
}

void make_deck_concentration(Card deck[]) {//神経衰弱用カードデックの作成
	int i = 0, k = 0, n = 0;
	for(i = 0; i <= 3; i++) {//suit
		for(k = 1; k <= 4; k++) {//rank
			deck[n].suit = i;
			deck[n].rank = k;
			deck[n].visible = 0;//デフォルトは裏面
			deck[n].clear = 0;//未クリア
			deck[n].touched = 0;//表にされたことがない
			n++;
		}
	}
	//メモリー用の2次元配列を99で初期化
	for(int i = 0; i < 100; i++) {
		for(int k = 0; k < 100; k++) {
			memory[i][k] = 99;
		}
	}
	return;
}

void draw_score() {
	printf("score\t\tplayer: %d\tcom: %d\n", player_score, com_score);
	return;
}

int check_input(Card mycard[], const int input1, const int input2) {
	if(input1 == 99) {
		printf("Finish the program.\n");
		exit(0);
	} else if((mycard[input1*4+input2].clear == 1) || (input1 < 0) || (input1 > ROW-1) || (input2 < 0) || (input2 > COL-1)) {
		//printf("There is no card there. Please choose it once again.\n");//そこにカードはありません
		return 0;
	}
	return 1;
}
int check_touched(Card mycard[], const int input1, const int input2) {
	if(mycard[input1*4+input2].touched == 1) {
		return 0;
	} else {
		return 1;
	}
}

void draw_field(const Card mycard[]) {
	int i, k;
	/* 列番号の表示 */
	for(i = 0; i < COL; i++) {
		if(i == 0) printf("   ");
		printf("_%d_", i);
		if(i != COL-1) printf("  ");
	}
	printf("\n");
	for(i = 0; i < ROW; i++) {
		/* 行番号の表示 */
		printf("%d|", i);
		/* カード情報の表示 */
		for(k = 0; k < COL; k++) {
			if(mycard[i*4+k].clear) {//クリア済みカードを空白で表示
				printf("     ");
			} else if(mycard[i*4+k].visible) {//表にされているならカード情報を表示
				printf(" %c%2s ", suit_mark[ mycard[i*4+k].suit ], rank_mark[ mycard[i*4+k].rank ]);
			} else {//裏にされているなら模様を表示
				printf(" *** ");
			}
		}
		printf("\n");
	}
	return;
}

void open_card(Card mycard[], const int input1, const int input2) {
	//input1は行、input2は列
	int opened_card = input1*4+input2;
	mycard[opened_card].visible = 1;
	if(mycard[opened_card].touched == 0) {
		memory_card(opened_card, mycard[opened_card].rank);//初めて表になったカードならメモリーする。(開いたカードのID、開いたカードの数字)
		mycard[opened_card].touched = 1;
	}
	return;
}

void memory_card(const int id, const int num) {
	//引数の数値に基いて、カード情報をメモリーする
	//id = カードの通し番号   num = そのカードのrank
	switch(num) {
		case 1:
			memory[1][cnt[1]] = id;
			cnt[1]++;
			break;
		case 2:
			memory[2][cnt[2]] = id;
			cnt[2]++;
			break;
		case 3:
			memory[3][cnt[3]] = id;
			cnt[3]++;
			break;
		case 4:
			memory[4][cnt[4]] = id;
			cnt[4]++;
			break;
		default:
			printf("memoryできませんでした\n");
			break;
	}
	return;
}

void delete_memory(const int address, const int num) {
	//既にメモリーされた情報を後から消す
	//address = カードの通し番号   num = そのカードのrank
	for(int i = 0; i < 5; i++) {
		if(memory[num][i] == address) memory[num][i] = 98;
	}
	return;
}

void next_card(const Card mycard[]) {
	//メモリーに基づき、次に選択するとペアになるカードを探す
	int i, k, j;
	static int normal_switch = 0;
	if(mode == 1) {//Normalのとき
		if(normal_switch) {
			normal_switch = 0;
			printf("normal_switch is 0. avoid.");
			return;
		}
		normal_switch = 1;
	}
	for(int i = 1; i <= 4; i++) {
		if(cnt[i] >=2) {
			for(k = 0; k < 4; k++) {
				if(memory[i][k] != 98 && memory[i][k] != 99) {//98はclear済み、99はデータなし
					for(int j = k+1; j < 4; j++) {
						if(memory[i][j] != 98 && memory[i][j] != 99) {//rankがiであるカードが2ペア分メモリーされているとき
							next_card1 = memory[i][k];
							next_card2 = memory[i][j];
							memory[i][k] = 98;
							memory[i][j] = 98;
							return;
						}
					}
				}
			}
		}
	}
	next_card1 = 98, next_card2 = 98;//ここまできてしまったら前と同じ数字なので機能しない
	return;
}

void judge(Card mycard[], const int input1_1, const int input1_2, const int input2_1, const int input2_2, const int target) {
	//2枚のカードがペアかどうか判定する関数
	int selected_card1 = input1_1*4+input1_2, selected_card2 = input2_1*4+input2_2;
	if(mycard[selected_card1].rank == mycard[selected_card2].rank) {
		printf("The number of the card you chose is the same.\n");
		mycard[selected_card1].clear = 1;
		mycard[selected_card2].clear = 1;
		delete_memory(selected_card1, mycard[selected_card1].rank);
		delete_memory(selected_card2, mycard[selected_card2].rank);
		add_score(target, 2);//1:playerに加算
		sum_clear += 2;
		draw_score();
	} else {
		printf("The number of the card you chose is different.\n");
		draw_score();
	}
	mycard[selected_card1].visible = 0;
	mycard[selected_card2].visible = 0;

	return;
}

void judge_win() {
	//ゲームの勝敗を判定する関数
	if(player_score > com_score) {
		printf("playerの勝ち\n");
	} else if(com_score > player_score) {
		printf("computerの勝ち\n");
	} else {
		printf("同点なので引き分けです\n");
	}
	return;
}

void add_score(const int target, const int score) {
	//引数に基づき、得点を加算する関数
	if(target) {//target == 1ならplayerに得点を加算
		player_score += score;
	} else {
		com_score += score;
	}
	return;
}

void window_size() {
	//最適なウィンドウサイズの目安
	for (int i = 0; i < 13; i++){
		printf("%d\n", i);
	}
	return;
}

void flush() {
	for(int i = 0; i < 13; i++) {
		printf("\n");
	}
	return;
}

void check_card(const Card mycard[]) {
	printf("-------------------------------------------------------------------------------\n");
	for(int i = 0; i < 52; i++){
		printf("mycard[%d]_%c%2s_%d\n", i, suit_mark[ mycard[i].suit ], rank_mark[ mycard[i].rank ], mycard[i].clear);
	}
	return;
}

void check_memory() {
	printf("\nmemoryの内容確認\n");
	for(int i = 1; i <= 4; i++) {
		printf("[%d]  ", i);
		for(int k = 0; k <= 3; k++) {
			if(memory[i][k] != 99) {
				printf("%d\t", memory[i][k]);
			}else{
				printf("-\t");
			}
		}
		printf("\n");
	}
	return;
}

void make_deck(Card deck[]) {//デックを作成する関数
	int i, k, n = 0;
	for(i = 0; i <= 3; i++) {
		for(k = 1; k <=13; k++) {
			deck[n].suit = i;
			deck[n].rank = k;
			n++;
		}
	}
	return;
}

void print_cards(const Card cards[], const int n) {//カード列を出力する関数
	int i, k;
	for(i = 0; i < n; i++) {
		printf("%c ", suit_mark[ cards[i].suit ]);
		printf("%s\n", rank_mark[ cards[i].rank ]);
	}
	return;
}

void shuffle_cards(Card cards[], const int n) {//シャッフルする関数
	int v1, v2;
	Card work;
	srand(time(NULL));
	for (int i = 0; i < 100000; i++) {
		v1 = rand() % n;
		v2 = rand() % n;
		work = cards[v1];
		cards[v1] = cards[v2];
		cards[v2] = work;
	}
	return;
}
