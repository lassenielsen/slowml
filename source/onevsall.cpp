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

const Label &OneVsAll::Eval(const Data<double> &instances, size_t pos) // {{{
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

double OneVsAll::Cost(const GuidedData<double,size_t> &instances, double lambda) // {{{
{ double cost(0.0);
  for (size_t model=0; model<myModels.size(); ++model)
  { GuidedDataClassWrapper wrapper(&instances,model);
    cost+=myModels[model].Cost(wrapper,lambda);
  }
  return cost;
} // }}}

void OneVsAll::FitParameters(GuidedData<double,size_t> &instances, double &alpha_inv, double lambda, size_t repetitions, bool debug) // {{{
{ for (size_t model=0; model<myModels.size(); ++model)
  { if (debug)
      cout << "OneVsAll: Fitting model " << model << endl;
    GuidedDataClassWrapper wrapper(&instances,model);
    myModels[model].FitParameters(wrapper,alpha_inv,lambda,repetitions,debug);
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
