#include <slowml/network.hpp>
#include <slowml/vectordata.hpp>
#include <slowml/vectormapdata.hpp>
#include <fstream>
#include <dirent.h>
#include <stdio.h>
#include <iomanip>
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
size_t maxid(const vector<double> &vec, size_t len) // {{{
{ size_t result=0;
  for (size_t i=1; i<vec.size() && i<len; ++i)
  { if (vec[i]>vec[result])
      result=i;
  }
  return result;
} // }}}
std::string vectorstr(const std::vector<double> &vec) // {{{
{ stringstream ss;
  ss << "[";
  for (size_t i=0; i<vec.size(); ++i)
  { if (i>0)
      ss << ", ";
    ss << std::setprecision(4) << vec[i];
  }
  ss << "]";
  return ss.str();
} // }}}

int main(int argc, char **argv)
{ try
  { if (argc==2 && string("--help")==argv[1] || argc==1)
      throw string("Syntax is usemodel [--model|-m <modelpath>] [--network|-n <network=\"100->[3*[3*[all]],[[all->yes],[all->no]]]\">] [--vectormap|-vm <map=\"X->X\">] [--filename_truths|-fnt <int=0>] <datapath>");

    string datapath="./data";
    string network="#features->[3*[3*[all]],[#labels*[all]]]";
    string vectormap="X->X";
    size_t filename_truths=0;
    Network *model=NULL;

    for (size_t arg=1; arg+1<argc; ++arg)
    { if ((string("--model")==argv[arg] || string("-m")==argv[arg]) && model==NULL)
      { ++arg;
        if (arg+1>=argc)
          throw string("--model must be succeeded by a value");
        ifstream fin(argv[arg]);
        model=new Network(1);
        model->LoadParameters(fin);
        fin.close();
        cout << "Loaded model" << endl;
      }
      else if (string("--vectormap")==argv[arg] || string("-vm")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--vectormap must be succeeded by another arg");
        vectormap=argv[arg];
      }
      else if (string("--network")==argv[arg] || string("-n")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--network must be succeeded by another arg");
       network=argv[arg];
      }
      else if (string("--filename_truths")==argv[arg] || string("-fnt")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--filename_results must be succeeded by an integer");
        stringstream ss;
        ss << argv[arg];
        ss >> filename_truths;
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

    // Load model
    if (model==NULL)
    { model=Network::Parse(network);
      cout << "Loaded model" << endl;
    }
    network.clear();

    //cout << "Network: " << endl;
    //model->SaveParameters(cout);
    //cout << endl;
    
    size_t successes=0;
    size_t errors=0;
    // Find labels
    vector<string> labels=GetLabelsDirs(datapath);
    
    // Read data
    for (size_t label=0; label<labels.size(); ++label)
    { cout << "Label: " << labels[label] << endl;
      vector<string> vfiles=GetVectorFiles(datapath+"/"+labels[label]);
      for (auto vfile=vfiles.begin(); vfile!=vfiles.end(); ++vfile)
      { ifstream fin(datapath+"/"+labels[label]+"/"+*vfile);
        VectorData<double> data(vector<double>(),0,0);
        //vector<vector<double>> truths;
        data.LoadRow(fin);
        fin.close();
        VectorMapData mapdata(&data,vecmap,vecmaparg);

        //vector<double> truth=IdVec(labels.size(),label);
        //// Add filename truths
        //string fname=*vfile;
        //size_t endpos=fname.rfind('.');
        //if (endpos==string::npos && filename_truths>0)
        //{ cerr << "Ill formatted filename " << fname << endl;
        //  for (size_t t=0; t<filename_truths; ++t)
        //    truth.push_back(0.0);
        //}
        //else
        //{ for (size_t t=0; t<filename_truths; ++t)
        //  { size_t startpos=fname.rfind('_',endpos);
        //    if (startpos==string::npos)
        //    { cerr << "Ill formatted filename truth " << fname << endl;
        //      truth.push_back(0.0);
        //    }
        //    else
        //    { string tstr=fname.substr(startpos+1,endpos);
        //      endpos=startpos-1;
        //      stringstream ss;
        //      ss << tstr;
        //      double tval;
        //      ss >> tval;
        //      cout << "Filename " << fname << " tval " << tval << endl;
        //      truth.push_back(tval);
        //    }
        //  }
        //}
        //truths.push_back(truth);
        // Eval
        vector<double> result=model->Eval(mapdata,0);
        size_t max_label=maxid(result,labels.size());
        if (max_label!=label)
        { cout << "Error on file: " << datapath << "/" <<labels[label] << "/" << *vfile << " (returned " << labels[max_label].substr(6) << "!=" << labels[label].substr(6) << " with result " << vectorstr(result) << ")" << endl;
          ++errors;
        }
        else
        { cout << "Success on file: " << datapath << "/" <<labels[label] << "/" << *vfile << " with result " << vectorstr(result) << endl;
          ++successes;
        }
      }
    }
    delete vecmap;
    cout << "Total " << successes << " succeeded (" << (100.0 * successes) / (successes+errors) << "%)" << endl
         << "Total " << errors << " failed (" << (100.0 * errors) / (successes+errors) << "%)" << endl;
  }
  catch (const std::string &error)
  { cerr << "Error: " << error << endl;
  }
}

