#include <slowml/logisticregressionmodel.hpp>
#include <cmath>
#include <sstream>

using namespace std;

inline double sigmoid(double t) // {{{
{ return 1.0/(1.0+exp(-t));
} // }}}

LogisticRegressionModel::LogisticRegressionModel() // {{{
{
} // }}}

LogisticRegressionModel::~LogisticRegressionModel() // {{{
{
} // }}}

double LogisticRegressionModel::Eval(const Data<double> &instances, size_t pos) // {{{
{ if (instances.Width()!=CountParameters())
  { stringstream ss;
    ss << "LogisticRegressionModel::Eval instance width(" << instances.Width() << ") does not match number of parameters(" << CountParameters() <<")" ;
    throw ss.str();
  }
  double result=0;
  for (size_t i=0; i<Parameters().size(); ++i)
    result += GetParameter(i)*instances.GetValue(pos,i);
  return sigmoid(result);
} // }}}
double LogisticRegressionModel::Eval(const vector<double> &instance) // {{{
{ if (instance.size()!=CountParameters())
  { stringstream ss;
    ss << "LogisticRegressionModel::Eval instance width(" << instance.size() << ") does not match number of parameters(" << CountParameters() <<")" ;
    throw ss.str();
  }
  double result=0;
  for (size_t i=0; i<Parameters().size(); ++i)
    result += GetParameter(i)*instance[i];
  return sigmoid(result);
} // }}}
double LogisticRegressionModel::LogDistance(double guess, double truth) // {{{
{ return -truth*log(guess)-(1-truth)*log(1-guess);
} // }}}
double LogisticRegressionModel::Cost(const GuidedData<double,double> &instances, double lambda) // {{{
{ double cost=0;
  for (size_t i=0; i<instances.Height(); ++i)
    cost += LogDistance(Eval(instances,i),instances.GetResult(i));
  cost = cost/(double)instances.Height();

  // Add Regularization
  for (size_t p=1; p<CountParameters(); ++p)
  { cost +=lambda*pow(GetParameter(p),2.0)/(2.0*instances.Height());
  }
  return cost;
} // }}}
void LogisticRegressionModel::AddDelta(const Data<double> &inputs, size_t pos, std::vector<double> &deltasum, const double &diff) // {{{
{ for (size_t p=0; p<inputs.Width(); ++p)
  { deltasum[p]+=diff*inputs.GetValue(pos,p); //*guess*(1.0-guess);
  }
} // }}}
vector<double> LogisticRegressionModel::Delta(const GuidedData<double,double> &instances, double lambda) // {{{
{ vector<double> delta=vector<double>(CountParameters(),0.0);
  for (size_t instance=0; instance<instances.Height(); ++instance)
  { double guess=Eval(instances,instance);
    AddDelta(instances,instance,delta,guess-instances.GetResult(instance));
    //for (size_t p=0; p<CountParameters(); ++p)
    //  delta[p]+=(guess-instances.GetResult(instance))*instances.GetValue(instance,p);
  }
  for (size_t p=0; p<CountParameters(); ++p)
    delta[p]=delta[p]/(double)instances.Height();
  // Add regularization
  for (size_t p=1; p<CountParameters(); ++p)
  { delta[p] +=lambda*GetParameter(p)/instances.Height();
  }
 
  return delta;
} // }}}
