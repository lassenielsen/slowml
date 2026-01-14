#include<slowml/linearregressionmodel.hpp>
#include <cmath>
#include <sstream>
using namespace std;

LinearRegressionModel::LinearRegressionModel() // {{{
{
} // }}}

LinearRegressionModel::~LinearRegressionModel() // {{{
{
} // }}}

double LinearRegressionModel::Eval(const vector<vector<double> > &instances, size_t pos) const // {{{
{ if (instances[pos].size()!=Parameters().size())
  { stringstream ss;
    ss << "LinearRegressionModel::Eval instance width(" << instances.size() << ") does not match number of parameters(" << Parameters().size() <<")" ;
    throw ss.str();
  }
  double result=0;
  for (size_t i=0; i<Parameters().size(); ++i)
    result += GetParameter(i)*instances[pos][i];
  return result;
} // }}}

double LinearRegressionModel::Eval(const vector<double> &instance) const // {{{
{ if (instance.size()!=Parameters().size())
  { stringstream ss;
    ss << "LinearRegressionModel::Eval instance width(" << instance.size() << ") does not match number of parameters(" << Parameters().size() <<")" ;
    throw ss.str();
  }
  double result=0;
  for (size_t i=0; i<Parameters().size(); ++i)
    result += GetParameter(i)*instance[i];
  return result;
} // }}}

double LinearRegressionModel::SquareDistance(const double &lhs, const double &rhs) // {{{
{ double dist=lhs-rhs;
  return dist*dist;
} // }}}

double LinearRegressionModel::Cost(const vector<vector<double> > &instances, const vector<double> &truths, double lambda) // {{{
{ double cost=0;
  for (size_t i=0; i<instances.size(); ++i)
    cost += SquareDistance(Eval(instances,i),truths[i]);
  cost=(cost/2)/(double)instances.size();
  // Add Regularization
  for (size_t p=1; p<CountParameters(); ++p)
  { cost +=lambda*pow(GetParameter(p),2.0)/(2.0*instances.size());
  }
  return cost;
} // }}}

void LinearRegressionModel::AddDelta(const vector<vector<double> > &inputs, size_t pos, std::vector<double> &deltasum, const double &diff) // {{{
{ for (size_t p=0; p<inputs[pos].size(); ++p)
  { deltasum[p]+=diff*inputs[pos][p];
  }
} // }}}

vector<double> LinearRegressionModel::Delta(const vector<vector<double> > &instances, const vector<double> &truths, double lambda) // {{{
{ vector<double> delta=vector<double>(instances.size(),0.0);
  for (size_t instance=0; instance<instances.size(); ++instance)
  { double guess=Eval(instances,instance);
    AddDelta(instances,instance,delta,guess-truths[instance]);
    //for (size_t p=0; p<CountParameters(); ++p)
    //{ //cout << "LRM::Delta: p=" << p << endl;
    //  delta[p]+=(guess-instances.GetResult(instance))*instances.GetValue(instance,p);
    //}
  }
  for (size_t p=0; p<CountParameters(); ++p)
    delta[p]=delta[p]/(double)instances.size();
  // Add regularization
  for (size_t p=1; p<CountParameters(); ++p)
  { delta[p] +=lambda*GetParameter(p)/instances.size();
  }
  return delta;
} // }}}
