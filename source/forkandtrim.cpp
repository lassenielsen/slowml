#include <slowml/forkandtrim.hpp>
#include <numeric>
#include <algorithm>

using namespace std;

ForkAndTrim::ForkAndTrim(Model<double> &m, size_t population, double alpha) // {{{
{ for (size_t i=0; i<population; ++i)
  { myPopulation.push_back(Clone(m,alpha));
  }
} // }}}

ForkAndTrim::~ForkAndTrim() // {{{
{ while (!myPopulation.empty())
  { delete myPopulation.back();
    myPopulation.pop_back();
  }
} // }}}

void ForkAndTrim::Train(double alpha, size_t clonings, size_t reps, size_t treps) // {{{
{ for (size_t r=0; r<reps; ++r)
  { vector<Model<double>*> ng; // The Next Generation
    // Create the clones
    for (size_t i=0; i<myPopulation.size(); ++i)
    { for (size_t c=0; c<clonings; ++c)
      { ng.push_back(Clone(*myPopulation[i], alpha));
      }
    }
    vector<double> ngscores(ng.size(),0.0);
    for (size_t ngi=0; ngi<ng.size(); ++ngi)
    { ngscores[ngi]=Test(*ng[ngi],treps);
    }
    // Find sorting
    std::vector<int> indices(ng.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(),
         [&](int A, int B) -> bool {
              return ngscores[A] < ngscores[B];
          });
    for (size_t i=0; i<ng.size(); ++i)
    { if (i<myPopulation.size()) // Use model in population
      { delete myPopulation[i];
        myPopulation[i]=ng[indices[ng.size()-1-i]];
      }
      else                       // Clean up
      { delete ng[indices[ng.size()-1-i]];
      }
    }
  }
} // }}}
