#pragma once
#include <slowml/shallowmodel.hpp>

class LogisticRegressionModel : public ShallowModel<double> // {{{
{ public:
    LogisticRegressionModel();
    virtual ~LogisticRegressionModel();

    std::string Type() const { return "Logistic Regression Model"; }
    double Eval(const std::vector<std::vector<double>> &instances, size_t pos) const;
    double Eval(const std::vector<double> &instance) const;
    double LogDistance(double guess, double truth);
    double Cost(const std::vector<std::vector<double>> &instances, const std::vector<double> &truths, double lambda=1.0);
    void AddDelta(const vector<vector<double> > &inputs, size_t pos, std::vector<double> &deltasum, const double &diff);
    std::vector<double> Delta(const std::vector<std::vector<double>> &instances, const std::vector<double> &truths, double lambda=1.0);
}; // }}}
