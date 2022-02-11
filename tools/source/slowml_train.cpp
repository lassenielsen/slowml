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
      throw string("Syntax is buildmodel [--gd_repetitions|-gdr <int=50000>] [--gd_alphainv|-gda <double=100>] [--gd_lambda|-gdl <double=1>] [--vectormap|-vm <map=\"X->X\">] [--output|-o <modelpath=./model.pars>] [--continue|-c] [--debug|-d] <datapath>");

    string datapath="./data";
    string modelfile="./model.pars";
    string vectormap="X->X";
    size_t gd_repetitions=50000;
    double gd_alphainv=100;
    double gd_lambda=1;
    bool option_debug=false;
    bool option_continue=false;

    for (size_t arg=1; arg+1<argc; ++arg)
    { if (string("--gd_repetitions")==argv[arg] || string("-gdr")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--gd_repetitions must be succeeded by a value");
        stringstream ss;
        ss << argv[arg];
        ss >> gd_repetitions;
        if (gd_repetitions==0)
          throw string("--gd_repetitions must be succeeded by positive integer");
      }
      else if (string("--gd_alphainv")==argv[arg] || string("-gda")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--gd_alphainv must be succeeded by a value");
        stringstream ss;
        ss << argv[arg];
        ss >> gd_alphainv;
        if (gd_alphainv<0)
          throw string("--gd_alphainv must be succeeded by a nonnegative value");
      }
      else if (string("--gd_lambda")==argv[arg] || string("-gdl")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--gd_lambda must be succeeded by a value");
        stringstream ss;
        ss << argv[arg];
        ss >> gd_lambda;
        if (gd_lambda<=0)
          throw string("--gd_lambda must be succeeded by positive number");
      }
      else if (string("--vectormap")==argv[arg] || string("-vm")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--vectormap must be succeeded by another arg");
        vectormap=argv[arg];
      }
      else if (string("--output")==argv[arg] || string("-o")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--output must be succeeded by a value");
        modelfile=argv[arg];
      }
      else if (string("--continue")==argv[arg] || string("-c")==argv[arg])
        option_continue=true;
      else if (string("--debug")==argv[arg] || string("-d")==argv[arg])
        option_debug=true;
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
    cout << "Loading data" << endl;
    VectorData<double> data(vector<double>(),0,0);
    VectorData<size_t> truths(vector<size_t>(),1,0);
    for (size_t label=0; label<labels.size(); ++label)
    { cout << "Label: " << labels[label] << flush;
      size_t count=0;
      vector<string> vfiles=GetVectorFiles(datapath+"/"+labels[label]);
      for (auto vfile=vfiles.begin(); vfile!=vfiles.end(); ++vfile)
      { ifstream fin(datapath+"/"+labels[label]+"/"+*vfile);
        data.LoadRow(fin);
        fin.close();
        truths.AddRow(vector<size_t>{ label });
        ++count;
      }
      cout << ". Loaded " << count << " instances." << endl;
    }
    cout << "Loading data done." << endl;
    VectorMapData mapdata(&data,vecmap,vecmaparg);

    vector<Label> mlabels;
    for (size_t i=0; i<labels.size(); ++i)
      mlabels.push_back(Label(labels[i].substr(6),labels[i],i,(double)i));
    OneVsAll model(mlabels);

    if (option_continue)
    { cout << "Loading model" << flush;
      ifstream fin(modelfile);
      model.Load(fin);
      fin.close();
      if (model.CountParameters()!=mapdata.Width())
        throw string("--continue was specified, but model does not contain the correct amount of parameters");
      cout << " done." << endl;
    }
    else
    { for (size_t i=0; i<mapdata.Width()-1; ++i)
        model.AddParameter(0.0);
    }

    GuidedVectorData<double,size_t> gdata(mapdata,truths.GetValues());
    cout << "Training " << mapdata.Width() << " parameters" << endl;
    model.FitParameters(gdata,gd_alphainv,gd_lambda,gd_repetitions,option_debug);
    cout << "Training parameters done" << endl;
    cout << "Saving model parameters in " << modelfile << flush;
    ofstream fout(modelfile);
    model.Save(fout);
    fout.close();
    cout << " done." << endl;
    delete vecmap;
  }
  catch (const std::string &error)
  { cerr << "Error: " << error << endl;
  }
}

