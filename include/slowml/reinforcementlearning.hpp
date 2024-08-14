#pragma once
#include <slowml/network.hpp>
#include <slowml/vectordata.hpp>
#include <vector>
#include <sstream>
#include <iostream>

class RLGame
{ public:
    //! Copy creates a copy of the current game state.
    virtual RLGame *Copy() const=0;
    //! State generates the vector-input representing the current state.
    virtual std::vector<double> State()=0;
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
};

double RelativeScore(const std::vector<double> &scores, size_t player) // {{{
{ double oscore=0.0d;
  for (size_t i=0; i<scores.size(); ++i)
  { if (i!=player && scores[i]>oscore)
      oscore=scores[i];
  }
  return scores[player]-oscore;
} // }}}

std::string vecstr(const std::vector<double> &vec)
{ stringstream ss;
  for (size_t i=0; i<vec.size(); ++i)
  { if (i!=0)
      ss << ", ";
    ss << vec[i];
  }
  return ss.str();
}
void MakeRLData(std::vector<Network*> &models, const RLGame &game, std::vector<std::vector<double> > &inputs, std::vector<std::vector<std::vector<double> > > &results) // {{{
{ RLGame *state=game.Copy();

  while (!state->Done())
  { vector<double> input=state->State();
    size_t player=state->Turn();
    //std::cout << "MakeRLData input: " << vecstr(input) << std::endl;
    //std::cout << "MakeRLData player: " << player << std::endl;
    vector<double> choices=models[player]->Eval(input);
    //std::cout << "MakeRLData choices: " << vecstr(choices) << std::endl;

    // Get chosen next state and its score with current model
    RLGame *nstate=state->Copy();
    nstate->Step(choices);
    vector<double> scores=nstate->Eval(models);
    //std::cout << "MakeRLData scores: " << vecstr(scores) << std::endl;
    vector<double> right_choices=choices;
    double best_score=RelativeScore(scores,player);
    //std::cout << "MakeRLData score: " << best_score << std::endl;
    // Compare possible choices, and select best
    vector<vector<double> > achoices=state->Choices();
    for (size_t a=0; a<achoices.size(); ++a)
    { 
      //std::cout << "MakeRLData alternative: " << vecstr(achoices[a]) << std::endl;
      RLGame *astate=state->Copy();
      astate->Step(achoices[a]);
      vector<double> ascores=astate->Eval(models);
      //std::cout << "MakeRLData ascores: " << vecstr(ascores) << std::endl;
      double ascore=RelativeScore(ascores,player);
      //std::cout << "MakeRLData ascore: " << ascore << std::endl;
      if (ascore>=best_score)
      { 
        //std::cout << "MakeRLData using alternative!" << std::endl;
        best_score=ascore;
        right_choices=achoices[a];
      }
      // Clean up
      delete astate;
    }
    //for (size_t i=0; i<choices.size(); ++i)
    //{ std::cout << "MakeRLData index: " << i << endl;
    //  vector<double> achoices=choices;
    //  achoices[i]=choices[i]>=0.5d?0.0d:1.0d;
    //  std::cout << "MakeRLData achoice: " << vecstr(achoices) << endl;
    //  // Get anternative next state and its score with current model
    //  RLGame *astate=state->Copy();
    //  astate->Step(achoices);
    //  std::vector<double> ascores=astate->Eval(models);
    //  std::cout << "MakeRLData ascores: " << vecstr(ascores) << std::endl;
    //  // Test if score improves, and use best scoring choice as right answer
    //  right_choices.push_back(RelativeScore(scores,player)>=RelativeScore(ascores,player)?1.0-achoices[i]:achoices[i]);
    //  std::cout << "MakeRLData right_choices: " << vecstr(right_choices) << std::endl;
    //  // Clean up
    //  delete astate;
    //}
    inputs[player].insert(inputs[player].end(),input.begin(),input.end());
    results[player].push_back(right_choices);
    delete state;
    state=nstate;
   }
   delete state;

   return;
} // }}}
