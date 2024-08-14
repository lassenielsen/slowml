#pragma once
#include<slowml/shallowmodel.hpp>

class LinearRegressionModel : public ShallowModel<double> // {{{
{ public:
    LinearRegressionModel();
    virtual ~LinearRegressionModel();

    std::string Type() { return "Linear Regression Model"; }
    double Eval(const Data<double> &instances, size_t pos) const;
    double Eval(const std::vector<double> &instance) const;
    double SquareDistance(const double &lhs, const double &rhs);
    double Cost(const GuidedData<double,double> &instances, double lambda=1.0);
    void AddDelta(const Data<double> &inputs, size_t pos, std::vector<double> &deltasum, const double &diff);
    std::vector<double> Delta(const GuidedData<double,double> &instances,double lambda=1.0);
}; // }}}

