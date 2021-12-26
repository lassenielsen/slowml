// The contents of this file are in the public domain. See LICENSE_FOR_EXAMPLE_PROGRAMS.txt
/*

    This is an example illustrating the use of the kernel ridge regression 
    object from the dlib C++ Library.  

    This example creates a simple set of data to train on and then shows
    you how to use the kernel ridge regression tool to find a good decision 
    function that can classify examples in our data set.


    The data used in this example will be 2 dimensional data and will
    come from a distribution where points with a distance less than 13
    from the origin are labeled +1 and all other points are labeled
    as -1.  All together, the dataset will contain 10201 sample points.
        
*/


#include <iostream>
#include <dlib/svm_threaded.h>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <dirent.h>
#include <stdio.h>
#include <slowml/vectordata.hpp>
#include <slowml/label.hpp>

using namespace dlib;

std::vector<std::string> GetLabelsDirs(const std::string &path) // {{{
{ std::vector<std::string> result;
  DIR *dir = opendir(path.c_str());
  struct dirent *entry = readdir(dir);
  while (entry != NULL)
  {
    if (entry->d_type == DT_DIR && std::string(entry->d_name).substr(0,6)=="label_")
      result.push_back(entry->d_name);
    entry = readdir(dir);
  }
  closedir(dir);
  return result;
} // }}}
std::vector<std::string> GetVectorFiles(const std::string &path) // {{{
{ std::vector<std::string> result;
  DIR *dir = opendir(path.c_str());
  struct dirent *entry = readdir(dir);
  while (entry != NULL)
  {
    std::string name(entry->d_name);
    if (name.size()>4 && entry->d_type == DT_REG && name.substr(name.size()-4,4)==".vec")
      result.push_back(entry->d_name);
    entry = readdir(dir);
  }
  closedir(dir);
  return result;
} // }}}

int main(int argc, char **argv)
{ try
  { if (argc==2 && std::string("--help")==argv[1] || argc==1)
      throw std::string("Syntax is usemodel [--model|-m <modelpath>] <datapath>");

    std::string datapath="./data";
    std::string modelfile="./model.smart";

    for (size_t arg=1; arg+1<argc; ++arg)
    { if (std::string("--model")==argv[arg] || std::string("-m")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw std::string("--model must be succeeded by a value");
        modelfile=argv[arg];
      }
      else
        throw std::string("Unknown argument") + argv[arg];
    }

    datapath=argv[argc-1];
    // Load model
    typedef matrix<double> sample_type;
    typedef radial_basis_kernel<sample_type> kernel_type;
//    typedef probabilistic_decision_function<kernel_type> probabilistic_funct_type;  
//    typedef normalized_function<probabilistic_funct_type> pfunct_type;

    typedef one_vs_one_trainer<any_trainer<sample_type> > ovo_trainer;
    one_vs_one_decision_function<ovo_trainer,decision_function<kernel_type> > df;
    deserialize(modelfile) >> df;
    std::cout << "Loaded model" << std::endl;
    
    size_t successes=0;
    size_t errors=0;

    // Find labels
    std::vector<std::string> labels=GetLabelsDirs(datapath);

    // Read data
    for (size_t label=0; label<labels.size(); ++label)
    { std::cout << "Label: " << labels[label] << std::endl;
      std::vector<std::string> vfiles=GetVectorFiles(datapath+"/"+labels[label]);
      for (auto vfile=vfiles.begin(); vfile!=vfiles.end(); ++vfile)
      { std::ifstream fin(datapath+"/"+labels[label]+"/"+*vfile);
        VectorData<double> data(std::vector<double>(),0,0);
        data.LoadRow(fin);
        fin.close();
        sample_type samp(data.Width(),1);
        for (size_t i=0; i<data.Width(); ++i)
          samp(i)=data.GetValue(0,i);
        // Eval
        int result=(int)df(samp);
        if (result!=label)
        { std::cout << "Error on file: " << datapath << "/" << labels[label] << "/" << *vfile << " (returned " << labels[result].substr(6) << "!=" << labels[label].substr(6) << ")" << std::endl;
          ++errors;
        }
        else
        { std::cout << "Success on file: " << datapath << "/" <<labels[label] << "/" << *vfile << std::endl;
          ++successes;
        }
      }
    }
    std::cout << "Total " << successes << " succeeded (" << (100.0 * successes) / (successes+errors) << "%)" << std::endl
         << "Total " << errors << " failed (" << (100.0 * errors) / (successes+errors) << "%)" << std::endl;
  }
  catch (const std::string &error)
  { std::cerr << "Error: " << error << std::endl;
  }
  return 0;
}
