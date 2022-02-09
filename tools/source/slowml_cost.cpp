#include <slowml/vectordata.hpp>
#include <slowml/vectormapdata.hpp>
#include <slowml/guidedvectordata.hpp>
#include <slowml/onevsall.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <dirent.h>
#include <stdio.h>
#include <algorithm>

using namespace std;

vector<string> GetLabelsDirs(const string &path) // {{{
{ vector<string> result;
  DIR *dir = opendir(path.c_str());
  struct dirent *entry = readdir(dir);
  while (entry != NULL)
  {
    if (entry->d_type == DT_DIR && string(entry->d_name).substr(0,6)=="label_")
      result.push_back(entry->d_name);
    entry = readdir(dir);
  }
  closedir(dir);

  // Sort result
  std::sort(result.begin(),result.end());
  return result;
} // }}}
vector<string> GetVectorFiles(const string &path) // {{{
{ vector<string> result;
  DIR *dir = opendir(path.c_str());
  struct dirent *entry = readdir(dir);
  while (entry != NULL)
  {
    string name(entry->d_name);
    if (name.size()>4 && entry->d_type == DT_REG && name.substr(name.size()-4,4)==".vec")
      result.push_back(entry->d_name);
    entry = readdir(dir);
  }
  closedir(dir);
  return result;
} // }}}

int main(int argc, char **argv)
{ try
  { if (argc==2 && string("--help")==argv[1] || argc==1)
      throw string("Syntax is slowml_cost [--lambda|-l <double>] [--model|-m <modelpath>] [--vectormap|-vm <map=\"X->X\">] <datapath>");

    double lambda=1.0;
    string datapath="";
    string modelfile="";
    string vectormap="X->X";

    for (size_t arg=1; arg+1<argc; ++arg)
    { if (string("--lambda")==argv[arg] || string("-l")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--lambda must be succeeded by a value");
        stringstream ss;
        ss << argv[arg];
        ss >> lambda;
        if (lambda<=0)
          throw string("--lambda must be succeeded by positive number");
      }
      else if (string("--model")==argv[arg] || string("-m")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--model must be succeeded by a value");
        modelfile=argv[arg];
      }
      else if (string("--vectormap")==argv[arg] || string("-vm")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--vectormap must be succeeded by another arg");
        vectormap=argv[arg];
      }
      else
        throw string("Unknown argument") + argv[arg];
    }

    datapath=argv[argc-1];

    // Parse map
    cout << "Parsing vector mapping" << flush;
    string vecmaparg;
    VectorMap *vecmap=VectorMap::Parse(vectormap,vecmaparg);
    cout << " done." << endl;

    // Find labels
    vector<string> labels=GetLabelsDirs(datapath);

    // Read data
    VectorData<double> data(vector<double>(),0,0);
    VectorData<size_t> truths(vector<size_t>(),1,0);
    for (size_t label=0; label<labels.size(); ++label)
    { cout << "Label: " << labels[label] << endl;
      vector<string> vfiles=GetVectorFiles(datapath+"/"+labels[label]);
      for (auto vfile=vfiles.begin(); vfile!=vfiles.end(); ++vfile)
      { ifstream fin(datapath+"/"+labels[label]+"/"+*vfile);
        data.LoadRow(fin);
        fin.close();
        truths.AddRow(vector<size_t>{ label });
        cout << "." << flush;
      }
      cout << endl;
    }

    vector<Label> mlabels;
    for (size_t i=0; i<labels.size(); ++i)
      mlabels.push_back(Label(labels[i].substr(6),labels[i],i,(double)i));
    OneVsAll model(mlabels);

    ifstream fin(modelfile);
    model.Load(fin);
    fin.close();
    if (model.CountParameters()!=data.Width())
      throw string("Model does not contain the correct amount of parameters");

    VectorMapData mapdata(&data,vecmap,vecmaparg);
    GuidedVectorData<double,size_t> gdata(mapdata,truths.GetValues());
    double cost=model.Cost(gdata,lambda);
    delete vecmap;
    cout << "Cost: " << cost << endl;
  }
  catch (const std::string &error)
  { cerr << "Error: " << error << endl;
  }
}

