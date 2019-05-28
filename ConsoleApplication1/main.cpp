#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <direct.h>
#include <mmsystem.h>
#include <tchar.h>
#include <afxcom_.h>
#pragma comment(lib, "winmm.lib")


#define LEFT 1
#define RIGHT 30
#define TOP 1
#define BUTTON 22
//============================================================
//�Զ���ȫ�ֱ�����ȫ�ֳ���
HANDLE g_hConsoleOutput;

short potVal[40][30];					//0-�հף�1-1�����飬2-2�����飬3-3�����飬4-Χǽ

int live = 1;							//����
int stop = 0;							//0->����ͣ��1->��ͣ
int speed = 1000;						//�ٶ�
int intGrade = 0;						//����
int intTreasure = 0;					//��ʯ��
int getTreasure = 0;					//��ñ�ʯ
int level_Now = 0;						//��ǰ�ؿ�
int level_Max = 0;						//�ؿ�����

char filename[100][20];					//Ŀǰ������ߵ�ͼ����Ϊ100��
char game_Map[20];						//��Ϸ��ͼ

clock_t treaTime_Last, treaTime_Now;			//�������ʱ��   ��ʱ����

static const char* BALL_SHAPE[]= { "��", "��", "��" };

//============================================================
//�Զ���ṹ��
typedef struct 	Point//��
{
	int x;
	int y;
};


struct BALL     //��
{
	int x;
	int y;
	int x_speed;
	int y_speed;
	int mode;	//0->��ͨ��1->ʵ�ģ�2->��˪��
}Ball[3];

struct BORD		//��
{
	int x;
	int y;
	int length;
}Bord;

struct TEASURE	//��ʯ
{
	int x;
	int y;
	int type;
}Treasure;

enum color
{
	green,
	blue,
	red
};

enum mode
{
	run_Game,
	creat_Map
};
//============================================================
//�Զ��庯������
void gotoxy(int x, int y);

int mainGame();					//���˵�
void initGame(enum mode mode);		//��ʼ����Ϸ

void runGame();					//������Ϸ
void creatMap();				//������ͼ
void helpGame();				//��Ϸ����
void selectMap();				//ѡ���ͼ
void playGame();				//��Ϸ����ʱ
int checkComplete();			//�����Ϸ�Ƿ����

void PrintBord();				//����ľ���С��
void PrintLev();				//��ʼ����عؿ�
void MoveBord(int orientation);	//�ƶ�ľ��
void MoveBall(int index);		//�ƶ�С��

void removeDiamond(int index, int direction);		//�Ƴ�����
void removeColor(enum color, Point diamond);		//�����ɫ
void showTreasure(int intRand, Point point_diamond);//����							
void move_Treasure();								//�ƶ���ʯ
void get_Treasure();								//��ñ�ʯ
void time_Treasure();								//��ʯʧЧ


//============================================================
//��ȫ�Ƕ�λ��ĳ��
void gotoxy(int x, int y)
{
	static COORD cd;			//�ڿ���̨�ϵ�����

	cd.X = (int)(x << 1);
	cd.Y = y;
	SetConsoleCursorPosition(g_hConsoleOutput, cd);
}



//////////////////////////////////////////////////////////////
//���˵�
int mainGame()
{
	FILE *fMap;

	int ch, index = 0;
	static const char* modeItem[] = { "�ʼ��Ϸ", "�������ؿ�", "����Ϸ����", "��ѡ��ؿ�" };

	system("cls");						//ִ����������

	SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);				//���ÿ���̨���ڵ�������ɫ�ͱ���ɫ
	gotoxy(15, 5);
	printf("��������������������������������");
	gotoxy(15, 6);
	printf("��%2s%s%2s��", "", "�ﵯ���֡�", "");
	gotoxy(15, 7);
	printf("��������������������������������");

	SetConsoleTextAttribute(g_hConsoleOutput, 0xF0);
	gotoxy(16, 14);
	printf("%2s%s%2s", "", modeItem[0], "");
	SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
	gotoxy(16, 16);
	printf("%2s%s%2s", "", modeItem[1], "");
	SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
	gotoxy(16, 18);
	printf("%2s%s%2s", "", modeItem[2], "");
	SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
	gotoxy(16, 20);
	printf("%2s%s%2s", "", modeItem[3], "");
	
	if (fopen_s(&fMap,"maps\\MapInfo.dat","r"))
	{
		gotoxy(LEFT + 4, TOP + 2);
		printf("�򿪵�ͼ��Ϣ�ļ���������");
		gotoxy(LEFT + 4, TOP + 4);
		printf("���ѯMapInfo.dat�ļ��Ƿ���ڣ�����");
		exit(0);											//��ʾ���̷������˳�
	}

	while (fgets(filename[level_Max], 20, fMap) != NULL)			//��ȡָ�������n���ַ�����Ϣ���洢��ַ���ɹ����ش洢����ַ
	{
		if (filename[level_Max][strlen(filename[level_Max]) - 1] == '\n')
		{
			filename[level_Max][strlen(filename[level_Max]) - 1] = 0;
		}
		level_Max++;
	}
	fclose(fMap);

	level_Now = 0;			//Ĭ�ϵ�һ����ͼ


	do
	{
		ch = _getch();		//��ȡһ���ַ���ת��ΪASCII�룬������ʾ����Ļ�� ��������enter

		switch (ch)
		{
		case 's':
		case 'S':
		case '2':
		case 80:	//��	getch�����ڶ�ȡһ�����ܼ����߼�ͷ�����򣩼���ʱ�������᷵�����Σ���һ�ε��÷���0����0xE0���ڶ��ε��÷���ʵ�ʵļ�ֵ��
			if (index == 0)
			{
				index = 1;
				SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
				gotoxy(16, 14);
				printf("%2s%s%2s", "", modeItem[0], "");
				SetConsoleTextAttribute(g_hConsoleOutput, 0xF0);
				gotoxy(16, 16);
				printf("%2s%s%2s", "", modeItem[1], "");
			}
			else if (index == 1)
			{
				index = 2;
				SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
				gotoxy(16, 16);
				printf("%2s%s%2s", "", modeItem[1], "");
				SetConsoleTextAttribute(g_hConsoleOutput, 0xF0);
				gotoxy(16, 18);
				printf("%2s%s%2s", "", modeItem[2], "");
			}
			else if (index == 2)
			{
				index = 3;
				SetConsoleTextAttribute(g_hConsoleOutput, 0xF0);
				gotoxy(16, 20);
				printf("%2s%s%2s", "", modeItem[3], "");
				SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
				gotoxy(16, 18);
				printf("%2s%s%2s", "", modeItem[2], "");
			}
			else if (index == 3)
			{
				index = 0;
				SetConsoleTextAttribute(g_hConsoleOutput, 0xF0);
				gotoxy(16, 14);
				printf("%2s%s%2s", "", modeItem[0], "");
				SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
				gotoxy(16, 20);
				printf("%2s%s%2s", "", modeItem[3], "");
			}
			break;

		case 'w':
		case 'W':
		case '8':
		case 72:		//��
			if (index == 0)
			{
				index = 3;
				SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
				gotoxy(16, 14);
				printf("%2s%s%2s", "", modeItem[0], "");
				SetConsoleTextAttribute(g_hConsoleOutput, 0xF0);
				gotoxy(16, 20);
				printf("%2s%s%2s", "", modeItem[3], "");
			}
			else if (index == 1)
			{
				index = 0;
				SetConsoleTextAttribute(g_hConsoleOutput, 0xF0);
				gotoxy(16, 14);
				printf("%2s%s%2s", "", modeItem[0], "");
				SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
				gotoxy(16, 16);
				printf("%2s%s%2s", "", modeItem[1], "");
			}
			else if (index == 2)
			{
				index = 1;
				SetConsoleTextAttribute(g_hConsoleOutput, 0xF0);
				gotoxy(16, 16);
				printf("%2s%s%2s", "", modeItem[1], "");
				SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
				gotoxy(16, 18);
				printf("%2s%s%2s", "", modeItem[2], "");
			}
			else if (index == 3)
			{
				index = 2;
				SetConsoleTextAttribute(g_hConsoleOutput, 0xF0);
				gotoxy(16, 18);
				printf("%2s%s%2s", "", modeItem[2], "");
				SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
				gotoxy(16, 20);
				printf("%2s%s%2s", "", modeItem[3], "");
			}
			break;

		case ' ':
		case 13:
			return index;
			break;
		}
	} while (1);
}

//������
int main()
{
	g_hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTitleA("~~~������~~~");
	char filename[1000];
	_getcwd(filename, sizeof(filename));					//��ȡ��ǰ·��
	char subfilename[] = "\\music\\bg_music.mp3";
	strcat(filename, subfilename);

	
	int ret = 0;
	char error[100] = { 0 };
	MCIERROR mciError;
	mciError = mciSendString("open filename alias music", error, 100, NULL);			//��
	mciError = mciSendString("play music repeat", error, 100, NULL);				//����
	mciError = mciSendString("setaudio music volume to 100", NULL, 0, NULL);
	//if (mciError)
	//{
	//	mciGetErrorString(mciError, error,100);
	//	printf("%s", error);
	//}
	do
	{
		CONSOLE_CURSOR_INFO cursorInfo = { 1,FALSE };			//�����Ϣ
		SetConsoleCursorInfo(g_hConsoleOutput, &cursorInfo);	//���ù������
		
		switch (mainGame())
		{
		case 0:
			playGame();
			break;
		case 1:
			creatMap();
			break;
		case 2:
			helpGame();
			break;
		case 3:
			selectMap();
			playGame();
			break;
		}
	
	
	} while (1);

	return 0;
}

//=========================================================
//����������
void playGame()			//��Ϸ���ʱ
{
	char ch;

	do
	{
		runGame();

		if (level_Now < level_Max&&checkComplete())
		{
			level_Now++;
			gotoxy(LEFT + 2, BUTTON - 3);
			printf("��ϲ���Ѿ��ɹ����أ�����");
			gotoxy(LEFT + 2, BUTTON - 3);
			printf("�Ƿ������Ϸ��<��ENTER����������������������˵�>");
		}
		else
		{
			break;
		}
	} while ((ch == _getch()) == 13);
}

//============================================================
//��ʼ��Ϸ
void runGame()
{
	int ch;
	clock_t clockLast, clockNow;       //��Ϸ������
	clock_t treasureLast, treasureNow; //�����������

	clockLast = treasureLast = clock(); // ��ʱ

	initGame(run_Game);

	PrintLev(); // ������Ϸ����

	while (live)
	{
		while (!stop && live)
		{
			while (_kbhit()) // �м�����
			{
				ch = _getch();
				switch (ch)
				{
				case 27:
					return;
					break;
				case 'a':
				case 'A':
				case '4':
				case 75:
					MoveBord(1);
					break;
				case 'd':
				case 'D':
				case '6':
				case 77:
					MoveBord(2);
					break;
				case 32:
					stop = 1;
				}
			}

			clockNow = clock(); // ��ʱ
								// ���μ�ʱ�ļ������0.45��
			if (clockNow - clockLast > 0.2F * speed)
			{
				clockLast = clockNow;
				MoveBall(0);
			}

			if (intTreasure == 1)
			{
				treasureNow = clock();
				if (treasureNow - treasureLast > 0.5F * CLOCKS_PER_SEC)
				{
					treasureLast = treasureNow;
					move_Treasure();
				}
			}

			if (getTreasure == 1)
			{
				treaTime_Now = clock();
				if (treaTime_Now - treaTime_Last > 1.0F * CLOCKS_PER_SEC)
				{
					if (treaTime_Now - treaTime_Last < 1.1F * CLOCKS_PER_SEC)
					{
						SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
						gotoxy(LEFT + 27, BUTTON - 2);
						printf("%2s", "");
					}
					else if (treaTime_Now - treaTime_Last < 2.1F * CLOCKS_PER_SEC)
					{
						SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
						gotoxy(LEFT + 26, BUTTON - 2);
						printf("%2s", "");
					}
					else if (treaTime_Now - treaTime_Last < 3.1F * CLOCKS_PER_SEC)
					{
						SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
						gotoxy(LEFT + 25, BUTTON - 2);
						printf("%2s", "");
					}
					else if (treaTime_Now - treaTime_Last < 4.1F * CLOCKS_PER_SEC)
					{
						SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
						gotoxy(LEFT + 24, BUTTON - 2);
						printf("%2s", "");
					}
					else if (treaTime_Now - treaTime_Last < 5.1F * CLOCKS_PER_SEC)
					{
						SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
						gotoxy(LEFT + 23, BUTTON - 2);
						printf("%2s", "");
					}
					else if (treaTime_Now - treaTime_Last < 6.1F * CLOCKS_PER_SEC)
					{
						SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
						gotoxy(LEFT + 22, BUTTON - 2);
						printf("%2s", "");
					}
					else if (treaTime_Now - treaTime_Last < 7.1F * CLOCKS_PER_SEC)
					{
						SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
						gotoxy(LEFT + 21, BUTTON - 2);
						printf("%2s", "");
					}
					else if (treaTime_Now - treaTime_Last < 8.1F * CLOCKS_PER_SEC)
					{
						SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
						gotoxy(LEFT + 20, BUTTON - 2);
						printf("%2s", "");
					}
					else if (treaTime_Now - treaTime_Last < 9.1F * CLOCKS_PER_SEC)
					{
						SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
						gotoxy(LEFT + 19, BUTTON - 2);
						printf("%2s", "");
					}
					else if (treaTime_Now - treaTime_Last < 10.1F * CLOCKS_PER_SEC)
					{
						SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
						gotoxy(LEFT + 18, BUTTON - 2);
						printf("%2s", "");
					}
					else if (treaTime_Now - treaTime_Last < 11.1F * CLOCKS_PER_SEC)
					{
						SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
						gotoxy(LEFT + 17, BUTTON - 2);
						printf("%2s", "");
					}
					else if (treaTime_Now - treaTime_Last < 12.1F * CLOCKS_PER_SEC)
					{
						SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
						gotoxy(LEFT + 16, BUTTON - 2);
						printf("%2s", "");
					}
					else if (treaTime_Now - treaTime_Last < 13.1F * CLOCKS_PER_SEC)
					{
						SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
						gotoxy(LEFT + 15, BUTTON - 2);
						printf("%2s", "");
					}
					else if (treaTime_Now - treaTime_Last < 14.1F * CLOCKS_PER_SEC)
					{
						SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
						gotoxy(LEFT + 14, BUTTON - 2);
						printf("%2s", "");
					}
					else if (treaTime_Now - treaTime_Last < 15.1F * CLOCKS_PER_SEC)
					{
						SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
						gotoxy(LEFT + 13, BUTTON - 2);
						printf("%2s", "");
					}
					else if (treaTime_Now - treaTime_Last < 16.1F * CLOCKS_PER_SEC)
					{
						SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
						gotoxy(LEFT + 12, BUTTON - 2);
						printf("%2s", "");
					}
					else if (treaTime_Now - treaTime_Last < 17.1F * CLOCKS_PER_SEC)
					{
						SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
						gotoxy(LEFT + 11, BUTTON - 2);
						printf("%2s", "");
					}
					else if (treaTime_Now - treaTime_Last < 18.1F * CLOCKS_PER_SEC)
					{
						SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
						gotoxy(LEFT + 10, BUTTON - 2);
						printf("%2s", "");
					}
					else if (treaTime_Now - treaTime_Last < 19.1F * CLOCKS_PER_SEC)
					{
						SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
						gotoxy(LEFT + 9, BUTTON - 2);
						printf("%2s", "");
					}
					else if (treaTime_Now - treaTime_Last < 20.1F * CLOCKS_PER_SEC)
					{
						SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
						gotoxy(LEFT + 8, BUTTON - 2);
						printf("%2s", "");
					}
					else if (treaTime_Now - treaTime_Last < 21.1F * CLOCKS_PER_SEC)
					{
						SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
						gotoxy(LEFT + 2, BUTTON - 2);
						printf("%50s", "");
						time_Treasure();
						getTreasure = 0;
					}
				}
			}
			if (checkComplete())
			{
				return;
			}
		}

		if (live)
		{
			if ((ch = _getch()) == 32)
				stop = 0;
		}
	}
}

//==================================================
//��ʼ����Ϸ�еĸ���ֵ
void initGame(enum mode game_Mode)
{
	int i, j;

	for (int i = LEFT+1; i < RIGHT; i++)
	{
		for (j = TOP + 1; j < BUTTON; j++)
		{
			potVal[i][j] = 0;
		}
	}

	SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
	system("cls");
	SetConsoleTextAttribute(g_hConsoleOutput, 0xF0);

	for ( i = LEFT; i <= RIGHT; i++)
	{
		gotoxy(i, TOP);
		printf("%2s", "");
		potVal[i][TOP] = 4;
	}

	for ( i = TOP; i < BUTTON + 3; i++)
	{
		gotoxy(LEFT, i);
		printf("%2s", "");
		potVal[LEFT][i] = 4;

		gotoxy(RIGHT, i);
		printf("%2s", "");
		potVal[RIGHT][i] = 4;
	}

	SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);

	//���Ʊ߽�
	for ( i = TOP+1; i < BUTTON + 3; i++)
	{
		gotoxy(LEFT + 1, i);
		printf("%2s", "");

		gotoxy(RIGHT - 1, i);
		printf("%2s", "");
	}

	switch (game_Mode)
	{
	case run_Game:
		live = 1;
		stop = 0;
		intGrade = 0;
		speed = 1000;
		intTreasure = 0;		//��ʯ��
		getTreasure = 0;		//��ñ�ʯ

		Ball[0].mode = 0;
		Ball[0].x_speed = 1;
		Ball[0].y_speed = 1;
		Bord.length = 3;

		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		gotoxy(RIGHT + 2, 2);
		printf("�ɼ�: 0");

		SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
		gotoxy(RIGHT + 2, 4);
		printf("===��Ʒ���===");

		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		gotoxy(RIGHT + 2, 6);
		printf("��:һ��ש��");

		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		gotoxy(RIGHT + 2, 8);
		printf("��:����ש��");

		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED | FOREGROUND_INTENSITY);
		gotoxy(RIGHT + 2, 10);
		printf("��:����ש��");

		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		gotoxy(RIGHT + 2, 12);
		printf("��:�Ͻ�[��͸]");

		SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
		gotoxy(RIGHT + 2, 14);
		printf("��:��˪[����]");

		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
		gotoxy(RIGHT + 2, 16);
		printf("��:̰��[�쳤]");

		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED | FOREGROUND_INTENSITY);
		gotoxy(RIGHT + 2, 18);
		printf("��:��ħ[����]");

		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		gotoxy(RIGHT + 2, 20);
		printf("��:��֮[����]");

		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		gotoxy(RIGHT + 2, 22);
		printf("��:���[Crazy]");

		break;
	case creat_Map:
		SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
		gotoxy(RIGHT + 2, 2);
		printf("������ʾ��");

		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
		gotoxy(RIGHT + 2, 4);
		printf("<0>-->ȡ����ǰ");

		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		gotoxy(RIGHT + 2, 6);
		printf("<1>-->��");

		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		gotoxy(RIGHT + 2, 8);
		printf("<2>-->��");

		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED | FOREGROUND_INTENSITY);
		gotoxy(RIGHT + 2, 10);
		printf("<3>-->��");

		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		gotoxy(RIGHT + 2, 15);
		printf("<ENTER>��ɹ���");

		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED | FOREGROUND_INTENSITY);
		gotoxy(RIGHT + 2, 17);
		printf("<ESC>  ȡ������");
		break;
	}
}

void PrintLev()
{
	int i, j;

	FILE* fp;							//��ȡ�ؿ��ļ���ָ��
	short file_temp;					//��ʱ�����Ĺؿ��ļ�
	char url_Map[30] = "maps//";		//��ͼ·��

	strcat_s(url_Map, sizeof(url_Map), filename[level_Now]);
	strcat_s(url_Map, sizeof(url_Map), ".map");
	if (fopen_s(&fp, url_Map, "rb"))
	{
		printf("�ؿ��ļ������ڻ��ܴ򿪹ؿ��ļ���");
		exit(0);
	}

	gotoxy(LEFT + 2, TOP + 2);

	for ( i = TOP+1; i < BUTTON-9; i++)
	{
		for (j = LEFT + 1; j < RIGHT; j++)
		{
			gotoxy(j, i);

			fread(&file_temp, sizeof(short), 1, fp);

			switch (file_temp)
			{
			case 1:
				SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				potVal[j][i] = 1;
				printf("��");
				break;
			case 2:
				SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				potVal[j][i] = 2;
				printf("��");
				break;
			case 3:
				SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED | FOREGROUND_INTENSITY);
				potVal[j][i] = 3;
				printf("��");
				break;
			default:
				potVal[j][i] = 0;
				break;
			}
			
			Sleep(10);
		}
	}

	fclose(fp);				//��ƨ��

	PrintBord();			//����ľ��
}

//======================================================
//����ľ���С��
void PrintBord()
{
	//����С��
	SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
	Ball[0].x = 19;
	Ball[0].y = 18;
	gotoxy(Ball[0].x, Ball[0].y);
	printf("%s", BALL_SHAPE[0]);

	//����ľ��
	SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	Bord.x = 18;
	Bord.y = 19;
	gotoxy(Bord.x, Bord.y);
	potVal[Bord.x][Bord.y]=potVal[Bord.x+1][Bord.y]= potVal[Bord.x + 2][Bord.y] = 4;
	printf("������");
}

//====================================================
//�ƶ�ľ��
void MoveBord(int orientation)
{
	if (orientation == 1)
	{
		if (Bord.length == 1)
		{
			if (Bord.x > LEFT +1)
			{
				Bord.x = Bord.x - 1;
				SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				gotoxy(Bord.x, Bord.y);
				printf("��  ");
				potVal[Bord.x][Bord.y] = 4;
				potVal[Bord.x][Bord.y] = 0;
			}
		}
		else if (Bord.length == 3)
		{
			if (Bord.x > LEFT + 1)
			{
				Bord.x = Bord.x - 1;
				SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				gotoxy(Bord.x, Bord.y);
				printf("������  ");
				potVal[Bord.x][Bord.y] = 4;
				potVal[Bord.x + 3][Bord.y] = 0;
			}
		}
		else if (Bord.length == 5)
		{
			if (Bord.x > LEFT + 1)
			{
				Bord.x = Bord.x - 1;
				SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				gotoxy(Bord.x, Bord.y);
				printf("����������  ");
				potVal[Bord.x][Bord.y] = 4;
				potVal[Bord.x + 5][Bord.y] = 0;
			}
		}
	}
	else
	{
		if (Bord.length == 1)
		{
			if ((Bord.x + 1) < RIGHT)
			{
				SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				gotoxy(Bord.x, Bord.y);
				printf("  ��");
				Bord.x = Bord.x + 1;
				potVal[Bord.x][Bord.y] = 4;
				potVal[Bord.x - 1][Bord.y] = 0;
			}
		}
		else if (Bord.length == 3)
		{
			if ((Bord.x + 3) < RIGHT)
			{
				SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				gotoxy(Bord.x, Bord.y);
				printf("  ������");
				Bord.x = Bord.x + 1;
				potVal[Bord.x + 2][Bord.y] = 4;
				potVal[Bord.x - 1][Bord.y] = 0;
			}
		}
		else if (Bord.length == 5)
		{
			if ((Bord.x + 5) < RIGHT)
			{
				SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				gotoxy(Bord.x, Bord.y);
				printf("  ����������");
				Bord.x = Bord.x + 1;
				potVal[Bord.x + 4][Bord.y] = 4;
				potVal[Bord.x - 1][Bord.y] = 0;
			}
		}
	}
}

//======================================================
//�ƶ�С��
void MoveBall(int index)
{
	//����
	if (Ball[index].x_speed == 1 && Ball[index].y_speed == 1)
	{
		if (potVal[Ball[index].x][Ball[index].y - 1] == 4)
		{
			Ball[index].y_speed = -1;
			PlaySound("music\\knock_1.wav", NULL, SND_FILENAME | SND_ASYNC);
		}
		else if (potVal[Ball[index].x + 1][Ball[index].y] == 4)
		{
			Ball[index].x_speed = -1;
			PlaySound("music\\knock_1.wav", NULL, SND_FILENAME | SND_ASYNC);
		}
		else if (potVal[Ball[index].x][Ball[index].y - 1] != 0)
		{
			removeDiamond(index, 1);
		}
		else if (potVal[Ball[index].x + 1][Ball[index].y] != 0)
		{
			removeDiamond(index, 3);
		}
		else if (potVal[Ball[index].x + 1][Ball[index].y - 1] != 0)
		{
			removeDiamond(index, 2);
		}
		else
		{
			if (Ball[index].mode == 0)
			{
				SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
				gotoxy(Ball[index].x, Ball[index].y);
				printf("%2s", "");
				Ball[index].x = Ball[index].x + 1;
				Ball[index].y = Ball[index].y - 1;
				gotoxy(Ball[index].x, Ball[index].y);
				printf("%s", BALL_SHAPE[0]);
			}
			else if (Ball[index].mode == 1)
			{

				gotoxy(Ball[index].x, Ball[index].y);
				if (potVal[Ball[index].x][Ball[index].y] == 1)
				{
					SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
					printf("��");
				}
				else if (potVal[Ball[index].x][Ball[index].y] == 0)
				{
					SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
					printf("%2s", "");
				}

				Ball[index].x = Ball[index].x + 1;
				Ball[index].y = Ball[index].y - 1;

				SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
				gotoxy(Ball[index].x, Ball[index].y);
				printf("%s", BALL_SHAPE[1]);
			}
			else
			{
				SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
				gotoxy(Ball[index].x, Ball[index].y);
				printf("%2s", "");
				Ball[index].x = Ball[index].x + 1;
				Ball[index].y = Ball[index].y - 1;
				gotoxy(Ball[index].x, Ball[index].y);
				printf("%s", BALL_SHAPE[2]);
			}
		}
	}

	//����
	if (Ball[index].x_speed == 1 && Ball[index].y_speed == -1)
	{

		if (potVal[Ball[index].x][Ball[index].y + 1] == 4)
		{
			Ball[index].y_speed = 1;
			PlaySound("music\\knock_1.wav", NULL, SND_FILENAME | SND_ASYNC);
		}
		else if (potVal[Ball[index].x + 1][Ball[index].y] == 4)
		{
			Ball[index].x_speed = -1;
			PlaySound("music\\knock_1.wav", NULL, SND_FILENAME | SND_ASYNC);
		}
		else if (potVal[Ball[index].x + 1][Ball[index].y + 1] == 4)
		{
			Ball[index].x_speed = -1;
			Ball[index].y_speed = 1;
			PlaySound("music\\knock_1.wav", NULL, SND_FILENAME | SND_ASYNC);
		}
		else if (potVal[Ball[index].x + 1][Ball[index].y] != 0)
		{
			removeDiamond(index, 3);
		}
		else if (potVal[Ball[index].x][Ball[index].y + 1] != 0)
		{
			removeDiamond(index, 5);
		}
		else if (potVal[Ball[index].x + 1][Ball[index].y + 1] != 0)
		{
			removeDiamond(index, 4);
		}
		else
		{
			if (Ball[index].mode == 0)
			{
				SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
				gotoxy(Ball[index].x, Ball[index].y);
				printf("%2s", "");
				Ball[index].x = Ball[index].x + 1;
				Ball[index].y = Ball[index].y + 1;
				gotoxy(Ball[index].x, Ball[index].y);
				printf("%s", BALL_SHAPE[0]);
			}
			else if (Ball[index].mode == 1)
			{

				gotoxy(Ball[index].x, Ball[index].y);
				if (potVal[Ball[index].x][Ball[index].y] == 1)
				{
					SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
					printf("��");
				}
				else if (potVal[Ball[index].x][Ball[index].y] == 0)
				{
					SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
					printf("%2s", "");
				}

				Ball[index].x = Ball[index].x + 1;
				Ball[index].y = Ball[index].y + 1;

				SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
				gotoxy(Ball[index].x, Ball[index].y);
				printf("%s", BALL_SHAPE[1]);
			}
			else
			{
				SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
				gotoxy(Ball[index].x, Ball[index].y);
				printf("%2s", "");
				Ball[index].x = Ball[index].x + 1;
				Ball[index].y = Ball[index].y + 1;
				gotoxy(Ball[index].x, Ball[index].y);
				printf("%s", BALL_SHAPE[2]);
			}
		}
	}

	//����
	if (Ball[index].x_speed == -1 && Ball[index].y_speed == -1)
	{
		if (potVal[Ball[index].x][Ball[index].y + 1] == 4)
		{
			Ball[index].y_speed = 1;
			PlaySound("music\\knock_1.wav", NULL, SND_FILENAME | SND_ASYNC);
		}
		else if (potVal[Ball[index].x - 1][Ball[index].y] == 4)
		{
			Ball[index].x_speed = 1;
			PlaySound("music\\knock_1.wav", NULL, SND_FILENAME | SND_ASYNC);
		}
		else if (potVal[Ball[index].x - 1][Ball[index].y + 1] == 4)
		{
			Ball[index].x_speed = 1;
			Ball[index].y_speed = 1;
			PlaySound("music\\knock_1.wav", NULL, SND_FILENAME | SND_ASYNC);
		}
		else if (potVal[Ball[index].x][Ball[index].y + 1] != 0)
		{
			removeDiamond(index, 5);
		}
		else if (potVal[Ball[index].x - 1][Ball[index].y] != 0)
		{
			removeDiamond(index, 7);
		}
		else if (potVal[Ball[index].x - 1][Ball[index].y + 1] != 0)
		{
			removeDiamond(index, 6);
		}
		else
		{
			if (Ball[index].mode == 0)
			{
				SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
				gotoxy(Ball[index].x, Ball[index].y);
				printf("%2s", "");
				Ball[index].x = Ball[index].x - 1;
				Ball[index].y = Ball[index].y + 1;
				gotoxy(Ball[index].x, Ball[index].y);
				printf("%s", BALL_SHAPE[0]);
			}
			else if (Ball[index].mode == 1)
			{

				gotoxy(Ball[index].x, Ball[index].y);
				if (potVal[Ball[index].x][Ball[index].y] == 1)
				{
					SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
					printf("��");
				}
				else if (potVal[Ball[index].x][Ball[index].y] == 0)
				{
					SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
					printf("%2s", "");
				}

				Ball[index].x = Ball[index].x - 1;
				Ball[index].y = Ball[index].y + 1;

				SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
				gotoxy(Ball[index].x, Ball[index].y);
				printf("%s", BALL_SHAPE[1]);
			}
			else
			{
				SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
				gotoxy(Ball[index].x, Ball[index].y);
				printf("%2s", "");
				Ball[index].x = Ball[index].x - 1;
				Ball[index].y = Ball[index].y + 1;
				gotoxy(Ball[index].x, Ball[index].y);
				printf("%s", BALL_SHAPE[2]);
			}
		}
	}

	//����
	if (Ball[index].x_speed == -1 && Ball[index].y_speed == 1)
	{
		if (potVal[Ball[index].x][Ball[index].y - 1] == 4)
		{
			Ball[index].y_speed = -1;
			PlaySound("music\\knock_1.wav", NULL, SND_FILENAME | SND_ASYNC);
		}
		else if (potVal[Ball[index].x - 1][Ball[index].y] == 4)
		{
			Ball[index].x_speed = 1;
			PlaySound("music\\knock_1.wav", NULL, SND_FILENAME | SND_ASYNC);
		}
		else if (potVal[Ball[index].x][Ball[index].y - 1] != 0)
		{
			removeDiamond(index, 1);
		}
		else if (potVal[Ball[index].x - 1][Ball[index].y] != 0)
		{
			removeDiamond(index, 7);
		}
		else if (potVal[Ball[index].x - 1][Ball[index].y - 1] != 0)
		{
			removeDiamond(index, 8);
		}
		else
		{
			if (Ball[index].mode == 0)
			{
				SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
				gotoxy(Ball[index].x, Ball[index].y);
				printf("%2s", "");
				Ball[index].x = Ball[index].x - 1;
				Ball[index].y = Ball[index].y - 1;
				gotoxy(Ball[index].x, Ball[index].y);
				printf("%s", BALL_SHAPE[0]);
			}
			else if (Ball[index].mode == 1)
			{

				gotoxy(Ball[index].x, Ball[index].y);
				if (potVal[Ball[index].x][Ball[index].y] == 1)
				{
					SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
					printf("��");
				}
				else if (potVal[Ball[index].x][Ball[index].y] == 0)
				{
					SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
					printf("%2s", "");
				}

				Ball[index].x = Ball[index].x - 1;
				Ball[index].y = Ball[index].y - 1;

				SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
				gotoxy(Ball[index].x, Ball[index].y);
				printf("%s", BALL_SHAPE[1]);
			}
			else
			{
				SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
				gotoxy(Ball[index].x, Ball[index].y);
				printf("%2s", "");
				Ball[index].x = Ball[index].x - 1;
				Ball[index].y = Ball[index].y - 1;
				gotoxy(Ball[index].x, Ball[index].y);
				printf("%s", BALL_SHAPE[2]);
			}
		}
	}

	if (Ball[index].y > BUTTON)
		live = 0;
}

//===========================================================================
//��������
void removeDiamond(int index, int direction)
{
	Point point_diamond;

	switch (direction)
	{
	case 1: //��

		if (Ball[index].mode != 1)
		{
			Ball[index].y_speed = -1;
		}

		point_diamond.x = Ball[index].x;
		point_diamond.y = Ball[index].y - 1;

		break;

	case 2: //����
		if (Ball[index].mode != 1)
		{
			Ball[index].x_speed = -1;
			Ball[index].y_speed = -1;
		}

		point_diamond.x = Ball[index].x + 1;
		point_diamond.y = Ball[index].y - 1;

		break;

	case 3: //��
		if (Ball[index].mode != 1)
		{
			Ball[index].x_speed = -1;
		}

		point_diamond.x = Ball[index].x + 1;
		point_diamond.y = Ball[index].y;

		break;

	case 4: //����
		if (Ball[index].mode != 1)
		{
			Ball[index].x_speed = -1;
			Ball[index].y_speed = 1;
		}

		point_diamond.x = Ball[index].x + 1;
		point_diamond.y = Ball[index].y + 1;

		break;

	case 5: //��
		if (Ball[index].mode != 1)
		{
			Ball[index].y_speed = 1;
		}

		point_diamond.x = Ball[index].x;
		point_diamond.y = Ball[index].y + 1;

		break;

	case 6: //����
		if (Ball[index].mode != 1)
		{
			Ball[index].x_speed = 1;
			Ball[index].y_speed = 1;
		}

		point_diamond.x = Ball[index].x - 1;
		point_diamond.y = Ball[index].y + 1;

		break;

	case 7: //��
		if (Ball[index].mode != 1)
		{
			Ball[index].x_speed = 1;
		}

		point_diamond.x = Ball[index].x - 1;
		point_diamond.y = Ball[index].y;

		break;

	case 8: //����
		if (Ball[index].mode != 1)
		{
			Ball[index].x_speed = 1;
			Ball[index].y_speed = -1;
		}

		point_diamond.x = Ball[index].x - 1;
		point_diamond.y = Ball[index].y - 1;

		break;
	}

	if (potVal[point_diamond.x][point_diamond.y] == 3)
	{
		removeColor(red, point_diamond);
	}
	else if (potVal[point_diamond.x][point_diamond.y] == 2)
	{
		removeColor(blue, point_diamond);
	}
	else if (potVal[point_diamond.x][point_diamond.y] == 1)
	{
		removeColor(green, point_diamond);
	}
}

//==========================================================================
//���������ɫ�ķ���
void removeColor(color color_diamond, Point point_diamond)
{
	PlaySound("music\\knock_2.wav", NULL, SND_FILENAME | SND_ASYNC);

	switch (color_diamond)
	{
	case green:
		SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
		gotoxy(point_diamond.x, point_diamond.y);
		potVal[point_diamond.x][point_diamond.y] = 0;
		printf("%2s", "");
		break;

	case blue:
		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		gotoxy(point_diamond.x, point_diamond.y);
		potVal[point_diamond.x][point_diamond.y] = 1;
		printf("��");
		break;

	case red:
		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		gotoxy(point_diamond.x, point_diamond.y);
		potVal[point_diamond.x][point_diamond.y] = 2;
		printf("��");
		break;
	}
	//�Ʒ�
	intGrade += 10;
	SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	gotoxy(RIGHT + 5, 2);
	printf("%d", intGrade);

	srand((int)time(NULL));
	if (rand() % 100 < 60 && intTreasure == 0 && getTreasure == 0)				//30%�ĸ��ʳ��ֱ���
	{
		showTreasure(rand() % 100, point_diamond);
	}
	
}


//===============================================================
//������ֱ���
void showTreasure(int intRand, Point point_diamond)
{
	int y = Bord.y;
	intTreasure = 1;

	if (intRand < 15)						//ʵ����
	{
		Treasure.type = 1;
		Treasure.x = point_diamond.x;
	}
	else if (intRand < 25)					//�����ٶȱ�����
	{
		Treasure.type = 2;
		Treasure.x = point_diamond.x;
	}
	else if (intRand < 40)					//��ӳ�
	{
		Treasure.type = 3;
		Treasure.x = point_diamond.x;	
	}
	else if (intRand < 65)					//������
	{
		Treasure.type = 4;
		Treasure.x = point_diamond.x;
	}
	else if (intRand < 75)					//�ٶȱ��
	{
		Treasure.type = 5;
		Treasure.x = point_diamond.x;
	}
	else									//���
	{
		Treasure.type = 6;
		Treasure.x = point_diamond.x;
	}

	while (potVal[point_diamond.x][y - 1] == 0 && y > point_diamond.y)
	{
		y--;
	}
	Treasure.y = y;
}

//=====================================================================
//����������
void move_Treasure()
{
	gotoxy(Treasure.x, Treasure.y);
	printf("%2s", "");

	if (Treasure.y+1 == Bord.y && potVal[Treasure.x][Treasure.y+1] == 4)
	{
		get_Treasure();
		intTreasure = 0;
		return;
	}
	else if (Treasure.y + 1 < BUTTON)
	{
		Treasure.y++;
	}
	else
	{
		intTreasure = 0;
		return;
	}

	gotoxy(Treasure.x, Treasure.y);

	switch (Treasure.type)
	{
	case 1: //��͸
		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		printf("��");
		break;

	case 2: //����
		SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
		printf("��");
		break;

	case 3: //�쳤
		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
		printf("��");
		break;

	case 4: //����
		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED | FOREGROUND_INTENSITY);
		printf("��");
		break;

	case 5: //����
		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		printf("��");
		break;

	case 6: //���
		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		printf("��");
		break;
	}

}

//============================================================================
//��ñ�ʯ
void get_Treasure()
{
	int i;

	switch (Treasure.type)
	{
	case 1:
		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		gotoxy(LEFT + 2, BUTTON - 2);
		printf("��");
		SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
		gotoxy(LEFT + 4, BUTTON - 2);
		printf("Time: ����������������������������������������");

		speed = 1000;
		Ball[0].mode = 1;

		break;

	case 2: //����

		SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
		gotoxy(LEFT + 2, BUTTON - 2);
		printf("��");
		SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
		gotoxy(LEFT + 4, BUTTON - 2);
		printf("Time: ����������������������������������������");

		speed = 1600;
		Ball[0].mode = 2;

		break;

	case 3:					//�쳤
		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
		gotoxy(LEFT + 2, BUTTON - 2);
		printf("��");
		SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
		gotoxy(LEFT + 4, BUTTON - 2);
		printf("Time: ����������������������������������������");

		Bord.length = 5;
		potVal[Bord.x][Bord.y] = potVal[Bord.x + 1][Bord.y] = potVal[Bord.x + 2][Bord.y] = 0;

		if (Bord.x + 5 >= RIGHT)					//��ֹľ�峬���ұ߽�
		{
			Bord.x = RIGHT - 5;
		}
		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		gotoxy(Bord.x, Bord.y);
		printf("����������");
		potVal[Bord.x][Bord.y] = potVal[Bord.x + 1][Bord.y] = potVal[Bord.x + 2][Bord.y] = potVal[Bord.x + 3][Bord.y] = potVal[Bord.x + 4][Bord.y] = 4;

		break;

	case 4:											//����
		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED | FOREGROUND_INTENSITY);
		gotoxy(LEFT + 2, BUTTON - 2);
		printf("��");
		SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
		gotoxy(LEFT + 4, BUTTON - 2);
		printf("Time: ����������������������������������������");

		Bord.length = 1;
		potVal[Bord.x + 1][Bord.y] = potVal[Bord.x + 2][Bord.y] = 0;

		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		gotoxy(Bord.x, Bord.y);
		printf("��%4s", "");
		potVal[Bord.x][Bord.y] = 4;

		break;

	case 5: //����

		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		gotoxy(LEFT + 2, BUTTON - 2);
		printf("��");
		SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
		gotoxy(LEFT + 4, BUTTON - 2);
		printf("Time: ����������������������������������������");

		speed = 500;
		break;

	case 6:			//���
		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		gotoxy(LEFT + 2, BUTTON - 2);
		printf("��");
		SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
		gotoxy(LEFT + 4, BUTTON - 2);
		printf("Time: ����������������������������������������");

		speed = 200;

		for ( i = LEFT+1; i < RIGHT-1; i++)
		{
			SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
			gotoxy(i, BUTTON - 4);
			printf("��");
			potVal[i][BUTTON - 4] = 4;
			Sleep(10);
		}

		break;
	}

	getTreasure = 1;			//��ñ�ʯ
	treaTime_Last = clock();
}

//=================================================================
//��ʯ����ʱ��
void time_Treasure()
{
	int i;

	switch (Treasure.type)
	{
	case 1:
		Ball[0].mode = 0;
		break;

	case 2:
		speed = 1000;
		Ball[0].mode = 0;
		break;

	case 3:
		Bord.length = 3;
		potVal[Bord.x + 3][Bord.y] = potVal[Bord.x + 4][Bord.y] = 0;

		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		gotoxy(Bord.x, Bord.y);
		printf("������%4s", "");
		break;

	case 4:

		Bord.length = 3;
		if (Bord.x + 3 >= RIGHT)
		{
			Bord.x = RIGHT - 3;
		}
		potVal[Bord.x][Bord.y] = potVal[Bord.x + 1][Bord.y] = potVal[Bord.x + 2][Bord.y] = 4;

		SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		gotoxy(Bord.x, Bord.y);
		printf("������");

		break;

	case 5:
		speed = 1000;
		break;

	case 6:
		speed = 1000;
		for ( i = LEFT+1; i < RIGHT-1; i++)
		{
			SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
			gotoxy(i, BUTTON - 4);
			printf("%2s", "");
			potVal[i][BUTTON - 4] = 0;
			Sleep(10);
		}

		break;
	}
}

//==================================================================
//�����Ϸ�Ƿ����
int checkComplete()
{
	int i, j;
	int intSurplus = 0;					//ʣ��

	for ( i = LEFT+1; i < RIGHT; i++)
	{
		for (j = TOP + 1; j < Bord.y; j++)
		{
			if (potVal[i][j] != 0)
			{
				intSurplus++;
			}
			if (intSurplus > 3)
			{
				return 0;
			}
		}
	}

	return 1;
}

//==========================================================================
//�����ؿ�
void creatMap()
{
	int i, j;
	char ch;
	Point pot_map;
	char map_Name[20] = "map//";
	char temp_Name[20];

	FILE* fp,* fmap;

	CONSOLE_CURSOR_INFO cursoInfo = { 1,TRUE };
	SetConsoleCursorInfo(g_hConsoleOutput, &cursoInfo);

	initGame(creat_Map);

	pot_map.x = LEFT + 1;
	pot_map.y = TOP + 1;
	gotoxy(pot_map.x, pot_map.y);

	do
	{
		switch (ch = _getch())
		{
		case 'w':
		case 'W':
		case 72:		//��
			if (pot_map.y > TOP + 1)
			{
				pot_map.y -= 1;
				gotoxy(pot_map.x, pot_map.y);
			}
			break;

		case 's':
		case 'S':
		case 80:		//��
			if (pot_map.y < BUTTON - 10)
			{
				pot_map.y += 1;
				gotoxy(pot_map.x, pot_map.y);
			}
			break;

		case 'a':
		case 'A':
		case 75:			//��
			if (pot_map.x > LEFT + 1)
			{
				pot_map.x -= 1;
				gotoxy(pot_map.x, pot_map.y);
			}
			break;

		case 'd':
		case 'D':
		case 77:			//��
			if (pot_map.x < RIGHT - 1)
			{
				pot_map.x += 1;
				gotoxy(pot_map.x, pot_map.y);
			}
			break;

		case '0':
			SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
			gotoxy(pot_map.x, pot_map.y);
			printf("%2s", "");
			potVal[pot_map.x][pot_map.y] = 0;
			break;

		case '1':
			SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			gotoxy(pot_map.x, pot_map.y);
			printf("��");
			potVal[pot_map.x][pot_map.y] = 1;
			break;

		case '2':
			SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			gotoxy(pot_map.x, pot_map.y);
			printf("��");
			potVal[pot_map.x][pot_map.y] = 2;
			break;

		case '3':
			SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED | FOREGROUND_INTENSITY);
			gotoxy(pot_map.x, pot_map.y);
			printf("��");
			potVal[pot_map.x][pot_map.y] = 3;
			break;

		case 32: //�ո�
			if (potVal[pot_map.x][pot_map.y] == 0)
			{
				SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				gotoxy(pot_map.x, pot_map.y);
				printf("��");
				potVal[pot_map.x][pot_map.y] = 1;
			}
			else if (potVal[pot_map.x][pot_map.y] == 1)
			{
				SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				gotoxy(pot_map.x, pot_map.y);
				printf("��");
				potVal[pot_map.x][pot_map.y] = 2;
			}
			else if (potVal[pot_map.x][pot_map.y] == 2)
			{
				SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED | FOREGROUND_INTENSITY);
				gotoxy(pot_map.x, pot_map.y);
				printf("��");
				potVal[pot_map.x][pot_map.y] = 3;
			}
			else if (potVal[pot_map.x][pot_map.y] == 3)
			{
				SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
				gotoxy(pot_map.x, pot_map.y);
				printf("%2s", "");
				potVal[pot_map.x][pot_map.y] = 0;
			}
			break;

		case 27:
			return;
			break;
		}

		if (ch == 13)
		{
			gotoxy(LEFT + 2, BUTTON - 5);
			printf("��ȷ���㱣������� <��ENTER��ȷ�ϣ�������������༭");
			_getch();
			if (ch == 13)
			{
				gotoxy(LEFT + 5, BUTTON - 3);
				printf("�����İ��ĵ�ͼȡһ�����������ְɣ�--> ");
				scanf_s("%s", temp_Name, sizeof(temp_Name));
				gotoxy(LEFT + 5, BUTTON - 2);
				printf("���ڱ���...");
				Sleep(100);
				break;
			}
			else
			{
				gotoxy(LEFT + 2, BUTTON - 5);
				printf("%53s", "");
			}
		}

		gotoxy(pot_map.x, pot_map.y);

	} while (1);

	strcat_s(map_Name, sizeof(map_Name), temp_Name);
	strcat_s(map_Name, sizeof(map_Name), ".map");

	if (fopen_s(&fp,map_Name,"wb")||fopen_s(&fmap,"maps\\MapInfo.dat","a"))
	{
		gotoxy(LEFT + 12, BUTTON - 3);
		printf("����ʧ��!");
		Sleep(1000);
		exit(0);
	}

	for ( i = TOP+1; i < BUTTON-9; i++)
	{
		for (j = LEFT + 1; j < RIGHT; j++)
		{
			fwrite(&potVal[j][i], sizeof(short), 1, fp);
		}
	}

	strcat_s(temp_Name, sizeof(temp_Name), "\n");
	fputs(temp_Name, fmap);

	fclose(fmap);
	fclose(fp);

	gotoxy(LEFT + 12, BUTTON - 2);
	printf("����ɹ���");
	Sleep(1000);
}

//====================================================================
//��Ϸ����
void helpGame()
{
	SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
	system("cls");

	SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
	gotoxy(LEFT, 2);
	printf("====================��Ʒ���=====================");

	SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	gotoxy(LEFT, 4);
	printf("��:һ��ש�飺���Ժ����׵İ�������");

	SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	gotoxy(LEFT, 6);
	printf("��:����ש�飺������Ҫһ���������㶨��");

	SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED | FOREGROUND_INTENSITY);
	gotoxy(LEFT, 8);
	printf("��:����ש�飺��һ��Ӳ�ģ����С��Ӧ��");

	SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	gotoxy(LEFT, 10);
	printf("��:�Ͻ�ʯ��������ʹС��������������ֱײ!");

	SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
	gotoxy(LEFT, 12);
	printf("��:��˪��ʯ��С�򱻱�˪���֣��ٶ�Ҳ����֮����~");

	SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
	gotoxy(LEFT, 14);
	printf("��:̰����ʯ�����ľ�彫�����������Ϣ������������أ�");

	SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED | FOREGROUND_INTENSITY);
	gotoxy(LEFT, 16);
	printf("��:��ħ��ʯ�����ľ��ᱻ��ħ�Ե�һ���֣�����㶮��~");

	SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	gotoxy(LEFT, 18);
	printf("��:��֮��ʯ����֮��������С���������ƶ��ٶ�~");

	SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	gotoxy(LEFT, 20);
	printf("��:���ʯ�����������ǹۿ����ݵ�ʱ����!");

	system("pause>nul");						//pause ������ͣ����  ����ʾ���밴���������. . .����Щ��

}

//================================================================
//ѡ���ͼ
void selectMap()
{
	FILE *fMap;
	char ch;

	level_Max = 0;

	SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
	system("cls");

	if (fopen_s(&fMap,"maps\\MapInfo.dat","r"))
	{
		gotoxy(LEFT + 4, TOP + 4);
		printf("�򿪵�ͼ��Ϣ�ļ�����");
		gotoxy(LEFT + 4, TOP + 6);
		printf("��鿴MapInfo.dat�ļ��Ƿ���ڣ�����");
		exit(0);
	}

	while (fgets(filename[level_Max], 20, fMap) != NULL)
	{
		if (filename[level_Max][strlen(filename[level_Max]) - 1] == '\n')
		{
			filename[level_Max][strlen(filename[level_Max]) - 1] = 0;
		}
		level_Max++;
	}
	fclose(fMap);

	SetConsoleTextAttribute(g_hConsoleOutput, 0x0F);
	for ( level_Now = 0; level_Now < level_Max; level_Now++)
	{
		gotoxy(LEFT + 8, TOP + 2 * level_Now);
		printf("%s", filename[level_Now]);
	}
}