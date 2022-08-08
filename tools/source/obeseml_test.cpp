#include <slowml/network.hpp>
#include <slowml/vectordata.hpp>
#include <slowml/vectormapdata.hpp>
#include <slowml/windowwrapperdata.hpp>
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
size_t maxid(const vector<double> &vec) // {{{
{ size_t result=0;
  for (size_t i=1; i<vec.size(); ++i)
  { if (vec[i]>vec[result])
      result=i;
  }
  return result;
} // }}}

int main(int argc, char **argv)
{ try
  { if (argc==2 && string("--help")==argv[1] || argc==1)
      throw string("Syntax is obeseml_test [--model|-m <modelpath>] [--network|-n <network=\"100->[3*[3*[all]],[[all->yes],[all->no]]]\">] [--vectormap|-vm <map=\"X->X\">] [--image-width|-imgw <width=100>] [--slidingwindow-step|-sws <stepx=10>] [--slidingwindow-maxscale|-swmaxs <scale=2>] [--slidingwindow-minscale|-swmins <scale=0.5>] [--slidingwindow-scalefactor|-swsf <factor=1.2>] [--slidingwindow-rotations|-swr <rotations=1>] [--slidingwindow-width|-sww <width=100>] [--model-width|-mw <width=100>] <datapath>");

    string datapath="./data";
    string network="#features->[3*[3*[all]],[#labels*[all]]]";
    string vectormap="X->X";
    Network *model=NULL;
    size_t imgw=100;
    size_t sws=10;
    double swsf=1.2;
    size_t swr=1;
    size_t sww=100;
    double min_scale=0.5;
    double max_scale=2;

    //size_t modelwidth=100;

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
      else if (string("--image-width")==argv[arg] || string("-imgw")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--slidingwindow-step must be succeeded by another arg");
        stringstream ss;
        ss << argv[arg];
        ss >> imgw;
      }
      else if (string("--slidingwindow-step")==argv[arg] || string("-sws")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--slidingwindow-step must be succeeded by another arg");
        stringstream ss;
        ss << argv[arg];
        ss >> sws;
      }
      else if (string("--slidingwindow-minscale")==argv[arg] || string("-swmins")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--slidingwindow-minscale must be succeeded by another arg");
        stringstream ss;
        ss << argv[arg];
        ss >> min_scale;
      }
      else if (string("--slidingwindow-maxscale")==argv[arg] || string("-swmaxs")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--slidingwindow-maxscale must be succeeded by another arg");
        stringstream ss;
        ss << argv[arg];
        ss >> max_scale;
      }
      else if (string("--slidingwindow-scalefactor")==argv[arg] || string("-swsf")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--slidingwindow-scalefactor must be succeeded by another arg");
        stringstream ss;
        ss << argv[arg];
        ss >> swsf;
      }
      else if (string("--slidingwindow-rotations")==argv[arg] || string("-swr")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--slidingwindow-rotations must be succeeded by another arg");
        stringstream ss;
        ss << argv[arg];
        ss >> swr;
        if (swr!=1)
        { cerr << "--slidingwindow-rotations must be 1. Defaulting to 1." << endl;
          swr=1;
        }
      }
      else if (string("--slidingwindow-width")==argv[arg] || string("-sww")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--slidingwindow-width must be succeeded by another arg");
        stringstream ss;
        ss << argv[arg];
        ss >> sww;
        if (sww==0)
        { cerr << "--slidingwindow-width must be positive. Defaulting to 100." << endl;
          sww=100;
        }
      }
      //else if (string("--model-width")==argv[arg] || string("-mw")==argv[arg])
      //{ ++arg;
      //  if (arg+1>=argc)
      //    throw string("--model-width must be succeeded by another arg");
      //  stringstream ss;
      //  ss << argv[arg];
      //  ss >> modelwidth;
      //  if (modelwidth==0)
      //  { cerr << "--model-width must be positive. Defaulting to 100." << endl;
      //    sww=100;
      //  }
      //}
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

    cout << "Network: " << endl;
    model->SaveParameters(cout);
    cout << endl;

    size_t swh=model->InputSize()/sww;
    cout << "Model Parameters: " << model->InputSize() << endl;
    cout << "SWW: " << sww << endl;
    cout << "SWH: " << swh << endl;
    
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
        data.LoadRow(fin);
        fin.close();
        VectorMapData mapdata(&data,vecmap,vecmaparg);
        // Perform sliding window
        WindowWrapperData<double> swdata(imgw,0,0,sww,swh,sww,swh,&mapdata);
        cout << "Window data width: " << swdata.Width() << endl;
        // Eval
        double result=0;
        size_t result_label=0;
        double hit_scale=0.5;
        size_t hit_x=0;
        size_t hit_y=0;

        for (double s=max(min_scale,1.0/swsf/swsf/swsf/swsf); s<=max_scale && s*sww<swdata.ImgWidth() && s*swh<swdata.ImgHeight(); s=s*swsf)
        { swdata.SetOffset(0,0);
          swdata.SetScaled(sww*s,swh*s);
          for (size_t x=0; x+s*sww<swdata.ImgWidth(); x+=sws)
          { for (size_t y=0; y+s*swh<swdata.ImgHeight(); y+=sws)
            { swdata.SetOffset(x,y);
              cout << "Evaluating at (" << x << "," << y << ") scale " << s << flush;
              vector<double> r=model->Eval(swdata,0);
              size_t max_label=maxid(r);
              cout << " to " << labels[max_label] << " with accuracy " << r[max_label] << endl;
              if (r[max_label]>result)
              { result=r[max_label];
                result_label=max_label;
                hit_x=x;
                hit_y=y;
                hit_scale=s;
              }
            }
          }
        }
        cout << "Result: " << result << endl;
        cout << "Result Label: " << result_label << endl;
        cout << "Result Label Name: " << labels[result_label] << endl;
        cout << "Hit position: " << hit_x << " " << hit_y << endl;
        cout << "Hit scale: " << hit_scale << endl;
        cout << "Hit section: " << endl;
        swdata.SetOffset(0,0);
        swdata.SetScaled(sww*hit_scale,swh*hit_scale);
        swdata.SetOffset(hit_x,hit_y);
        { for (size_t y1=0; y1<swh; ++y1)
          { for (size_t x1=0; x1<sww; ++x1)
              cout << int(9.9*swdata.GetValue(0,y1*sww+x1+1));
            cout << endl;
          }
        }
        if (result_label!=label)
        { cout << "Error on file: " << datapath << "/" <<labels[label] << "/" << *vfile << " (returned " << labels[result_label].substr(6) << "!=" << labels[label].substr(6) << ")" << endl;
          ++errors;
        }
        else
        { cout << "Success on file: " << datapath << "/" <<labels[label] << "/" << *vfile << endl;
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

