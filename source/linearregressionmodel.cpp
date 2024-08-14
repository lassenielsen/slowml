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

double LinearRegressionModel::Eval(const Data<double> &instances, size_t pos) const // {{{
{ if (instances.Width()!=Parameters().size())
  { stringstream ss;
    ss << "LinearRegressionModel::Eval instance width(" << instances.Width() << ") does not match number of parameters(" << Parameters().size() <<")" ;
    throw ss.str();
  }
  double result=0;
  for (size_t i=0; i<Parameters().size(); ++i)
    result += GetParameter(i)*instances.GetValue(pos,i);
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

double LinearRegressionModel::Cost(const GuidedData<double,double> &instances, double lambda) // {{{
{ double cost=0;
  for (size_t i=0; i<instances.Height(); ++i)
    cost += SquareDistance(Eval(instances,i),instances.GetResult(i));
  cost=(cost/2)/(double)instances.Height();
  // Add Regularization
  for (size_t p=1; p<CountParameters(); ++p)
  { cost +=lambda*pow(GetParameter(p),2.0)/(2.0*instances.Height());
  }
  return cost;
} // }}}

void LinearRegressionModel::AddDelta(const Data<double> &inputs, size_t pos, std::vector<double> &deltasum, const double &diff) // {{{
{ for (size_t p=0; p<inputs.Width(); ++p)
  { deltasum[p]+=diff*inputs.GetValue(pos,p);
  }
} // }}}

vector<double> LinearRegressionModel::Delta(const GuidedData<double,double> &instances,double lambda) // {{{
{ vector<double> delta=vector<double>(instances.Width(),0.0);
  for (size_t instance=0; instance<instances.Height(); ++instance)
  { double guess=Eval(instances,instance);
    AddDelta(instances,instance,delta,guess-instances.GetResult(instance));
    //for (size_t p=0; p<CountParameters(); ++p)
    //{ //cout << "LRM::Delta: p=" << p << endl;
    //  delta[p]+=(guess-instances.GetResult(instance))*instances.GetValue(instance,p);
    //}
  }
  for (size_t p=0; p<CountParameters(); ++p)
    delta[p]=delta[p]/(double)instances.Height();
  // Add regularization
  for (size_t p=1; p<CountParameters(); ++p)
  { delta[p] +=lambda*GetParameter(p)/instances.Height();
  }
  return delta;
} // }}}
