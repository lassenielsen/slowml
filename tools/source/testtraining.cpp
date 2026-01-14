#include <slowml/linearregressionmodel.hpp>
#include <slowml/logisticregressionmodel.hpp>
#include <slowml/guidedvectordata.hpp>
#include <slowml/network.hpp>
#include <slowml/onevsall.hpp>
#include <slowml/reinforcementlearning.hpp>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

using namespace std;
bool TestLinRM1() // {{{
{ // INIT DATA
  VectorData<double> t_samples_data(std::vector<double>(),3,0);
  VectorData<double> t_samples_result(std::vector<double>(),1,0);
  // DATA {{{
  t_samples_data.AddRow(std::vector<double>{1.0,1.0,1.0});
  t_samples_result.AddRow(std::vector<double>{4.0});
  t_samples_data.AddRow(std::vector<double>{1.0,3.0,3.0});
  t_samples_result.AddRow(std::vector<double>{10.0});
  t_samples_data.AddRow(std::vector<double>{1.0,0.0,1.0});
  t_samples_result.AddRow(std::vector<double>{3.0});
  t_samples_data.AddRow(std::vector<double>{1.0,2.0,1.0});
  t_samples_result.AddRow(std::vector<double>{5.0});
  t_samples_data.AddRow(std::vector<double>{1.0,30.0,1.0});
  t_samples_result.AddRow(std::vector<double>{33.0});
  t_samples_data.AddRow(std::vector<double>{1.0,1.0,30.0});
  t_samples_result.AddRow(std::vector<double>{62.0});
  // }}}
  GuidedVectorData<double,double> t_samples(t_samples_data,t_samples_result.GetValues());
  // DEFINE MODEL
  LinearRegressionModel linrm;
  linrm.AddParameter(0.0);
  linrm.AddParameter(0.0);
  // FIT MODEL
  double alpha_inv=100.0;
  double lambda=0.0;
  linrm.FitParameters(t_samples,alpha_inv,lambda,1500,5.0,false); // Fit
  // VALIDATE FIT
  if (abs(linrm.GetParameter(0)-1.0)>0.1)
  { cerr << "TestLinRM1 failed - Fitted first parameter is too far from 1: " << linrm.GetParameter(0) << endl;
    return false;
  }
  if (abs(linrm.GetParameter(1)-1.0)>0.1)
  { cerr << "TestLinRM1 failed - Fitted second parameter is too far from 1: " << linrm.GetParameter(1) << endl;
    return false;
  }
  if (abs(linrm.GetParameter(2)-2.0)>0.1)
  { cerr << "TestLinRM1 failed - Fitted third parameter is too far from 2: " << linrm.GetParameter(2) << endl;
    return false;
  }
  double cost=linrm.Cost(t_samples,lambda);
  if (cost>0.1)
  { cerr << "TestLinRM1 failed - Cost after fit is too far from 0: " << cost << endl;
    return false;
  }
  // Test
  VectorData<double> t_test_data(std::vector<double>(),3,0);
  VectorData<double> t_test_result(std::vector<double>(),1,0);
  // DATA {{{
  t_test_data.AddRow(std::vector<double>{1.0,2.0,1.0});
  t_test_result.AddRow(std::vector<double>{5.0});
  t_test_data.AddRow(std::vector<double>{1.0,4.0,3.0});
  t_test_result.AddRow(std::vector<double>{11.0});
  t_test_data.AddRow(std::vector<double>{1.0,1.0,1.0});
  t_test_result.AddRow(std::vector<double>{4.0});
  t_test_data.AddRow(std::vector<double>{1.0,3.0,1.0});
  t_test_result.AddRow(std::vector<double>{6.0});
  t_test_data.AddRow(std::vector<double>{1.0,31.0,1.0});
  t_test_result.AddRow(std::vector<double>{34.0});
  t_test_data.AddRow(std::vector<double>{1.0,2.0,30.0});
  t_test_result.AddRow(std::vector<double>{63.0});
  // }}}
  GuidedVectorData<double,double> t_test(t_test_data,t_test_result.GetValues());
  for (size_t instance=0; instance<t_test.Height(); ++instance)
  { double eval=linrm.Eval(t_test,instance);
    if (abs(eval-t_test.GetResult(instance))>0.1)
    { cerr << "TestLinRM1 failed - Predicted " << eval << "!=" << t_test.GetResult(instance) << " on test " << instance << endl;
      return false;
    }
  }
  return true;
} // }}}
bool TestLinRM2() // {{{
{ // INIT DATA
  VectorData<double> t_samples_data(std::vector<double>(),2,0);
  VectorData<double> t_samples_result(std::vector<double>(),1,0);
  // DATA {{{
  t_samples_data.AddRow(std::vector<double>{1.0, 6.1101});
  t_samples_result.AddRow(std::vector<double>{17.592});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.5277});
  t_samples_result.AddRow(std::vector<double>{9.1302});
  t_samples_data.AddRow(std::vector<double>{1.0, 8.5186});
  t_samples_result.AddRow(std::vector<double>{13.662});
  t_samples_data.AddRow(std::vector<double>{1.0, 7.0032});
  t_samples_result.AddRow(std::vector<double>{11.854});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.8598});
  t_samples_result.AddRow(std::vector<double>{6.8233});
  t_samples_data.AddRow(std::vector<double>{1.0, 8.3829});
  t_samples_result.AddRow(std::vector<double>{11.886});
  t_samples_data.AddRow(std::vector<double>{1.0, 7.4764});
  t_samples_result.AddRow(std::vector<double>{4.3483});
  t_samples_data.AddRow(std::vector<double>{1.0, 8.5781});
  t_samples_result.AddRow(std::vector<double>{12});
  t_samples_data.AddRow(std::vector<double>{1.0, 6.4862});
  t_samples_result.AddRow(std::vector<double>{6.5987});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.0546});
  t_samples_result.AddRow(std::vector<double>{3.8166});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.7107});
  t_samples_result.AddRow(std::vector<double>{3.2522});
  t_samples_data.AddRow(std::vector<double>{1.0, 14.164});
  t_samples_result.AddRow(std::vector<double>{15.505});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.734});
  t_samples_result.AddRow(std::vector<double>{3.1551});
  t_samples_data.AddRow(std::vector<double>{1.0, 8.4084});
  t_samples_result.AddRow(std::vector<double>{7.2258});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.6407});
  t_samples_result.AddRow(std::vector<double>{0.71618});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.3794});
  t_samples_result.AddRow(std::vector<double>{3.5129});
  t_samples_data.AddRow(std::vector<double>{1.0, 6.3654});
  t_samples_result.AddRow(std::vector<double>{5.3048});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.1301});
  t_samples_result.AddRow(std::vector<double>{0.56077});
  t_samples_data.AddRow(std::vector<double>{1.0, 6.4296});
  t_samples_result.AddRow(std::vector<double>{3.6518});
  t_samples_data.AddRow(std::vector<double>{1.0, 7.0708});
  t_samples_result.AddRow(std::vector<double>{5.3893});
  t_samples_data.AddRow(std::vector<double>{1.0, 6.1891});
  t_samples_result.AddRow(std::vector<double>{3.1386});
  t_samples_data.AddRow(std::vector<double>{1.0, 20.27});
  t_samples_result.AddRow(std::vector<double>{21.767});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.4901});
  t_samples_result.AddRow(std::vector<double>{4.263});
  t_samples_data.AddRow(std::vector<double>{1.0, 6.3261});
  t_samples_result.AddRow(std::vector<double>{5.1875});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.5649});
  t_samples_result.AddRow(std::vector<double>{3.0825});
  t_samples_data.AddRow(std::vector<double>{1.0, 18.945});
  t_samples_result.AddRow(std::vector<double>{22.638});
  t_samples_data.AddRow(std::vector<double>{1.0, 12.828});
  t_samples_result.AddRow(std::vector<double>{13.501});
  t_samples_data.AddRow(std::vector<double>{1.0, 10.957});
  t_samples_result.AddRow(std::vector<double>{7.0467});
  t_samples_data.AddRow(std::vector<double>{1.0, 13.176});
  t_samples_result.AddRow(std::vector<double>{14.692});
  t_samples_data.AddRow(std::vector<double>{1.0, 22.203});
  t_samples_result.AddRow(std::vector<double>{24.147});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.2524});
  t_samples_result.AddRow(std::vector<double>{-1.22});
  t_samples_data.AddRow(std::vector<double>{1.0, 6.5894});
  t_samples_result.AddRow(std::vector<double>{5.9966});
  t_samples_data.AddRow(std::vector<double>{1.0, 9.2482});
  t_samples_result.AddRow(std::vector<double>{12.134});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.8918});
  t_samples_result.AddRow(std::vector<double>{1.8495});
  t_samples_data.AddRow(std::vector<double>{1.0, 8.2111});
  t_samples_result.AddRow(std::vector<double>{6.5426});
  t_samples_data.AddRow(std::vector<double>{1.0, 7.9334});
  t_samples_result.AddRow(std::vector<double>{4.5623});
  t_samples_data.AddRow(std::vector<double>{1.0, 8.0959});
  t_samples_result.AddRow(std::vector<double>{4.1164});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.6063});
  t_samples_result.AddRow(std::vector<double>{3.3928});
  t_samples_data.AddRow(std::vector<double>{1.0, 12.836});
  t_samples_result.AddRow(std::vector<double>{10.117});
  t_samples_data.AddRow(std::vector<double>{1.0, 6.3534});
  t_samples_result.AddRow(std::vector<double>{5.4974});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.4069});
  t_samples_result.AddRow(std::vector<double>{0.55657});
  t_samples_data.AddRow(std::vector<double>{1.0, 6.8825});
  t_samples_result.AddRow(std::vector<double>{3.9115});
  t_samples_data.AddRow(std::vector<double>{1.0, 11.708});
  t_samples_result.AddRow(std::vector<double>{5.3854});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.7737});
  t_samples_result.AddRow(std::vector<double>{2.4406});
  t_samples_data.AddRow(std::vector<double>{1.0, 7.8247});
  t_samples_result.AddRow(std::vector<double>{6.7318});
  t_samples_data.AddRow(std::vector<double>{1.0, 7.0931});
  t_samples_result.AddRow(std::vector<double>{1.0463});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.0702});
  t_samples_result.AddRow(std::vector<double>{5.1337});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.8014});
  t_samples_result.AddRow(std::vector<double>{1.844});
  t_samples_data.AddRow(std::vector<double>{1.0, 11.7});
  t_samples_result.AddRow(std::vector<double>{8.0043});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.5416});
  t_samples_result.AddRow(std::vector<double>{1.0179});
  t_samples_data.AddRow(std::vector<double>{1.0, 7.5402});
  t_samples_result.AddRow(std::vector<double>{6.7504});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.3077});
  t_samples_result.AddRow(std::vector<double>{1.8396});
  t_samples_data.AddRow(std::vector<double>{1.0, 7.4239});
  t_samples_result.AddRow(std::vector<double>{4.2885});
  t_samples_data.AddRow(std::vector<double>{1.0, 7.6031});
  t_samples_result.AddRow(std::vector<double>{4.9981});
  t_samples_data.AddRow(std::vector<double>{1.0, 6.3328});
  t_samples_result.AddRow(std::vector<double>{1.4233});
  t_samples_data.AddRow(std::vector<double>{1.0, 6.3589});
  t_samples_result.AddRow(std::vector<double>{-1.4211});
  t_samples_data.AddRow(std::vector<double>{1.0, 6.2742});
  t_samples_result.AddRow(std::vector<double>{2.4756});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.6397});
  t_samples_result.AddRow(std::vector<double>{4.6042});
  t_samples_data.AddRow(std::vector<double>{1.0, 9.3102});
  t_samples_result.AddRow(std::vector<double>{3.9624});
  t_samples_data.AddRow(std::vector<double>{1.0, 9.4536});
  t_samples_result.AddRow(std::vector<double>{5.4141});
  t_samples_data.AddRow(std::vector<double>{1.0, 8.8254});
  t_samples_result.AddRow(std::vector<double>{5.1694});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.1793});
  t_samples_result.AddRow(std::vector<double>{-0.74279});
  t_samples_data.AddRow(std::vector<double>{1.0, 21.279});
  t_samples_result.AddRow(std::vector<double>{17.929});
  t_samples_data.AddRow(std::vector<double>{1.0, 14.908});
  t_samples_result.AddRow(std::vector<double>{12.054});
  t_samples_data.AddRow(std::vector<double>{1.0, 18.959});
  t_samples_result.AddRow(std::vector<double>{17.054});
  t_samples_data.AddRow(std::vector<double>{1.0, 7.2182});
  t_samples_result.AddRow(std::vector<double>{4.8852});
  t_samples_data.AddRow(std::vector<double>{1.0, 8.2951});
  t_samples_result.AddRow(std::vector<double>{5.7442});
  t_samples_data.AddRow(std::vector<double>{1.0, 10.236});
  t_samples_result.AddRow(std::vector<double>{7.7754});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.4994});
  t_samples_result.AddRow(std::vector<double>{1.0173});
  t_samples_data.AddRow(std::vector<double>{1.0, 20.341});
  t_samples_result.AddRow(std::vector<double>{20.992});
  t_samples_data.AddRow(std::vector<double>{1.0, 10.136});
  t_samples_result.AddRow(std::vector<double>{6.6799});
  t_samples_data.AddRow(std::vector<double>{1.0, 7.3345});
  t_samples_result.AddRow(std::vector<double>{4.0259});
  t_samples_data.AddRow(std::vector<double>{1.0, 6.0062});
  t_samples_result.AddRow(std::vector<double>{1.2784});
  t_samples_data.AddRow(std::vector<double>{1.0, 7.2259});
  t_samples_result.AddRow(std::vector<double>{3.3411});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.0269});
  t_samples_result.AddRow(std::vector<double>{-2.6807});
  t_samples_data.AddRow(std::vector<double>{1.0, 6.5479});
  t_samples_result.AddRow(std::vector<double>{0.29678});
  t_samples_data.AddRow(std::vector<double>{1.0, 7.5386});
  t_samples_result.AddRow(std::vector<double>{3.8845});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.0365});
  t_samples_result.AddRow(std::vector<double>{5.7014});
  t_samples_data.AddRow(std::vector<double>{1.0, 10.274});
  t_samples_result.AddRow(std::vector<double>{6.7526});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.1077});
  t_samples_result.AddRow(std::vector<double>{2.0576});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.7292});
  t_samples_result.AddRow(std::vector<double>{0.47953});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.1884});
  t_samples_result.AddRow(std::vector<double>{0.20421});
  t_samples_data.AddRow(std::vector<double>{1.0, 6.3557});
  t_samples_result.AddRow(std::vector<double>{0.67861});
  t_samples_data.AddRow(std::vector<double>{1.0, 9.7687});
  t_samples_result.AddRow(std::vector<double>{7.5435});
  t_samples_data.AddRow(std::vector<double>{1.0, 6.5159});
  t_samples_result.AddRow(std::vector<double>{5.3436});
  t_samples_data.AddRow(std::vector<double>{1.0, 8.5172});
  t_samples_result.AddRow(std::vector<double>{4.2415});
  t_samples_data.AddRow(std::vector<double>{1.0, 9.1802});
  t_samples_result.AddRow(std::vector<double>{6.7981});
  t_samples_data.AddRow(std::vector<double>{1.0, 6.002});
  t_samples_result.AddRow(std::vector<double>{0.92695});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.5204});
  t_samples_result.AddRow(std::vector<double>{0.152});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.0594});
  t_samples_result.AddRow(std::vector<double>{2.8214});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.7077});
  t_samples_result.AddRow(std::vector<double>{1.8451});
  t_samples_data.AddRow(std::vector<double>{1.0, 7.6366});
  t_samples_result.AddRow(std::vector<double>{4.2959});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.8707});
  t_samples_result.AddRow(std::vector<double>{7.2029});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.3054});
  t_samples_result.AddRow(std::vector<double>{1.9869});
  t_samples_data.AddRow(std::vector<double>{1.0, 8.2934});
  t_samples_result.AddRow(std::vector<double>{0.14454});
  t_samples_data.AddRow(std::vector<double>{1.0, 13.394});
  t_samples_result.AddRow(std::vector<double>{9.0551});
  t_samples_data.AddRow(std::vector<double>{1.0, 5.4369});
  t_samples_result.AddRow(std::vector<double>{0.61705});
  // }}}
  GuidedVectorData<double,double> t_samples(t_samples_data,t_samples_result.GetValues());
  // DEFINE MODEL
  LinearRegressionModel linrm;
  linrm.AddParameter(0.0);
  // FIT MODEL
  double alpha_inv=100.0;
  double lambda=1.0;
  linrm.FitParameters(t_samples,alpha_inv,lambda,1500,5.0,false); // Fit
  if (abs(linrm.GetParameter(0)+3.63)>0.1)
  { cerr << "TestLinRM2 failed - Fitted first parameter is too far from -3.63: " << linrm.GetParameter(0) << endl;
    return false;
  }
  if (abs(linrm.GetParameter(1)-1.16)>0.1)
  { cerr << "TestLinRM2 failed - Fitted second parameter is too far from 1.16: " << linrm.GetParameter(1) << endl;
    return false;
  }
  double cost=linrm.Cost(t_samples,lambda);
  if (abs(cost-4.48)>0.1)
  { cerr << "TestLinRM2 failed - Cost after fit is too far from 4.48: " << cost << endl;
    return false;
  }
  return true;
} // }}}
bool TestLogRM1() // {{{
{ // INIT DATA
  VectorData<double> t_samples_data(std::vector<double>(),3,0);
  VectorData<double> t_samples_result(std::vector<double>(),1,0);
  //// DATA {{{
  t_samples_data.AddRow(std::vector<double>{1.0,34.62365962451697,78.0246928153624});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,30.28671076822607,43.89499752400101});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,35.84740876993872,72.90219802708364});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,60.18259938620976,86.30855209546826});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,79.0327360507101,75.3443764369103});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,45.08327747668339,56.3163717815305});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,61.10666453684766,96.51142588489624});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,75.02474556738889,46.55401354116538});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,76.09878670226257,87.42056971926803});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,84.43281996120035,43.53339331072109});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,95.86155507093572,38.22527805795094});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,75.01365838958247,30.60326323428011});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,82.30705337399482,76.48196330235604});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,69.36458875970939,97.71869196188608});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,39.53833914367223,76.03681085115882});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,53.9710521485623,89.20735013750205});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,69.07014406283025,52.74046973016765});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,67.94685547711617,46.67857410673128});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,70.66150955499435,92.92713789364831});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,76.97878372747498,47.57596364975532});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,67.37202754570876,42.83843832029179});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,89.67677575072079,65.79936592745237});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,50.534788289883,48.85581152764205});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,34.21206097786789,44.20952859866288});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,77.9240914545704,68.9723599933059});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,62.27101367004632,69.95445795447587});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,80.1901807509566,44.82162893218353});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,93.114388797442,38.80067033713209});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,61.83020602312595,50.25610789244621});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,38.78580379679423,64.99568095539578});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,61.379289447425,72.80788731317097});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,85.40451939411645,57.05198397627122});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,52.10797973193984,63.12762376881715});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,52.04540476831827,69.43286012045222});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,40.23689373545111,71.16774802184875});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,54.63510555424817,52.21388588061123});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,33.91550010906887,98.86943574220611});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,64.17698887494485,80.90806058670817});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,74.78925295941542,41.57341522824434});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,34.1836400264419,75.2377203360134});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,83.90239366249155,56.30804621605327});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,51.54772026906181,46.85629026349976});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,94.44336776917852,65.56892160559052});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,82.36875375713919,40.61825515970618});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,51.04775177128865,45.82270145776001});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,62.22267576120188,52.06099194836679});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,77.19303492601364,70.45820000180959});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,97.77159928000232,86.7278223300282});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,62.07306379667647,96.76882412413983});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,91.56497449807442,88.69629254546599});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,79.94481794066932,74.16311935043758});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,99.2725269292572,60.99903099844988});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,90.54671411399852,43.39060180650027});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,34.52451385320009,60.39634245837173});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,50.2864961189907,49.80453881323059});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,49.58667721632031,59.80895099453265});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,97.64563396007767,68.86157272420604});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,32.57720016809309,95.59854761387875});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,74.24869136721598,69.82457122657193});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,71.79646205863379,78.45356224515052});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,75.3956114656803,85.75993667331619});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,35.28611281526193,47.02051394723416});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,56.25381749711624,39.26147251058019});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,30.05882244669796,49.59297386723685});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,44.66826172480893,66.45008614558913});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,66.56089447242954,41.09209807936973});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,40.45755098375164,97.53518548909936});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,49.07256321908844,51.88321182073966});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,80.27957401466998,92.11606081344084});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,66.74671856944039,60.99139402740988});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,32.72283304060323,43.30717306430063});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,64.0393204150601,78.03168802018232});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,72.34649422579923,96.22759296761404});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,60.45788573918959,73.09499809758037});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,58.84095621726802,75.85844831279042});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,99.82785779692128,72.36925193383885});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,47.26426910848174,88.47586499559782});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,50.45815980285988,75.80985952982456});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,60.45555629271532,42.50840943572217});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,82.22666157785568,42.71987853716458});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,88.9138964166533,69.80378889835472});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,94.83450672430196,45.69430680250754});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,67.31925746917527,66.58935317747915});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,57.23870631569862,59.51428198012956});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,80.36675600171273,90.96014789746954});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,68.46852178591112,85.59430710452014});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,42.0754545384731,78.84478600148043});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,75.47770200533905,90.42453899753964});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,78.63542434898018,96.64742716885644});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,52.34800398794107,60.76950525602592});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,94.09433112516793,77.15910509073893});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,90.44855097096364,87.50879176484702});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,55.48216114069585,35.57070347228866});
  t_samples_result.AddRow(std::vector<double>{0.0});
  t_samples_data.AddRow(std::vector<double>{1.0,74.49269241843041,84.84513684930135});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,89.84580670720979,45.35828361091658});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,83.48916274498238,48.38028579728175});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,42.2617008099817,87.10385094025457});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,99.31500880510394,68.77540947206617});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,55.34001756003703,64.9319380069486});
  t_samples_result.AddRow(std::vector<double>{1.0});
  t_samples_data.AddRow(std::vector<double>{1.0,74.77589300092767,89.52981289513276});
  t_samples_result.AddRow(std::vector<double>{1.0});
  // }}}
  GuidedVectorData<double,double> t_samples(t_samples_data,t_samples_result.GetValues());
  // DEFINE MODEL
  LogisticRegressionModel logrm;
  logrm.AddParameter(0.0);
  logrm.AddParameter(0.0);
  // Test Delta
  vector<double> delta=logrm.Delta(t_samples,0.0);
  //cout << "Delta=[" << delta[0] << "," << delta[1] << "," << delta[2] << "]" << endl;
  //cout << "Cost=" << logrm.Cost(t_samples,0.0) << endl;
  logrm.SetParameter(0,-24.0);
  logrm.SetParameter(1,0.2);
  logrm.SetParameter(2,0.2);
  delta=logrm.Delta(t_samples,0.0);
  //cout << "Delta=[" << delta[0] << "," << delta[1] << "," << delta[2] << "]" << endl;
  //cout << "Cost=" << logrm.Cost(t_samples,0.0) << endl;
  //// FIT MODEL
  double alpha_inv=250.0;
  double lambda=0.0;
  logrm.FitParameters(t_samples,alpha_inv,lambda,15000,5.0,false); // Fit
  //cout << "Fittet parameters: [" << logrm.GetParameter(0) << "," << logrm.GetParameter(1) << "," << logrm.GetParameter(2) << "]" << endl;
  //cout << "Fittet cost: " << logrm.Cost(t_samples,lambda) << endl;
  //// VALIDATE FIT
  if (abs(logrm.GetParameter(0)+25.16)>1.5)
  { cerr << "TestLogRM1 failed - Fitted first parameter is too far from -25.16: " << logrm.GetParameter(0) << endl;
    return false;
  }
  if (abs(logrm.GetParameter(1)-0.206)>0.1)
  { cerr << "TestLogRM1 failed - Fitted second parameter is too far from 1: " << logrm.GetParameter(1) << endl;
    return false;
  }
  if (abs(logrm.GetParameter(2)-0.201)>0.1)
  { cerr << "TestLogRM1 failed - Fitted third parameter is too far from 2: " << logrm.GetParameter(2) << endl;
    return false;
  }
  double cost=logrm.Cost(t_samples,lambda);
  if (abs(cost-0.203)>0.1)
  { cerr << "TestLogRM1 failed - Cost after fit is too far from 0: " << cost << endl;
    return false;
  }
  //// Test
  VectorData<double> t_test_data(std::vector<double>(),3,0);
  VectorData<double> t_test_result(std::vector<double>(),1,0);
  // DATA {{{
  t_test_data.AddRow(std::vector<double>{1.0,61.379289447425,72.80788731317097});
  t_test_result.AddRow(std::vector<double>{1.0});
  t_test_data.AddRow(std::vector<double>{1.0,85.40451939411645,57.05198397627122});
  t_test_result.AddRow(std::vector<double>{1.0});
  t_test_data.AddRow(std::vector<double>{1.0,52.10797973193984,63.12762376881715});
  t_test_result.AddRow(std::vector<double>{0.0});
  t_test_data.AddRow(std::vector<double>{1.0,52.04540476831827,79.43286012045222});
  t_test_result.AddRow(std::vector<double>{1.0});
  t_test_data.AddRow(std::vector<double>{1.0,40.23689373545111,71.16774802184875});
  t_test_result.AddRow(std::vector<double>{0.0});
  t_test_data.AddRow(std::vector<double>{1.0,54.63510555424817,52.21388588061123});
  t_test_result.AddRow(std::vector<double>{0.0});
  // }}}
  GuidedVectorData<double,double> t_test(t_test_data,t_test_result.GetValues());
  for (size_t instance=0; instance<t_test.Height(); ++instance)
  { double eval=logrm.Eval(t_test,instance);
    if (abs(eval-t_test.GetResult(instance))>0.2)
    { cerr << "TestLinRM1 failed - Predicted " << eval << "!=" << t_test.GetResult(instance) << " on test " << instance << endl;
      return false;
    }
  }
  return true;
} // }}}
bool TestLogRM2() // {{{
{ // INIT DATA
  VectorData<double> t_samples_data(std::vector<double>(),3,0);
  vector<double> t_samples_result;
  // DATA {{{
  t_samples_data.AddRow(std::vector<double>{1.0,34.62365962451697,78.0246928153624});  t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,30.28671076822607,43.89499752400101}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,35.84740876993872,72.90219802708364}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,60.18259938620976,86.30855209546826}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,79.0327360507101,75.3443764369103});   t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,45.08327747668339,56.3163717815305});  t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,61.10666453684766,96.51142588489624}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,75.02474556738889,46.55401354116538}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,76.09878670226257,87.42056971926803}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,84.43281996120035,43.53339331072109}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,95.86155507093572,38.22527805795094}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,75.01365838958247,30.60326323428011}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,82.30705337399482,76.48196330235604}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,69.36458875970939,97.71869196188608}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,39.53833914367223,76.03681085115882}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,53.9710521485623,89.20735013750205});  t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,69.07014406283025,52.74046973016765}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,67.94685547711617,46.67857410673128}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,70.66150955499435,92.92713789364831}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,76.97878372747498,47.57596364975532}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,67.37202754570876,42.83843832029179}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,89.67677575072079,65.79936592745237}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,50.534788289883,48.85581152764205});   t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,34.21206097786789,44.20952859866288}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,77.9240914545704,68.9723599933059});   t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,62.27101367004632,69.95445795447587}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,80.1901807509566,44.82162893218353});  t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,93.114388797442,38.80067033713209});   t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,61.83020602312595,50.25610789244621}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,38.78580379679423,64.99568095539578}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,61.379289447425,72.80788731317097});   t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,85.40451939411645,57.05198397627122}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,52.10797973193984,63.12762376881715}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,52.04540476831827,69.43286012045222}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,40.23689373545111,71.16774802184875}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,54.63510555424817,52.21388588061123}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,33.91550010906887,98.86943574220611}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,64.17698887494485,80.90806058670817}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,74.78925295941542,41.57341522824434}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,34.1836400264419,75.2377203360134});   t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,83.90239366249155,56.30804621605327}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,51.54772026906181,46.85629026349976}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,94.44336776917852,65.56892160559052}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,82.36875375713919,40.61825515970618}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,51.04775177128865,45.82270145776001}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,62.22267576120188,52.06099194836679}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,77.19303492601364,70.45820000180959}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,97.77159928000232,86.7278223300282});  t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,62.07306379667647,96.76882412413983}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,91.56497449807442,88.69629254546599}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,79.94481794066932,74.16311935043758}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,99.2725269292572,60.99903099844988});  t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,90.54671411399852,43.39060180650027}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,34.52451385320009,60.39634245837173}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,50.2864961189907,49.80453881323059});  t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,49.58667721632031,59.80895099453265}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,97.64563396007767,68.86157272420604}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,32.57720016809309,95.59854761387875}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,74.24869136721598,69.82457122657193}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,71.79646205863379,78.45356224515052}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,75.3956114656803,85.75993667331619});  t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,35.28611281526193,47.02051394723416}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,56.25381749711624,39.26147251058019}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,30.05882244669796,49.59297386723685}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,44.66826172480893,66.45008614558913}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,66.56089447242954,41.09209807936973}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,40.45755098375164,97.53518548909936}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,49.07256321908844,51.88321182073966}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,80.27957401466998,92.11606081344084}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,66.74671856944039,60.99139402740988}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,32.72283304060323,43.30717306430063}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,64.0393204150601,78.03168802018232});  t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,72.34649422579923,96.22759296761404}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,60.45788573918959,73.09499809758037}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,58.84095621726802,75.85844831279042}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,99.82785779692128,72.36925193383885}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,47.26426910848174,88.47586499559782}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,50.45815980285988,75.80985952982456}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,60.45555629271532,42.50840943572217}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,82.22666157785568,42.71987853716458}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,88.9138964166533,69.80378889835472});  t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,94.83450672430196,45.69430680250754}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,67.31925746917527,66.58935317747915}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,57.23870631569862,59.51428198012956}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,80.36675600171273,90.96014789746954}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,68.46852178591112,85.59430710452014}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,42.0754545384731,78.84478600148043});  t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,75.47770200533905,90.42453899753964}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,78.63542434898018,96.64742716885644}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,52.34800398794107,60.76950525602592}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,94.09433112516793,77.15910509073893}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,90.44855097096364,87.50879176484702}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,55.48216114069585,35.57070347228866}); t_samples_result.push_back(0.0);
  t_samples_data.AddRow(std::vector<double>{1.0,74.49269241843041,84.84513684930135}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,89.84580670720979,45.35828361091658}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,83.48916274498238,48.38028579728175}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,42.2617008099817,87.10385094025457});  t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,99.31500880510394,68.77540947206617}); t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,55.34001756003703,64.9319380069486});  t_samples_result.push_back(1.0);
  t_samples_data.AddRow(std::vector<double>{1.0,74.77589300092767,89.52981289513276}); t_samples_result.push_back(1.0);
  // }}}
  VectorData<double> t_input_data(std::vector<double>(),28,0);
  // DATA {{{
  for (size_t instance=0; instance<t_samples_data.Height(); ++instance)
  { double x1=(t_samples_data.GetValue(instance,1)-50.0)/50.0;
    double x2=(t_samples_data.GetValue(instance,2)-50.0)/50.0;
     t_input_data.AddRow(vector<double>
      { t_samples_data.GetValue(instance,0) // 1.0
       ,x1                             // x1
       ,x2                             // x2
       ,pow(x1,2.0)                    // x1^2
       ,x1*x2                          // x1*x2
       ,pow(x2,2.0)                    // x2^2
       ,pow(x1,3.0)                    // x1^3
       ,pow(x1,2.0)*x2                 // x1^2*x2
       ,x1*pow(x2,2.0)                 // x1*x2^2
       ,pow(x2,3.0)                    // x2^3
       ,pow(x1,4.0)                    // x1^4
       ,pow(x1,3.0)*x2                 // x1^3*x2
       ,pow(x1,2.0)*pow(x2,2.0)        // x1^2*x2^2
       ,x1*pow(x2,3.0)                 // x1*x2^3
       ,pow(x2,4.0)                    // x2^4
       ,pow(x1,5.0)                    // x1^5
       ,pow(x1,4.0)*x2                 // x1^4*x2
       ,pow(x1,3.0)*pow(x2,2.0)        // x1^3*x2^2
       ,pow(x1,2.0)*pow(x2,3.0)        // x1^2*x2^3
       ,x1*pow(x2,4.0)                 // x1*x2^4
       ,pow(x2,5.0)                    // x2^5
       ,pow(x1,6.0)                    // x1^6
       ,pow(x1,5.0)*x2                 // x1^5*x2
       ,pow(x1,4.0)*pow(x2,2.0)        // x1^4*x2^2
       ,pow(x1,3.0)*pow(x2,3.0)        // x1^3*x2^3
       ,pow(x1,2.0)*pow(x2,4.0)        // x1^2*x2^4
       ,x1*pow(x2,5.0)                 // x1*x2^5
       ,pow(x2,6.0)                    // x2^6
      });
  }
  GuidedVectorData<double,double> t_input(t_input_data,t_samples_result);
  // }}}
  // DEFINE MODEL
  LogisticRegressionModel logrm;
  // DEFINE FEATURES {{{
  logrm.AddParameter(0.0);
  logrm.AddParameter(0.0);
  logrm.AddParameter(0.0);
  logrm.AddParameter(0.0);
  logrm.AddParameter(0.0);
  logrm.AddParameter(0.0);
  logrm.AddParameter(0.0);
  logrm.AddParameter(0.0);
  logrm.AddParameter(0.0);
  logrm.AddParameter(0.0);
  logrm.AddParameter(0.0);
  logrm.AddParameter(0.0);
  logrm.AddParameter(0.0);
  logrm.AddParameter(0.0);
  logrm.AddParameter(0.0);
  logrm.AddParameter(0.0);
  logrm.AddParameter(0.0);
  logrm.AddParameter(0.0);
  logrm.AddParameter(0.0);
  logrm.AddParameter(0.0);
  logrm.AddParameter(0.0);
  logrm.AddParameter(0.0);
  logrm.AddParameter(0.0);
  logrm.AddParameter(0.0);
  logrm.AddParameter(0.0);
  logrm.AddParameter(0.0);
  logrm.AddParameter(0.0);
  // }}}
  // FIT MODEL
  double alpha_inv=10.0;
  double lambda=1.0;
  logrm.FitParameters(t_input,alpha_inv,lambda,5000,5.0,false); // Fit
  //cout << "Fitted cost: " << logrm.Cost(t_input,lambda) << endl;
  // VALIDATE FIT
  //if (abs(logrm.GetParameter(0)-1.0)>0.1)
  //{ cerr << "TestLogRM1 failed - Fitted first parameter is too far from 1: " << logrm.GetParameter(0) << endl;
  //  return false;
  //}
  //if (abs(logrm.GetParameter(1)-1.0)>0.1)
  //{ cerr << "TestLogRM1 failed - Fitted second parameter is too far from 1: " << logrm.GetParameter(1) << endl;
  //  return false;
  //}
  //if (abs(logrm.GetParameter(2)-2.0)>0.1)
  //{ cerr << "TestLogRM1 failed - Fitted third parameter is too far from 2: " << logrm.GetParameter(2) << endl;
  //  return false;
  //}
  //double cost=logrm.Cost(t_samples,lambda);
  //if (cost>0.1
  //{ cerr << "TestLogRM1 failed - Cost after fit is too far from 0: " << cost << endl;
  //  return false;
  //}
  //// Test
  VectorData<double> t_test_samples(std::vector<double>(),3,0);
  std::vector<double> t_test_results;
  // DATA {{{
  t_test_samples.AddRow(std::vector<double>{1.0,61.379289447425,72.80788731317097});   t_test_results.push_back(1.0);
  t_test_samples.AddRow(std::vector<double>{1.0,85.40451939411645,57.05198397627122}); t_test_results.push_back(1.0);
  t_test_samples.AddRow(std::vector<double>{1.0,52.10797973193984,63.12762376881715}); t_test_results.push_back(0.0);
  t_test_samples.AddRow(std::vector<double>{1.0,52.04540476831827,79.43286012045222}); t_test_results.push_back(1.0);
  t_test_samples.AddRow(std::vector<double>{1.0,40.23689373545111,71.16774802184875}); t_test_results.push_back(0.0);
  t_test_samples.AddRow(std::vector<double>{1.0,54.63510555424817,52.21388588061123}); t_test_results.push_back(0.0);
  // }}}
  VectorData<double> t_test_inputs(std::vector<double>(),28,0);
  // DATA {{{
  for (size_t instance=0; instance<t_test_samples.Height(); ++instance)
  { double x1=(t_test_samples.GetValue(instance,1)-50.0)/50.0;
    double x2=(t_test_samples.GetValue(instance,2)-50.0)/50.0;
     t_test_inputs.AddRow(vector<double>
      { t_test_samples.GetValue(instance,0)   // 1.0
       ,x1                             // x1
       ,x2                             // x2
       ,pow(x1,2.0)                    // x1^2
       ,x1*x2                          // x1*x2
       ,pow(x2,2.0)                    // x2^2
       ,pow(x1,3.0)                    // x1^3
       ,pow(x1,2.0)*x2                 // x1^2*x2
       ,x1*pow(x2,2.0)                 // x1*x2^2
       ,pow(x2,3.0)                    // x2^3
       ,pow(x1,4.0)                    // x1^4
       ,pow(x1,3.0)*x2                 // x1^3*x2
       ,pow(x1,2.0)*pow(x2,2.0)        // x1^2*x2^2
       ,x1*pow(x2,3.0)                 // x1*x2^3
       ,pow(x2,4.0)                    // x2^4
       ,pow(x1,5.0)                    // x1^5
       ,pow(x1,4.0)*x2                 // x1^4*x2
       ,pow(x1,3.0)*pow(x2,2.0)        // x1^3*x2^2
       ,pow(x1,2.0)*pow(x2,3.0)        // x1^2*x2^3
       ,x1*pow(x2,4.0)                 // x1*x2^4
       ,pow(x2,5.0)                    // x2^5
       ,pow(x1,6.0)                    // x1^6
       ,pow(x1,5.0)*x2                 // x1^5*x2
       ,pow(x1,4.0)*pow(x2,2.0)        // x1^4*x2^2
       ,pow(x1,3.0)*pow(x2,3.0)        // x1^3*x2^3
       ,pow(x1,2.0)*pow(x2,4.0)        // x1^2*x2^4
       ,x1*pow(x2,5.0)                 // x1*x2^5
       ,pow(x2,6.0)                    // x2^6
      });
  }
  // }}}
  GuidedVectorData<double,double> t_test(t_test_inputs,t_test_results);
  for (size_t instance=0; instance<t_test.Height(); ++instance)
  { double eval=logrm.Eval(t_test,instance);
    if (abs(eval-t_test.GetResult(instance))>0.4)
    { cerr << "TestLinRM2 failed - Predicted " << eval << "!=" << t_test.GetResult(instance) << " on test " << instance << endl;
      return false;
    }
    else
    { //cout << "TesLogRM2 test " << instance << " succeeded: truth=" << t_test.GetResult(instance) << " guess=" << eval << endl;
    }
  }
  return true;
} // }}}
bool TestOVA1() // {{{
{ 
  srand(200); // Fix (random) map
  // INIT DATA
  VectorData<double> t_input_data(std::vector<double>(),3,0);
  std::vector<size_t> t_input_result;
  // DATA {{{
  t_input_data.AddRow(std::vector<double>{1.0,34.62365962451697,78.0246928153624});  t_input_result.push_back((3*7)/5);
  t_input_data.AddRow(std::vector<double>{1.0,30.28671076822607,43.89499752400101}); t_input_result.push_back((3*4)/5);
  t_input_data.AddRow(std::vector<double>{1.0,35.84740876993872,72.90219802708364}); t_input_result.push_back((3*7)/5);
  t_input_data.AddRow(std::vector<double>{1.0,60.18259938620976,86.30855209546826}); t_input_result.push_back((6*8)/5);
  t_input_data.AddRow(std::vector<double>{1.0,79.0327360507101,75.3443764369103});   t_input_result.push_back((7*7)/5);
  t_input_data.AddRow(std::vector<double>{1.0,45.08327747668339,56.3163717815305});  t_input_result.push_back((4*5)/5);
  t_input_data.AddRow(std::vector<double>{1.0,61.10666453684766,96.51142588489624}); t_input_result.push_back((6*9)/5);
  t_input_data.AddRow(std::vector<double>{1.0,75.02474556738889,46.55401354116538}); t_input_result.push_back((7*4)/5);
  t_input_data.AddRow(std::vector<double>{1.0,76.09878670226257,87.42056971926803}); t_input_result.push_back((7*8)/5);
  t_input_data.AddRow(std::vector<double>{1.0,84.43281996120035,43.53339331072109}); t_input_result.push_back((8*4)/5);
  t_input_data.AddRow(std::vector<double>{1.0,95.86155507093572,38.22527805795094}); t_input_result.push_back((9*3)/5);
  t_input_data.AddRow(std::vector<double>{1.0,75.01365838958247,30.60326323428011}); t_input_result.push_back((7*3)/5);
  t_input_data.AddRow(std::vector<double>{1.0,82.30705337399482,76.48196330235604}); t_input_result.push_back((8*7)/5);
  t_input_data.AddRow(std::vector<double>{1.0,69.36458875970939,97.71869196188608}); t_input_result.push_back((6*9)/5);
  t_input_data.AddRow(std::vector<double>{1.0,39.53833914367223,76.03681085115882}); t_input_result.push_back((3*7)/5);
  t_input_data.AddRow(std::vector<double>{1.0,53.9710521485623,89.20735013750205});  t_input_result.push_back((5*8)/5);
  t_input_data.AddRow(std::vector<double>{1.0,69.07014406283025,52.74046973016765}); t_input_result.push_back((6*5)/5);
  t_input_data.AddRow(std::vector<double>{1.0,67.94685547711617,46.67857410673128}); t_input_result.push_back((6*4)/5);
  t_input_data.AddRow(std::vector<double>{1.0,70.66150955499435,92.92713789364831}); t_input_result.push_back((7*9)/5);
  t_input_data.AddRow(std::vector<double>{1.0,76.97878372747498,47.57596364975532}); t_input_result.push_back((7*4)/5);
  t_input_data.AddRow(std::vector<double>{1.0,67.37202754570876,42.83843832029179}); t_input_result.push_back((6*4)/5);
  t_input_data.AddRow(std::vector<double>{1.0,89.67677575072079,65.79936592745237}); t_input_result.push_back((8*6)/5);
  t_input_data.AddRow(std::vector<double>{1.0,50.534788289883,48.85581152764205});   t_input_result.push_back((5*4)/5);
  t_input_data.AddRow(std::vector<double>{1.0,34.21206097786789,44.20952859866288}); t_input_result.push_back((3*4)/5);
  t_input_data.AddRow(std::vector<double>{1.0,77.9240914545704,68.9723599933059});   t_input_result.push_back((7*6)/5);
  t_input_data.AddRow(std::vector<double>{1.0,62.27101367004632,69.95445795447587}); t_input_result.push_back((6*6)/5);
  t_input_data.AddRow(std::vector<double>{1.0,80.1901807509566,44.82162893218353});  t_input_result.push_back((8*4)/5);
  t_input_data.AddRow(std::vector<double>{1.0,93.114388797442,38.80067033713209});   t_input_result.push_back((9*3)/5);
  t_input_data.AddRow(std::vector<double>{1.0,61.83020602312595,50.25610789244621}); t_input_result.push_back((6*5)/5);
  t_input_data.AddRow(std::vector<double>{1.0,38.78580379679423,64.99568095539578}); t_input_result.push_back((3*6)/5);
  t_input_data.AddRow(std::vector<double>{1.0,61.379289447425,72.80788731317097});   t_input_result.push_back((6*7)/5);
  t_input_data.AddRow(std::vector<double>{1.0,85.40451939411645,57.05198397627122}); t_input_result.push_back((8*5)/5);
  t_input_data.AddRow(std::vector<double>{1.0,52.10797973193984,63.12762376881715}); t_input_result.push_back((5*6)/5);
  t_input_data.AddRow(std::vector<double>{1.0,52.04540476831827,69.43286012045222}); t_input_result.push_back((5*6)/5);
  t_input_data.AddRow(std::vector<double>{1.0,40.23689373545111,71.16774802184875}); t_input_result.push_back((4*7)/5);
  t_input_data.AddRow(std::vector<double>{1.0,54.63510555424817,52.21388588061123}); t_input_result.push_back((5*5)/5);
  t_input_data.AddRow(std::vector<double>{1.0,33.91550010906887,98.86943574220611}); t_input_result.push_back((3*9)/5);
  t_input_data.AddRow(std::vector<double>{1.0,64.17698887494485,80.90806058670817}); t_input_result.push_back((6*8)/5);
  t_input_data.AddRow(std::vector<double>{1.0,74.78925295941542,41.57341522824434}); t_input_result.push_back((7*4)/5);
  t_input_data.AddRow(std::vector<double>{1.0,34.1836400264419,75.2377203360134});   t_input_result.push_back((3*7)/5);
  t_input_data.AddRow(std::vector<double>{1.0,83.90239366249155,56.30804621605327}); t_input_result.push_back((8*5)/5);
  t_input_data.AddRow(std::vector<double>{1.0,51.54772026906181,46.85629026349976}); t_input_result.push_back((5*4)/5);
  t_input_data.AddRow(std::vector<double>{1.0,94.44336776917852,65.56892160559052}); t_input_result.push_back((9*6)/5);
  t_input_data.AddRow(std::vector<double>{1.0,82.36875375713919,40.61825515970618}); t_input_result.push_back((8*4)/5);
  t_input_data.AddRow(std::vector<double>{1.0,51.04775177128865,45.82270145776001}); t_input_result.push_back((5*4)/5);
  t_input_data.AddRow(std::vector<double>{1.0,62.22267576120188,52.06099194836679}); t_input_result.push_back((6*5)/5);
  t_input_data.AddRow(std::vector<double>{1.0,77.19303492601364,70.45820000180959}); t_input_result.push_back((7*7)/5);
  t_input_data.AddRow(std::vector<double>{1.0,97.77159928000232,86.7278223300282});  t_input_result.push_back((9*8)/5);
  t_input_data.AddRow(std::vector<double>{1.0,62.07306379667647,96.76882412413983}); t_input_result.push_back((6*9)/5);
  t_input_data.AddRow(std::vector<double>{1.0,91.56497449807442,88.69629254546599}); t_input_result.push_back((9*8)/5);
  t_input_data.AddRow(std::vector<double>{1.0,79.94481794066932,74.16311935043758}); t_input_result.push_back((7*7)/5);
  t_input_data.AddRow(std::vector<double>{1.0,99.2725269292572,60.99903099844988});  t_input_result.push_back((9*6)/5);
  t_input_data.AddRow(std::vector<double>{1.0,90.54671411399852,43.39060180650027}); t_input_result.push_back((9*4)/5);
  t_input_data.AddRow(std::vector<double>{1.0,34.52451385320009,60.39634245837173}); t_input_result.push_back((3*6)/5);
  t_input_data.AddRow(std::vector<double>{1.0,50.2864961189907,49.80453881323059});  t_input_result.push_back((5*4)/5);
  t_input_data.AddRow(std::vector<double>{1.0,49.58667721632031,59.80895099453265}); t_input_result.push_back((4*5)/5);
  t_input_data.AddRow(std::vector<double>{1.0,97.64563396007767,68.86157272420604}); t_input_result.push_back((9*6)/5);
  t_input_data.AddRow(std::vector<double>{1.0,32.57720016809309,95.59854761387875}); t_input_result.push_back((3*9)/5);
  t_input_data.AddRow(std::vector<double>{1.0,74.24869136721598,69.82457122657193}); t_input_result.push_back((7*6)/5);
  t_input_data.AddRow(std::vector<double>{1.0,71.79646205863379,78.45356224515052}); t_input_result.push_back((7*7)/5);
  t_input_data.AddRow(std::vector<double>{1.0,75.3956114656803,85.75993667331619});  t_input_result.push_back((7*8)/5);
  t_input_data.AddRow(std::vector<double>{1.0,35.28611281526193,47.02051394723416}); t_input_result.push_back((3*4)/5);
  t_input_data.AddRow(std::vector<double>{1.0,56.25381749711624,39.26147251058019}); t_input_result.push_back((5*3)/5);
  t_input_data.AddRow(std::vector<double>{1.0,30.05882244669796,49.59297386723685}); t_input_result.push_back((3*4)/5);
  t_input_data.AddRow(std::vector<double>{1.0,44.66826172480893,66.45008614558913}); t_input_result.push_back((4*6)/5);
  t_input_data.AddRow(std::vector<double>{1.0,66.56089447242954,41.09209807936973}); t_input_result.push_back((6*4)/5);
  t_input_data.AddRow(std::vector<double>{1.0,40.45755098375164,97.53518548909936}); t_input_result.push_back((4*9)/5);
  t_input_data.AddRow(std::vector<double>{1.0,49.07256321908844,51.88321182073966}); t_input_result.push_back((4*5)/5);
  t_input_data.AddRow(std::vector<double>{1.0,80.27957401466998,92.11606081344084}); t_input_result.push_back((8*9)/5);
  t_input_data.AddRow(std::vector<double>{1.0,66.74671856944039,60.99139402740988}); t_input_result.push_back((6*6)/5);
  t_input_data.AddRow(std::vector<double>{1.0,32.72283304060323,43.30717306430063}); t_input_result.push_back((3*4)/5);
  t_input_data.AddRow(std::vector<double>{1.0,64.0393204150601,78.03168802018232});  t_input_result.push_back((6*7)/5);
  t_input_data.AddRow(std::vector<double>{1.0,72.34649422579923,96.22759296761404}); t_input_result.push_back((7*9)/5);
  t_input_data.AddRow(std::vector<double>{1.0,60.45788573918959,73.09499809758037}); t_input_result.push_back((6*7)/5);
  t_input_data.AddRow(std::vector<double>{1.0,58.84095621726802,75.85844831279042}); t_input_result.push_back((5*7)/5);
  t_input_data.AddRow(std::vector<double>{1.0,99.82785779692128,72.36925193383885}); t_input_result.push_back((9*7)/5);
  t_input_data.AddRow(std::vector<double>{1.0,47.26426910848174,88.47586499559782}); t_input_result.push_back((4*8)/5);
  t_input_data.AddRow(std::vector<double>{1.0,50.45815980285988,75.80985952982456}); t_input_result.push_back((5*7)/5);
  t_input_data.AddRow(std::vector<double>{1.0,60.45555629271532,42.50840943572217}); t_input_result.push_back((6*4)/5);
  t_input_data.AddRow(std::vector<double>{1.0,82.22666157785568,42.71987853716458}); t_input_result.push_back((8*4)/5);
  t_input_data.AddRow(std::vector<double>{1.0,88.9138964166533,69.80378889835472});  t_input_result.push_back((8*6)/5);
  t_input_data.AddRow(std::vector<double>{1.0,94.83450672430196,45.69430680250754}); t_input_result.push_back((9*4)/5);
  t_input_data.AddRow(std::vector<double>{1.0,67.31925746917527,66.58935317747915}); t_input_result.push_back((6*6)/5);
  t_input_data.AddRow(std::vector<double>{1.0,57.23870631569862,59.51428198012956}); t_input_result.push_back((5*5)/5);
  t_input_data.AddRow(std::vector<double>{1.0,80.36675600171273,90.96014789746954}); t_input_result.push_back((8*9)/5);
  t_input_data.AddRow(std::vector<double>{1.0,68.46852178591112,85.59430710452014}); t_input_result.push_back((6*8)/5);
  t_input_data.AddRow(std::vector<double>{1.0,42.0754545384731,78.84478600148043});  t_input_result.push_back((4*7)/5);
  t_input_data.AddRow(std::vector<double>{1.0,75.47770200533905,90.42453899753964}); t_input_result.push_back((7*9)/5);
  t_input_data.AddRow(std::vector<double>{1.0,78.63542434898018,96.64742716885644}); t_input_result.push_back((7*9)/5);
  t_input_data.AddRow(std::vector<double>{1.0,52.34800398794107,60.76950525602592}); t_input_result.push_back((5*6)/5);
  t_input_data.AddRow(std::vector<double>{1.0,94.09433112516793,77.15910509073893}); t_input_result.push_back((9*7)/5);
  t_input_data.AddRow(std::vector<double>{1.0,90.44855097096364,87.50879176484702}); t_input_result.push_back((9*8)/5);
  t_input_data.AddRow(std::vector<double>{1.0,55.48216114069585,35.57070347228866}); t_input_result.push_back((5*3)/5);
  t_input_data.AddRow(std::vector<double>{1.0,74.49269241843041,84.84513684930135}); t_input_result.push_back((7*8)/5);
  t_input_data.AddRow(std::vector<double>{1.0,89.84580670720979,45.35828361091658}); t_input_result.push_back((8*4)/5);
  t_input_data.AddRow(std::vector<double>{1.0,83.48916274498238,48.38028579728175}); t_input_result.push_back((8*4)/5);
  t_input_data.AddRow(std::vector<double>{1.0,42.2617008099817,87.10385094025457});  t_input_result.push_back((4*8)/5);
  t_input_data.AddRow(std::vector<double>{1.0,99.31500880510394,68.77540947206617}); t_input_result.push_back((9*6)/5);
  t_input_data.AddRow(std::vector<double>{1.0,55.34001756003703,64.9319380069486});  t_input_result.push_back((5*6)/5);
  t_input_data.AddRow(std::vector<double>{1.0,74.77589300092767,89.52981289513276}); t_input_result.push_back((7*8)/5);
  // }}}
  VectorData<double> t_samples_data(std::vector<double>(),28,0);
  // DATA {{{
  for (size_t instance=0; instance<t_input_data.Height(); ++instance)
  { double x1=(t_input_data.GetValue(instance,1)-50.0)/50.0;
    double x2=(t_input_data.GetValue(instance,2)-50.0)/50.0;
     t_samples_data.AddRow(vector<double>
      { t_input_data.GetValue(instance,0)   // 1.0
       ,x1                             // x1
       ,x2                             // x2
       ,pow(x1,2.0)                    // x1^2
       ,x1*x2                          // x1*x2
       ,pow(x2,2.0)                    // x2^2
       ,pow(x1,3.0)                    // x1^3
       ,pow(x1,2.0)*x2                 // x1^2*x2
       ,x1*pow(x2,2.0)                 // x1*x2^2
       ,pow(x2,3.0)                    // x2^3
       ,pow(x1,4.0)                    // x1^4
       ,pow(x1,3.0)*x2                 // x1^3*x2
       ,pow(x1,2.0)*pow(x2,2.0)        // x1^2*x2^2
       ,x1*pow(x2,3.0)                 // x1*x2^3
       ,pow(x2,4.0)                    // x2^4
       ,pow(x1,5.0)                    // x1^5
       ,pow(x1,4.0)*x2                 // x1^4*x2
       ,pow(x1,3.0)*pow(x2,2.0)        // x1^3*x2^2
       ,pow(x1,2.0)*pow(x2,3.0)        // x1^2*x2^3
       ,x1*pow(x2,4.0)                 // x1*x2^4
       ,pow(x2,5.0)                    // x2^5
       ,pow(x1,6.0)                    // x1^6
       ,pow(x1,5.0)*x2                 // x1^5*x2
       ,pow(x1,4.0)*pow(x2,2.0)        // x1^4*x2^2
       ,pow(x1,3.0)*pow(x2,3.0)        // x1^3*x2^3
       ,pow(x1,2.0)*pow(x2,4.0)        // x1^2*x2^4
       ,x1*pow(x2,5.0)                 // x1*x2^5
       ,pow(x2,6.0)                    // x2^6
      });
  }
  // }}}
  GuidedVectorData<double,size_t> t_samples(t_samples_data,t_input_result);
  // DEFINE MODEL
  vector<Label> mclabels;
  mclabels.push_back(Label("Zero","Desc",0,0.0));
  mclabels.push_back(Label("One","Desc",1,1.0));
  mclabels.push_back(Label("Two","Desc",2,2.0));
  mclabels.push_back(Label("Three","Desc",3,3.0));
  mclabels.push_back(Label("Four","Desc",4,4.0));
  mclabels.push_back(Label("Five","Desc",5,5.0));
  mclabels.push_back(Label("Six","Desc",6,6.0));
  mclabels.push_back(Label("Seven","Desc",7,7.0));
  mclabels.push_back(Label("Eight","Desc",8,8.0));
  mclabels.push_back(Label("Nine","Desc",9,9.0));
  OneVsAll mcmodel(mclabels);
  // DEFINE FEATURES {{{
  mcmodel.AddParameter(0.0);
  mcmodel.AddParameter(0.0);
  mcmodel.AddParameter(0.0);
  mcmodel.AddParameter(0.0);
  mcmodel.AddParameter(0.0);
  mcmodel.AddParameter(0.0);
  mcmodel.AddParameter(0.0);
  mcmodel.AddParameter(0.0);
  mcmodel.AddParameter(0.0);
  mcmodel.AddParameter(0.0);
  mcmodel.AddParameter(0.0);
  mcmodel.AddParameter(0.0);
  mcmodel.AddParameter(0.0);
  mcmodel.AddParameter(0.0);
  mcmodel.AddParameter(0.0);
  mcmodel.AddParameter(0.0);
  mcmodel.AddParameter(0.0);
  mcmodel.AddParameter(0.0);
  mcmodel.AddParameter(0.0);
  mcmodel.AddParameter(0.0);
  mcmodel.AddParameter(0.0);
  mcmodel.AddParameter(0.0);
  mcmodel.AddParameter(0.0);
  mcmodel.AddParameter(0.0);
  mcmodel.AddParameter(0.0);
  mcmodel.AddParameter(0.0);
  mcmodel.AddParameter(0.0);
  // }}}
  // FIT MODEL
  double alpha_inv=0.0;
  double lambda=0.05;
  mcmodel.FitParameters(t_samples,alpha_inv,lambda,1000,2.0d,false); // Fit
  cout << "Fitted cost: " << mcmodel.Cost(t_samples,lambda) << endl;
  // VALIDATE FIT
  //if (abs(mcmodel.GetParameter(0)-1.0)>0.1)
  //{ cerr << "TestLogRM1 failed - Fitted first parameter is too far from 1: " << mcmodel.GetParameter(0) << endl;
  //  return false;
  //}
  //if (abs(mcmodel.GetParameter(1)-1.0)>0.1)
  //{ cerr << "TestLogRM1 failed - Fitted second parameter is too far from 1: " << mcmodel.GetParameter(1) << endl;
  //  return false;
  //}
  //if (abs(mcmodel.GetParameter(2)-2.0)>0.1)
  //{ cerr << "TestLogRM1 failed - Fitted third parameter is too far from 2: " << mcmodel.GetParameter(2) << endl;
  //  return false;
  //}
  //double cost=mcmodel.Cost(t_samples,lambda);
  //if (cost>0.1
  //{ cerr << "TestLogRM1 failed - Cost after fit is too far from 0: " << cost << endl;
  //  return false;
  //}
  //// Test
  VectorData<double> t_test_data(vector<double>(),3,0);
  std::vector<size_t> t_test_result;
  // DATA {{{
  t_test_data.AddRow(std::vector<double>{1.0,61.379289447425,72.80788731317097});   t_test_result.push_back((6*7)/5);
  t_test_data.AddRow(std::vector<double>{1.0,85.40451939411645,57.05198397627122}); t_test_result.push_back((8*5)/5);
  t_test_data.AddRow(std::vector<double>{1.0,52.10797973193984,63.12762376881715}); t_test_result.push_back((5*6)/5);
  t_test_data.AddRow(std::vector<double>{1.0,52.04540476831827,79.43286012045222}); t_test_result.push_back((5*7)/5);
  t_test_data.AddRow(std::vector<double>{1.0,40.23689373545111,71.16774802184875}); t_test_result.push_back((4*7)/5);
  t_test_data.AddRow(std::vector<double>{1.0,54.63510555424817,52.21388588061123}); t_test_result.push_back((5*5)/5);
  // }}}
  VectorData<double> t_test_sample(vector<double>(),28,0);
  // DATA {{{
  for (size_t instance=0; instance<t_test_data.Height(); ++instance)
  { double x1=(t_test_data.GetValue(instance,1)-50.0)/50.0;
    double x2=(t_test_data.GetValue(instance,2)-50.0)/50.0;
     t_test_sample.AddRow(vector<double>
      { t_test_data.GetValue(instance,0)   // 1.0
       ,x1                             // x1
       ,x2                             // x2
       ,pow(x1,2.0)                    // x1^2
       ,x1*x2                          // x1*x2
       ,pow(x2,2.0)                    // x2^2
       ,pow(x1,3.0)                    // x1^3
       ,pow(x1,2.0)*x2                 // x1^2*x2
       ,x1*pow(x2,2.0)                 // x1*x2^2
       ,pow(x2,3.0)                    // x2^3
       ,pow(x1,4.0)                    // x1^4
       ,pow(x1,3.0)*x2                 // x1^3*x2
       ,pow(x1,2.0)*pow(x2,2.0)        // x1^2*x2^2
       ,x1*pow(x2,3.0)                 // x1*x2^3
       ,pow(x2,4.0)                    // x2^4
       ,pow(x1,5.0)                    // x1^5
       ,pow(x1,4.0)*x2                 // x1^4*x2
       ,pow(x1,3.0)*pow(x2,2.0)        // x1^3*x2^2
       ,pow(x1,2.0)*pow(x2,3.0)        // x1^2*x2^3
       ,x1*pow(x2,4.0)                 // x1*x2^4
       ,pow(x2,5.0)                    // x2^5
       ,pow(x1,6.0)                    // x1^6
       ,pow(x1,5.0)*x2                 // x1^5*x2
       ,pow(x1,4.0)*pow(x2,2.0)        // x1^4*x2^2
       ,pow(x1,3.0)*pow(x2,3.0)        // x1^3*x2^3
       ,pow(x1,2.0)*pow(x2,4.0)        // x1^2*x2^4
       ,x1*pow(x2,5.0)                 // x1*x2^5
       ,pow(x2,6.0)                    // x2^6
      });
  }
  // }}}
  GuidedVectorData<double,size_t> t_test(t_test_sample,t_test_result);
  bool result=true;
  for (size_t instance=0; instance<t_test.Height(); ++instance)
  {
    Label eval=mcmodel.Eval(t_test,instance);
    if (eval.Value()!=t_test.GetResult(instance))
    { cerr << "TestOVA1 failed - Predicted " << eval.Name() << "(" << eval.Value() << ")!=" << t_test.GetResult(instance) << " on test " << instance << endl;
      result=false;
    }
    else
    { //cout << "TestOVA1 test " << instance << " succeeded: truth=" << t_test.GetResult(instance) << " guess=" << eval.Name() << "(" << eval.Value() << ")" << endl;
    }
  }
  return result;
} // }}}
bool TestNetwork0() // {{{
{ // INIT DATA
  VectorData<double> t_samples_data(std::vector<double>(),3,0);
  VectorData<vector<double> > t_samples_result(std::vector<std::vector<double> >(),1,0);
  //// DATA {{{
  t_samples_data.AddRow(std::vector<double>{1.0,1.0,0.0});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  // }}}
  GuidedVectorData<double,std::vector<double> > t_samples(t_samples_data,t_samples_result.GetValues());

  // DEFINE MODEL
  Network model(3);
  vector<size_t> inputs({0,1,2}) ;
  model.AddNode(0,inputs);
  model.AddNode(0,inputs);
  model.AddNode(1,inputs);

  // Set parameters
  model.GetNode(0,0).second->SetParameter(0,1.0);
  model.GetNode(0,0).second->SetParameter(1,1.0);
  model.GetNode(0,0).second->SetParameter(2,-1.0);
  model.GetNode(0,1).second->SetParameter(0,1.0);
  model.GetNode(0,1).second->SetParameter(1,-1.0);
  model.GetNode(0,1).second->SetParameter(2,1.0);
  model.GetNode(1,0).second->SetParameter(0,-1.0);
  model.GetNode(1,0).second->SetParameter(1,1.0);
  model.GetNode(1,0).second->SetParameter(2,1.0);

  // Test Eval
  vector<double> res=model.Eval(t_samples,0);
  if (res.size()!=1)
  { cerr << "Error: size of eval is " << res.size() << "!=1" << endl;
    return false;
  }
  if (abs(res[0]-0.59)>0.02)
  { cerr << "Error: result of eval is " << res[0] << "0.59" << endl;
    return false;
  }
  
  // Test Delta
  //cout << "Eval before fit:" << endl << model.Eval(t_samples,0)[0] << endl;
  double alpha_inv=10.0;
  double lambda=0.0;
  for (size_t c=0; c<100; ++c)
  { model.FitParameters(t_samples,alpha_inv,lambda,300,5.0,false); // Fit
    //cout << "Cost: " << model.Cost(t_samples,lambda) << endl; // Debug cost
  }
  //cout << "Eval after fit:" << endl << model.Eval(t_samples,0)[0] << endl;
  //// Test
  for (size_t instance=0; instance<t_samples.Height(); ++instance)
  { double eval=model.Eval(t_samples,instance)[0];
    if (abs(eval-t_samples.GetResult(instance)[0])>0.3)
    { cerr << "TestNetwork0 failed - Predicted " << eval << "!=" << t_samples.GetResult(instance)[0] << " on test " << instance << endl;
      return false;
    }
  }
  return true;
} // }}}
bool TestNetwork1() // {{{
{ // INIT DATA
  VectorData<double> t_samples_data(std::vector<double>(),3,0);
  VectorData<vector<double> > t_samples_result(std::vector<std::vector<double> >(),1,0);
  //// DATA {{{
  t_samples_data.AddRow(std::vector<double>{1.0,0.3462365962451697,0.780246928153624});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.3028671076822607,0.4389499752400101});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.3584740876993872,0.7290219802708364});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.6018259938620976,0.8630855209546826});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.790327360507101,0.753443764369103});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.4508327747668339,0.563163717815305});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.6110666453684766,0.9651142588489624});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.7502474556738889,0.4655401354116538});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.7609878670226257,0.8742056971926803});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.8443281996120035,0.4353339331072109});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.9586155507093572,0.3822527805795094});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.7501365838958247,0.3060326323428011});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.8230705337399482,0.7648196330235604});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.6936458875970939,0.9771869196188608});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.3953833914367223,0.7603681085115882});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.539710521485623,0.8920735013750205});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.6907014406283025,0.5274046973016765});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.6794685547711617,0.4667857410673128});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.7066150955499435,0.9292713789364831});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.7697878372747498,0.4757596364975532});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.6737202754570876,0.4283843832029179});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.8967677575072079,0.6579936592745237});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.50534788289883,0.4885581152764205});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.3421206097786789,0.4420952859866288});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.779240914545704,0.689723599933059});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.6227101367004632,0.6995445795447587});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.801901807509566,0.4482162893218353});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.93114388797442,0.3880067033713209});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.6183020602312595,0.5025610789244621});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.3878580379679423,0.6499568095539578});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.61379289447425,0.7280788731317097});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.8540451939411645,0.5705198397627122});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.5210797973193984,0.6312762376881715});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.5204540476831827,0.6943286012045222});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.4023689373545111,0.7116774802184875});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.5463510555424817,0.5221388588061123});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.3391550010906887,0.9886943574220611});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.6417698887494485,0.8090806058670817});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.7478925295941542,0.4157341522824434});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.341836400264419,0.752377203360134});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.8390239366249155,0.5630804621605327});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.5154772026906181,0.4685629026349976});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.9444336776917852,0.6556892160559052});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.8236875375713919,0.4061825515970618});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.5104775177128865,0.4582270145776001});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.6222267576120188,0.5206099194836679});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.7719303492601364,0.7045820000180959});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.9777159928000232,0.867278223300282});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.6207306379667647,0.9676882412413983});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.9156497449807442,0.8869629254546599});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.7994481794066932,0.7416311935043758});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.992725269292572,0.6099903099844988});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.9054671411399852,0.4339060180650027});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.3452451385320009,0.6039634245837173});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.502864961189907,0.4980453881323059});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.4958667721632031,0.5980895099453265});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.9764563396007767,0.6886157272420604});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.3257720016809309,0.9559854761387875});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.7424869136721598,0.6982457122657193});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.7179646205863379,0.7845356224515052});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.753956114656803,0.8575993667331619});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.3528611281526193,0.4702051394723416});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.5625381749711624,0.3926147251058019});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.3005882244669796,0.4959297386723685});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.4466826172480893,0.6645008614558913});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.6656089447242954,0.4109209807936973});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.4045755098375164,0.9753518548909936});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.4907256321908844,0.5188321182073966});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.8027957401466998,0.9211606081344084});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.6674671856944039,0.6099139402740988});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.3272283304060323,0.4330717306430063});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.640393204150601,0.7803168802018232});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.7234649422579923,0.9622759296761404});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.6045788573918959,0.7309499809758037});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.5884095621726802,0.7585844831279042});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.9982785779692128,0.7236925193383885});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.4726426910848174,0.8847586499559782});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.5045815980285988,0.7580985952982456});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.6045555629271532,0.4250840943572217});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.8222666157785568,0.4271987853716458});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.889138964166533,0.6980378889835472});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.9483450672430196,0.4569430680250754});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.6731925746917527,0.6658935317747915});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.5723870631569862,0.5951428198012956});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.8036675600171273,0.9096014789746954});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.6846852178591112,0.8559430710452014});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.420754545384731,0.7884478600148043});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.7547770200533905,0.9042453899753964});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.7863542434898018,0.9664742716885644});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.5234800398794107,0.6076950525602592});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.9409433112516793,0.7715910509073893});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.9044855097096364,0.8750879176484702});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.5548216114069585,0.3557070347228866});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.7449269241843041,0.8484513684930135});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.8984580670720979,0.4535828361091658});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.8348916274498238,0.4838028579728175});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.422617008099817,0.8710385094025457});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.9931500880510394,0.6877540947206617});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.5534001756003703,0.649319380069486});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_samples_data.AddRow(std::vector<double>{1.0,0.7477589300092767,0.8952981289513276});
  t_samples_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  // }}}
  GuidedVectorData<double,std::vector<double> > t_samples(t_samples_data,t_samples_result.GetValues());

  VectorData<double> t_test_data(std::vector<double>(),3,0);
  VectorData<vector<double> > t_test_result(std::vector<std::vector<double> >(),1,0);
  // DATA {{{
  t_test_data.AddRow(std::vector<double>{1.0,0.61379289447425,0.7280788731317097});
  t_test_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_test_data.AddRow(std::vector<double>{1.0,0.8540451939411645,0.5705198397627122});
  t_test_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_test_data.AddRow(std::vector<double>{1.0,0.5210797973193984,0.6312762376881715});
  t_test_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_test_data.AddRow(std::vector<double>{1.0,0.5204540476831827,0.7943286012045222});
  t_test_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({1.0})}));
  t_test_data.AddRow(std::vector<double>{1.0,0.4023689373545111,0.7116774802184875});
  t_test_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  t_test_data.AddRow(std::vector<double>{1.0,0.5463510555424817,0.5221388588061123});
  t_test_result.AddRow(std::vector<std::vector<double> >({std::vector<double>({0.0})}));
  // }}}
  GuidedVectorData<double,vector<double> > t_test(t_test_data,t_test_result.GetValues());
  // DEFINE MODEL
  Network model(3);
  vector<size_t> inputs({0,1,2}) ;
  model.AddNode(0,inputs);
  model.AddNode(0,inputs);
  model.AddNode(0,inputs);
  model.AddNode(0,inputs);
  model.AddNode(0,inputs);
  model.AddNode(0,inputs);
  vector<size_t> inputs2({0,1,2,3,4,5,6}) ;
  model.AddNode(1,inputs2);
  model.AddNode(1,inputs2);
  model.AddNode(1,inputs2);
  model.AddNode(1,inputs2);
  model.AddNode(1,inputs2);
  model.AddNode(1,inputs2);
  model.AddNode(2,inputs2);
  double alpha_inv=0.0;
  double lambda=0.2;
  for (size_t c=0; c<100; ++c)
  { model.FitParameters(t_samples,alpha_inv,lambda,100,5.0,false); // Fit
    //cout << "Cost: " << model.Cost(t_samples,lambda) << endl; // Cost
  }
  //// Test
  for (size_t instance=0; instance<t_test.Height(); ++instance)
  { double eval=model.Eval(t_test,instance)[0];
    if (abs(eval-t_test.GetResult(instance)[0])>0.3)
    { cerr << "TestLinRM1 failed - Predicted " << eval << "!=" << t_test.GetResult(instance)[0] << " on test " << instance << endl;
      return false;
    }
  }
  return true;
} // }}}

class RLNimGame : public RLGame // {{{
{ public: 
    RLNimGame(int n, int turn) // {{{
    { myN=n;
      myTurn=turn%2;
      if (myN==0)
        myWinner=1-turn;
      else
        myWinner=-1;
    } // }}}
    virtual ~RLNimGame() {}
  
    RLGame *Copy() const { return new RLNimGame(myN, myTurn); }
    void Init() { myN=2+rand()%20; myTurn=rand()%2; myWinner=-1; }
    vector<double> State() // 5 features representing myN in binary {{{
    { vector<double> result;
      result.push_back(1.0d);
      result.push_back(double(myN%2));
      result.push_back(double((myN/2)%2));
      result.push_back(double((myN/4)%2));
      result.push_back(double((myN/8)%2));
      result.push_back(double((myN/16)%2));
      return result;
    } // }}}
    vector<vector<double> > Choices() // {{{
    { vector<vector<double> > choices;
      if (myN>=1)
      { vector<double> res;
        res.push_back(0.0d);
        res.push_back(0.0d);
        choices.push_back(res);
      }
      if (myN>=2)
      { vector<double> res;
        res.push_back(1.0d);
        res.push_back(0.0d);
        choices.push_back(res);
      }
      if (myN>=3)
      { vector<double> res;
        res.push_back(0.0d);
        res.push_back(1.0d);
        choices.push_back(res);
      }
      return choices;
    } // }}}
    void Step(const std::vector<double> &inputs) // {{{
    { size_t pins=1;
      if (inputs[0]>=0.5d)
        pins=2;
      if (inputs[1]>=0.5d)
        pins=3;
  
      if (myN<=pins)
      { myN=0;
        myWinner=1-myTurn;
      }
      else
      { myN-=pins;
        myTurn=1-myTurn;
      }
    } // }}}
    size_t Players() const { return 2; }
    size_t Turn() const { return myTurn; }
    bool Done() const { return myN<=0; }
    vector<double> Score() const // {{{
    { vector<double> score;
      if (!Done())
      { score.push_back(0.0d);
        score.push_back(0.0d);
      }
      else
      { score.push_back(myWinner==0?1.0d:0.0d);
        score.push_back(myWinner==1?1.0d:0.0d);
      }
      return score;
    } // }}}
  
    string GameString() const // {{{
    { stringstream result;
      for (size_t i=0; i<myN; ++i)
        result << " I";
      //result << "DrNim state: " << myN << ", " << myTurn << ", " << myWinner << endl;
      return result.str();
    } // }}}
  
  private:
    int myN;
    size_t myTurn;
    int myWinner;
}; // }}}
bool TestModel_RL1(Network *model, double maxcost) // {{{
{ // DISABELING MANUAL TEST // Play game
  // DISABELING MANUAL TEST RLNimGame playnim(21,0);
  // DISABELING MANUAL TEST 
  // DISABELING MANUAL TEST while (!playnim.Done())
  // DISABELING MANUAL TEST { cout << "DrNim State: " << playnim.GameString() << endl;
  // DISABELING MANUAL TEST   if (playnim.Turn()==0)
  // DISABELING MANUAL TEST   { cout << "Choose 1 to 3: " << flush;
  // DISABELING MANUAL TEST     int c;
  // DISABELING MANUAL TEST     cin >> c;
  // DISABELING MANUAL TEST     vector<double> cinput;
  // DISABELING MANUAL TEST     cinput.push_back(c==2?1.0d:0.0d);
  // DISABELING MANUAL TEST     cinput.push_back(c==3?1.0d:0.0d);
  // DISABELING MANUAL TEST     playnim.Step(cinput);
  // DISABELING MANUAL TEST   }
  // DISABELING MANUAL TEST   else
  // DISABELING MANUAL TEST     playnim.Step(model->Eval(playnim.State()));
  // DISABELING MANUAL TEST }
  // DISABELING MANUAL TEST cout << "Score: " << playnim.GameString() << endl;
  // DISABELING MANUAL TEST cout << "Score 0: " << playnim.Score()[0] << endl;
  // DISABELING MANUAL TEST cout << "Score 1: " << playnim.Score()[1] << endl;
  // DISABELING MANUAL TEST if (playnim.Score()[0]>=playnim.Score()[1])
  // DISABELING MANUAL TEST   return false;

  RLNimGame drnim(3,0);
  vector<double> choices=model->Eval(drnim.State());
  if (choices[0]<=0.5d)
  { cerr << "Choice 0 wrong" << endl;
    return false;
  }
  if (choices[1]>=0.5d)
  { cerr << "Choice 1 wrong" << endl;
    return false;
  }
  return true;
} // }}}
bool TestRL1() // {{{
{ // Create player networks
  vector<Network*> models;
  models.push_back(Network::Parse("6->[[2*[all]]]"));
  models.push_back(Network::Parse("6->[[21*[all]],[2*[all]]]"));

  // Initialize
  RLNimGame drnim(21,1);

  // Train players
  for (size_t rep=0; rep<20; ++rep)
  { if (!(rep%10))
      cout << "." << flush;
    drnim.TrainRLGame(models,200,100,0.0,0.01);
  }

  // Test models
  bool result=true;
  result=result && TestModel_RL1(models[0],0.5d);
  result=result && TestModel_RL1(models[1],0.5d);
  return result;
} // }}}

class Wall // {{{
{ public:
   Wall(size_t x, size_t y, char dir)
   : myX(x)
   , myY(y)
   , myDir(dir)
   {}

   bool operator<(const Wall &rhs) const // {{{
   { return (myX<rhs.myX || (myX==rhs.myX && myY<rhs.myY));
   } // }}}

   size_t myX, myY; 
   char myDir;
}; // }}}
class MazeMap // {{{
{ public:
    MazeMap(size_t width, size_t height) // {{{
    : myWidth(width)
    , myHeight(height)
    { // Map cell bits info: 0: wall right, 1: wall down, 8: visited.
      myMap=new char[myWidth*myHeight];
      myDistMap=new size_t[myWidth*myHeight];

      // Prims maze-generating algorithm
      // Init all walls
      for (size_t x=0; x<myWidth; ++x)
      { for (size_t y=0; y<myHeight; ++y)
        { myMap[x+y*myWidth]=0x03; // Set walls
        }
      }
      // Set starting tile as visited
      size_t startX=rand()%myWidth;
      size_t startY=rand()%myHeight;
      myMap[startX+startY*myWidth]=myMap[startX+startY*myWidth]|0x80; // Set visited
      // Add posible walls
      set<Wall> myWalls;
      if (startX>0)
        myWalls.insert(Wall(startX-1,startY,'R'));
      if (startX+1<myWidth)
        myWalls.insert(Wall(startX,startY,'R'));
      if (startY>0)
        myWalls.insert(Wall(startX,startY-1,'D'));
      if (startY+1<myHeight)
        myWalls.insert(Wall(startX,startY,'D'));
      // Remove random walls to unvisited tiles until connected map
      while (!myWalls.empty())
      { set<Wall>::iterator wall=myWalls.begin();
        std::advance(wall,rand()%myWalls.size());
        size_t wallX2, wallY2;
        if (wall->myDir=='R')
        { wallX2=wall->myX+1;
          wallY2=wall->myY;
        }
        else
        { wallX2=wall->myX;
          wallY2=wall->myY+1;
        }
        if (!(myMap[wall->myX+wall->myY*myWidth]&0x80) ||
            !(myMap[wallX2+wallY2*myWidth]&0x80))
        { // Remove wall
          if (wall->myDir=='R')
            myMap[wall->myX+wall->myY*myWidth]=myMap[wall->myX+wall->myY*myWidth]&~2;
          else
            myMap[wall->myX+wall->myY*myWidth]=myMap[wall->myX+wall->myY*myWidth]&~1;
          myMap[wall->myX+wall->myY*myWidth]=myMap[wall->myX+wall->myY*myWidth]|0x80;
          if (wall->myX>0)
            myWalls.insert(Wall(wall->myX-1,wall->myY,'R'));
          if (wall->myX+1<myWidth)
            myWalls.insert(Wall(wall->myX,wall->myY,'R'));
          if (wall->myY>0)
            myWalls.insert(Wall(wall->myX,wall->myY-1,'D'));
          if (wall->myY+1<myHeight)
            myWalls.insert(Wall(wall->myX,wall->myY,'D'));
          myMap[wallX2+wallY2*myWidth]=myMap[wallX2+wallY2*myWidth]|0x80;
          if (wallX2>0)
            myWalls.insert(Wall(wallX2-1,wallY2,'R'));
          if (wallX2+1<myWidth)
            myWalls.insert(Wall(wallX2,wallY2,'R'));
          if (wallY2>0)
            myWalls.insert(Wall(wallX2,wallY2-1,'D'));
          if (wallY2+1<myHeight)
            myWalls.insert(Wall(wallX2,wallY2,'D'));
        }
        myWalls.erase(wall);
      }
      // Calculate distance to goal for all tiles
      InitDistMap();


    } // }}}
    virtual ~MazeMap() // {{{
    { delete [] myMap;
      delete [] myDistMap;
    } // }}}

    size_t GetWidth() const { return myWidth; }
    size_t GetHeight() const { return myHeight; }
    bool WallRight(size_t x, size_t y) const { return myMap[x+y*myWidth]&0x2; }
    bool WallDown(size_t x, size_t y) const { return myMap[x+y*myWidth]&0x1; }
    size_t Dist(size_t x, size_t y) const { return myDistMap[x+y*myWidth]; }
    bool IsGoal(size_t x, size_t y) const { return Dist(x,y)==0; }

  private:
    void InitDistMap() // {{{
    { // Calculate Dists to goal using Dijkstras Shortest Path Algorithm
      for (size_t i=0; i<myWidth*myHeight; ++i)
        myDistMap[i]=myWidth+myHeight+1; // Set too high

      size_t goalx=rand()%myWidth;
      size_t goaly=rand()%myHeight;
      vector<pair<size_t,size_t>> worklist;
      worklist.push_back(pair<size_t,size_t>(goalx,goaly));
      myDistMap[goalx+goaly*myWidth]=0;

      while (!worklist.empty())
      { pair<size_t,size_t> pos=worklist[0];
        worklist.erase(worklist.begin());
        // Can we go left?
        if (pos.first>0 && !(myMap[pos.first-1+pos.second*myWidth]&2) &&
            myDistMap[pos.first-1+pos.second*myWidth]>1+myDistMap[pos.first+pos.second*myWidth])
        { myDistMap[pos.first-1+pos.second*myWidth]=1+myDistMap[pos.first+pos.second*myWidth];
          worklist.push_back(pair<size_t,size_t>(pos.first-1,pos.second));
        }
        // Can we go right?
        if (pos.first+1<myWidth && !(myMap[pos.first+pos.second*myWidth]&2) &&
            myDistMap[pos.first+1+pos.second*myWidth]>1+myDistMap[pos.first+pos.second*myWidth])
        { myDistMap[pos.first+1+pos.second*myWidth]=1+myDistMap[pos.first+pos.second*myWidth];
          worklist.push_back(pair<size_t,size_t>(pos.first+1,pos.second));
        }
        // Can we go up?
        if (pos.second>0 && !(myMap[pos.first+(pos.second-1)*myWidth]&1) &&
            myDistMap[pos.first+(pos.second-1)*myWidth]>1+myDistMap[pos.first+pos.second*myWidth])
        { myDistMap[pos.first+(pos.second-1)*myWidth]=1+myDistMap[pos.first+pos.second*myWidth];
          worklist.push_back(pair<size_t,size_t>(pos.first,pos.second-1));
        }
        // Can we go down?
        if (pos.second+1<myHeight && !(myMap[pos.first+pos.second*myWidth]&1) &&
            myDistMap[pos.first+(pos.second+1)*myWidth]>1+myDistMap[pos.first+pos.second*myWidth])
        { myDistMap[pos.first+(pos.second+1)*myWidth]=1+myDistMap[pos.first+pos.second*myWidth];
          worklist.push_back(pair<size_t,size_t>(pos.first,pos.second+1));
        }
      }
    } // }}}
    size_t myWidth;
    size_t myHeight;
    char *myMap;
    size_t *myDistMap;
}; // }}}
class RLMazeGame : public RLGame // {{{
{ public: 
    RLMazeGame(const MazeMap *maze) // {{{
    : myMaze(maze)
    , myX(rand()%maze->GetWidth())
    , myY(rand()%maze->GetHeight())
    , myDir(rand()%4)
    , myTime(100)
    //, myScore(0)
    { Init();
    } // }}}
    RLMazeGame(const RLMazeGame &rhs) // {{{
    : myMaze(rhs.myMaze)
    , myX(rhs.myX)
    , myY(rhs.myY)
    , myDir(rhs.myDir)
    , myTime(rhs.myTime)
    //, myScore(rhs.myScore)
    { myHistory=rhs.myHistory;
    } // }}}
    virtual ~RLMazeGame() { }
  
    RLGame *Copy() const { return new RLMazeGame(*this); }
    void Init() // {{{
    { myTime=100;
      myX=rand()%myMaze->GetWidth();
      myY=rand()%myMaze->GetHeight();
      myDir=rand()%4;

      // Set initial score
      //myScore=myMaze->GetWidth()*myMaze->GetHeight()-myMaze->Dist(myX,myY);
      // Reset History
      myHistory.clear();
      while (myHistory.size()<100)
        myHistory.push_back(0.0);
    } // }}}
    void SetStart(size_t x, size_t y, size_t t=100) // {{{
    { myX=x;
      myY=y;
      myTime=t;
    } // }}}
    vector<double> State() // {{{
    { vector<double> result;
      result.push_back(1.0d);
      // Make State
      result.push_back((double)(myDir&0x01?1.0d:0.0d));
      result.push_back((double)(myDir&0x02?1.0d:0.0d));
      for (int dy=-3; dy<=3; ++dy)
      { for (int dx=-3; dx<3; ++dx)
        { if (dx+myX<0 || dy+myY<0 || dx+myX>=myMaze->GetWidth() || dy+myY>=myMaze->GetHeight()) // Out of map
          { result.push_back(1.0d);
            result.push_back(1.0d);
            result.push_back(0.0d);
          }
          else
          { result.push_back((double)(myMaze->WallDown(dx+myX,dy+myY)?1.0d:0.0d));
            result.push_back((double)(myMaze->WallRight(dx+myX,dy+myY)?1.0d:0.0d));
            result.push_back((double)(myMaze->IsGoal(dx+myX,dy+myY)?1.0d:0.0d));
          }
        }
      }
      // Add 100 features of history
      while (myHistory.size()>100)
        myHistory.pop_back();
      while(myHistory.size()<100)
        myHistory.push_back(0.0);
      result.insert(result.end(),myHistory.begin(),myHistory.end());
      //Done
      return result;
    } // }}}
    vector<vector<double> > Choices() // {{{
    { vector<vector<double> > choices;
      // Add Choices
      // Check if move left is possible
      if (myX>0 && !(myMaze->WallRight(myX-1,myY)))
      { vector<double> move;
        move.push_back(1.0d);
        move.push_back(0.0d);
        move.push_back(0.0d);
        move.push_back(0.0d);
        choices.push_back(move);
      }
      // Check if move up is possible
      if (myY>0 && !(myMaze->WallDown(myX,myY-1)))
      { vector<double> move;
        move.push_back(0.0d);
        move.push_back(1.0d);
        move.push_back(0.0d);
        move.push_back(0.0d);
        choices.push_back(move);
      }
      // Check if move right is possible
      if (myX+1<myMaze->GetWidth() && !(myMaze->WallRight(myX,myY)))
      { vector<double> move;
        move.push_back(0.0d);
        move.push_back(0.0d);
        move.push_back(1.0d);
        move.push_back(0.0d);
        choices.push_back(move);
      }
      // Check if move down is possible
      if (myY+1<myMaze->GetHeight() && !(myMaze->WallDown(myX,myY)))
      { vector<double> move;
        move.push_back(0.0d);
        move.push_back(0.0d);
        move.push_back(0.0d);
        move.push_back(1.0d);
        choices.push_back(move);
      }
      return choices;
    } // }}}
    void Step(const std::vector<double> &inputs) // {{{
    { if (myTime==0)
        return;
      --myTime;
      //cout << "Stepping from " << myX << "," << myY << "->";
      // Perform Step
      int move=-1;
      double move_confidence=-1.0;
      // Check if move left is possible and more confident
      if (inputs[0]>=move_confidence && myX>0 && !(myMaze->WallRight(myX-1,myY)))
      { move=0;
        move_confidence=inputs[0];
      }
      // Check if move up is possible and more confident
      if (inputs[1]>=move_confidence && myY>0 && !(myMaze->WallDown(myX,myY-1)))
      { move=1;
        move_confidence=inputs[1];
      }
      // Check if move right is possible and more confident
      if (inputs[2]>=move_confidence && myX+1<myMaze->GetWidth() && !(myMaze->WallRight(myX,myY)))
      { move=2;
        move_confidence=inputs[2];
      }
      // Check if move down is possible and more confident
      if (inputs[3]>=move_confidence && myY+1<myMaze->GetHeight() && !(myMaze->WallDown(myX,myY)))
      { move=3;
        move_confidence=inputs[3];
      }

      // Perform move
      if (move==0)
      { --myX;
        myDir=0;
      }
      else if (move==1)
      { --myY;
        myDir=1;
      }
      else if (move==2)
      { ++myX;
        myDir=2;
      }
      else if (move==3)
      { ++myY;
        myDir=3;
      }
      else
      { stringstream ss;
        ss << GameString() << endl
           << "RLMazeGame::Step: Unable to move at " << myX << "," << myY;
        cerr << "Error: " << ss.str() << " - " << inputs[0] << ", " << inputs[1] << ", " << inputs[2] << ", " << inputs[3] << endl;
        throw ss.str();
      }

      // Update score
      double curscore=myMaze->GetWidth()*myMaze->GetHeight()-myMaze->Dist(myX,myY);
      //if (curscore>myScore)
      //  myScore=curscore;

      // Add choices to history
      myHistory.insert(myHistory.begin(),inputs.begin(),inputs.end());
      while(myHistory.size()>100)
        myHistory.pop_back();
      //cout << myX << "," << myY << "." << endl;
    } // }}}
    size_t Players() const { return 1; }
    size_t Turn() const { return 0; }
    bool Done() const { return myTime==0 || myMaze->IsGoal(myX,myY); }
    vector<double> Score() const // {{{
    { vector<double> result;
      result.push_back(myMaze->IsGoal(myX,myY)?1.0d+myTime:0.0d);
      return result;
    } // }}}
  
    string GameString() const // {{{
    { stringstream result;
      result << " ";
      for (size_t x=0;x<myMaze->GetWidth(); ++x)
        result << "___";
      result << endl;
      for (size_t y=0;y<myMaze->GetHeight(); ++y)
      { result << "|";
        for (size_t x=0;x<myMaze->GetWidth(); ++x)
        { if (x==myX && y==myY)
            result << "*";
          else if (myMaze->IsGoal(x,y))
            result << "G";
          else if (myMaze->WallDown(x,y))
            result << "_";
          else
            result << " ";

          if (myMaze->WallRight(x,y) && myMaze->WallDown(x,y))
            result << "_|";
          else if (myMaze->WallRight(x,y))
            result << " |";
          else if (myMaze->WallDown(x,y))
            result << "__";
          else
            result << "  ";
        }
        result << endl;
      }
      //result << "DrNim state: " << myN << ", " << myTurn << ", " << myWinner << endl;
      return result.str();
    } // }}}
    size_t Dist() { return myMaze->Dist(myX,myY); }
  
  private:
    const MazeMap *myMaze;
    vector<double> myHistory;
    size_t myX;
    size_t myY;
    char myDir;
    size_t myTime;
}; // }}}
bool TestRL2() // {{{
{ // Create game
  srand(200); // Fix (random) map
  MazeMap mazemap(10,10);
  RLMazeGame maze(&mazemap);
  maze.Init();

  // Create player networks
  vector<Network*> models;
  models.push_back(Network::Parse("229->[[4*[all]]]"));

  // Train model
  for (size_t rep=0; rep<20; ++rep)
  { cout << "." << flush;
    maze.TrainRLGame(models,100,100,0.0,0.01);
  }

  // Test model from top-left position
  maze.Init();
  maze.SetStart(0,0);

  while (!maze.Done())
  { cout << "Step: " << endl << maze.GameString() << endl;
    maze.Step(models[maze.Turn()]->Eval(maze.State()));
  }
  bool result=maze.Dist()==0;

  // Clean up
  while (!models.empty())
  { delete models.back();
    models.pop_back();
  }
  return result;
} // }}}

//#include "rlsnake.hpp"
//bool TestRL3() // {{{
//{ // Create game
//  RLSnake game(19,19);
//  //cout << "Input width: " << game.State().size() << endl;
//  // Create player networks
//  vector<Network*> models;
//  stringstream ss;
//  ss << 5+(2*fow+1)*(2*fow+1)-1 << "->[[4*[all]]]";
//  models.push_back(Network::Parse(ss.str()));
//
//  // Train model
//  double score=0.0d;
//  for (size_t rep=0; rep<10 && score<10.0d; ++rep)
//  { game.TrainRLGame(models,500,500,0.0,0.00001,500,true,0.0,true);
//    score=0.0d;
//    for (size_t i=0; i<1000; ++i)
//    {
//      auto gc=game.Copy();
//      score+=gc->Eval(models,500,0.1)[0];
//      delete gc;
//    }
//    score=score/1000.0d;
//    cout << "Iteration " << rep << ": Average points after 500 steps: " << score << endl;
//
//    // Save Model
//    ofstream fout("snake.mod");
//    models[0]->SaveParameters(fout);
//    fout.close();
//  }
//
//  // Test model
//  srand(200); // Fix (random) map
//  size_t step=0;
//  for (size_t step=0; !game.Done() && step<500; ++step)
//  { //cout << "Step: " << step << " score: " << game.Score()[0] << endl
//    //     << game.GameString() << endl;
//    game.RLGame::Step(models,0.1);
//    //this_thread::sleep_for(chrono::milliseconds(50));
//  }
//  bool result=game.Score()[0]>=10.0;
//
//  // Clean up
//  while (!models.empty())
//  { delete models.back();
//    models.pop_back();
//  }
//  return result;
//} // }}}

int main() // {{{
{ try
  { 
    cout << "TestLinRM1 - Simpel Linear Regression Model training " << flush
         << "skipped" << endl; //<< (TestLinRM1()?string("succeeded"):string("failed")) << endl;
    cout << "TestLinRM2 - A bit larger Linear Regression Model training " << flush
         << "skipped" << endl; //<< (TestLinRM2()?string("succeeded"):string("failed")) << endl;
    cout << "TestLogRM1 - Simpel Logistic Regression Model training " << flush
         << "skipped" << endl; //<< (TestLogRM1()?string("succeeded"):string("failed")) << endl;
    cout << "TestLogRM2 - A bit larger Logistic Regression Model training " << flush
         << "skipped" << endl; //(TestLogRM2()?string("succeeded"):string("failed")) << endl;
    cout << "TestOVA1 - Testing One Versus All Model training " << flush
         << "skipped" << endl; //<< (TestOVA1()?string("succeeded"):string("failed")) << endl;
    cout << "TestNetwork0 - Simpel Neural Network Model (2 hidden nodes) training " << flush
         << "skipped" << endl; //<< (TestNetwork0()?string("succeeded"):string("failed")) << endl;
    cout << "TestNetwork1 - A bit larger Neural Network Model (6+6 hidden nodes) training " << flush
         << "skipped" << endl; //<< (TestNetwork1()?string("succeeded"):string("failed")) << endl;
    cout << "TestRL1 - Reinforcement Learning of DrNim " << flush
         << "skipped" << endl; //<< (TestRL1()?string("succeeded"):string("failed")) << endl;
    cout << "TestRL2 - Reinforcement Learning of Maze solving " << flush
         << "skipped" << endl; //<< (TestRL2()?string("succeeded"):string("failed")) << endl;
    //cout << "TestRL3 - Reinforcement Learning of Snake game "
    //     << flush <<(TestRL3()?string("succeeded"):string("failed")) << endl;
  }
  catch (const std::string &error)
  { cerr << "Error: " << error << endl;
  }
} // }}}
