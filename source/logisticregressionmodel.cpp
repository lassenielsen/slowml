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

double LogisticRegressionModel::Eval(const vector<vector<double>> &instances, size_t pos) const // {{{
{ if (instances.size()<=pos)
  { stringstream ss;
    ss << "LogisticRegressionModel::Eval position " << pos << "out of bounds" ;
    throw ss.str();
  }
  if (instances[pos].size()!=CountParameters())
  { stringstream ss;
    ss << "LogisticRegressionModel::Eval instance width(" << instances[pos].size() << ") does not match number of parameters(" << CountParameters() <<")" ;
    throw ss.str();
  }
  double result=0;
  for (size_t i=0; i<CountParameters(); ++i)
    result += GetParameter(i)*instances[pos][i];
  return sigmoid(result);
} // }}}
double LogisticRegressionModel::Eval(const vector<double> &instance) const // {{{
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
double LogisticRegressionModel::Cost(const vector<vector<double> > &instances, const vector<double> &truths, double lambda) // {{{
{ double cost=0;
  for (size_t i=0; i<instances.size(); ++i)
    cost += LogDistance(Eval(instances,i),truths[i]);
  cost = cost/(double)instances.size();

  // Add Regularization
  for (size_t p=1; p<CountParameters(); ++p)
  { cost +=lambda*pow(GetParameter(p),2.0)/(2.0*instances.size());
  }
  return cost;
} // }}}
void LogisticRegressionModel::AddDelta(const vector<vector<double> > &inputs, size_t pos, std::vector<double> &deltasum, const double &diff) // {{{
{ for (size_t p=0; p<inputs[pos].size(); ++p)
  { deltasum[p]+=diff*inputs[pos][p];
  }
} // }}}
vector<double> LogisticRegressionModel::Delta(const vector<vector<double> > &instances, const vector<double> &truths, double lambda) // {{{
{ vector<double> delta=vector<double>(CountParameters(),0.0);
  for (size_t instance=0; instance<instances.size(); ++instance)
  { double guess=Eval(instances,instance);
    AddDelta(instances,instance,delta,guess-truths[instance]);
    //for (size_t p=0; p<CountParameters(); ++p)
    //  delta[p]+=(guess-instances.GetResult(instance))*instances.GetValue(instance,p);
  }
  for (size_t p=0; p<CountParameters(); ++p)
    delta[p]=delta[p]/(double)instances.size();
  // Add regularization
  for (size_t p=1; p<CountParameters(); ++p)
  { delta[p] +=lambda*GetParameter(p)/instances.size();
  }
 
  return delta;
} // }}}
