#pragma once
#include <slowml/model.hpp>
#include <vector>

class ForkAndTrim
{ public:
    ForkAndTrim(Model<double> &m, size_t population=1, double alpha=0.1d);
    virtual ~ForkAndTrim();

    void Train(double alpha=0.1d, size_t clonings=10, size_t reps=1, size_t treps=1);
    const std::vector<Model<double>*> &GetPopulation() const { return myPopulation; }

  private:
    /*! Test is a function, that evaluates a model, and returns a score.
     ** Higher scores are better.
     */
    virtual double Test(Model<double> &m, size_t reps=1) const=0;
    /*! Clone a function, that creates clones of the models used in the population.
     ** Each clone has random changes scaled by alpha.
     */
    virtual Model<double> *Clone(Model<double> &m, double alpha) const=0;

    std::vector<Model<double>*> myPopulation;
};
