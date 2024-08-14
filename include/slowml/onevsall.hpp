#pragma once
#include <slowml/label.hpp>
#include <slowml/feature.hpp>
#include <slowml/data.hpp>
#include <slowml/guideddata.hpp>
#include <slowml/guideddataclasswrapper.hpp>
#include <slowml/logisticregressionmodel.hpp>
#include <istream>
#include <ostream>

class OneVsAll // {{{
{ public:
    OneVsAll(const std::vector<Label> &labels);
    virtual ~OneVsAll();

    void Clear() { myLabels.clear(); myModels.clear(); }
    size_t CountParameters() const { if (myModels.empty()) return 0; else return myModels[0].CountParameters(); }
    void AddParameter(double parameter=0.0);
    const std::vector<Label> &GetLabels() const { return myLabels; }
    const std::vector<LogisticRegressionModel> &GetModels() const { return myModels; }
    const Label &Eval(const Data<double> &instances, size_t pos) const;
    double Cost(const GuidedData<double,size_t> &instances, double lambda=1.0);
    void FitParameters(GuidedData<double,size_t> &instances, double &alpha_inv, double lambda=1.0, size_t repetitions=1000, bool debug=false);
    void Save(std::ostream &dest, bool saveSize=true);
    void Load(std::istream &dest, bool loadSize=true);

  private:
    std::vector<Label> myLabels;
    std::vector<LogisticRegressionModel> myModels;
}; // }}}
