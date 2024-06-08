#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <process.h>

char name[20];
FILE* scoreBoard;
int timeLimit = 30;	//시간제한(이게 되려나..)

unsigned _stdcall ThreadTimer(int* isEnd)
{
	//타이머 코드

	clock_t startTime = clock();
	//형식 변환 

	for (;;) {
		clock_t curr_time = clock() - startTime;
		double time_double = (double)curr_time / 1000;
		Sleep(100);
		printf("\033[1;1f             ");
		printf("\033[1;1f%.1lf\n", time_double);

		if (*isEnd == 1) {
			break;
		}
	}

	_endthreadex(0);
	return 0;
}


void rankingRead() {
	scoreBoard = fopen("scoreBoard.txt", "r");
	char line[255];
	char nameArray[255][20] = { "", };
	char scoreArray[255][20] = { "", };
	char temp[20] = "\0";
	int scoreLength;

	if (scoreBoard == NULL) {
		printf("랭킹 기록이 없습니다.\n");
		return;
	}

	for (int i = 0; i < 255; i++) {
		if (fgets(line, sizeof(line), scoreBoard) != NULL)
		{
			char* ptr = strtok(line, ":");      // ":" 공백 문자를 기준으로 문자열을 자름, 포인터 반환
			strcpy(nameArray[i], ptr);    // 이름 저장

			ptr = strtok(NULL, " ");      // 다음 문자열을 잘라서 포인터를 
			strcpy(scoreArray[i], ptr);    // 기록 저장
		}
		else {
			scoreLength = i;
			break;
		}
	}

	fclose(scoreBoard);

	for (int last = scoreLength; last >= 0; last--)
	{
		// 영역의 첫번째 칸부터 인접한 두 값을 비교하면서
		for (int i = 0; i < last; i++)
		{
			// 첫 번째 랭킹보다 더 앞에 있는 랭킹을 만나면 두 기록을 맞바꾸기
			if (atof(scoreArray[i]) > atof(scoreArray[i + 1]))
			{
				strcpy(temp, scoreArray[i]);
				strcpy(scoreArray[i], scoreArray[i + 1]);
				strcpy(scoreArray[i + 1], temp);

				strcpy(temp, nameArray[i]);
				strcpy(nameArray[i], nameArray[i + 1]);
				strcpy(nameArray[i + 1], temp);
			}
		}
	}

	for (int i = 1; i < scoreLength + 1; i++) {
		printf("%s : %s", nameArray[i], scoreArray[i]);
		if (i > 10) {
			break;
		}
	}

	printf("\n");
}

void rankingUpload(int score) {
	char fileBuffer[100] = "";
	char stringTime[10];

	sprintf(stringTime, "%d", score);
	scoreBoard = fopen("scoreBoard.txt", "a");
	strcpy(fileBuffer, strcat(strcat(name, ":"), stringTime));
	fileBuffer[strlen(fileBuffer)] = '\n';

	fputs(fileBuffer, scoreBoard);
	fclose(scoreBoard);

	rankingRead();
}

void mainGame() {
	int arrowAscii[4] = { 72, 75, 80, 77 };	//아스키코드 번호, 순서대로 ↑, ←, ↓, →
	int pattern[5000];	//패턴이 들어가는 배열
	int arrowInput;	//입력된 키가 무엇인지 받아내는 변수
	int score = 0;		//점수
	int i = 0;			//패턴 배열의 몇번째를 치고 있는가
	clock_t start_time;
	clock_t end_time;
	double time_elapsed = 0;
	int isEnd = 0;

	srand(time(NULL));

	printf("닉네임을 설정해주세요: ");
	scanf("%s", name);

	system("cls");

	for (int a = 0; a < 5000; a++) {
		pattern[a] = arrowAscii[rand() % 4];	//패턴 랜덤 제작
	}

	start_time = clock();

	_beginthreadex(NULL, 0, ThreadTimer, (void*)&isEnd, 0, NULL);

	for (;;) {
		end_time = clock();
		time_elapsed = (double)(end_time - start_time) / CLOCKS_PER_SEC;

		printf("\033[36m");
		printf("\033[4;24f■■■\n");
		printf("\033[5;24f■  ■\n");
		printf("\033[6;24f■■■\n");
		printf("\033[0m");

		for (int k = 0; k < 7; k++) {	//화면에 보여지는 화살표가 7개
			switch (pattern[i + k - 2]) {	//내가 최근에 친 2개를 포함해서 7개를 보여줌
			case 72:
				printf("\033[5;%df↑", 36 - k * 5);		//뒤에서부터 화살표를 출력한다
				break;
			case 75:
				printf("\033[5;%df←", 36 - k * 5);
				break;
			case 80:
				printf("\033[5;%df↓", 36 - k * 5);
				break;
			case 77:
				printf("\033[5;%df→", 36 - k * 5);
				break;
			}
		}
		arrowInput = _getch();	//입력된 키 처리
		if (arrowInput == 224) {	//만약 화살표라면
			arrowInput = _getch();	//화살표 방향을 가져옴
			if (arrowInput == pattern[i]) {		//arrowAscii의 아스키 코드와 입력한 아스키 코드가 같다면(맞았다면)
				score += 10000;	//점수가 오르고
				i++;		//다음 화살표를 출력
				printf("\033[3;1f");
				printf("\033[0m                                       ");
				printf("\033[2;1f%d", score);	//점수 출력

				if (time_elapsed >= timeLimit) {	//만약 시간제한이 끝났다면
					break;		//게임 끝
				}
			}
			else {		//틀렸다면
				score -= 5000;  //점수가 내려가고
				if (score < 0) {
					score = 0;  //점수가 음수이면 0으로 고정
					printf("\033[2;1f%d", score);	//점수 출력
				}
				else {
					printf("\033[2;1f%d", score);	//점수 출력
				}
				printf("\033[3;1f");
				printf("\033[41m                                       \033[0m");	//빨간색으로 표시
			}
		}
	}

	isEnd = 1;

	system("cls");
	printf("\033[1;1f점수: %d\n\n", score);

	rankingUpload(score);
}

void intro1() {

	printf("\033[31m                                            #\n");
	printf("           #              #              #  #          #\n");
	printf("########   #   #          #   ########   #  #     #    #\n");
	printf("#      #   #   #          #          #   #  #    # #   #\n");
	printf("#      # ###   #          #          # ###  #     #    #\n");
	printf("#      #   #   #          #          #   #  #          #\n");
	printf("########   #   ########   #          #   #  #     ######\n");
	printf("           #              #              #  #     #    #\n");
	printf("                                            #     ######\033[0m\n\n\n\n");
}

void intro2() {

	system("cls");

	printf("일기\n");
	printf("2030. 05. 14.\n");
	printf("나는 평소와 다름없이 아침에 일어나 가방을 챙긴 후 바로 학교로 향했다.과방에 도착했지만 아무도 없었다.\n");
	printf("전날 박소영 교수님이 내주신 과제를 마저 하려고 노트북을 켰다.그런데 조금 이상해보였다.\n");
	printf("바탕화면에는 아무것도 없었고, '머니게임'이라는 프로그램만 깔려있었다.\n");
	printf("난 속으로 생각했다. '머니게임? 게임으로 돈을 번다는 건가?'\n");
	printf("게임으로 돈을 벌 수 있다면 얼마나 좋을까 ? 난 홀린듯이 머니게임을 클릭했다...\n\n\n\n");
}


int main() {
	char menu[128];
	char next1 = '\0';
	char next2 = '\0';
	intro1();
	while (1) {
		printf("게임을 진행하시려면 (n)을 눌러주세요: ");
		scanf("%c", &next1);
		getchar();
		if (next1 == 'n' || next1 == 'N') {
			break;
		}
		else {
			printf("잘못 입력하셨어요.\n");
		}
	}

	intro2();
	while (1) {
		printf("게임을 진행하시려면 (n)을 눌러주세요: ");
		scanf("%c", &next2);
		getchar();
		if (next2 == 'n' || next2 == 'N') {
			break;
		}
		else {
			printf("잘못 입력하셨어요.\n");
		}
	}

	system("cls");

	for (;;) {
		strcpy(menu, "");
		printf(" __________메뉴__________ \n");
		printf("|                        |\n");
		printf("|        게임시작        |\n");
		printf("|                        |\n");
		printf("|        게임설명        |\n");
		printf("|                        |\n");
		printf("|          랭킹          |\n");
		printf("|                        |\n");
		printf("|          종료          |\n");
		printf("|________________________|\n\n");
		printf("메뉴를 선택하세요 : ");
		scanf("%s", &menu);

		if (!strcmp(menu, "게임시작")) {
			fflush(stdin);
			mainGame();
		}
		else if (!strcmp(menu, "게임설명")) {
			fflush(stdin);
			printf("\n\n\n게임설명\n");
			printf("1. 화면에 나오는 화살표와 일치하는 방향키를 눌러 점수를 획득하세요\n");
			printf("2. 게임시작 입력 후 방향키를 누르면 타이머가 시작됩니다\n");
			printf("3. 제한시간은 30초이고 그 안에 최대한 많은 점수를 획득하시면 됩니다\n");
			printf("4. 여러분의 점수는 돈이됩니다...  아주 큰 돈이요...\n\n\n");
		}
		else if (!strcmp(menu, "랭킹")) {
			fflush(stdin);
			rankingRead();
		}
		else if (!strcmp(menu, "종료")) {
			break;
		}
		else {
			printf("잘못 입력하셨어요.\n\n");
		}
	}

	return 0;
}
