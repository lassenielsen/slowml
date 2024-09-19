#pragma once
#include <slowml/network.hpp>
#include <slowml/vectordata.hpp>
#include <vector>
#include <sstream>
#include <iostream>
#include <thread>
#include <mutex>

class RLGame;

struct MakeRLDataArg // {{{
{ MakeRLDataArg(const RLGame &game, const std::vector<Network*> &models, size_t sims, size_t limit, std::mutex &m, vector<set<vector<double>>> &states);
  const RLGame &myGame;
  const std::vector<Network*> &myModels;
  std::vector<std::vector<double> > myInputs;
  std::vector<std::vector<std::vector<double> > > myResults;
  size_t mySims;
  size_t myLimit;
  std::mutex &myMutex;
  vector<set<vector<double> > > &myStates;
}; // }}}
void MakeRLData(MakeRLDataArg *arg);

class RLGame // {{{
{ public:
    virtual ~RLGame() {}
    //! Copy creates a copy of the current game state.
    virtual RLGame *Copy() const=0;
    //! Init initializes the game for a new game with a random position
    virtual void Init()=0;

    //! State generates the vector-input representing the current state.
    virtual std::vector<double> State()=0;
    //! List the possible choices in the current state.
    virtual std::vector<std::vector<double> > Choices()=0;
    //! Step tells the game to proceed with the provided choices for the current player.
    virtual void Step(const std::vector<double> &inputs)=0;
    //! Players return the number of players in the game
    virtual size_t Players() const=0;
    //! Turn trturns the index of the playwe whose turn it is.
    virtual size_t Turn() const=0;
    //! Done returns true if the game is complete, and false otherwise.
    virtual bool Done() const=0;
    /*! Score returns the accumulated scores for each player in the current state.
     ** Higher scores are better.
     */
    virtual std::vector<double> Score() const=0;


    /*! Eval is a function, that evaluates a model, and return the score for each player.
     ** Higher scores are better.
     */
    std::vector<double> Eval(const std::vector<Network*> &models, size_t limit=100) const // {{{
    { RLGame *g=Copy();
      while (0<limit && !g->Done())
      { g->Step(models[g->Turn()]->Eval(g->State()));
        --limit;
      }
      vector<double> res=g->Score();
      delete g;
      return res;
    } // }}}

    //! Train networks to play game using Reinforcement Learning
    void TrainRLGame(vector<Network*> &models, size_t sims, size_t reps, double ainv, double lambda, size_t limit=100, bool debug=false) const // {{{
    { vector<vector<double> > inputs;
      vector<vector<vector<double> > > results;

      std::mutex mtx;
      vector<set<vector<double>> > statehashes;

      for (size_t player=0; player<Players(); ++player)
      {  inputs.push_back(vector<double>());
         results.push_back(vector<vector<double> >());
         statehashes.push_back(set<vector<double> >());
      }

      // Start threads to generate trainingdata    
      size_t cores=std::thread::hardware_concurrency();
      vector<MakeRLDataArg*> args;
      vector<thread> threads;
      // Start threads
      for (size_t core=0; core<cores; ++core)
      { MakeRLDataArg *arg=new MakeRLDataArg(*this,models,sims*(core+1)/cores-sims*core/cores,limit,mtx,statehashes);
        args.push_back(arg);
        threads.push_back(thread(MakeRLData,arg));
      }
      // Collect results
      for (size_t core=0; core<cores; ++core)
      { threads[core].join();
        for (size_t player=0; player<Players(); ++player)
        { inputs[player].insert(inputs[player].end(),args[core]->myInputs[player].begin(),args[core]->myInputs[player].end());
          results[player].insert(results[player].end(),args[core]->myResults[player].begin(),args[core]->myResults[player].end());
        }
        delete args[core];
      }

      // Train models on trainingdata
      for (size_t player=0; player<Players(); ++player)
      { double alpha=ainv;
        VectorData<double> data(inputs[player],models[player]->InputSize(),inputs[player].size()/models[player]->InputSize());
        GuidedVectorData<double,vector<double> > trainingdata(data,results[player]);
        double precost=models[player]->Cost(trainingdata,lambda);
        models[player]->FitParameters(trainingdata,alpha,lambda,reps,false);
        double postcost=models[player]->Cost(trainingdata,lambda);
        if (debug)
          cout << "TrainRLGame: Player " << player << " on " << results[player].size() << " truths from cost " << precost << " to " << postcost << endl;
      }
    } // }}}

}; // }}}

MakeRLDataArg::MakeRLDataArg(const RLGame &game, const std::vector<Network*> &models, size_t sims, size_t limit, std::mutex &m, vector<set<vector<double> > > &states) // {{{
: myGame(game)
, myModels(models)
, mySims(sims)
, myLimit(limit)
, myMutex(m)
, myStates(states)
{ for (size_t player=0; player<myGame.Players(); ++player)
  {  myInputs.push_back(vector<double>());
     myResults.push_back(vector<vector<double> >());
  }
} // }}}

double RelativeScore(const std::vector<double> &scores, size_t player) // {{{
{ double oscore=0.0d;
  for (size_t i=0; i<scores.size(); ++i)
  { if (i!=player && scores[i]>oscore)
      oscore=scores[i];
  }
  return scores[player]-oscore;
} // }}}

//! MakeRLData plays the RLGame using the provided models as players to create trainingdata
void MakeRLData(MakeRLDataArg *arg) // {{{
{ for (size_t sim=0; sim<arg->mySims; ++sim)
  { RLGame *state=arg->myGame.Copy();
    state->Init();
    for (size_t limit=0; limit<arg->myLimit && !state->Done(); ++limit)
    { vector<double> input=state->State();
      size_t player=state->Turn();
      vector<double> choices=arg->myModels[player]->Eval(input);

      arg->myMutex.lock();
      bool old=arg->myStates[player].find(input)!=arg->myStates[player].end();
      if (!old)
        arg->myStates[player].insert(input);
      arg->myMutex.unlock();
      if (old) // Old state
      { state->Step(choices);
        continue;
      }

      // Get chosen next state and its score with current model
      vector<double> scores=state->Eval(arg->myModels,arg->myLimit+1);
      vector<double> right_choices=choices;
      double best_score=RelativeScore(scores,player);
      double worst_score=best_score;
      // Compare possible choices, and select best
      vector<vector<double> > achoices=state->Choices();
      for (size_t a=0; a<achoices.size(); ++a)
      { 
        RLGame *astate=state->Copy();
        astate->Step(achoices[a]);
        vector<double> ascores=astate->Eval(arg->myModels,arg->myLimit);
        double ascore=RelativeScore(ascores,player);
        if (ascore>best_score)
        { 
          best_score=ascore;
          right_choices=achoices[a];
        }
        else if (ascore<worst_score)
          worst_score=ascore;
        // Clean up
        delete astate;
      }
      if (best_score>worst_score) // Is training meaningful?
      { 
        arg->myInputs[player].insert(arg->myInputs[player].end(),input.begin(),input.end());
        arg->myResults[player].push_back(right_choices);
      }
      state->Step(right_choices);
    }
    delete state;
  }
  return;
} // }}}
