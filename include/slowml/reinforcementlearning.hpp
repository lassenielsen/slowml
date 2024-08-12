#pragma once
#include <slowml/network.hpp>
#include <slowml/vectordata.hpp>
#include <vector>

class RLGame
{ public:
    //! Copy creates a copy of the current game state.
    virtual RLGame *Copy() const=0;
    //! State generates the vector-input representing the current state.
    virtual std::vector<double> State()=0;
    /*! Eval is a function, that evaluates a model, and return the score for each player.
     ** Higher scores are better.
     */
    std::vector<double> Eval(const std::vector<Network> &models) const // {{{
    { RLGame *g=Copy();
      while (!g->Done())
      { g->Step(models[g->Turn()].Eval(g->State()));
      }
      vector<double> res=g->Score();
      delete g;
      return res;
    } // }}}

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
};

std::vector<GuidedVectorData<double,std::vector<double> > > MakeRLData(std::vector<Network> &models, const RLGame &game);

double RelativeScore(const std::vector<double> &scores, size_t player) // {{{
{ double oscore=0.0d;
  for (size_t i=0; i<scores.size(); ++i)
  { if (i!=player && scores[i]>oscore)
      oscore=scores[i];
  }
  return scores[player]-oscore;
} // }}}

std::vector<GuidedVectorData<T,R> > MakeRLData(std::vector<Network> &models, const RLGame &game) // {{{
{ std::vector<VectorData<T> > tests;
  for (size_t i=0; i<game.Players(); ++i)
    tests.push_back(VectorData(std::vector<double>(),models[i].LayerSize(models[i].Layers()-1),0);
  std::vector<std::vector<std::vector<double> > > results;

  RLGame *state=game.Copy();

  while (!state->Done())
  { vector<double> statevec=state->State();
    size_t player=state->Turn();
    vector<double> choices=model[player].Eval(statevec);

    // Get chosen next state and its score with current model
    RLGame *nstate=state->Step(choices);
    vector<double> scores=nstate->Eval(model);
    vector<double> right_choices;
    // Test
    for (size_t i=0; i<choices.size(); ++i)
    { vector<double> achoices=choices;
      achoices[i]=choices[i]>=0.5d?0.0d:1.0d;
      // Get anternative next state and its score with current model
      RLGame *astate=state->Copy();
      astate->Step(achoices);
      std::vector<double> ascores=astate->Eval(model);
      // Test if score improves, and use best scoring choice as right answer
      right_choices.push_back(RelativeScore(scores,player)>=RelativeScore(ascore,player)?choices[i]:achoices[i]);
      // Clean up
      delete astate;
    }
    tests[player].push_back(statevec);
    results[player].push_back(right_choices);
    delete state;
    state=nstate;
   }
   delete state;
   // Create result and return
   vector<GuidedVectorData<double,std::vector<double> > > result;
   for (size_t i=0; i<tests.size(); ++i)
     result.push_back(GuidedVectorData<double,std::vector<double> >(tests[i],results[i]));
   return result;
} // }}}
