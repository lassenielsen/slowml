#pragma once
#include <slowml/shallowmodel.hpp>

class LogisticRegressionModel : public ShallowModel<double> // {{{
{ public:
    LogisticRegressionModel();
    virtual ~LogisticRegressionModel();

    std::string Type() { return "Logistic Regression Model"; }
    double Eval(const Data<double> &instances, size_t pos);
    double Eval(const std::vector<double> &instance);
    double LogDistance(double guess, double truth);
    double Cost(const GuidedData<double,double> &instances, double lambda=1.0);
    void AddDelta(const Data<double> &inputs, size_t pos, std::vector<double> &deltasum, const double &diff);
    std::vector<double> Delta(const GuidedData<double,double> &instances, double lambda=1.0);
}; // }}}
