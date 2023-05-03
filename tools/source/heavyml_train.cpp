#include <slowml/vectordata.hpp>
#include <slowml/vectormapdata.hpp>
#include <slowml/guidedvectordata.hpp>
#include <slowml/network.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
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
std::string string_replace(const std::string &src,const std::string &search, const std::string &replace) // {{{
{ stringstream result;
  size_t index=0;
  while (index!=std::string::npos)
  { size_t next=src.find(search, index);
    if (next==std::string::npos)
    { result << src.substr(index);
      index=next;
    }
    else
    { result << src.substr(index,next-index) << replace;
      index=next+search.size();
    }
  }
  return result.str();
} // }}}
vector<double> IdVec(size_t length, size_t pos) // {{{
{ vector<double> result;
  for (size_t i=0; i<length; ++i)
  { if (i==pos)
      result.push_back(1.0);
    else
      result.push_back(0.0);
  }

  return result;
} // }}}

int main(int argc, char **argv)
{ try
  { if (argc==2 && string("--help")==argv[1] || argc==1)
      throw string("Syntax is heavyml_train [--gd_repetitions|-gdr <int=50000>] [--gd_alphainv|-gda <double=100>] [--gd_lambda|-gdl <double=1>] [--model|-m <modelpath>] [--network|-n <network=\"#features->[3*[3*[all]],[#labels*[all]]]\">][--vectormap|-vm <map=\"X->X\">] [--filename_truths|-fnt <int=0>] [--output|-o <modelpath=./model.pars>] [--continue|-c] [--debug|-d] <datapath>");

    string datapath="./data";
    string modelfile="./model.pars";
    string network="#features->[3*[3*[all]],[#labels*[all]]]";
    string vectormap="X->X";
    size_t filename_truths=0;
    size_t gd_repetitions=50000;
    double gd_alphainv=100;
    double gd_lambda=1;
    bool option_debug=false;
    bool option_continue=false;
    Network *model=NULL;

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
      else if ((string("--model")==argv[arg] || string("-m")==argv[arg]) && model==NULL)
      { ++arg;
        if (arg+1>=argc)
          throw string("--model must be succeeded by a value");
        ifstream fin(argv[arg]);
        model=new Network(1);
        model->LoadParameters(fin);
        fin.close();
        cout << "Loaded model" << endl;
      }
      else if (string("--network")==argv[arg] || string("-n")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--network must be succeeded by another arg");
       network=argv[arg];
      }
      else if (string("--vectormap")==argv[arg] || string("-vm")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--vectormap must be succeeded by another arg");
        vectormap=argv[arg];
      }
      else if (string("--filename_truths")==argv[arg] || string("-fnt")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--filename_results must be succeeded by an integer");
        stringstream ss;
        ss << argv[arg];
        ss >> filename_truths;
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
    vector<vector<double>> truths;
    for (size_t label=0; label<labels.size(); ++label)
    { cout << "Label: " << labels[label] << flush;
      size_t count=0;
      vector<string> vfiles=GetVectorFiles(datapath+"/"+labels[label]);
      for (auto vfile=vfiles.begin(); vfile!=vfiles.end(); ++vfile)
      { ifstream fin(datapath+"/"+labels[label]+"/"+*vfile);
        data.LoadRow(fin);
        fin.close();
        vector<double> truth=IdVec(labels.size(),label);
        // Add filename truths
        string fname=*vfile;
        size_t endpos=fname.rfind('.');
        if (endpos==string::npos && filename_truths>0)
        { cerr << "Ill formatted filename " << fname << endl;
          for (size_t t=0; t<filename_truths; ++t)
            truth.push_back(0.0);
        }
        else
        { for (size_t t=0; t<filename_truths; ++t)
          { size_t startpos=fname.rfind('_',endpos);
            if (startpos==string::npos)
            { cerr << "Ill formatted filename truth " << fname << endl;
              truth.push_back(0.0);
            }
            else
            { string tstr=fname.substr(startpos+1,endpos);
              endpos=startpos-1;
              stringstream ss;
              ss << tstr;
              double tval;
              ss >> tval;
              //cout << "Filename " << fname << " tval " << tval << endl;
              truth.push_back(tval);
            }
          }
        }
        truths.push_back(truth);
        ++count;
      }
      cout << ". Loaded " << count << " instances." << endl;
    }
    cout << "Loading data done." << endl;
    VectorMapData mapdata(&data,vecmap,vecmaparg);

    // Parse network
    if (model==NULL)
    { stringstream c_features;
      c_features << mapdata.Width();
      stringstream c_labels;
      c_labels << labels.size();
      string network_str=string_replace(string_replace(network,"#features",c_features.str()),"#labels",c_labels.str());
      model=Network::Parse(network_str);
      network_str="";
      cout << "Loaded model" << endl;
    }
    GuidedVectorData<double,vector<double> > gdata(mapdata,truths);
    cout << "Training " << model->CountParameters() << " parameters" << endl;
    model->FitParameters(gdata,gd_alphainv,gd_lambda,gd_repetitions,option_debug);
    cout << "Training parameters done" << endl;
    cout << "Saving model parameters in " << modelfile << flush;
    ofstream fout(modelfile);
    model->SaveParameters(fout);
    fout.close();
    cout << " done." << endl;
    delete vecmap;
    delete model;
  }
  catch (const std::string &error)
  { cerr << "Error: " << error << endl;
  }
}

