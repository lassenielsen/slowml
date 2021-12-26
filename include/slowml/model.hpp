#pragma once
#include <slowml/data.hpp>
#include <slowml/guideddata.hpp>
#include <vector>
#include <iostream>
#include <sstream>
#include <ostream>
#include <istream>

using namespace std;

template<class Result> class Model // {{{
{ public:
    Model();
    virtual ~Model();

    virtual std::string Type()=0;

    virtual size_t CountParameters() const=0;
    //virtual void AddParameter(double parameter=0.0)=0;
    virtual double GetParameter(size_t i) const=0;
    virtual void SetParameter(size_t i, double val)=0;

    virtual Result Eval(const Data<double> &instances, size_t pos)=0;
    virtual Result Eval(const std::vector<double> &instance)=0;
    virtual double Cost(const GuidedData<double,Result> &instances, double lambda=1.0)=0;
    virtual std::vector<double> Delta(const GuidedData<double,Result> &instances, double lambda=1.0)=0;

    virtual void SaveParameters(std::ostream &dest, bool saveSize=true) const=0;
    virtual void LoadParameters(std::istream &src, bool loadSize=true)=0;

    void FitParameters(GuidedData<double,Result> &instances, double &alpha_inv, double lambda=1.0, size_t repetitions=100, bool debug=false);
}; // }}}

template<class Result> Model<Result>::Model() // {{{
{
} // }}}

template<class Result> Model<Result>::~Model() // {{{
{
} // }}}

template<class Result> void Model<Result>::FitParameters(GuidedData<double,Result> &instances, double &alpha_inv, double lambda, size_t repetitions, bool debug) // {{{
{ bool alpha_dynamic=false;
  double cost=0;
  if (alpha_inv==0.0)
  { alpha_dynamic=true;
    alpha_inv=50.0;
    cost=Cost(instances,lambda);
  }
  for (size_t r=0; r<repetitions; ++r)
  { // Calculate Delta
    std::vector<double> delta=Delta(instances,lambda);
    // Simultaneous update of parameters
    for (size_t p=0; p<CountParameters(); ++p)
      SetParameter(p,GetParameter(p)-delta[p]/alpha_inv);
    if (debug)
    { for (size_t p=0; p<CountParameters(); ++p)
        std::cout << "  delta_" << p << "=" << delta[p] << std::endl;
    }
    if (alpha_dynamic)
    { double tmp_cost=Cost(instances,lambda);
      double new_cost=cost;
      // Try increasing alpha
      while (tmp_cost<new_cost)
      {
        for (size_t p=0; p<CountParameters(); ++p)
          SetParameter(p,GetParameter(p)+delta[p]/alpha_inv);
        alpha_inv-=1.0;
        for (size_t p=0; p<CountParameters(); ++p)
          SetParameter(p,GetParameter(p)-delta[p]/alpha_inv);
        new_cost=tmp_cost;
        tmp_cost=Cost(instances,lambda);
        if (tmp_cost>=new_cost)
        { // Reverse last step
          for (size_t p=0; p<CountParameters(); ++p)
            SetParameter(p,GetParameter(p)+delta[p]/alpha_inv);
          alpha_inv+=1.0;
          for (size_t p=0; p<CountParameters(); ++p)
            SetParameter(p,GetParameter(p)-delta[p]/alpha_inv);
          tmp_cost=new_cost;
          break;
        }
      }
      new_cost=tmp_cost;
      // Decrease alpha until decreasing cost
      while (new_cost>cost && alpha_inv<30000)
      { if (debug)
          std::cout << ".";//"LinearRegressionModel::FitParameters: cost increased - updating alpha" << std::endl;
        for (size_t p=0; p<CountParameters(); ++p)
          SetParameter(p,GetParameter(p)+delta[p]/alpha_inv);
        alpha_inv+=1.0;
        for (size_t p=0; p<CountParameters(); ++p)
          SetParameter(p,GetParameter(p)-delta[p]/alpha_inv);
        new_cost=Cost(instances,lambda);
      }
      cost=new_cost;
    }
    else if (debug)
      cost=Cost(instances,lambda);
  
    if (debug)
    { std::cout << "Model::FitParameters: After step " << r << std::endl
           << "  alpha_inv=" << alpha_inv << std::endl;
      for (size_t p=0; p<CountParameters(); ++p)
        std::cout << "  parameter " << p << "=" << GetParameter(p) << std::endl;
      std::cout << "  cost=" << cost << std::endl;
    }
  }
} // }}}
