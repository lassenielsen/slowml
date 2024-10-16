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
#include <chrono>
#include <thread>

using namespace std;

#include "rlsnake.hpp"
void TrainSnake(vector<Network*> models, size_t width, size_t height, size_t its, size_t reps, size_t sims, double alphainv, double lambda, double randomness) // {{{
{ // Create game
  RLSnake game(width,height);
  cout << "Input width: " << game.State().size() << endl;
  // Create player networks

  // Train model
  for (size_t it=0; it<its; ++it)
  { game.TrainRLGame(models,sims,reps,alphainv,lambda,500,false,0.0,true);
    double score=0.0d;
    for (size_t i=0; i<1000; ++i)
    {
      auto gc=game.Copy();
      score+=gc->Eval(models,500,0.1)[0];
      delete gc;
    }
    cout << "Iteration " << it << ": Average points after 500 steps: " << score/1000.0 << endl;
  }
  return;
} // }}}

int main(int argc, char **argv) // {{{
{ if (argc<2)
  { cerr << "Syntax: " << argv[0] << "[--continue|-c <model path>]* [--network|-n <network> <model path>]* [--iterations|-i <int] [--repetitions|-r <int>] [--simulations|-s <int>] [--gd_alphainv|-gda <double>] [--gd_lambda|-gdl <double>]" << endl;
    return 0;
  }
  vector<Network*> models;
  vector<string> paths;
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
    }
    else if (i+2<argc && (arg=="-n" || arg=="--network"))
    { string network=string(argv[++i]);
      string path=string(argv[++i]);
      Network *model=Network::Parse(network);
      models.push_back(model);
      paths.push_back(path);
    }
    else if (i+1<argc && (arg=="-i" || arg=="--iterations"))
    { stringstream ss;
      ss << string(argv[++i]);
      ss >> opt_its;
    }
    else if (i+1<argc && (arg=="-r" || arg=="--repetitions"))
    { stringstream ss;
      ss << string(argv[++i]);
      ss >> opt_reps;
    }
    else if (i+1<argc && (arg=="-s" || arg=="--simulations"))
    { stringstream ss;
      ss << string(argv[++i]);
      ss >> opt_sims;
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
    else
      cerr << "Unknown option " << arg << endl;
  }
  try
  { TrainSnake(models, opt_width, opt_height, opt_its, opt_reps, opt_sims, opt_alphainv, opt_lambda, opt_randomness);
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
