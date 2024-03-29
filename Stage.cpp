#include "Stage.h"

/************************************************
    @file Stage.cpp
    @author 윤낙원(Nakwon Yun), 김현민(Hyunmin Kim) 
    @version 1.0 06/26/2020
************************************************/
Stage::Stage()
{
    srand((unsigned) time(0));
    initscr();
    keypad(stdscr, TRUE);
    cbreak();
    noecho();

    start_color();
    if (has_colors() == FALSE)
    {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }
    if (init_color(COLOR_BLUE, 0, 0, 300) == ERR)
    {
        printw("Your terminal cannot change the color definitions\n");
        printw("press any key to continue...\n");
        getch();
        moveSnake();
    }
    init_pair(EMPTY, COLOR_WHITE, COLOR_BLACK);          //0(EMPTY)
    init_pair(WALL, COLOR_BLACK, COLOR_WHITE);           //1(WALL)
    init_pair(IMMUNE_WALL, COLOR_BLACK, COLOR_WHITE);    //2(IMMUNE WALL)
    init_pair(SNAKE_HEAD, COLOR_GREEN, COLOR_BLACK);     //3(SNAKE HEAD)
    init_pair(SNAKE_BODY, COLOR_GREEN, COLOR_BLACK);     //4(SNAKE BODY)
    init_pair(GROWTH_ITEM, COLOR_YELLOW, COLOR_RED);     //5(+ ITEM)
    init_pair(POISON_ITEM, COLOR_YELLOW, COLOR_MAGENTA); //6(- ITEM)
    init_pair(GATE, COLOR_CYAN, COLOR_CYAN);             //7(GATE)
    init_pair(10, COLOR_GREEN, COLOR_BLACK);             //10(TITLE/SCROLL/BOARD)
    init_pair(11, COLOR_BLACK, COLOR_WHITE);             //11(FOCUS)
    init_pair(12, COLOR_RED, COLOR_BLACK);               //12(OPTION)

    menuLastFocus = 0,
    speed = 4,
    optLastFocus = speed - 1,
    tcount = 0;
    manualTitle = "< MANUAL >",
    menuTitle = "< MENU >",
    menuTxt[0] = " - PLAY: Start the game",
    menuTxt[1] = " - HELP: Manual of the game",
    menuTxt[2] = " - OPTION: Setting of the game",
    menuTxt[3] = " - EXIT: Exit the game";
    shorTitle = "< SHORTCUTS >";
    shorTxt[0] = " - Arrow up(^): MOVE UP",
    shorTxt[1] = " - Arrow down(v): MOVE DOWN",
    shorTxt[2] = " - Arrow left(<): MOVE LEFT",
    shorTxt[3] = " - Arrow right(>): MOVE RIGHT",
    shorTxt[4] = " - 'p': GAME PAUSE",
    shorTxt[5] = " - 'r': GAME RESUME",
    shorTxt[6] = " - 'esc': BACK TO THE MAIN MENU";
}

Stage::~Stage()
{
    delwin(scrollBar);
    delwin(description);
    delwin(manual);
    delwin(info);
    delwin(mission);
    delwin(score);
    delwin(game);
    endwin();
}

void Stage::screenLock()
{
    cout << "\e[3;240;120t";
    cout << "\e[8;40;120t";
    system("resize -s 40 120");
    y = 40, x = 120;
    mvprintw(y - 1, 0, "SnakeGame ver. 1.0");
    sizeY = y / 1.5,
    sizeX = x / 1.5,
    startY = y / 2 - sizeY / 2,
    startX = x / 2 - sizeX / 2,
    desSizeY = sizeY - 6,
    desSizeX = sizeX - 6,
    desStartY = startY + 3,
    desStartX = startX + 3,
    txtLines = 26,
    hidTxtLen = txtLines - desSizeY > 0 ? txtLines - desSizeY : 0,
    scrollBarLen = desSizeY - hidTxtLen;
}

string Stage::menu()
{
    clear();
    screenLock();
    curs_set(0);
    string txt[5];
    txt[0] = "[ SNAKE GAME ]";
    int focus = menuLastFocus;
    level = 0;
    while (1)
    {
        if (!focus)
        {
            focus = 300;
        }
        txt[1] = "PLAY";
        txt[2] = "HELP";
        txt[3] = "OPTION";
        txt[4] = "EXIT";
        attron(COLOR_PAIR(10));
        mvprintw(y / 2 - 2, x / 2 - txt[0].length() / 2, txt[0].c_str());
        attroff(COLOR_PAIR(10));
        for (int i = 1; i <= sizeof(txt) / sizeof(txt[0]); i++)
        {
            if (i == abs(focus % 4 + 1))
            {
                attron(COLOR_PAIR(11));
                mvprintw(y / 2 + i, x / 2 - (txt[i].length() / 2), txt[i].c_str());
                attroff(COLOR_PAIR(11));
            }
            else
            {
                mvprintw(y / 2 + i, x / 2 - (txt[i].length() / 2), txt[i].c_str());
            }
        }
        switch (getch())
        {
            case UP:
                focus--;
                break;
            case DOWN:
                focus++;
                break;
            case ENTER:
                menuLastFocus = focus;
                return txt[abs(focus % 4 + 1)];
        }
    }
    return NULL;
}

void Stage::play()
{
    screenLock();
    setMap();
    int n;
    timeoutMs = SPEED_TIME[speed - 1];
    for (int i = 0; i < STAGE_NUM; i++)
    {
        msTime = n = 0;
        dir = LEFT;
        copyMap(i);
        setMission();
        makeSnake();
        appearGate();
        drawMap();
        while (1)
        {
            switch (getch())
            {
                case LEFT:
                    if (dir != RIGHT)
                    {
                        dir = LEFT;
                    }
                    break;
                case UP:
                    if (dir != DOWN)
                    {
                        dir = UP;
                    }
                    break;
                case RIGHT:
                    if (dir != LEFT)
                    {
                        dir = RIGHT;
                    }
                    break;
                case DOWN:
                    if (dir != UP)
                    {
                        dir = DOWN;
                    }
                    break;
                case PAUSE:
                    alert(y / 2 - 4, x / 2 - 34, "Press 'r' to play!", TRUE);
                    while (1)
                    {
                        if (getch() == RESUME)
                        {
                            break;
                        }
                    }
                    break;
                case ESC:
                    endwin();
                    return;
            }
            moveSnake();
            if (chkEnter) // After entering the Gate
            {
                if (++n >= stat[0]) // If tail successed that exit from the Gate, gate regeneration
                {
                    disappearGate();
                    appearGate();
                    n = 0;
                    chkEnter = FALSE;
                }
            }
            if (++msTime % (TIME_DIV_NUM[speed - 1] * 5) == 0) // Item regeneration every 5 seconds
            {
                disappearItem();
                appearItem();
            }
            if (stat[0] < 3)
            { // If the Snake length less than 3, game over
                gameOver();
            }
            if (isMissionClear())
            {
                alert(y / 2 - 4, x / 2 - 27, "Stage Clear!", FALSE);
                break;
            }
            if (checkGameOver())
            {
                alert(y / 2 - 4, x / 2 - 25, "Game Over!", FALSE);
                return;
            }
            drawMap();
            timeout(timeoutMs);
        }
        level++;
    }
    endwin();
}

void Stage::help()
{
    screenLock();
    int ySize = 0, yScroll = 0;
    while (1)
    {
        manual = newwin(sizeY, sizeX, startY, startX);
        description = newwin(desSizeY, desSizeX, desStartY, desStartX);
        scrollBar = newwin(scrollBarLen, 2, desStartY + yScroll, startX + sizeX - 6);
        wattron(manual, COLOR_PAIR(10));
        box(manual, 0, 0);
        mvwprintw(manual, 0, sizeX / 2 - manualTitle.length() / 2, "%s", manualTitle.c_str());
        wattroff(manual, COLOR_PAIR(10));

        mvwprintw(description, 0 + ySize,
                  sizeX / 2 - menuTitle.length() / 2 - 3, "%s", menuTitle.c_str());
        for (int i = 0; i < sizeof(menuTxt) / sizeof(menuTxt[0]); i++)
        {
            mvwprintw(description, 2 + (i * 2) + ySize, sizeX / 2 - menuTxt[2].length() / 2 - 3, "%s",
                      menuTxt[i].c_str());
        }

        mvwprintw(description, 11 + ySize,
                  sizeX / 2 - shorTitle.length() / 2 - 3, "%s", shorTitle.c_str());
        for (int i = 0; i < sizeof(shorTxt) / sizeof(shorTxt[0]); i++)
        {
            mvwprintw(description, 13 + (i * 2) + ySize, sizeX / 2 - shorTxt[6].length() / 2 - 3, "%s",
                      shorTxt[i].c_str());
        }

        if (txtLines >= desSizeY)
        {
            wattron(scrollBar, COLOR_PAIR(10));
            box(scrollBar, 0, 0);
            wattroff(scrollBar, COLOR_PAIR(10));
        }
        refresh();
        wrefresh(manual);
        wrefresh(description);
        wrefresh(scrollBar);
        RE:
        switch (getch())
        {
            case UP:
                if (yScroll)
                {
                    yScroll--;
                }
                else
                {
                    goto RE;
                }
                if (ySize)
                {
                    ySize++;
                }
                break;
            case DOWN:
                if (yScroll < desSizeY - scrollBarLen)
                {
                    yScroll++;
                }
                else
                {
                    goto RE;
                }
                if (ySize > desSizeY - txtLines && txtLines > desSizeY)
                {
                    ySize--;
                }
                break;
            case ESC:
                return;
        }
    }
}

void Stage::option()
{
    clear();
    screenLock();
    string optTitle = "[ OPTION ]";
    string txt[5];
    string optTxt[2];
    optTxt[0] = "SPEED:";
    optTxt[1] = " SLOW <--> FAST ";
    int focus = optLastFocus;
    level = 0;
    while (1)
    {
        if (!focus)
        {
            focus = 500;
        }
        for (int i = 0; i < sizeof(txt) / sizeof(txt[0]); i++)
        {
            txt[i] = to_string(i + 1);
        }

        attron(COLOR_PAIR(10));
        mvprintw(y / 2 - 2, x / 2 - optTitle.length() / 2, optTitle.c_str());
        attroff(COLOR_PAIR(10));
        for (int i = 0; i < sizeof(txt) / sizeof(txt[0]); i++)
        {
            if (i == abs(focus % 5))
            {
                attron(COLOR_PAIR(12));
                mvprintw(y / 2, x / 2 - sizeof(txt) / sizeof(txt[0]) + i * 3 - 1, txt[i].c_str());
                attroff(COLOR_PAIR(12));
            }
            else
            {
                mvprintw(y / 2, x / 2 - sizeof(txt) / sizeof(txt[0]) + i * 3 - 1, txt[i].c_str());
            }
        }

        mvprintw(y / 2 + 2, x / 2 - optTxt[1].length() / 2 + 1, optTxt[1].c_str());

        optLastFocus = focus;
        speed = atoi(txt[abs(focus % 5)].c_str());
        switch (getch())
        {
            case LEFT:
                focus--;
                break;
            case RIGHT:
                focus++;
                break;
            case ESC:
                return;
        }
    }
}

void Stage::setMap()
{
    int i, j, k;
    stage = new int **[STAGE_NUM];
    for (i = 0; i < STAGE_NUM; i++)
    {
        stage[i] = new int *[MAP_ROW];
        for (j = 0; j < MAP_ROW; j++)
        {
            stage[i][j] = new int[MAP_COL];
        }
    }

    for (i = 0; i < STAGE_NUM; i++)
    {
        for (j = 0; j < MAP_ROW; j++)
        {
            for (k = 0; k < MAP_COL; k++)
            {
                if (!j || !k || j == ROW_END || k == COL_END)
                {
                    stage[i][j][k] = WALL;
                }
                else
                {
                    stage[i][j][k] = EMPTY;
                }
            }
        }
        stage[i][0][0] = IMMUNE_WALL;
        stage[i][0][COL_END] = IMMUNE_WALL;
        stage[i][ROW_END][0] = IMMUNE_WALL;
        stage[i][ROW_END][COL_END] = IMMUNE_WALL;
        if (i == 1)
        {
            for (int z = 10; z < 40; z++)
            {
                stage[i][7][z] = WALL;
            }
            for (int z = 10; z < 40; z++)
            {
                stage[i][MAP_ROW - 7][z] = WALL;
            }
        }
        if (i == 2)
        {
            for (int z = 5; z < 20; z++)
            {
                stage[i][z][MAP_COL - 15] = WALL;
            }
            for (int z = 5; z < 20; z++)
            {
                stage[i][z][15] = WALL;
            }
        }
        if (i == 3)
        {
            for (int z = 10; z < 40; z++)
            {
                if (z > 22 && z < 27)
                {
                    continue;
                }
                stage[i][7][z] = WALL;
            }
            for (int z = 10; z < 40; z++)
            {
                if (z > 22 && z < 27)
                {
                    continue;
                }
                stage[i][MAP_ROW - 7][z] = WALL;
            }
            for (int z = 5; z < 20; z++)
            {
                if (z > 10 && z < 14)
                {
                    continue;
                }
                if (stage[i][z][MAP_COL - 15] == WALL)
                {
                    stage[i][z][MAP_COL - 15] = IMMUNE_WALL;
                }
                else
                {
                    stage[i][z][MAP_COL - 15] = WALL;
                }
            }
            for (int z = 5; z < 20; z++)
            {
                if (z > 10 && z < 14)
                {
                    continue;
                }
                if (stage[i][z][15] == WALL)
                {
                    stage[i][z][15] = IMMUNE_WALL;
                }
                else
                {
                    stage[i][z][15] = WALL;
                }
            }
        }
    }
}

void Stage::copyMap(int nStage)
{
    map = new int *[MAP_ROW];
    for (int i = 0; i < MAP_COL; i++)
    {
        map[i] = new int[MAP_COL];
    }
    for (int i = 0; i < MAP_ROW; i++)
    {
        for (int j = 0; j < MAP_COL; j++)
        {
            map[i][j] = stage[nStage][i][j];
        }
    }
}

void Stage::drawMap()
{
    game = newwin(MAP_ROW, MAP_COL,
                  y / 2 - MAP_ROW / 2, x / 2 - (MAP_COL / 2 + 16));
    for (int i = 0; i < MAP_ROW; i++)
    {
        for (int j = 0; j < MAP_COL; j++)
        {
            int index = map[i][j];
            wattron(game, COLOR_PAIR(index));
            mvwaddch(game, i, j, ITEM_CHAR_LIST[index]);
            wattroff(game, COLOR_PAIR(index));
        }
        printw("\n");
    }

    score = newwin(16, 30, y / 2 - (MAP_ROW / 2 + 4), x / 2 + MAP_COL / 2 - 7.4);
    wattron(score, COLOR_PAIR(10));
    box(score, 0, 0);
    mvwprintw(score, 0, 10, "[ SCORE ]");
    wattroff(score, COLOR_PAIR(10));
    mvwprintw(score, 3, 5, "B: %d / %d", stat[0], SNAKE_MAX_LENGTH);
    mvwprintw(score, 6, 5, "+: %d", stat[1]);
    mvwprintw(score, 9, 5, "-: %d", stat[2]);
    mvwprintw(score, 12, 5, "G: %d", stat[3]);

    mission = newwin(16, 30, y / 2 - (MAP_ROW / 2 + 4) + 17, x / 2 + MAP_COL / 2 - 7.4);
    wattron(mission, COLOR_PAIR(10));
    box(mission, 0, 0);
    mvwprintw(mission, 0, 9, "[ MISSION ]");
    wattroff(mission, COLOR_PAIR(10));
    mvwprintw(mission, 3, 5, "B: %d ( %c )", statMission[0], chkMission[0]);
    mvwprintw(mission, 6, 5, "+: %d ( %c )", statMission[1], chkMission[1]);
    mvwprintw(mission, 9, 5, "-: %d ( %c )", statMission[2], chkMission[2]);
    mvwprintw(mission, 12, 5, "G: %d ( %c )", statMission[3], chkMission[3]);

    info = newwin(4, 15, y / 2 - (MAP_ROW / 2 + 4), x / 2 + MAP_COL / 2 - 47.4);
    mvwprintw(info, 0, 1, "[ STAGE %d/%d ]", level + 1, STAGE_NUM);
    mvwprintw(info, 2, 3, "< %02d:%02d >", msTime / (TIME_DIV_NUM[speed - 1] * 60),
              (msTime / TIME_DIV_NUM[speed - 1]) % 60);

    refresh();
    wrefresh(info);
    wrefresh(game);
    wrefresh(score);
    wrefresh(mission);
}

void Stage::appearItem()
{
    int appearNum = rand() % 3 + 1;
    for (int i = 0; i < appearNum; i++)
    {
        int itemType = rand() % 2 + GROWTH_ITEM;
        if (chkMission[2] == 'v')
        {
            itemType = GROWTH_ITEM;
        }
        else if (stat[0] >= SNAKE_MAX_LENGTH)
        {
            itemType = POISON_ITEM;
        }
        while (1)
        {
            int y = rand() % (MAP_ROW - 2) + 1;
            int x = rand() % (MAP_COL - 2) + 1;
            if (map[y][x] == EMPTY &&
                map[y][x - 1] != GATE && map[y][x + 1] != GATE &&
                map[y + 1][x] != GATE && map[y - 1][x] != GATE)
            {
                map[y][x] = itemType;
                itemPos.push_back(make_pair(y, x));
                break;
            }
        }
    }
}

void Stage::appearGate()
{
    int n, y, x;
    for (int i = 0; i < 2; i++)
    {
        while (1)
        {
            n = rand() % (!level ? 4 : 5);
            y = rand() % (MAP_ROW - (i ? 3 : 2)) + (i ? 2 : 1);
            x = rand() % (MAP_COL - (i ? 3 : 2)) + (i ? 2 : 1);
            switch (n)
            {
                case 0: // upper side
                    y = 0;
                    break;
                case 1: // left side
                    x = 0;
                    break;
                case 2: // right side
                    x = COL_END;
                    break;
                case 3: // lower side
                    y = ROW_END;
                    break;
                case 4: // middle
                    while (1)
                    {
                        x = rand() % 30 + 10;
                        y = rand() % 15 + 5;
                        if (map[y][x] == WALL)
                        {
                            break;
                        }
                    }
            }
            if (map[y][x] == WALL)
            {
                map[y][x] = GATE;
                gatePos.push_back(make_pair(y, x));
                break;
            }
        }
        if (i == 0)
        {
            gate1 = new Something(y, x, GATE);
        }
        if (i == 1)
        {
            gate2 = new Something(y, x, GATE);
        }
    }
}

void Stage::disappearItem()
{
    for (auto item: itemPos)
    {
        if (map[item.first][item.second] == GROWTH_ITEM || map[item.first][item.second] == POISON_ITEM)
        {
            map[item.first][item.second] = EMPTY;
        }
    }
    itemPos.clear();
}

void Stage::disappearGate()
{
    for (auto gate: gatePos)
    {
        if (map[gate.first][gate.second] == GATE)
        {
            map[gate.first][gate.second] = WALL;
        }
    }
    gatePos.clear();
}

void Stage::makeSnake()
{
    stat[0] = 3;
    int row = 13;
    int col = 26;
    Bam = new Something(row, col--, SNAKE_BODY);
    Something *p = new Something(row, col--, SNAKE_BODY);
    Bam->link = p;
    p = new Something(row, col--, SNAKE_HEAD);
    Bam->link->link = p;
    map[Bam->y][Bam->x] = Bam->who;
    p = Bam->link;
    map[p->y][p->x] = p->who;
    p = p->link;
    map[p->y][p->x] = p->who;
}

void Stage::moveSnake()
{
    if (map[Bam->y][Bam->x] != WALL)
    {
        map[Bam->y][Bam->x] = EMPTY;
    }
    Something *q = Bam;
    Something *p = q->link;
    while (p->link != NULL)
    {
        q->x = p->x;
        q->y = p->y;
        q = p;
        p = p->link;
    }
    if (dir == LEFT)
    {
        map[p->y][p->x] = q->who;
        q->x = p->x;
        q->y = p->y;
        p->x--;
    }
    else if (dir == UP)
    {
        map[p->y][p->x] = q->who;
        q->x = p->x;
        q->y = p->y;
        p->y--;
    }
    else if (dir == RIGHT)
    {
        map[p->y][p->x] = q->who;
        q->x = p->x;
        q->y = p->y;
        p->x++;
    }
    else if (dir == DOWN)
    {
        map[p->y][p->x] = q->who;
        q->x = p->x;
        q->y = p->y;
        p->y++;
    }
    if (map[p->y][p->x] == WALL || map[p->y][p->x] == SNAKE_BODY)
    {
        map[p->y][p->x] = IMMUNE_WALL;
        gameOver();
    }
    if (map[p->y][p->x] == GATE)
    {
        enterGate(p);
    }
    if (map[p->y][p->x] == GROWTH_ITEM || map[p->y][p->x] == POISON_ITEM)
    {
        eatItem(map[p->y][p->x]);
    }
    map[p->y][p->x] = p->who;
}

void Stage::enterGate(Something *head)
{
    chkEnter = TRUE;
    if (gate1->x == head->x && gate1->y == head->y)
    {
        if (gate2->x == 0)
        {
            head->x = 1;
            head->y = gate2->y;
            dir = RIGHT;
        }
        else if (gate2->x == COL_END)
        {
            head->x = COL_END - 1;
            head->y = gate2->y;
            dir = LEFT;
        }
        else if (gate2->y == 0)
        {
            head->x = gate2->x;
            head->y = 1;
            dir = DOWN;
        }
        else if (gate2->y == ROW_END)
        {
            head->x = gate2->x;
            head->y = ROW_END - 1;
            dir = UP;
        }
        findRoot(gate2); // If gate on the middle wall, find the exit root.
        if (dir == LEFT)
        {
            head->x = gate2->x - 1;
            head->y = gate2->y;
        }
        else if (dir == UP)
        {
            head->x = gate2->x;
            head->y = gate2->y - 1;
        }
        else if (dir == RIGHT)
        {
            head->x = gate2->x + 1;
            head->y = gate2->y;
        }
        else if (dir == DOWN)
        {
            head->x = gate2->x;
            head->y = gate2->y + 1;
        }
    }
    else if (gate2->x == head->x && gate2->y == head->y)
    {
        if (gate1->x == 0)
        {
            head->x = 1;
            head->y = gate1->y;
            dir = RIGHT;
        }
        else if (gate1->x == COL_END)
        {
            head->x = COL_END - 1;
            head->y = gate1->y;
            dir = LEFT;
        }
        else if (gate1->y == 0)
        {
            head->x = gate1->x;
            head->y = 1;
            dir = DOWN;
        }
        else if (gate1->y == ROW_END)
        {
            head->x = gate1->x;
            head->y = ROW_END - 1;
            dir = UP;
        }
        findRoot(gate1); // If gate on the middle wall, find the exit root.
        if (dir == LEFT)
        {
            head->x = gate1->x - 1;
            head->y = gate1->y;
        }
        else if (dir == UP)
        {
            head->x = gate1->x;
            head->y = gate1->y - 1;
        }
        else if (dir == RIGHT)
        {
            head->x = gate1->x + 1;
            head->y = gate1->y;
        }
        else if (dir == DOWN)
        {
            head->x = gate1->x;
            head->y = gate1->y + 1;
        }
    }
    stat[3]++;
}

int Stage::findRoot(Something *gate)
{
    for (int i = 0; i < 4; i++)
    {
        if (dir == LEFT)
        {
            if (map[gate->y][gate->x - 1] == EMPTY)
            {
                return dir;
            }
            else
            {
                dir = UP;
            }
        }
        else if (dir == UP)
        {
            if (map[gate->y - 1][gate->x] == EMPTY)
            {
                return dir;
            }
            else
            {
                dir = RIGHT;
            }
        }
        else if (dir == RIGHT)
        {
            if (map[gate->y][gate->x + 1] == EMPTY)
            {
                return dir;
            }
            else
            {
                dir = DOWN;
            }
        }
        else if (dir == DOWN)
        {
            if (map[gate->y + 1][gate->x] == EMPTY)
            {
                return dir;
            }
            else
            {
                dir = LEFT;
            }
        }
    }
    return dir;
}

void Stage::eatItem(int item)
{
    if (item == GROWTH_ITEM)
    {
        if (stat[0] == 10)
        {
            return;
        }
        Something *p = new Something(Bam->y, Bam->x, SNAKE_BODY);
        if (Bam->x - Bam->link->x == 1)
        {
            p->x++;
        }
        else if (Bam->y - Bam->link->y == 1)
        {
            p->y++;
        }
        else if (Bam->x - Bam->link->x == -1)
        {
            p->x--;
        }
        else if (Bam->y - Bam->link->y == -1)
        {
            p->y--;
        }
        p->link = Bam;
        Bam = p;
        if (map[Bam->y][Bam->x] != WALL)
        {
            map[Bam->y][Bam->x] = Bam->who;
        }
        stat[0]++;
        stat[1]++;
    }
    else if (item == POISON_ITEM)
    {
        map[Bam->y][Bam->x] = EMPTY;
        Bam = Bam->link;
        stat[0]--;
        stat[2]++;
    }
}

void Stage::setMission()
{
    finish = chkEnter = FALSE;
    memset(stat, 0, sizeof(stat));
    memset(statMission, 0, sizeof(statMission));
    memset(chkMission, ' ', sizeof(chkMission));

    // SET MISSION RANDOM NUMBER THAT EACH ITEMS(ITEM: range).
    statMission[0] = rand() % 5 + 6; // SNAKE LENGTH: 6~10
    statMission[1] = rand() % 5 + 4; // INCREASE ITEM: 4~8
    statMission[2] = rand() % 4 + 3; // DECREASE ITEM: 3~6
    statMission[3] = rand() % 5 + 1; // GATE : 1~5
}

bool Stage::isMissionClear()
{
    int count = 0;
    for (int i = 0; i < 4; i++)
    {
        if (stat[i] >= statMission[i])
        {
            chkMission[i] = 'v';
            count++;
        }
        else if (!i)
        {
            chkMission[i] = ' ';
        }
    }
    if (count == 4)
    {
        return TRUE;
    }
    return FALSE;
}

void Stage::gameOver()
{
    finish = true;
}

void Stage::alert(int posY, int posX, const string msg, bool stopFlag)
{
    WINDOW *alert = newwin(7, msg.length() * 2, posY, posX);
    box(alert, 0, 0);
    wattron(alert, COLOR_PAIR(0));
    wbkgd(alert, COLOR_PAIR(2));
    mvwprintw(alert, 3, msg.length() / 2, msg.c_str());
    wrefresh(alert);
    if (!stopFlag)
    {
        usleep(1750000);
    }
}
