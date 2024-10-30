#include <slowml/linearregressionmodel.hpp>
#include <slowml/logisticregressionmodel.hpp>
#include <slowml/guidedvectordata.hpp>
#include <slowml/network.hpp>
#include <slowml/onevsall.hpp>
#include <slowml/reinforcementlearning.hpp>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>

using namespace std;

#include "rlsnake_multiplayer.hpp"
void TrainSnake(vector<Network*> models, vector<RLSnake::snake> &snakes, size_t width, size_t height, size_t its, size_t reps, size_t sims, double alphainv, double lambda, double randomness) // {{{
{ // Create game
  RLSnake game(width,height,snakes);
  //cout << "Input width: " << game.State().size() << endl;
  // Create player networks

  // Train model
  for (size_t it=0; it<its; ++it)
  { game.TrainRLGame(models,sims,reps,alphainv,lambda,500,true,0.0,true);
    vector<double> scores(models.size(),0.0d);
    for (size_t i=0; i<1000; ++i)
    {
      auto gc=game.Copy();
      vector<double> s=gc->Eval(models,500,0.1);
      delete gc;
      for (size_t i=0; i<s.size(); ++i)
        scores[i]+=s[i];
    }
    cout << "After iteraion " <<it << endl;
    for (size_t i=0; i<scores.size(); ++i)
      cout << "Player " << i << " scored " << scores[i]/1000.0d << " average points after 500 steps." << endl;
  }
  return;
} // }}}

int main(int argc, char **argv) // {{{
{ if (argc<2)
  { cerr << "Syntax: " << argv[0] << "[--continue|-c <model path> <fov>]* [--network|-n <network> <model path> <fov>]* [--iterations|-i <int=100>] [--simulations|-s <int=500>] [--randomness|-rnd <double=0.0>] [--repetitions|-gdr|-r <int=500>] [--gd_alphainv|-gda <double=0.0>] [--gd_lambda|-gdl <double=0.0001>] [-w|--width <int=20>] [-h|--height <int=20>]" << endl;
    return 0;
  }
  vector<Network*> models;
  vector<string> paths;
  vector<RLSnake::snake> snakes;
  size_t opt_width=19;
  size_t opt_height=19;
  // opt_map
  size_t opt_its=100;
  size_t opt_reps=500;
  size_t opt_sims=500;
  double opt_alphainv=0.0;
  double opt_lambda=0.0001;
  double opt_randomness=0.0;

  for (size_t i=1; i<argc-1; ++i)
  { string arg(argv[i]);
    if (i+1<argc && arg=="-c" || arg=="--continue")
    { string path=string(argv[++i]);
      Network *model=new Network(1);
      ifstream fin(path);
      model->LoadParameters(fin);
      fin.close();
      models.push_back(model);
      paths.push_back(path);
      RLSnake::snake s;
      stringstream ss;
      ss << argv[++i];
      ss >> s.myFov;
      snakes.push_back(s);
    }
    else if (i+2<argc && (arg=="-n" || arg=="--network"))
    { string network=string(argv[++i]);
      string path=string(argv[++i]);
      Network *model=Network::Parse(network);
      models.push_back(model);
      paths.push_back(path);
      RLSnake::snake s;
      stringstream ss;
      ss << argv[++i];
      ss >> s.myFov;
      snakes.push_back(s);
    }
    else if (i+1<argc && (arg=="-i" || arg=="--iterations"))
    { stringstream ss;
      ss << string(argv[++i]);
      ss >> opt_its;
    }
    else if (i+1<argc && (arg=="-s" || arg=="--simulations"))
    { stringstream ss;
      ss << string(argv[++i]);
      ss >> opt_sims;
    }
    else if (i+1<argc && (arg=="-rnd" || arg=="--randomness"))
    { stringstream ss;
      ss << string(argv[++i]);
      ss >> opt_randomness;
    }
    else if (i+1<argc && (arg=="-r" || arg=="-gdr" || arg=="--repetitions"))
    { stringstream ss;
      ss << string(argv[++i]);
      ss >> opt_reps;
    }
    else if (i+1<argc && (arg=="-gda" || arg=="--gd_alphainv"))
    { stringstream ss;
      ss << string(argv[++i]);
      ss >> opt_alphainv;
    }
    else if (i+1<argc && (arg=="-gdl" || arg=="--gd_lambda"))
    { stringstream ss;
      ss << string(argv[++i]);
      ss >> opt_lambda;
    }
    else if (i+1<argc && (arg=="-w" || arg=="--width"))
    { stringstream ss;
      ss << string(argv[++i]);
      ss >> opt_width;
    }
    else if (i+1<argc && (arg=="-h" || arg=="--height"))
    { stringstream ss;
      ss << string(argv[++i]);
      ss >> opt_height;
    }
    else
      cerr << "Unknown option " << arg << endl;
  }
  try
  { TrainSnake(models, snakes, opt_width, opt_height, opt_its, opt_reps, opt_sims, opt_alphainv, opt_lambda, opt_randomness);
    for (size_t i=0; i<models.size(); ++i)
    { cout << "Saving model " << paths[i] << endl;
      ofstream fout(paths[i]);
      models[i]->SaveParameters(fout);
      fout.close();
    }
  }
  catch (const std::string &error)
  { cerr << "Error: " << error << endl;
  }
} // }}}
