#pragma once
#include <slowml/network.hpp>
#include <slowml/vectordata.hpp>
#include <vector>
#include <sstream>
#include <iostream>

class RLGame
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
    std::vector<double> Eval(const std::vector<Network*> &models) const // {{{
    { RLGame *g=Copy();
      while (!g->Done())
      { g->Step(models[g->Turn()]->Eval(g->State()));
      }
      vector<double> res=g->Score();
      delete g;
      return res;
    } // }}}

    //! MakeRLData simulates games to create trainingdata
    void MakeRLData(std::vector<Network*> &models, std::vector<std::vector<double> > &inputs, std::vector<std::vector<std::vector<double> > > &results) const // {{{
    { RLGame *state=Copy();
      while (!state->Done())
      { vector<double> input=state->State();
        size_t player=state->Turn();
        vector<double> choices=models[player]->Eval(input);
    
        // Get chosen next state and its score with current model
        RLGame *nstate=state->Copy();
        nstate->Step(choices);
        vector<double> scores=nstate->Eval(models);
        vector<double> right_choices=choices;
        double best_score=RelativeScore(scores,player);
        // Compare possible choices, and select best
        vector<vector<double> > achoices=state->Choices();
        for (size_t a=0; a<achoices.size(); ++a)
        { 
          RLGame *astate=state->Copy();
          astate->Step(achoices[a]);
          vector<double> ascores=astate->Eval(models);
          double ascore=RelativeScore(ascores,player);
          if (ascore>=best_score)
          { 
            best_score=ascore;
            right_choices=achoices[a];
          }
          // Clean up
          delete astate;
        }
        inputs[player].insert(inputs[player].end(),input.begin(),input.end());
        results[player].push_back(right_choices);
        delete state;
        state=nstate;
       }
       delete state;
    
       return;
    } // }}}

    //! Train networks to play game using Reinforcement Learning
    void TrainRLGame(vector<Network*> &models, size_t sims, size_t reps, double ainv, double lambda) const // {{{
    { vector<vector<double> > inputs;
      vector<vector<vector<double> > > results;
      for (size_t player=0; player<Players(); ++player)
      {  inputs.push_back(vector<double>());
         results.push_back(vector<vector<double> >());
      }
    
      // Play game from different positions
      for (size_t sim=0; sim<sims; ++sim)
      { RLGame *g=Copy();
        g->Init();
        g->MakeRLData(models,inputs,results);
        delete g;
      }
    
      for (size_t player=0; player<Players(); ++player)
      { double alpha=ainv;
        VectorData<double> data(inputs[player],models[player]->InputSize(),inputs[player].size()/models[player]->InputSize());
        GuidedVectorData<double,vector<double> > trainingdata(data,results[player]);
        double precost=models[player]->Cost(trainingdata,lambda);
        models[player]->FitParameters(trainingdata,alpha,lambda,reps,false);
        double postcost=models[player]->Cost(trainingdata,lambda);
        //cout << "TrainRLGame: Player " << player << " cost " << precost << " -> " << postcost << endl;
      }
    } // }}}

  private:
    static double RelativeScore(const std::vector<double> &scores, size_t player) // {{{
    { double oscore=0.0d;
      for (size_t i=0; i<scores.size(); ++i)
      { if (i!=player && scores[i]>oscore)
          oscore=scores[i];
      }
      return scores[player]-oscore;
    } // }}}
};

