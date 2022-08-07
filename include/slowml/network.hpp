#pragma once
#include <slowml/shallowmodel.hpp>
#include <slowml/wrapperdata.hpp>
#include <dpl/parsetree.hpp>
#include <tuple>

class Network : public Model<std::vector<double> > // {{{
{ public:
    typedef std::pair<std::vector<size_t>,ShallowModel<double>*> Node;

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
    size_t CountParameters() const // {{{
    { return myPmap.size();
    } // }}}
    double GetParameter(size_t i) const // {{{
    { return myNodes[std::get<0>(myPmap[i])][std::get<1>(myPmap[i])].second->GetParameter(std::get<2>(myPmap[i]));
    } // }}}
    void SetParameter(size_t i, double val) // {{{
    { myNodes[std::get<0>(myPmap[i])][std::get<1>(myPmap[i])].second->SetParameter(std::get<2>(myPmap[i]),val);
    } // }}}

    void SaveParameters(std::ostream &dest, bool saveSize=true) const;

    static double ReadDouble(istream &src);
    static int ReadInt(istream &src);
    static std::string ReadString(istream &src, size_t size);
    void LoadParameters(std::istream &src, bool loadSize=true);

    static Network *Parse(const std::string &network);
    static Network *ParseNetwork(const dpl::parsetree &tree);
    static void ParseLayers(Network &dest, const dpl::parsetree &tree);
    static void ParseNodes(Network &dest, size_t idx, const dpl::parsetree &tree);
    static void ParseNode(Network &dest, size_t idx, const dpl::parsetree &tree);
    static void ParseInputs(size_t inputs, std::vector<size_t> &sources, std::vector<double> &weights, const dpl::parsetree &tree);

    const std::vector<std::vector<Node> > &Nodes() const { return myNodes; }
    std::vector<std::vector<Node> > &Nodes() { return myNodes; }
  private:
    void InitPmap() // {{{
    { myPmap.clear();
      for (size_t l=0; l<myNodes.size(); ++l)
      { for (size_t n=0; n<myNodes[l].size(); ++n)
        { for (size_t p=0; p<myNodes[l][n].second->CountParameters(); ++p)
            myPmap.push_back(tuple<size_t,size_t,size_t>(l,n,p));
        }
      }
    } // }}}

    std::vector<std::vector<Node> > myNodes;
    std::vector<std::tuple<size_t,size_t,size_t> > myPmap;
    //std::vector<std::pair<std::string,std::pair<size_t,size_t> > > myOutputs;
    size_t myInputSize;
}; // }}}
