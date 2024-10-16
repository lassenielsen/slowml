#include <slowml/network.hpp>
//#include <cmath>
#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include "rlsnake_multiplayer.hpp"
#include <ncurses.h>
#include <unistd.h>

using namespace std;

void TestSnake(vector<Network*> &models, size_t width, size_t height) // {{{
{ // Create game
  srand(time(NULL)); // Fix (random) map
  RLSnake game(width,height,models.size());
  // Create player networks

  size_t step=0;
  for (step=0; true; ++step)
  { //clear();
    move(0,0); // Goto top left corner
    stringstream ss;
    ss << "Step: " << step << " score: " << game.Score()[0] << "\n\r"
       << game.GameString();
    mvaddstr(1,1,ss.str().c_str());
    wrefresh(stdscr);
    // Perform step
    if (game.Done())
      break;
    game.RLGame::Step(models,0.1);
    this_thread::sleep_for(chrono::milliseconds(30));
  }

  // Clean up
  while (!models.empty())
  { delete models.back();
    models.pop_back();
  }
  return;
} // }}}

int main(int argc, char **argv) // {{{
{ try
  { if (argc<2)
    { cout << "Usage: " << argv[0] << " [-m <model path>]* [-p <keys>]*" << endl;
      return 0;
    }
    cout << "TestRL3 - Testing snake model " << argv[1] << endl;
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr,true);
    nodelay(stdscr,true);
    TestSnake(argv[1],80,19);
    usleep(3000000);
    endwin();
    return 0;
  }
  catch (const string &s)
  { cout << "Error: " << s << endl;
    return -1;
  }
} // }}}

