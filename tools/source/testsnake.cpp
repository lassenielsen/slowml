#include <slowml/network.hpp>
//#include <cmath>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include "rlsnake.hpp"

bool TestSnake(const string &model_name, size_t width, size_t height) // {{{
{ // Create game
  srand(time(NULL)); // Fix (random) map
  RLSnake game(width,height);
  // Create player networks
  vector<Network*> models;
  models.push_back(new Network(101));
  ifstream inet(model_name);
  models[0]->LoadParameters(inet);
  inet.close();

  size_t step=0;
  for (size_t step=0; !game.Done(); ++step)
  { cout << "Step: " << step << " score: " << game.Score()[0] << endl
         << game.GameString() << endl;
    // Perform step
    game.RLGame::Step(models,0.1);
    this_thread::sleep_for(chrono::milliseconds(50));
  }
  bool result=game.Score()[0]>=10.0;

  // Clean up
  while (!models.empty())
  { delete models.back();
    models.pop_back();
  }
  return result;
} // }}}

int main(int argc, char **argv) // {{{
{ try
  { if (argc<2)
      cout << "Usage: " << argv[0] << " <snake model filename>" << endl;
    else
    { cout << "TestRL3 - Testing snake model " << argv[1] << endl;
      TestSnake(argv[1],80,19);
    }
  }
  catch (const string &s)
  { cout << "Error: " << s << endl;
  }
} // }}}

