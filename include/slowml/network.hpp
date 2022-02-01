#pragma once
#include <slowml/shallowmodel.hpp>
#include <slowml/wrapperdata.hpp>
#include <dpl/parsetree.hpp>

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

    size_t InputSize() const { return myInputSize; }
    size_t LayerSize(size_t layer) const { return myNodes.at(layer).size(); }
    size_t Layers() const { return myNodes.size(); }
    size_t CountParameters() const;
    double GetParameter(size_t i) const;
    void SetParameter(size_t i, double val);
    void SaveStructure(std::ostream &dest) const;
    void LoadStructure(std::istream &dest) ;
    void SaveParameters(std::ostream &dest, bool saveSize=true) const;
    void LoadParameters(std::istream &src, bool loadSize=true);
    void Save(std::ostream &dest, bool saveStructure=true) const;
    void Load(std::istream &src, bool loadStructure=true);

    static Network *Parse(const std::string &network);
    static Network *ParseNetwork(const dpl::parsetree &tree);
    static void ParseLayers(Network &dest, const dpl::parsetree &tree);
    static void ParseNodes(Network &dest, size_t idx, const dpl::parsetree &tree);
    static void ParseNode(Network &dest, size_t idx, const dpl::parsetree &tree);
    static void ParseInputs(size_t inputs, std::vector<size_t> &sources, std::vector<double> &weights, const dpl::parsetree &tree);

  private:
    std::vector<std::vector<Node> > myNodes;
    //std::vector<std::pair<std::string,std::pair<size_t,size_t> > > myOutputs;
    size_t myInputSize;
}; // }}}
