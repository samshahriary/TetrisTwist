/*
 wanted to create a tetris game from scratch using c++ and library ncurses
 to compile: g++ -std=c++11 main.cpp -lncurses
 -std=c++11 is for constructor to initialize vector< pair < > >
 -lncurses is for including ncurses library
 */

#include <ncurses.h>                                                        //library includes window creation and key inputs
#include <vector>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>
#include <utility>
#include <thread>         // this_thread::sleep_for
#include <chrono>         // chrono::seconds

using namespace std;
const int GxSIZE = 15;
const int GySIZE = 18;

void DisplayTitleScreen()
{
    
    int KeyInput;
    WINDOW *Twin = newwin(18, 45, 1, 15);                                   //Title message window creation
    nodelay(Twin,TRUE);                                                     //waits for user input
    
    //Title Message
    do
    {
        box(Twin, 0, 0);                                                        //Title window
        wmove(Twin,1,1);
        wprintw(Twin,"*Rules:");
        wmove(Twin,2,1);
        wprintw(Twin, "*Use right and left arrow keys to move");
        wmove(Twin, 3, 1);
        wprintw(Twin, "*Use up and down arrow keys to rotate piece");
        wmove(Twin,4, 1);
        wprintw(Twin, "*Press Delete key to exit game at anytime");
        wmove(Twin,9,10);
        wprintw(Twin, "Press Enter to start");
        wrefresh(Twin);
        refresh();
    }
    while((KeyInput=wgetch(Twin)) != 10);
    werase(Twin);                                                           //make all spot in window blank
    wrefresh(Twin);                                                         //refresh title window
    delwin(Twin);                                                           //delete window
    return;
}

int FindSmallestX(vector < pair<int, int> > Gameboard)        //function to find smallest x value on piece so we dont move left off map
{
    int smll = Gameboard[0].second;
    
    for(int i = 1; i <Gameboard.size(); i++)
    {
        if(Gameboard[i].second < smll)
        {
            smll = Gameboard[i].second;
        }
    }
    
    return smll;                                                                                //smll can now be used to compare with far left side of map when trying to move left
}

int FindLargestX(vector < pair<int, int> > Gameboard)         //function to find largest x value on piece so we dont move right off map
{
    int lrg = Gameboard[0].second;
    
    for(int i = 1; i < Gameboard.size(); i++)
    {
        if(Gameboard[i].second > lrg)
        {
            lrg = Gameboard[i].second;
        }
    }
    
    return lrg;                                                                                 //lrg can now be used to compare with far right side of map when trying to move right
}

int FindLargestY(vector < pair<int, int> > Gameboard)
{
    int lrg = Gameboard[0].first;
    
    for(int i = 1; i < Gameboard.size(); i++)
    {
        if(Gameboard[i].first > lrg)
        {
            lrg = Gameboard[i].first;
        }
    }
    
    return lrg;
}


void ErasePiece(vector < pair<int, int>> Gameboard, WINDOW *Gwin)        //erase piece when before moving
{
    for(int i = 0; i < Gameboard.size(); i++)
    {
        mvwaddstr(Gwin, Gameboard[i].first, Gameboard[i].second, " ");
    }
    wrefresh(Gwin);
    refresh();
    
    return;
}

void DrawPiece(vector< pair<int, int> > GameBoard, WINDOW *Gwin)         //draw piece in new location
{
    for(int j = 0; j < GameBoard.size(); j++)
    {
        mvwaddstr(Gwin, GameBoard[j].first, GameBoard[j].second, "*");
    }
    wrefresh(Gwin);
    refresh();
    
    return;
}

bool MoveDown( vector < pair<int,int> >  & Gameboard, WINDOW *Gwin, int Gy, bool GameTrack [GxSIZE][GySIZE])     //check if you can move down, if you can, erase, and draw, refresh board
{
    int lrg = FindLargestY(Gameboard);
    // for each coordinate in the gameTrack. If there's a piece underneath it, don't move down
    bool BreakMoveDown = false;
    for(int i = 0; i < Gameboard.size(); i++)
    {
        if(GameTrack[Gameboard[i].second][Gameboard[i].first+1])
        {
            mvprintw(i+5, 0,"%d , %d", Gameboard[i].first, Gameboard[i].first+1);
            BreakMoveDown = true;
        }
    }
    //if there's room for piece to fall then move down piece
    if(lrg+1 < Gy-1 && !BreakMoveDown)
    {
        ErasePiece(Gameboard, Gwin);
        for(int i = 0; i < Gameboard.size(); i++)
        {
            Gameboard[i].first++;
        }
        DrawPiece(Gameboard, Gwin);
        wrefresh(Gwin);
        refresh();
    }
    else
    {
        return true;            //if true we want to break loop in main and go to next piece
    }
    return false;               //if false we keep checking and falling
}

void MoveLeft(vector < pair<int,int> > & Gameboard, WINDOW *Gwin, int XPos, bool GameTrack [GxSIZE][GySIZE])
{
    XPos = FindSmallestX(Gameboard);
    bool BreakMoveLeft = false;
    for(int i = 0; i < Gameboard.size(); i++)
    {
        if(GameTrack[Gameboard[i].second-1][Gameboard[i].first])
        {
            BreakMoveLeft = true;
        }
    }
    if(XPos-1 > 0 && !BreakMoveLeft)
    {
        ErasePiece(Gameboard, Gwin);
        for(int i = 0; i < Gameboard.size(); i++)
        {
            Gameboard[i].second--;
        }
        DrawPiece(Gameboard, Gwin);
    }
    wrefresh(Gwin);
    refresh();
    
    return;
}

void MoveRight(vector< pair<int, int> > & Gameboard, WINDOW *Gwin, int XPos, int Gx, bool GameTrack [GxSIZE][GySIZE])
{
    XPos = FindLargestX(Gameboard);             //find coordinate that would reach end of gameboard first
    bool BreakMoveRight = false;
    for(int i = 0; i < Gameboard.size(); i++)
    {
        if(GameTrack[Gameboard[i].second+1][Gameboard[i].first])
        {
            BreakMoveRight = true;
        }
    }
    
    if(XPos+1 < Gx-1 && !BreakMoveRight)                                               //if largest coornidate is in range of game board then erase piece and draw it in new position
    {
        mvprintw(3, 0, "if Move Right, Xpos: %d", XPos);            //comment
        refresh();
        ErasePiece(Gameboard, Gwin);
        for(int i = 0; i < Gameboard.size(); i++)
        {
            Gameboard[i].second++;
        }
        DrawPiece(Gameboard, Gwin);
    }
    wrefresh(Gwin);
    refresh();
}


int main()
{
    
    //INITIALIZING FUNCTIONS
    initscr();                                            //initializes terminal to one large window made up of a 2D array
    cbreak();                                             //key inputsreturned immediatley from getch rather than pushed onto queue
    keypad(stdscr, TRUE);                                 //reads special keys such as arroy keys
    noecho();                                             //user inputted keys not displayed
    curs_set(0);                                          //hide cursor
    nodelay(stdscr,TRUE);                                 //Doesn't wait for user to press key to print screen
    
    int XPos = 7;                                         //position of where to spawn piece on gameboard
    
    
    //FOR BELOW! first vector is a vector of pairs. each pair has coordinates and each vector represents a piece. outer vector used to iterate through.
    //we need to create a new instance of each piece so gameboard holds piece
    //Game track keeps coordinates taken unlike Gameboard which just updates '*' and then forgets coordinates
    vector< vector< pair<int, int> > > GameBoard;
    bool GameTrack [GxSIZE][GySIZE];                                //true = spot used, false = open
    for(int i = 0; i < GxSIZE; i++)
    {
        for(int j = 0; j < GySIZE; j++)
        {
            GameTrack[i][j] = false;
        }
    }
    vector< vector< pair<int, int> > > PiecePatterns =
    {
        {{1, XPos},{1, XPos+1},{2, XPos},{2, XPos+1}},
        {{1, XPos+1},{1, XPos+2},{2,XPos+1},{2, XPos}},
        {{1, XPos},{2, XPos},{3,XPos},{4,XPos}},
        {{1, XPos},{2, XPos-1}, {2, XPos}, {2, XPos+1}},
        {{1, XPos},{2, XPos},{2, XPos+1}, {2, XPos+2}},
        {{1, XPos},{1, XPos+1},{2, XPos+1}, {2, XPos+2}},
        {{1, XPos+2},{2,XPos+2},{2, XPos+1},{2, XPos}}
    };
    
    bool BrkToNewPiece = false;                            //once piece has fallen
    int KeyInput, Gx, Gy, RandomPieceNum;                  //Gx = Gameboard max X, Gy = Gameboard max Y
    int GameScore;                                         //keep track of user score
    
    
    srand(time(NULL));                                                      //initiliaze random seed
    
    RandomPieceNum = rand()%7;                                              //pick random piece
    
    
    WINDOW *Gwin = newwin(GySIZE, GxSIZE, 1, 30);                           //create gameboard window made up of 2D array
    box(Gwin, 0 , 0);                                                       //set borders
    WINDOW *Swin = newwin(10, 10, 5, 50);                                   //create scoreboard window
    box(Swin, 0, 0);                                                        //set borders
    
    getmaxyx(Gwin,Gy,Gx);                                                   //get size of baord
    mvprintw(0, 0, "%d and %d",Gx, Gy);                                     //comment
    mvprintw(1, 0,"%d", XPos);
    mvprintw(2, 0, "%d", PiecePatterns[RandomPieceNum].size());
    mvprintw(0, 35,"TETRIS");                                               //moves to position on window and prints title
    
    DisplayTitleScreen();                                                   //prompt user/display rules
    
    mvwprintw(Swin, 3, 2, "Score: ");
    wrefresh(Swin);
    do
    {
        RandomPieceNum = rand()%7;
        
        GameBoard.push_back(PiecePatterns[RandomPieceNum]);
        
        DrawPiece(GameBoard.back(), Gwin);                              //draw a new piece on board
        wrefresh(Gwin);                                                 //refresh gameboard/update
        
        for (;;) {
            switch (KeyInput = getch()) {
                case KEY_LEFT:              // left key
                    MoveLeft(GameBoard.back(), Gwin, XPos, GameTrack);
                    break;
                case KEY_UP:                // user pressed up arrow key
                    
                    break;
                case KEY_DOWN:              // user pressed up arrow key
                    
                    break;
                case KEY_RIGHT:             //user pressed right key
                    MoveRight(GameBoard.back(), Gwin, XPos, Gx, GameTrack);
                    break;
                case ERR:
                    this_thread::sleep_for (chrono::milliseconds(500));              //sleep for 1 second
                    BrkToNewPiece = MoveDown(GameBoard.back(), Gwin, Gy, GameTrack);
                    break;
            }
            if(BrkToNewPiece)
            {
                for(int i = 0; i < GameBoard.back().size(); i++)
                {
                    mvprintw(4, 0,"%d    %d", GameBoard.back().at(i).second,GameBoard.back().at(i).first);
                    GameTrack[GameBoard.back().at(i).second][GameBoard.back().at(i).first] = true;
                }
                break;
            }
        }
        
        mvprintw(0, 5, "%d", RandomPieceNum);
        refresh();
    }
    while((KeyInput=getch()) != 127);
    
    delwin(Gwin);
    delwin(Swin);
    endwin();
    return 0;
}

//UPDATE: GOT THE NEW PIECES TO WORK NOW WE HAVE TO CHECK IF SPOT TAKEN BEFORE MOVING PIECE DOWN EVERY STEP. USING GAMETRACK TO CHECK COORDINATES TAKEN IN MOVE DOWN. ALSO START FIGURING OUT HOW TO COUNT AND DELETE A ROW