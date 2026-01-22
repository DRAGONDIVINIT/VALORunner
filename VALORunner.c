#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <string.h>
// 一堆头文件

#define ROWS 16 // 定义地图行数
#define COLS 3  // 定义地图列数

char M[ROWS][COLS]; // 定义地图
char player;        // 定义玩家
int playerCol;      // 定义玩家所在列
int score;          // 定义学分
int jidian;         // 定义绩点
int delay;          // 定义初始速度
int RMB;            // 定义抽卡货币（学分累计）
int status;         // 定义状态
int gameover;       // 定义判断游戏结束常量
int char1;          // 判断猎枭是否解锁
int char2;          // 判断火男是否解锁
int chara;          // 判断游戏角色
int baodi;          // 判断你是不是保底
int energy;         // 定义技能充能
int DIS;            // 碰撞判断开关

void HideCursor() // 前人的隐藏光标函数
{
    CONSOLE_CURSOR_INFO cursor_info = {1, 0};
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

void gotoxy(int x, int y) // 覆盖清屏
{
    COORD pos = {x, y};
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hOut, pos);
    return;
}

void clearscreen() // 前人的清屏函数
{
    COORD topLeft = {0, 0};
    DWORD screenSize;
    CONSOLE_SCREEN_BUFFER_INFO screenInfo;

    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(console, &screenInfo);
    screenSize = screenInfo.dwSize.X * screenInfo.dwSize.Y;

    FillConsoleOutputCharacter(console, ' ', screenSize, topLeft, &screenSize);
    gotoxy(0, 0);
}

int getsecond() // 前人的时间获取函数
{
    struct tm *ptr;
    time_t lt;
    lt = time(NULL);
    ptr = localtime(&lt);
    return (int)ptr->tm_sec;
}

void initializeM() // 初始化
{
    clearscreen();
    HideCursor();
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            M[i][j] = ' ';
        }
    }
    player = 'O';
    playerCol = COLS / 2;
    M[ROWS - 1][playerCol] = player;
    gameover = 0;
    score = 0;
    status = 1;
    jidian = 0;
    delay = 500;
    energy = 0;
    DIS = 0;
}

void printM() // 打印地图
{
    HideCursor();
    clearscreen();
    M[ROWS - 1][playerCol] = player;
    if (status > 1)
    {
        player = '^'; // 跳跃时小人变为'^'
    }
    else if (status < 1)
    {
        player = 'v'; // 滑铲时小人变为'v'
    }
    else
    {
        player = 'O';
    }
    M[ROWS - 1][playerCol] = player;
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            printf("| %c ", M[i][j]);
        }
        printf("|\n");
    }
    gotoxy(0, ROWS + 1);
    printf("-------------\n");
    printf("学分:%d\n", score);
    printf("绩点:%d\n", jidian);
    if (status == 1)
    {
        printf("玩家状态 : 正常\n");
    }
    else if (status > 1)
    {
        printf("玩家状态 : 跳跃\n");
    }
    else if (status < 1)
    {
        printf("玩家状态 : 滑铲\n");
    }
    if (chara == 1)
    {
        printf("我，是猎人！充能：%d\n", energy);
    }
    else if (chara == 2)
    {
        printf("来吧，走起！充能：%d\n", energy);
    }
    else
    {
        printf("你是白板\n");
    }
}

void generate() // 生成障碍、道具
{
    if (status != 1)
    {
        status = 1;
    }
    srand(time(NULL) + rand() * getsecond());
    for (int i = 0; i < 3; i += 1)
    {
        int temp1 = getsecond() * rand() % 1000;
        if (temp1 <= 120)
        {
            M[0][i] = 'Y'; // 生成学分
        }
        else if (temp1 <= 200)
        {
            M[0][i] = 'X'; // 生成不可通行障碍
        }
        else if (temp1 <= 400)
        {
            M[0][i] = 'D'; // 生成跳跃障碍
        }
        else if (temp1 <= 600)
        {
            M[0][i] = 'U'; // 生成滑铲障碍
        }
        else if (temp1 <= 650)
        {
            M[0][i] = '@'; // 生成蚂蚁
        }
        else
        {
            M[0][i] = ' ';
        }
    }

    if (M[0][0] == 'X' && M[0][1] == 'X' && M[0][2] == 'X') // 从舍友那学习的简单粗暴防止死路方法
    {
        M[0][0] = ' ';
        M[0][1] = ' ';
        M[0][2] = ' ';
    }
    if (M[1][0] != ' ')
    {
        M[0][0] = ' ';
    }
    if (M[1][1] != ' ')
    {
        M[0][1] = ' ';
    }
    if (M[1][2] != ' ')
    {
        M[0][2] = ' ';
    }
}

void moveM() // 生成物移动
{
    for (int i = ROWS - 1; i > 0; i--)
    {
        for (int j = 0; j < COLS; j++)
        {
            M[i][j] = M[i - 1][j];
        }
    }
}

void moveP() // 玩家移动
{
    char x;
    while (kbhit())
    {
        x = getch();
        if (x == 'a' && playerCol > 0)
            playerCol -= 1;
        else if (x == 'd' && playerCol < 2)
            playerCol += 1;
        else if (x == 'w')
            status = 2;
        else if (x == 's')
            status = 0;
        else if (x == ' ')
            system("pause");
        else if (x == 'c' && energy == 50)
        {
            energy -= 50;
            if (chara == 1)
            {
                int temp7 = 0;
                for (int i = 0; i < ROWS - 1; i++)
                {
                    if (M[i][playerCol] != ' ')
                    {
                        M[i][playerCol] = ' ';
                        temp7++;
                    }
                }
                score += temp7;
            }
            else if (chara = 2)
            {
                DIS = 10;
            }
        }
        else
            x = '\0';
    }
}

void dis() // 碰撞判断
{
    if (M[ROWS - 1][playerCol] == ' ')
    {
        return;
    }
    else if (M[ROWS - 1][playerCol] == 'Y')
    {
        score++;
    }
    else if (M[ROWS - 1][playerCol] == 'X')
    {
        if (!DIS)
        {
            gameover = 1;
        }
        else
        {
            return;
        }
    }
    else if (M[ROWS - 1][playerCol] == 'D')
    {
        if (status < 1)
        {
            return;
        }
        else
        {
            if (DIS < 0)
            {
                gameover = 1;
            }
            else
            {
                return;
            }
        }
    }
    else if (M[ROWS - 1][playerCol] == 'U')
    {
        if (status > 1)
        {
            return;
        }
        else
        {
            if (DIS < 0)
            {
                gameover = 1;
            }
            else
            {
                return;
            }
        }
    }
    else if (M[ROWS - 1][playerCol] == '@')
    {
        int temp2 = 0;
        for (int i = 0; i < ROWS; i++)
        {
            for (int j = 0; j < COLS; j++)
            {
                if (M[i][j] == 'Y')
                {
                    temp2++;
                }
                if (M[i][j] != player)
                {
                    M[i][j] = ' ';
                }
            }
        }
        score += temp2;
    }
}

void speedchange() // 变速
{
    if (delay > 300)
    {
        delay--;
    }
    Sleep(delay);
}

int rule() // 规则
{
rule:
    clearscreen();
    printf("规则\n");
    printf("Loading......\n");
    Sleep(350);
    printf("基础设置：\n");
    printf("O表示你操作的角色\n");
    printf("a控制人物向左移动，d控制角色向右移动\n");
    printf("w使角色跳跃，角色会变为^，可通过U形障碍；s使角色滑铲，角色会变为v，可通过D形障碍\n");
    printf("随着分数的增高障碍物的移速线性提升\n");
    printf("按下space即可暂停\n");
    printf(" \n");
    Sleep(350);
    printf("道具设置:\n");
    printf("Y为学分,拾取它来赚满毕业学分\n");
    printf("@是蚂蚁老师，他会帮你一键清屏\n");
    printf(" \n");
    Sleep(350);
    printf("角色设置\n");
    printf("目前实装角色有猎枭和不死鸟\n");
    printf("角色技能每50绩点充能好一次，不可叠加\n");
    printf("角色技能按C释放\n");
    printf(" \n");
    Sleep(350);
    printf("开始游戏请输入start\n");
    printf("结束游戏请输入end\n");
    char temp3[10005];
save2:
    scanf("%s", temp3);
    if (strcmp(temp3, "start") == 0)
    {
        return start();
    }
    else if (strcmp(temp3, "end") == 0)
    {
        return 2;
    }
    else
    {
        printf("听不懂思密达\n");
        goto save2;
    }
}

int shop() // 商店
{
    clearscreen();
    printf("商店\n");
    printf("Loading...\n");
    Sleep(350);
    printf("当期商店：\n");
    printf("烈火与猎人\n");
    printf("猎枭 ⭐⭐⭐⭐⭐\n不死鸟 ⭐⭐⭐⭐⭐\n");
    Sleep(350);
    printf("猎枭技能：我，是猎人！\n");
    printf("释放时摧毁前排所有生成物，学分上升破坏的生成物的数量的数值\n");
    printf(" \n");
    printf("不死鸟技能：来吧，走起！\n");
    printf("释放后在十米内无敌，可无视障碍\n");
    printf(" \n");
    Sleep(350);
    printf("你现在攒下了%d学分\n", RMB);
    printf("100学分购买角色，要购买吗?\n");
    printf(" \n");
    printf("输入1购买猎枭；输入2购买不死鸟\n");
    printf("输入rule查看规则\n");
    printf("输入start开始游戏\n");
    printf("输入end结束游戏\n");
save4:
    char temp5[10005];
    scanf("%s", temp5);
    if (strcmp(temp5, "1") == 0)
    {
        if (RMB < 100)
        {
            Sleep(350);
            printf("你的学分不够，再去练练吧\n");
            printf("你现在还有%d学分\n", RMB);
            goto save4;
        }
        else
        {
            RMB -= 100;
            Sleep(350);
            printf("猎枭，参上。\n");
            printf("你现在还有%d学分\n", RMB);
            char1 = 1;
            goto save4;
        }
    }
    else if (strcmp(temp5, "2") == 0)
    {
        if (RMB < 100)
        {
            Sleep(350);
            printf("你的学分不够，再去练练吧\n");
            printf("你现在还有%d学分\n", RMB);
            goto save4;
        }
        else
        {
            RMB -= 100;
            Sleep(350);
            printf("不死鸟会帮你的！总之先当心眼睛！\n");
            printf("你现在还有%d学分\n", RMB);
            char2 = 1;
            goto save4;
        }
    }
    else if (strcmp(temp5, "rule") == 0)
    {
        return rule();
    }
    else if (strcmp(temp5, "start") == 0)
    {
        return start();
    }
    else if (strcmp(temp5, "end") == 0)
    {
        return 2;
    }
    else
    {
        Sleep(350);
        printf("听不懂思密达\n");
    }
    goto save4;
}

int start() // 角色选择
{
    clearscreen();
    printf("输入1选择猎枭\n");
    printf("输入2选择不死鸟\n");
    printf("输入3选择白板作者君\n");
save5:
    char temp6[10005];
    scanf("%s", temp6);
    if (strcmp(temp6, "1") == 0)
    {
        if (char1 != 1)
        {
            printf("少年你还没有这个角色呢，是没高校认证吗？\n");
            goto save5;
        }
        else
        {
            chara = 1;
            return 1;
        }
    }
    else if (strcmp(temp6, "2") == 0)
    {
        if (char2 != 1)
        {
            printf("少年你还没有这个角色呢，是没高校认证吗？\n");
            goto save5;
        }
        else
        {
            chara = 2;
            return 1;
        }
    }
    else if (strcmp(temp6, "3") == 0)
    {
        chara = 0;
        return 1;
    }
    else
    {
        printf("听不懂思密达\n");
        goto save5;
    }
}

int menu() // 主页面
{
    char1 = 0;
    char2 = 0;
    printf("向智科奔去v1.0.0\n");
    Sleep(350);
    printf("game made by 曾闻天 231880147\n");
    Sleep(350);
    printf("推荐循环播放夜驱游玩，毕竟作者是一直循环着夜驱写的代码\n");
    Sleep(350);
    printf("因为作者菜而且不想多练所以本游戏不支持大写键盘也不支持小键盘上下左右更不支持数字键盘控制\n");
    Sleep(350);
    printf(" \n");
    printf("输入start以开始游戏\n");
    printf("输入rule以查看游戏规则\n");
    printf("输入shop以进入商店\n");
    printf("输入end以直接结束粪作旅途\n");
save1:
    char temp3[10005];
    scanf("%s", temp3);
    if (strcmp(temp3, "start") == 0)
    {
        return start();
    }
    else if (strcmp(temp3, "rule") == 0)
    {
        return rule();
    }
    else if (strcmp(temp3, "shop") == 0)
    {
        return shop();
    }
    else if (strcmp(temp3, "end") == 0)
    {
        return 2;
    }
    else
    {
        printf("听不懂思密达\n");
    }
    goto save1;
}

int remake() // 重开
{
    clearscreen();
    printf("GAME OVER!\n");
    printf("Loading...\n");
    Sleep(350);
    printf("您这局抢到的学分共计%d\n", score);
    Sleep(350);
    printf("您现在一共有%d学分\n", RMB);
    Sleep(350);
    printf("您的绩点为%d\n", jidian);
    Sleep(350);
    if (jidian <= 50)
    {
        printf("助教哥哥真是杂鱼呢~zakozako~\n");
    }
    else if (jidian <= 100)
    {
        printf("菜，就多练！\"\n");
    }
    else if (jidian <= 150)
    {
        printf("有点哈人了\n");
    }
    else if (jidian <= 200)
    {
        printf("您玩音游？\n");
    }
    else
    {
        printf("是什么让你玩这玩意这么上头的，原神的三个粉球吗？\n");
    }
    printf(" \n");
    Sleep(350);
    printf("再次开始游戏请输入remake\n");
    printf("结束游戏请输入end\n");
    printf("查看规则请输入rule\n");
    printf("进入商店请输入shop\n");
    char temp4[10005];
save3:
    scanf("%s", temp4);
    if (strcmp(temp4, "remake") == 0)
    {
        return start();
    }
    else if (strcmp(temp4, "end") == 0)
    {
        return 2;
    }
    else if (strcmp(temp4, "rule") == 0)
    {
        return rule();
    }
    else if (strcmp(temp4, "shop") == 0)
    {
        return shop();
    }
    else
    {
        printf("听不懂思密达\n");
        goto save3;
    }
}

void run() // 运行
{

    while (!gameover)
    {
        generate();
        moveP();
        gotoxy(0, 0);
        printM();
        moveM();
        speedchange();
        dis();
        jidian++;
        energy++;
        if (energy > 50)
        {
            energy = 50;
        }
        DIS--;
    }
    RMB += score;
}

int main()
{
    system("chcp 65001"); // 中文适配
    if (menu() == 2)
    {
        goto end;
    }
re:
    initializeM();
    run();
end:
    if (remake() == 2)
    {
        clearscreen();
        printf("求你捞捞我吧\n");
        printf("助教，我想写代码QAQ\n");
        printf("再输入一遍end狠心退出\n");
        char temp5[10005];
        scanf("%s", temp5);
        while (strcmp(temp5, "end") != 0)
        {
        }
    }
    else
    {
        goto re;
    }
    return 0;
}