#pragma once
#include<slowml/shallowmodel.hpp>

class LinearRegressionModel : public ShallowModel<double> // {{{
{ public:
    LinearRegressionModel();
    virtual ~LinearRegressionModel();

    std::string Type() const { return "Linear Regression Model"; }
    double Eval(const vector<vector<double> > &instances, size_t pos) const;
    double Eval(const std::vector<double> &instance) const;
    double SquareDistance(const double &lhs, const double &rhs);
    double Cost(const vector<vector<double> > &instances, const vector<double> &truths, double lambda=1.0);
    void AddDelta(const vector<vector<double> > &inputs, size_t pos, std::vector<double> &deltasum, const double &diff);
    std::vector<double> Delta(const vector<vector<double> > &instances, const vector<double> &truths,double lambda=1.0);
}; // }}}

