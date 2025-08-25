#include <slowml/network.hpp>
//#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <sstream>
#include "rlsnake_multiplayer.hpp"
#include <ncurses.h>
#include <unistd.h>

using namespace std;

class Player // {{{
{ public:
    Player(char chup, char chdown, char chleft, char chright) // {{{
    : myUp(chup)
    , myDown(chdown)
    , myLeft(chleft)
    , myRight(chright)
    , myDir(0)
    {
    } // }}}
    virtual ~Player() // {{{
    {
    } // }}}

    vector<double> Keys() const // {{{
    { vector<double> result;
      if (myDir==0) // Up
      { result.push_back(1.0d);
        result.push_back(0.0d);
        result.push_back(0.0d);
        result.push_back(0.0d);
      }
      else if (myDir==1) // Down
      { result.push_back(0.0d);
        result.push_back(1.0d);
        result.push_back(0.0d);
        result.push_back(0.0d);
      }
      else if (myDir==2) // Left
      { result.push_back(0.0d);
        result.push_back(0.0d);
        result.push_back(1.0d);
        result.push_back(0.0d);
      }
      else // Must be Right
      { result.push_back(0.0d);
        result.push_back(0.0d);
        result.push_back(0.0d);
        result.push_back(1.0d);
      }
      return result;
    } // }}}
    void InputKey(char ch) // {{{
    { if (ch==myUp)
        myDir=0;
      else if (ch==myDown)
        myDir=1;
      else if (ch==myLeft)
        myDir=2;
      else if (ch==myRight)
        myDir=3;
    } // }}}

  private:
    char myUp, myDown, myLeft, myRight;
    char myDir;
}; // }}}

void Input(vector<Player*> &players) // {{{
{
  // Håndter handlinger
  for (int ch=getch(); ch!=ERR; ch=getch())
  { for (auto it=players.begin(); it!=players.end(); ++it)
      (*it)->InputKey(ch);
  }
} // }}}

void TestSnake(vector<Network*> &models, vector<Player*> &players, vector<RLSnake::snake> &snakes, size_t width, size_t height, double randomness) // {{{
{ // Create game
  srand(time(NULL)); // Fix (random) map
  RLSnake game(width,height,snakes);
  // Create player networks
  vector<vector<double> > states;
  for (size_t i=0; i<snakes.size(); ++i)
    states.push_back(game.State());

  size_t step=0;
  for (step=0; true; ++step)
  { //clear();
    states[game.Turn()]=game.State();
    stringstream ss;
    for (size_t i=0; i<players.size(); ++i)
    { ss << "Player " << i << ": ";
      for (size_t j=0; j<states[i].size(); ++j)
      { ss.precision(2);
        ss << states[i][j] << ", ";
      }
      ss << endl;
    }
    ss << "Step: " << step << "\n\r"
       << game.GameString();
    mvaddstr(0,0,ss.str().c_str());
    wrefresh(stdscr);
    // Perform step
    if (game.Done())
      break;
    if (game.Turn()<models.size())
      game.RLGame::Step(models,randomness);
    else
      game.Step(players[game.Turn()-models.size()]->Keys());

    if (game.LiveSnakes()>0)
      this_thread::sleep_for(chrono::milliseconds(150/(game.LiveSnakes()+1)));
    Input(players);
  }

  // Clean up
  while (!models.empty())
  { delete models.back();
    models.pop_back();
  }
  return;
} // }}}

int main(int argc, char **argv) // {{{
{ initscr();
  start_color();
  cbreak();
  noecho();
  keypad(stdscr,true);
  nodelay(stdscr,true);

  double opt_randomness=0.01d;
  size_t opt_width;
  size_t opt_height;
  getmaxyx(stdscr,opt_height,opt_width);
  opt_width=opt_width-3;
  opt_height=opt_height-10;

  try
  { if (argc<2)
    { endwin();
      cout << "Usage: " << argv[0] << " [-m <model path> <fov>]* [-p <keys>]* [-rnd|--randomness <double=0.01>] [-w|--width <int="<<opt_width<<">] [-h|--height <int="<<opt_height<<">]" << endl;
      return 0;
    }

    vector<Network*> models;
    vector<Player*> players;
    vector<RLSnake::snake> snakes;

    players.push_back(new Player('w','s','a','d'));
    for (size_t i=1; i<argc; ++i)
    { if (i+1<argc && string("-m")==argv[i])
      { ifstream fin(argv[++i]);
        Network *model=new Network(1);
        model->LoadParameters(fin);
        fin.close();
        models.push_back(model);
        RLSnake::snake s;
        stringstream ss;
        ss << argv[++i];
        ss >> s.myFov;
        snakes.push_back(s);
      }
      else if (i+1<argc && string("-p")==argv[i])
      { ++i;
        RLSnake::snake s;
        s.myFov=0;
        snakes.push_back(s);
        cerr << "Player snakes not yet supported" << endl;
      }
      else if (i+1<argc && (string("-rnd")==argv[i] || string("--randomness")==argv[i]))
      { stringstream ss;
        ss << argv[++i];
        ss >> opt_randomness;
      }
      else if (i+1<argc && (string("-w")==argv[i] || string("--width")==argv[i]))
      { stringstream ss;
        ss << argv[++i];
        ss >> opt_width;
      }
      else if (i+1<argc && (string("-h")==argv[i] || string("--height")==argv[i]))
      { stringstream ss;
        ss << argv[++i];
        ss >> opt_height;
      }
      else
        cerr << "Unknown argument " << argv[i];
    }
    TestSnake(models,players,snakes,opt_width,opt_height,opt_randomness);
    while (!models.empty())
    { delete models.back();
      models.pop_back();
    }
    while (!players.empty())
    { delete players.back();
      players.pop_back();
    }
    usleep(3000000);
    endwin();
    return 0;
  }
  catch (const string &s)
  { endwin();
    cout << "Error: " << s << endl;
    return -1;
  }
} // }}}

