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

int FindSmallestX(vector< vector < pair<int, int> > > PiecePatterns, int RandomPieceNum)        //function to find smallest x value on piece so we dont move left off map
{
    int smll = PiecePatterns[RandomPieceNum][0].second;
    
    for(int i = 1; i <PiecePatterns[RandomPieceNum].size(); i++)
    {
        if(PiecePatterns[RandomPieceNum][i].second < smll)
        {
            smll = PiecePatterns[RandomPieceNum][i].second;
        }
    }
    
    return smll;                                                                                //smll can now be used to compare with far left side of map when trying to move left
}

int FindLargestX(vector< vector < pair<int, int> > > PiecePatterns, int RandomPieceNum)         //function to find largest x value on piece so we dont move right off map
{
    int lrg = PiecePatterns[RandomPieceNum][0].second;
    
    for(int i = 1; i < PiecePatterns[RandomPieceNum].size(); i++)
    {
        if(PiecePatterns[RandomPieceNum][i].second > lrg)
        {
            lrg = PiecePatterns[RandomPieceNum][i].second;
        }
    }
    
    return lrg;                                                                                 //lrg can now be used to compare with far right side of map when trying to move right
}

int FindLargestY(vector< vector < pair<int, int> > > PiecePatterns, int RandomPieceNum)
{
    int lrg = PiecePatterns[RandomPieceNum][0].first;
    
    for(int i = 1; i < PiecePatterns[RandomPieceNum].size(); i++)
    {
        if(PiecePatterns[RandomPieceNum][i].first > lrg)
        {
            lrg = PiecePatterns[RandomPieceNum][i].first;
        }
    }
    
    return lrg;
}


void ErasePiece(vector< vector < pair<int, int> > > PiecePatterns, WINDOW *Gwin, int RandomPieceNum)        //erase piece when before moving
{
    for(int i = 0; i < PiecePatterns[RandomPieceNum].size(); i++)
    {
        mvwaddstr(Gwin, PiecePatterns[RandomPieceNum][i].first, PiecePatterns[RandomPieceNum][i].second, " ");
    }
    wrefresh(Gwin);
    refresh();
    
    return;
}

void DrawPiece(vector< vector < pair<int, int> > > PiecePatterns, WINDOW *Gwin, int RandomPieceNum)         //draw piece in new location
{
    for(int j = 0; j < PiecePatterns[RandomPieceNum].size(); j++)
    {
        mvwaddstr(Gwin, PiecePatterns[RandomPieceNum][j].first, PiecePatterns[RandomPieceNum][j].second, "*");
    }
    wrefresh(Gwin);
    refresh();
    
    return;
}

bool MoveDown(vector< vector < pair<int,int> > > & PiecePatterns, WINDOW *Gwin, int RandomPieceNum, int Gy)     //check if you can move down, if you can, erase, and draw, refresh board
{
    int lrg = FindLargestY(PiecePatterns,RandomPieceNum);
    
    if(lrg+1 < Gy-1)
    {
        ErasePiece(PiecePatterns, Gwin, RandomPieceNum);
        for(int i = 0; i < PiecePatterns[RandomPieceNum].size(); i++)
        {
            PiecePatterns[RandomPieceNum][i].first++;
        }
        DrawPiece(PiecePatterns, Gwin, RandomPieceNum);
        wrefresh(Gwin);
        refresh();
    }
    else
    {
        return true;
    }
    return false;
}

void MoveLeft(vector< vector < pair<int,int> > > & PiecePatterns, WINDOW *Gwin, int RandomPieceNum, int XPos)
{
    XPos = FindSmallestX(PiecePatterns, RandomPieceNum);
    mvprintw(3, 0, "Move Left, Xpos: ", XPos);
    refresh();
    
    if(XPos-1 > 0)
    {
        mvprintw(3, 0, "if Move Left, Xpos: %d", XPos);
        refresh();
        ErasePiece(PiecePatterns, Gwin, RandomPieceNum);
        for(int i = 0; i < PiecePatterns[RandomPieceNum].size(); i++)
        {
            PiecePatterns[RandomPieceNum][i].second--;
        }
        DrawPiece(PiecePatterns, Gwin, RandomPieceNum);
    }
    wrefresh(Gwin);
    refresh();
    
    return;
}

void MoveRight(vector< vector< pair<int, int> > > & PiecePatterns, WINDOW *Gwin, int RandomPieceNum, int XPos, int Gx)
{
    XPos = FindLargestX(PiecePatterns, RandomPieceNum);             //find coordinate that would reach end of gameboard first
    mvprintw(3, 0, "Move Right, Xpos: %d", XPos);                   //comment
    refresh();
    if(XPos+1 < Gx-1)                                               //if largest coornidate is in range of game board then erase piece and draw it in new position
    {
        mvprintw(3, 0, "if Move Right, Xpos: %d", XPos);            //comment
        refresh();
        ErasePiece(PiecePatterns, Gwin, RandomPieceNum);
        for(int i = 0; i < PiecePatterns[RandomPieceNum].size(); i++)
        {
            PiecePatterns[RandomPieceNum][i].second++;
        }
        DrawPiece(PiecePatterns, Gwin, RandomPieceNum);
    }
    wrefresh(Gwin);
    refresh();
}

//void MoveDown(vector< vector< pair< int, int> > >)

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
    
    
    //FOR BELOW! each piece has 2 sets of coordinates to be plotted on window
    //vector< vector< pair< int, int> > > cannot be initialized (no constructor available) so individually push back
    //plan to create another vector to hold possible rotations of pieces
    
    vector< vector< pair<int, int> > > PiecePatterns;
    vector< vector< int > > GameBoard;
    vector < pair<int, int> > Piece1 = {{1, XPos},{1, XPos+1},{2, XPos},{2, XPos+1}};
    vector < pair<int, int> > Piece2 = {{1, XPos+1},{1, XPos+2},{2,XPos+1},{2, XPos}};
    vector < pair<int, int> > Piece3 = {{1, XPos},{2, XPos},{3,XPos},{4,XPos}};
    vector < pair<int, int> > Piece4 = {{1, XPos},{2, XPos-1}, {2, XPos}, {2, XPos+1}};
    vector < pair<int, int> > Piece5 = {{1, XPos},{2, XPos},{2, XPos+1}, {2, XPos+2}};
    vector < pair<int, int> > Piece6 = {{1, XPos},{1, XPos+1},{2, XPos+1}, {2, XPos+2}};
    vector < pair<int, int> > Piece7 = {{1, XPos+2},{2,XPos+2},{2, XPos+1},{2, XPos}};
    
    PiecePatterns.push_back(Piece1);
    PiecePatterns.push_back(Piece2);
    PiecePatterns.push_back(Piece3);
    PiecePatterns.push_back(Piece4);
    PiecePatterns.push_back(Piece5);
    PiecePatterns.push_back(Piece6);
    PiecePatterns.push_back(Piece7);
    
    bool BrkToNewPiece = false;                            //once piece has fallen
    int KeyInput, Gx, Gy, RandomPieceNum;                  //Gx = Gameboard max X, Gy = Gameboard max Y
    int GameScore;                                         //keep track of user score
    bool NewPiece;
    
    
    srand(time(NULL));                                                      //initiliaze random seed
    
    RandomPieceNum = rand()%7;                                              //pick random piece
    
    
    WINDOW *Gwin = newwin(18, 15, 1, 25);                                   //create gameboard window made up of 2D array
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
        NewPiece = true;
        RandomPieceNum = rand()%7;
        
        DrawPiece(PiecePatterns, Gwin, RandomPieceNum);                 //draw a new piece on board
        wrefresh(Gwin);                                                 //refresh gameboard/update
        
        for (;;) {
            switch (KeyInput = getch()) {
                case KEY_LEFT:              // left key
                    MoveLeft(PiecePatterns, Gwin, RandomPieceNum, XPos);
                    break;
                case KEY_UP:                // user pressed up arrow key
                    
                    break;
                case KEY_DOWN:              // user pressed up arrow key
                    
                    break;
                case KEY_RIGHT:             //user pressed right key
                    MoveRight(PiecePatterns, Gwin, RandomPieceNum, XPos, Gx);
                    break;
                case ERR:
                    this_thread::sleep_for (chrono::milliseconds(500));              //sleep for 1 second
                    BrkToNewPiece = MoveDown(PiecePatterns, Gwin, RandomPieceNum, Gy);
                    break;
            }
            if(BrkToNewPiece)
                break;
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

/*
 0 = square
 
 1 =  //zig zag face right
 
 2 = straight
 
 3 = //straight pnt mid
 
 4 =  straight pnt left
 
 5 =  zig zag face left
 
 6 = straight pnt right
 
 */


/*switch (KeyInput) {
 case KEY_LEFT:              // left key
 MoveLeft(PiecePatterns, Gwin, RandomPieceNum, XPos);
 break;
 case KEY_UP:                // user pressed up arrow key
 
 break;
 case KEY_DOWN:              // user pressed up arrow key
 
 break;
 case KEY_RIGHT:             //user pressed right key
 break;
 case ERR:
 break;
 }*/