#include <slowml/onevsall.hpp>
#include <iostream>
using namespace std;

OneVsAll::OneVsAll(const std::vector<Label> &labels) // {{{
: myLabels(labels)
, myModels(labels.size())
{ if (labels.size()==0)
    throw std::string("OneVsAll: Created model with no labels");
} // }}}

OneVsAll::~OneVsAll() // {{{
{
} // }}}

void OneVsAll::AddParameter(double parameter) // {{{
{ for (size_t model=0; model<myModels.size(); ++model)
    myModels[model].AddParameter(parameter);
} // }}}

const Label &OneVsAll::Eval(const vector<vector<double> > &instances, size_t pos) const // {{{
{ size_t best_model=0;
  double propability=myModels[0].Eval(instances,pos);
  for (size_t model=1; model<myModels.size(); ++model)
  { double p=myModels[model].Eval(instances,pos);
    //cout << "OneVsAll::Eval model " << model << " has propability " << p << endl;
    if (p>propability)
    { best_model=model;
      propability=p;
    }
  }
  return myLabels[best_model];
} // }}}

double OneVsAll::Cost(const vector<vector<double> >&instances, const vector<size_t> &truths, double lambda) // {{{
{ double cost(0.0);
  for (size_t model=0; model<myModels.size(); ++model)
  { vector<double> mtruths;
    for (size_t i=0; i<truths.size(); ++i)
    { mtruths.push_back(model==truths[i]?1.0:0.0);
    }
    cost+=myModels[model].Cost(instances,mtruths,lambda);
  }
  return cost;
} // }}}

void OneVsAll::FitParameters(const vector<vector<double>> &instances, const vector<size_t> &truths, double &alpha_inv, double lambda, size_t repetitions, double max_alpha_inv, bool debug) // {{{
{ double ainv;
  for (size_t model=0; model<myModels.size(); ++model)
  { if (debug)
      cout << "OneVsAll: Fitting model " << model << endl;
    vector<double> mtruths;
    for (size_t i=0; i<truths.size(); ++i)
    { mtruths.push_back(model==truths[i]?1.0:0.0);
    }
    ainv=alpha_inv;
    myModels[model].FitParameters(instances,mtruths,ainv,lambda,repetitions,max_alpha_inv,debug);
  }
} // }}}

void OneVsAll::Save(ostream &dest, bool saveSize) // {{{
{ size_t count=myLabels.size();
  if (saveSize)
    dest.write((const char*)&count,sizeof(size_t));
  for (size_t i=0; i<count; ++i)
  { myLabels[i].Save(dest);
    myModels[i].SaveParameters(dest);
  }
} // }}}

void OneVsAll::Load(istream &src, bool loadSize) // {{{
{ size_t count;
  if (loadSize)
    src.read((char*)&count,sizeof(size_t));
  else
    count=myLabels.size();

  Clear();
  for (size_t i=0; i<count; ++i)
  { Label l("","",0);
    l.Load(src);
    myLabels.push_back(l);
    LogisticRegressionModel m;
    m.LoadParameters(src);
    myModels.push_back(m);
  }
} // }}}
