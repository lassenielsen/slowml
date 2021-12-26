#pragma once
#include <slowml/shallowmodel.hpp>
#include <slowml/wrapperdata.hpp>

class Network : public Model<std::vector<double> > // {{{
{ public:
    typedef std::pair<WrapperData<double>,ShallowModel<double>*> Node;

    Network(size_t input_size); // TODO DSL FOR NETWORK DESIGN
    virtual ~Network();

    std::string Type() { return "Network Model"; }

    size_t AddNode(size_t layer, const std::vector<size_t> &inputs);
    Node &GetNode(size_t layer, size_t node) { return myNodes.at(layer).at(node); }
    const Node &GetNode(size_t layer, size_t node) const { return myNodes.at(layer).at(node); }

    std::vector<double> Eval(const Data<double> &instances, size_t pos);
    std::vector<double> Eval(const std::vector<double> &instance);
    double LogDistance(const std::vector<double> &guess, const std::vector<double> &truth);
    double Cost(const GuidedData<double,std::vector<double> > &instances, double lambda=1.0);
    std::vector<double> Delta(const GuidedData<double,std::vector<double> > &instances, double lambda=1.0);

    size_t CountParameters() const;
    double GetParameter(size_t i) const;
    void SetParameter(size_t i, double val);
    void SaveParameters(std::ostream &dest, bool saveSize=true) const;
    void LoadParameters(std::istream &src, bool loadSize=true);

  private:
    std::vector<std::vector<Node> > myNodes;
    size_t myInputSize;
}; // }}}
