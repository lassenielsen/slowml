#pragma once

class ForkAndTrim
{ public:
    ForkAndTrim(Model &m, double alpha=0.1d) {
    virtual ~ForkAndTrim();

    virtual float Test(Model &m, size_t reps=1)=0;
    void Train(double alpha=0.1d, size_t clonings=10, size_t reps=1);

  private:
    std::vector<Model*> myPopulation;
};
