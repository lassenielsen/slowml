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
      throw std::string("Syntax is smartml_trainer [--gamma|-g <double>] [--output|-o <modelpath>] [--continue|-c] [--debug|-d] <datapath>");

    std::string datapath="./data";
    std::string modelfile="./model.pars";
    double gamma=0.000625;
    bool option_debug=false;
    bool option_continue=false;

    for (size_t arg=1; arg+1<argc; ++arg)
    { if (std::string("--gamma")==argv[arg] || std::string("-g")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw std::string("--alphainv must be succeeded by a value");
        std::stringstream ss;
        ss << argv[arg];
        ss >> gamma;
        if (gamma<=0)
          throw std::string("--alpha must be succeeded by positive integer");
      }
      else if (std::string("--output")==argv[arg] || std::string("-o")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw std::string("--output must be succeeded by a value");
        modelfile=argv[arg];
      }
      else if (std::string("--continue")==argv[arg] || std::string("-c")==argv[arg])
        option_continue=true;
      else if (std::string("--debug")==argv[arg] || std::string("-d")==argv[arg])
        option_debug=true;
      else
        throw std::string("Unknown argument") + argv[arg];
    }

    datapath=argv[argc-1];

    // Find labels
    std::vector<std::string> labels=GetLabelsDirs(datapath);

    // Read data
    typedef matrix<double> sample_type;
    std::vector<sample_type> data;
    std::vector<double> truths;
    for (size_t label=0; label<labels.size(); ++label)
    { std::cout << "Label: " << labels[label] << std::endl;
      std::vector<std::string> vfiles=GetVectorFiles(datapath+"/"+labels[label]);
      for (auto vfile=vfiles.begin(); vfile!=vfiles.end(); ++vfile)
      { VectorData<double> vdata(std::vector<double>(),0,0);
        std::ifstream fin(datapath+"/"+labels[label]+"/"+*vfile);
        vdata.LoadRow(fin);
        fin.close();
        sample_type samp(vdata.Width(),1);
        for (size_t i=0; i<vdata.Width(); ++i)
          samp(i)=vdata.GetValue(0,i);
        data.push_back(samp);
        truths.push_back((double)label);
        std::cout << "." << std::flush;
      }
      std::cout << std::endl;
    }

    std::cout << "Number of samples: " << data.size() << std::endl;
    for (size_t label=0; label<labels.size(); ++label)
      std::cout << "  number of " << labels[label] << " samples: " << sum(mat(truths) == label) << std::endl;

    // Finally, make the one_vs_one_trainer.
    typedef one_vs_one_trainer<any_trainer<sample_type> > ovo_trainer;
    ovo_trainer trainer;
    typedef radial_basis_kernel<sample_type> kernel_type;
    krr_trainer<kernel_type> rbf_trainer;
    rbf_trainer.set_kernel(kernel_type(gamma));
    trainer.set_trainer(rbf_trainer);

    // Randomize training data
    randomize_samples(data, truths);

    // The model (function) to train
    one_vs_one_decision_function<ovo_trainer,decision_function<kernel_type>> df;

    df=trainer.train(data, truths);
    //vector_normalizer<sample_type> normalizer;
    //normalizer.train(data);
    //for (unsigned long i=0; i<data.size(); ++i)
    //  data[i] = normalizer(data[i]); 

    //krr_trainer<kernel_type> trainer;
    //trainer.use_classification_loss_for_loo_cv();

    //std::cout << "\ndoing leave-one-out cross-validation" << std::endl;
    //for (double tgamma = 0.000001; tgamma <= 1; tgamma *= 5)
    //{
    //    // tell the trainer the parameters we want to use
    //    trainer.set_kernel(kernel_type(tgamma));

    //    // loo_values will contain the LOO predictions for each sample.  In the case
    //    // of perfect prediction it will end up being a copy of labels.
    //    std::vector<double> loo_values; 
    //    trainer.train(data, truths, loo_values);

    //    // Print gamma and the fraction of samples correctly classified during LOO cross-validation.
    //    const double classification_accuracy = mean_sign_agreement(truths, loo_values);
    //    std::cout << "gamma: " << tgamma << "     LOO accuracy: " << classification_accuracy << std::endl;
    //}


    // From looking at the output of the above loop it turns out that a good value for 
    // gamma for this problem is 0.000625.  So that is what we will use.
    //trainer.set_kernel(kernel_type(gamma));
    //typedef probabilistic_decision_function<kernel_type> probabilistic_funct_type;  
    //typedef normalized_function<probabilistic_funct_type> pfunct_type;
    //pfunct_type learned_pfunct; 
    //learned_pfunct.normalizer = normalizer;
    //learned_pfunct.function = train_probabilistic_decision_function(trainer, data, truths, 3);
    serialize(modelfile) << df;
  }
  catch (const std::string &error)
  { std::cerr << "Error: " << error << std::endl;
  }
  return 0;
}
