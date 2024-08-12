#include <slowml/linearregressionmodel.hpp>
#include <slowml/logisticregressionmodel.hpp>
#include <slowml/guidedvectordata.hpp>
#include <slowml/network.hpp>
#include <slowml/onevsall.hpp>
#include <slowml/reinforcementlearning.hpp>
#include <cmath>
#include <iostream>
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
  linrm.FitParameters(t_samples,alpha_inv,lambda,1500,false); // Fit
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
  linrm.FitParameters(t_samples,alpha_inv,lambda,1500,false); // Fit
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
  logrm.FitParameters(t_samples,alpha_inv,lambda,15000,false); // Fit
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
/*bool TestLogRM2() // {{{
{ // INIT DATA
  GuidedVectorData<double,double> t_input(3,0,std::vector<double>(),std::vector<double>());
  // DATA {{{
  t_input.AddRow(std::vector<double>{1.0,34.62365962451697,78.0246928153624},0.0);
  t_input.AddRow(std::vector<double>{1.0,30.28671076822607,43.89499752400101},0.0);
  t_input.AddRow(std::vector<double>{1.0,35.84740876993872,72.90219802708364},0.0);
  t_input.AddRow(std::vector<double>{1.0,60.18259938620976,86.30855209546826},1.0);
  t_input.AddRow(std::vector<double>{1.0,79.0327360507101,75.3443764369103},1.0);
  t_input.AddRow(std::vector<double>{1.0,45.08327747668339,56.3163717815305},0.0);
  t_input.AddRow(std::vector<double>{1.0,61.10666453684766,96.51142588489624},1.0);
  t_input.AddRow(std::vector<double>{1.0,75.02474556738889,46.55401354116538},1.0);
  t_input.AddRow(std::vector<double>{1.0,76.09878670226257,87.42056971926803},1.0);
  t_input.AddRow(std::vector<double>{1.0,84.43281996120035,43.53339331072109},1.0);
  t_input.AddRow(std::vector<double>{1.0,95.86155507093572,38.22527805795094},0.0);
  t_input.AddRow(std::vector<double>{1.0,75.01365838958247,30.60326323428011},0.0);
  t_input.AddRow(std::vector<double>{1.0,82.30705337399482,76.48196330235604},1.0);
  t_input.AddRow(std::vector<double>{1.0,69.36458875970939,97.71869196188608},1.0);
  t_input.AddRow(std::vector<double>{1.0,39.53833914367223,76.03681085115882},0.0);
  t_input.AddRow(std::vector<double>{1.0,53.9710521485623,89.20735013750205},1.0);
  t_input.AddRow(std::vector<double>{1.0,69.07014406283025,52.74046973016765},1.0);
  t_input.AddRow(std::vector<double>{1.0,67.94685547711617,46.67857410673128},0.0);
  t_input.AddRow(std::vector<double>{1.0,70.66150955499435,92.92713789364831},1.0);
  t_input.AddRow(std::vector<double>{1.0,76.97878372747498,47.57596364975532},1.0);
  t_input.AddRow(std::vector<double>{1.0,67.37202754570876,42.83843832029179},0.0);
  t_input.AddRow(std::vector<double>{1.0,89.67677575072079,65.79936592745237},1.0);
  t_input.AddRow(std::vector<double>{1.0,50.534788289883,48.85581152764205},0.0);
  t_input.AddRow(std::vector<double>{1.0,34.21206097786789,44.20952859866288},0.0);
  t_input.AddRow(std::vector<double>{1.0,77.9240914545704,68.9723599933059},1.0);
  t_input.AddRow(std::vector<double>{1.0,62.27101367004632,69.95445795447587},1.0);
  t_input.AddRow(std::vector<double>{1.0,80.1901807509566,44.82162893218353},1.0);
  t_input.AddRow(std::vector<double>{1.0,93.114388797442,38.80067033713209},0.0);
  t_input.AddRow(std::vector<double>{1.0,61.83020602312595,50.25610789244621},0.0);
  t_input.AddRow(std::vector<double>{1.0,38.78580379679423,64.99568095539578},0.0);
  t_input.AddRow(std::vector<double>{1.0,61.379289447425,72.80788731317097},1.0);
  t_input.AddRow(std::vector<double>{1.0,85.40451939411645,57.05198397627122},1.0);
  t_input.AddRow(std::vector<double>{1.0,52.10797973193984,63.12762376881715},0.0);
  t_input.AddRow(std::vector<double>{1.0,52.04540476831827,69.43286012045222},1.0);
  t_input.AddRow(std::vector<double>{1.0,40.23689373545111,71.16774802184875},0.0);
  t_input.AddRow(std::vector<double>{1.0,54.63510555424817,52.21388588061123},0.0);
  t_input.AddRow(std::vector<double>{1.0,33.91550010906887,98.86943574220611},0.0);
  t_input.AddRow(std::vector<double>{1.0,64.17698887494485,80.90806058670817},1.0);
  t_input.AddRow(std::vector<double>{1.0,74.78925295941542,41.57341522824434},0.0);
  t_input.AddRow(std::vector<double>{1.0,34.1836400264419,75.2377203360134},0.0);
  t_input.AddRow(std::vector<double>{1.0,83.90239366249155,56.30804621605327},1.0);
  t_input.AddRow(std::vector<double>{1.0,51.54772026906181,46.85629026349976},0.0);
  t_input.AddRow(std::vector<double>{1.0,94.44336776917852,65.56892160559052},1.0);
  t_input.AddRow(std::vector<double>{1.0,82.36875375713919,40.61825515970618},0.0);
  t_input.AddRow(std::vector<double>{1.0,51.04775177128865,45.82270145776001},0.0);
  t_input.AddRow(std::vector<double>{1.0,62.22267576120188,52.06099194836679},0.0);
  t_input.AddRow(std::vector<double>{1.0,77.19303492601364,70.45820000180959},1.0);
  t_input.AddRow(std::vector<double>{1.0,97.77159928000232,86.7278223300282},1.0);
  t_input.AddRow(std::vector<double>{1.0,62.07306379667647,96.76882412413983},1.0);
  t_input.AddRow(std::vector<double>{1.0,91.56497449807442,88.69629254546599},1.0);
  t_input.AddRow(std::vector<double>{1.0,79.94481794066932,74.16311935043758},1.0);
  t_input.AddRow(std::vector<double>{1.0,99.2725269292572,60.99903099844988},1.0);
  t_input.AddRow(std::vector<double>{1.0,90.54671411399852,43.39060180650027},1.0);
  t_input.AddRow(std::vector<double>{1.0,34.52451385320009,60.39634245837173},0.0);
  t_input.AddRow(std::vector<double>{1.0,50.2864961189907,49.80453881323059},0.0);
  t_input.AddRow(std::vector<double>{1.0,49.58667721632031,59.80895099453265},0.0);
  t_input.AddRow(std::vector<double>{1.0,97.64563396007767,68.86157272420604},1.0);
  t_input.AddRow(std::vector<double>{1.0,32.57720016809309,95.59854761387875},0.0);
  t_input.AddRow(std::vector<double>{1.0,74.24869136721598,69.82457122657193},1.0);
  t_input.AddRow(std::vector<double>{1.0,71.79646205863379,78.45356224515052},1.0);
  t_input.AddRow(std::vector<double>{1.0,75.3956114656803,85.75993667331619},1.0);
  t_input.AddRow(std::vector<double>{1.0,35.28611281526193,47.02051394723416},0.0);
  t_input.AddRow(std::vector<double>{1.0,56.25381749711624,39.26147251058019},0.0);
  t_input.AddRow(std::vector<double>{1.0,30.05882244669796,49.59297386723685},0.0);
  t_input.AddRow(std::vector<double>{1.0,44.66826172480893,66.45008614558913},0.0);
  t_input.AddRow(std::vector<double>{1.0,66.56089447242954,41.09209807936973},0.0);
  t_input.AddRow(std::vector<double>{1.0,40.45755098375164,97.53518548909936},1.0);
  t_input.AddRow(std::vector<double>{1.0,49.07256321908844,51.88321182073966},0.0);
  t_input.AddRow(std::vector<double>{1.0,80.27957401466998,92.11606081344084},1.0);
  t_input.AddRow(std::vector<double>{1.0,66.74671856944039,60.99139402740988},1.0);
  t_input.AddRow(std::vector<double>{1.0,32.72283304060323,43.30717306430063},0.0);
  t_input.AddRow(std::vector<double>{1.0,64.0393204150601,78.03168802018232},1.0);
  t_input.AddRow(std::vector<double>{1.0,72.34649422579923,96.22759296761404},1.0);
  t_input.AddRow(std::vector<double>{1.0,60.45788573918959,73.09499809758037},1.0);
  t_input.AddRow(std::vector<double>{1.0,58.84095621726802,75.85844831279042},1.0);
  t_input.AddRow(std::vector<double>{1.0,99.82785779692128,72.36925193383885},1.0);
  t_input.AddRow(std::vector<double>{1.0,47.26426910848174,88.47586499559782},1.0);
  t_input.AddRow(std::vector<double>{1.0,50.45815980285988,75.80985952982456},1.0);
  t_input.AddRow(std::vector<double>{1.0,60.45555629271532,42.50840943572217},0.0);
  t_input.AddRow(std::vector<double>{1.0,82.22666157785568,42.71987853716458},0.0);
  t_input.AddRow(std::vector<double>{1.0,88.9138964166533,69.80378889835472},1.0);
  t_input.AddRow(std::vector<double>{1.0,94.83450672430196,45.69430680250754},1.0);
  t_input.AddRow(std::vector<double>{1.0,67.31925746917527,66.58935317747915},1.0);
  t_input.AddRow(std::vector<double>{1.0,57.23870631569862,59.51428198012956},1.0);
  t_input.AddRow(std::vector<double>{1.0,80.36675600171273,90.96014789746954},1.0);
  t_input.AddRow(std::vector<double>{1.0,68.46852178591112,85.59430710452014},1.0);
  t_input.AddRow(std::vector<double>{1.0,42.0754545384731,78.84478600148043},0.0);
  t_input.AddRow(std::vector<double>{1.0,75.47770200533905,90.42453899753964},1.0);
  t_input.AddRow(std::vector<double>{1.0,78.63542434898018,96.64742716885644},1.0);
  t_input.AddRow(std::vector<double>{1.0,52.34800398794107,60.76950525602592},0.0);
  t_input.AddRow(std::vector<double>{1.0,94.09433112516793,77.15910509073893},1.0);
  t_input.AddRow(std::vector<double>{1.0,90.44855097096364,87.50879176484702},1.0);
  t_input.AddRow(std::vector<double>{1.0,55.48216114069585,35.57070347228866},0.0);
  t_input.AddRow(std::vector<double>{1.0,74.49269241843041,84.84513684930135},1.0);
  t_input.AddRow(std::vector<double>{1.0,89.84580670720979,45.35828361091658},1.0);
  t_input.AddRow(std::vector<double>{1.0,83.48916274498238,48.38028579728175},1.0);
  t_input.AddRow(std::vector<double>{1.0,42.2617008099817,87.10385094025457},1.0);
  t_input.AddRow(std::vector<double>{1.0,99.31500880510394,68.77540947206617},1.0);
  t_input.AddRow(std::vector<double>{1.0,55.34001756003703,64.9319380069486},1.0);
  t_input.AddRow(std::vector<double>{1.0,74.77589300092767,89.52981289513276},1.0);
  // }}}
  GuidedVectorData<double,double> t_samples(28,0,std::vector<double>(),std::vector<double>());
  // DATA {{{
  for (size_t instance=0; instance<t_input.Height(); ++instance)
  { double x1=(t_input.GetValue(instance,1)-50.0)/50.0;
    double x2=(t_input.GetValue(instance,2)-50.0)/50.0;
     t_samples.AddRow(vector<double>
      { t_input.GetValue(instance,0)   // 1.0
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
      }, t_input.GetResult(instance));
  }
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
  logrm.FitParameters(t_samples,alpha_inv,lambda,5000,false); // Fit
  cout << "Fitted cost: " << logrm.Cost(t_samples,lambda) << endl;
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
  GuidedVectorData<double,double> t_test_points(3,0,std::vector<double>(),std::vector<double>());
  // DATA {{{
  t_test_points.AddRow(std::vector<double>{1.0,61.379289447425,72.80788731317097},1.0);
  t_test_points.AddRow(std::vector<double>{1.0,85.40451939411645,57.05198397627122},1.0);
  t_test_points.AddRow(std::vector<double>{1.0,52.10797973193984,63.12762376881715},0.0);
  t_test_points.AddRow(std::vector<double>{1.0,52.04540476831827,79.43286012045222},1.0);
  t_test_points.AddRow(std::vector<double>{1.0,40.23689373545111,71.16774802184875},0.0);
  t_test_points.AddRow(std::vector<double>{1.0,54.63510555424817,52.21388588061123},0.0);
  // }}}
  GuidedVectorData<double,double> t_test(28,0,std::vector<double>(),std::vector<double>());
  // DATA {{{
  for (size_t instance=0; instance<t_test_points.Height(); ++instance)
  { double x1=(t_test_points.GetValue(instance,1)-50.0)/50.0;
    double x2=(t_test_points.GetValue(instance,2)-50.0)/50.0;
     t_test.AddRow(vector<double>
      { t_test_points.GetValue(instance,0)   // 1.0
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
      }, t_test_points.GetResult(instance));
  }
  // }}}
  for (size_t instance=0; instance<t_test.Height(); ++instance)
  { double eval=logrm.Eval(t_test,instance);
    if (abs(eval-t_test.GetResult(instance))>0.4)
    { cerr << "TestLinRM2 failed - Predicted " << eval << "!=" << t_test.GetResult(instance) << " on test " << instance << endl;
      return false;
    }
    else
    { cout << "TesLogRM2 test " << instance << " succeeded: truth=" << t_test.GetResult(instance) << " guess=" << eval << endl;
    }
  }
  return true;
} // }}}
bool TestOVA1() // {{{
{ // INIT DATA
  GuidedVectorData<double,size_t> t_input(3,0,std::vector<double>(),std::vector<size_t>());
  // DATA {{{
  t_input.AddRow(std::vector<double>{1.0,34.62365962451697,78.0246928153624} ,(3*7)/5);
  t_input.AddRow(std::vector<double>{1.0,30.28671076822607,43.89499752400101},(3*4)/5);
  t_input.AddRow(std::vector<double>{1.0,35.84740876993872,72.90219802708364},(3*7)/5);
  t_input.AddRow(std::vector<double>{1.0,60.18259938620976,86.30855209546826},(6*8)/5);
  t_input.AddRow(std::vector<double>{1.0,79.0327360507101,75.3443764369103}  ,(7*7)/5);
  t_input.AddRow(std::vector<double>{1.0,45.08327747668339,56.3163717815305} ,(4*5)/5);
  t_input.AddRow(std::vector<double>{1.0,61.10666453684766,96.51142588489624},(6*9)/5);
  t_input.AddRow(std::vector<double>{1.0,75.02474556738889,46.55401354116538},(7*4)/5);
  t_input.AddRow(std::vector<double>{1.0,76.09878670226257,87.42056971926803},(7*8)/5);
  t_input.AddRow(std::vector<double>{1.0,84.43281996120035,43.53339331072109},(8*4)/5);
  t_input.AddRow(std::vector<double>{1.0,95.86155507093572,38.22527805795094},(9*3)/5);
  t_input.AddRow(std::vector<double>{1.0,75.01365838958247,30.60326323428011},(7*3)/5);
  t_input.AddRow(std::vector<double>{1.0,82.30705337399482,76.48196330235604},(8*7)/5);
  t_input.AddRow(std::vector<double>{1.0,69.36458875970939,97.71869196188608},(6*9)/5);
  t_input.AddRow(std::vector<double>{1.0,39.53833914367223,76.03681085115882},(3*7)/5);
  t_input.AddRow(std::vector<double>{1.0,53.9710521485623,89.20735013750205} ,(5*8)/5);
  t_input.AddRow(std::vector<double>{1.0,69.07014406283025,52.74046973016765},(6*5)/5);
  t_input.AddRow(std::vector<double>{1.0,67.94685547711617,46.67857410673128},(6*4)/5);
  t_input.AddRow(std::vector<double>{1.0,70.66150955499435,92.92713789364831},(7*9)/5);
  t_input.AddRow(std::vector<double>{1.0,76.97878372747498,47.57596364975532},(7*4)/5);
  t_input.AddRow(std::vector<double>{1.0,67.37202754570876,42.83843832029179},(6*4)/5);
  t_input.AddRow(std::vector<double>{1.0,89.67677575072079,65.79936592745237},(8*6)/5);
  t_input.AddRow(std::vector<double>{1.0,50.534788289883,48.85581152764205}  ,(5*4)/5);
  t_input.AddRow(std::vector<double>{1.0,34.21206097786789,44.20952859866288},(3*4)/5);
  t_input.AddRow(std::vector<double>{1.0,77.9240914545704,68.9723599933059}  ,(7*6)/5);
  t_input.AddRow(std::vector<double>{1.0,62.27101367004632,69.95445795447587},(6*6)/5);
  t_input.AddRow(std::vector<double>{1.0,80.1901807509566,44.82162893218353} ,(8*4)/5);
  t_input.AddRow(std::vector<double>{1.0,93.114388797442,38.80067033713209}  ,(9*3)/5);
  t_input.AddRow(std::vector<double>{1.0,61.83020602312595,50.25610789244621},(6*5)/5);
  t_input.AddRow(std::vector<double>{1.0,38.78580379679423,64.99568095539578},(3*6)/5);
  t_input.AddRow(std::vector<double>{1.0,61.379289447425,72.80788731317097}  ,(6*7)/5);
  t_input.AddRow(std::vector<double>{1.0,85.40451939411645,57.05198397627122},(8*5)/5);
  t_input.AddRow(std::vector<double>{1.0,52.10797973193984,63.12762376881715},(5*6)/5);
  t_input.AddRow(std::vector<double>{1.0,52.04540476831827,69.43286012045222},(5*6)/5);
  t_input.AddRow(std::vector<double>{1.0,40.23689373545111,71.16774802184875},(4*7)/5);
  t_input.AddRow(std::vector<double>{1.0,54.63510555424817,52.21388588061123},(5*5)/5);
  t_input.AddRow(std::vector<double>{1.0,33.91550010906887,98.86943574220611},(3*9)/5);
  t_input.AddRow(std::vector<double>{1.0,64.17698887494485,80.90806058670817},(6*8)/5);
  t_input.AddRow(std::vector<double>{1.0,74.78925295941542,41.57341522824434},(7*4)/5);
  t_input.AddRow(std::vector<double>{1.0,34.1836400264419,75.2377203360134}  ,(3*7)/5);
  t_input.AddRow(std::vector<double>{1.0,83.90239366249155,56.30804621605327},(8*5)/5);
  t_input.AddRow(std::vector<double>{1.0,51.54772026906181,46.85629026349976},(5*4)/5);
  t_input.AddRow(std::vector<double>{1.0,94.44336776917852,65.56892160559052},(9*6)/5);
  t_input.AddRow(std::vector<double>{1.0,82.36875375713919,40.61825515970618},(8*4)/5);
  t_input.AddRow(std::vector<double>{1.0,51.04775177128865,45.82270145776001},(5*4)/5);
  t_input.AddRow(std::vector<double>{1.0,62.22267576120188,52.06099194836679},(6*5)/5);
  t_input.AddRow(std::vector<double>{1.0,77.19303492601364,70.45820000180959},(7*7)/5);
  t_input.AddRow(std::vector<double>{1.0,97.77159928000232,86.7278223300282} ,(9*8)/5);
  t_input.AddRow(std::vector<double>{1.0,62.07306379667647,96.76882412413983},(6*9)/5);
  t_input.AddRow(std::vector<double>{1.0,91.56497449807442,88.69629254546599},(9*8)/5);
  t_input.AddRow(std::vector<double>{1.0,79.94481794066932,74.16311935043758},(7*7)/5);
  t_input.AddRow(std::vector<double>{1.0,99.2725269292572,60.99903099844988} ,(9*6)/5);
  t_input.AddRow(std::vector<double>{1.0,90.54671411399852,43.39060180650027},(9*4)/5);
  t_input.AddRow(std::vector<double>{1.0,34.52451385320009,60.39634245837173},(3*6)/5);
  t_input.AddRow(std::vector<double>{1.0,50.2864961189907,49.80453881323059} ,(5*4)/5);
  t_input.AddRow(std::vector<double>{1.0,49.58667721632031,59.80895099453265},(4*5)/5);
  t_input.AddRow(std::vector<double>{1.0,97.64563396007767,68.86157272420604},(9*6)/5);
  t_input.AddRow(std::vector<double>{1.0,32.57720016809309,95.59854761387875},(3*9)/5);
  t_input.AddRow(std::vector<double>{1.0,74.24869136721598,69.82457122657193},(7*6)/5);
  t_input.AddRow(std::vector<double>{1.0,71.79646205863379,78.45356224515052},(7*7)/5);
  t_input.AddRow(std::vector<double>{1.0,75.3956114656803,85.75993667331619} ,(7*8)/5);
  t_input.AddRow(std::vector<double>{1.0,35.28611281526193,47.02051394723416},(3*4)/5);
  t_input.AddRow(std::vector<double>{1.0,56.25381749711624,39.26147251058019},(5*3)/5);
  t_input.AddRow(std::vector<double>{1.0,30.05882244669796,49.59297386723685},(3*4)/5);
  t_input.AddRow(std::vector<double>{1.0,44.66826172480893,66.45008614558913},(4*6)/5);
  t_input.AddRow(std::vector<double>{1.0,66.56089447242954,41.09209807936973},(6*4)/5);
  t_input.AddRow(std::vector<double>{1.0,40.45755098375164,97.53518548909936},(4*9)/5);
  t_input.AddRow(std::vector<double>{1.0,49.07256321908844,51.88321182073966},(4*5)/5);
  t_input.AddRow(std::vector<double>{1.0,80.27957401466998,92.11606081344084},(8*9)/5);
  t_input.AddRow(std::vector<double>{1.0,66.74671856944039,60.99139402740988},(6*6)/5);
  t_input.AddRow(std::vector<double>{1.0,32.72283304060323,43.30717306430063},(3*4)/5);
  t_input.AddRow(std::vector<double>{1.0,64.0393204150601,78.03168802018232} ,(6*7)/5);
  t_input.AddRow(std::vector<double>{1.0,72.34649422579923,96.22759296761404},(7*9)/5);
  t_input.AddRow(std::vector<double>{1.0,60.45788573918959,73.09499809758037},(6*7)/5);
  t_input.AddRow(std::vector<double>{1.0,58.84095621726802,75.85844831279042},(5*7)/5);
  t_input.AddRow(std::vector<double>{1.0,99.82785779692128,72.36925193383885},(9*7)/5);
  t_input.AddRow(std::vector<double>{1.0,47.26426910848174,88.47586499559782},(4*8)/5);
  t_input.AddRow(std::vector<double>{1.0,50.45815980285988,75.80985952982456},(5*7)/5);
  t_input.AddRow(std::vector<double>{1.0,60.45555629271532,42.50840943572217},(6*4)/5);
  t_input.AddRow(std::vector<double>{1.0,82.22666157785568,42.71987853716458},(8*4)/5);
  t_input.AddRow(std::vector<double>{1.0,88.9138964166533,69.80378889835472} ,(8*6)/5);
  t_input.AddRow(std::vector<double>{1.0,94.83450672430196,45.69430680250754},(9*4)/5);
  t_input.AddRow(std::vector<double>{1.0,67.31925746917527,66.58935317747915},(6*6)/5);
  t_input.AddRow(std::vector<double>{1.0,57.23870631569862,59.51428198012956},(5*5)/5);
  t_input.AddRow(std::vector<double>{1.0,80.36675600171273,90.96014789746954},(8*9)/5);
  t_input.AddRow(std::vector<double>{1.0,68.46852178591112,85.59430710452014},(6*8)/5);
  t_input.AddRow(std::vector<double>{1.0,42.0754545384731,78.84478600148043} ,(4*7)/5);
  t_input.AddRow(std::vector<double>{1.0,75.47770200533905,90.42453899753964},(7*9)/5);
  t_input.AddRow(std::vector<double>{1.0,78.63542434898018,96.64742716885644},(7*9)/5);
  t_input.AddRow(std::vector<double>{1.0,52.34800398794107,60.76950525602592},(5*6)/5);
  t_input.AddRow(std::vector<double>{1.0,94.09433112516793,77.15910509073893},(9*7)/5);
  t_input.AddRow(std::vector<double>{1.0,90.44855097096364,87.50879176484702},(9*8)/5);
  t_input.AddRow(std::vector<double>{1.0,55.48216114069585,35.57070347228866},(5*3)/5);
  t_input.AddRow(std::vector<double>{1.0,74.49269241843041,84.84513684930135},(7*8)/5);
  t_input.AddRow(std::vector<double>{1.0,89.84580670720979,45.35828361091658},(8*4)/5);
  t_input.AddRow(std::vector<double>{1.0,83.48916274498238,48.38028579728175},(8*4)/5);
  t_input.AddRow(std::vector<double>{1.0,42.2617008099817,87.10385094025457} ,(4*8)/5);
  t_input.AddRow(std::vector<double>{1.0,99.31500880510394,68.77540947206617},(9*6)/5);
  t_input.AddRow(std::vector<double>{1.0,55.34001756003703,64.9319380069486} ,(5*6)/5);
  t_input.AddRow(std::vector<double>{1.0,74.77589300092767,89.52981289513276},(7*8)/5);
  // }}}
  GuidedVectorData<double,size_t> t_samples(28,0,std::vector<double>(),std::vector<size_t>());
  // DATA {{{
  for (size_t instance=0; instance<t_input.Height(); ++instance)
  { double x1=(t_input.GetValue(instance,1)-50.0)/50.0;
    double x2=(t_input.GetValue(instance,2)-50.0)/50.0;
     t_samples.AddRow(vector<double>
      { t_input.GetValue(instance,0)   // 1.0
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
      }, t_input.GetResult(instance));
  }
  // }}}
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
  double alpha_inv=30.0;
  double lambda=0.0;
  mcmodel.FitParameters(t_samples,alpha_inv,lambda,200000,false); // Fit
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
  GuidedVectorData<double,size_t> t_test_points(3,0,std::vector<double>(),std::vector<size_t>());
  // DATA {{{
  t_test_points.AddRow(std::vector<double>{1.0,61.379289447425,72.80788731317097}  ,(6*7)/5);
  t_test_points.AddRow(std::vector<double>{1.0,85.40451939411645,57.05198397627122},(8*5)/5);
  t_test_points.AddRow(std::vector<double>{1.0,52.10797973193984,63.12762376881715},(5*6)/5);
  t_test_points.AddRow(std::vector<double>{1.0,52.04540476831827,79.43286012045222},(5*7)/5);
  t_test_points.AddRow(std::vector<double>{1.0,40.23689373545111,71.16774802184875},(4*7)/5);
  t_test_points.AddRow(std::vector<double>{1.0,54.63510555424817,52.21388588061123},(5*5)/5);
  // }}}
  GuidedVectorData<double,size_t> t_test(28,0,std::vector<double>(),std::vector<size_t>());
  // DATA {{{
  for (size_t instance=0; instance<t_test_points.Height(); ++instance)
  { double x1=(t_test_points.GetValue(instance,1)-50.0)/50.0;
    double x2=(t_test_points.GetValue(instance,2)-50.0)/50.0;
     t_test.AddRow(vector<double>
      { t_test_points.GetValue(instance,0)   // 1.0
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
      }, t_test_points.GetResult(instance));
  }
  // }}}
  bool result=true;
  for (size_t instance=0; instance<t_test.Height(); ++instance)
  {
    Label eval=mcmodel.Eval(t_test,instance);
    if (eval.Value()!=t_test.GetResult(instance))
    { cerr << "TestOVA1 failed - Predicted " << eval.Name() << "(" << eval.Value() << ")!=" << t_test.GetResult(instance) << " on test " << instance << endl;
      result=false;
    }
    else
    { cout << "TestOVA1 test " << instance << " succeeded: truth=" << t_test.GetResult(instance) << " guess=" << eval.Name() << "(" << eval.Value() << ")" << endl;
    }
  }
  return result;
} // }}}
*/
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
  { model.FitParameters(t_samples,alpha_inv,lambda,300,false); // Fit
    //cout << "Cost: " << model.Cost(t_samples,lambda) << endl; // Debug cost
  }
  //cout << "Eval after fit:" << endl << model.Eval(t_samples,0)[0] << endl;
  //// Test
  for (size_t instance=0; instance<t_samples.Height(); ++instance)
  { double eval=model.Eval(t_samples,instance)[0];
    if (abs(eval-t_samples.GetResult(instance)[0])>0.3)
    { cerr << "TestLinRM1 failed - Predicted " << eval << "!=" << t_samples.GetResult(instance)[0] << " on test " << instance << endl;
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
  { model.FitParameters(t_samples,alpha_inv,lambda,100,false); // Fit
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

class RLSimpleGame : public RLGame // {{{
{ RLSimpleGame(int n, int turn) // {{{
  { myN=n;
    myTurn=turn%2;
    if (myN==0)
      myWinner=1-turn;
    else
      myWinner=-1;
  } // }}}
  ~RLSimpleGame() {}

  RLSimpleGame *Copy() { return new RLRimpleGame(myN); }
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
  } // }}}

  string GameString() const // {{{
  { stringstream result;
    result << "DrNim state: " << myN << endl;
    return result.str();
  } // }}}

  private:
    int myN;
    size_t myTurn;
    int myWinner;
}; // }}}
bool TestRL1() // {{{
{ // Create game
  RLSimpleGame drnim(21,0);

  // Create player networks
  Network robert=Network::Parse("6->[[2*[all]]]");
  Network kai=Network::Parse("6->[[5*[all]],[2*[all]]]");

  // Train players

  // INIT DATA
  GuidedVectorData<double,size_t> t_input(3,0,std::vector<double>(),std::vector<size_t>());
  // DATA {{{
  t_input.AddRow(std::vector<double>{1.0,34.62365962451697,78.0246928153624} ,(3*7)/5);
  t_input.AddRow(std::vector<double>{1.0,30.28671076822607,43.89499752400101},(3*4)/5);
  t_input.AddRow(std::vector<double>{1.0,35.84740876993872,72.90219802708364},(3*7)/5);
  t_input.AddRow(std::vector<double>{1.0,60.18259938620976,86.30855209546826},(6*8)/5);
  t_input.AddRow(std::vector<double>{1.0,79.0327360507101,75.3443764369103}  ,(7*7)/5);
  t_input.AddRow(std::vector<double>{1.0,45.08327747668339,56.3163717815305} ,(4*5)/5);
  t_input.AddRow(std::vector<double>{1.0,61.10666453684766,96.51142588489624},(6*9)/5);
  t_input.AddRow(std::vector<double>{1.0,75.02474556738889,46.55401354116538},(7*4)/5);
  t_input.AddRow(std::vector<double>{1.0,76.09878670226257,87.42056971926803},(7*8)/5);
  t_input.AddRow(std::vector<double>{1.0,84.43281996120035,43.53339331072109},(8*4)/5);
  t_input.AddRow(std::vector<double>{1.0,95.86155507093572,38.22527805795094},(9*3)/5);
  t_input.AddRow(std::vector<double>{1.0,75.01365838958247,30.60326323428011},(7*3)/5);
  t_input.AddRow(std::vector<double>{1.0,82.30705337399482,76.48196330235604},(8*7)/5);
  t_input.AddRow(std::vector<double>{1.0,69.36458875970939,97.71869196188608},(6*9)/5);
  t_input.AddRow(std::vector<double>{1.0,39.53833914367223,76.03681085115882},(3*7)/5);
  t_input.AddRow(std::vector<double>{1.0,53.9710521485623,89.20735013750205} ,(5*8)/5);
  t_input.AddRow(std::vector<double>{1.0,69.07014406283025,52.74046973016765},(6*5)/5);
  t_input.AddRow(std::vector<double>{1.0,67.94685547711617,46.67857410673128},(6*4)/5);
  t_input.AddRow(std::vector<double>{1.0,70.66150955499435,92.92713789364831},(7*9)/5);
  t_input.AddRow(std::vector<double>{1.0,76.97878372747498,47.57596364975532},(7*4)/5);
  t_input.AddRow(std::vector<double>{1.0,67.37202754570876,42.83843832029179},(6*4)/5);
  t_input.AddRow(std::vector<double>{1.0,89.67677575072079,65.79936592745237},(8*6)/5);
  t_input.AddRow(std::vector<double>{1.0,50.534788289883,48.85581152764205}  ,(5*4)/5);
  t_input.AddRow(std::vector<double>{1.0,34.21206097786789,44.20952859866288},(3*4)/5);
  t_input.AddRow(std::vector<double>{1.0,77.9240914545704,68.9723599933059}  ,(7*6)/5);
  t_input.AddRow(std::vector<double>{1.0,62.27101367004632,69.95445795447587},(6*6)/5);
  t_input.AddRow(std::vector<double>{1.0,80.1901807509566,44.82162893218353} ,(8*4)/5);
  t_input.AddRow(std::vector<double>{1.0,93.114388797442,38.80067033713209}  ,(9*3)/5);
  t_input.AddRow(std::vector<double>{1.0,61.83020602312595,50.25610789244621},(6*5)/5);
  t_input.AddRow(std::vector<double>{1.0,38.78580379679423,64.99568095539578},(3*6)/5);
  t_input.AddRow(std::vector<double>{1.0,61.379289447425,72.80788731317097}  ,(6*7)/5);
  t_input.AddRow(std::vector<double>{1.0,85.40451939411645,57.05198397627122},(8*5)/5);
  t_input.AddRow(std::vector<double>{1.0,52.10797973193984,63.12762376881715},(5*6)/5);
  t_input.AddRow(std::vector<double>{1.0,52.04540476831827,69.43286012045222},(5*6)/5);
  t_input.AddRow(std::vector<double>{1.0,40.23689373545111,71.16774802184875},(4*7)/5);
  t_input.AddRow(std::vector<double>{1.0,54.63510555424817,52.21388588061123},(5*5)/5);
  t_input.AddRow(std::vector<double>{1.0,33.91550010906887,98.86943574220611},(3*9)/5);
  t_input.AddRow(std::vector<double>{1.0,64.17698887494485,80.90806058670817},(6*8)/5);
  t_input.AddRow(std::vector<double>{1.0,74.78925295941542,41.57341522824434},(7*4)/5);
  t_input.AddRow(std::vector<double>{1.0,34.1836400264419,75.2377203360134}  ,(3*7)/5);
  t_input.AddRow(std::vector<double>{1.0,83.90239366249155,56.30804621605327},(8*5)/5);
  t_input.AddRow(std::vector<double>{1.0,51.54772026906181,46.85629026349976},(5*4)/5);
  t_input.AddRow(std::vector<double>{1.0,94.44336776917852,65.56892160559052},(9*6)/5);
  t_input.AddRow(std::vector<double>{1.0,82.36875375713919,40.61825515970618},(8*4)/5);
  t_input.AddRow(std::vector<double>{1.0,51.04775177128865,45.82270145776001},(5*4)/5);
  t_input.AddRow(std::vector<double>{1.0,62.22267576120188,52.06099194836679},(6*5)/5);
  t_input.AddRow(std::vector<double>{1.0,77.19303492601364,70.45820000180959},(7*7)/5);
  t_input.AddRow(std::vector<double>{1.0,97.77159928000232,86.7278223300282} ,(9*8)/5);
  t_input.AddRow(std::vector<double>{1.0,62.07306379667647,96.76882412413983},(6*9)/5);
  t_input.AddRow(std::vector<double>{1.0,91.56497449807442,88.69629254546599},(9*8)/5);
  t_input.AddRow(std::vector<double>{1.0,79.94481794066932,74.16311935043758},(7*7)/5);
  t_input.AddRow(std::vector<double>{1.0,99.2725269292572,60.99903099844988} ,(9*6)/5);
  t_input.AddRow(std::vector<double>{1.0,90.54671411399852,43.39060180650027},(9*4)/5);
  t_input.AddRow(std::vector<double>{1.0,34.52451385320009,60.39634245837173},(3*6)/5);
  t_input.AddRow(std::vector<double>{1.0,50.2864961189907,49.80453881323059} ,(5*4)/5);
  t_input.AddRow(std::vector<double>{1.0,49.58667721632031,59.80895099453265},(4*5)/5);
  t_input.AddRow(std::vector<double>{1.0,97.64563396007767,68.86157272420604},(9*6)/5);
  t_input.AddRow(std::vector<double>{1.0,32.57720016809309,95.59854761387875},(3*9)/5);
  t_input.AddRow(std::vector<double>{1.0,74.24869136721598,69.82457122657193},(7*6)/5);
  t_input.AddRow(std::vector<double>{1.0,71.79646205863379,78.45356224515052},(7*7)/5);
  t_input.AddRow(std::vector<double>{1.0,75.3956114656803,85.75993667331619} ,(7*8)/5);
  t_input.AddRow(std::vector<double>{1.0,35.28611281526193,47.02051394723416},(3*4)/5);
  t_input.AddRow(std::vector<double>{1.0,56.25381749711624,39.26147251058019},(5*3)/5);
  t_input.AddRow(std::vector<double>{1.0,30.05882244669796,49.59297386723685},(3*4)/5);
  t_input.AddRow(std::vector<double>{1.0,44.66826172480893,66.45008614558913},(4*6)/5);
  t_input.AddRow(std::vector<double>{1.0,66.56089447242954,41.09209807936973},(6*4)/5);
  t_input.AddRow(std::vector<double>{1.0,40.45755098375164,97.53518548909936},(4*9)/5);
  t_input.AddRow(std::vector<double>{1.0,49.07256321908844,51.88321182073966},(4*5)/5);
  t_input.AddRow(std::vector<double>{1.0,80.27957401466998,92.11606081344084},(8*9)/5);
  t_input.AddRow(std::vector<double>{1.0,66.74671856944039,60.99139402740988},(6*6)/5);
  t_input.AddRow(std::vector<double>{1.0,32.72283304060323,43.30717306430063},(3*4)/5);
  t_input.AddRow(std::vector<double>{1.0,64.0393204150601,78.03168802018232} ,(6*7)/5);
  t_input.AddRow(std::vector<double>{1.0,72.34649422579923,96.22759296761404},(7*9)/5);
  t_input.AddRow(std::vector<double>{1.0,60.45788573918959,73.09499809758037},(6*7)/5);
  t_input.AddRow(std::vector<double>{1.0,58.84095621726802,75.85844831279042},(5*7)/5);
  t_input.AddRow(std::vector<double>{1.0,99.82785779692128,72.36925193383885},(9*7)/5);
  t_input.AddRow(std::vector<double>{1.0,47.26426910848174,88.47586499559782},(4*8)/5);
  t_input.AddRow(std::vector<double>{1.0,50.45815980285988,75.80985952982456},(5*7)/5);
  t_input.AddRow(std::vector<double>{1.0,60.45555629271532,42.50840943572217},(6*4)/5);
  t_input.AddRow(std::vector<double>{1.0,82.22666157785568,42.71987853716458},(8*4)/5);
  t_input.AddRow(std::vector<double>{1.0,88.9138964166533,69.80378889835472} ,(8*6)/5);
  t_input.AddRow(std::vector<double>{1.0,94.83450672430196,45.69430680250754},(9*4)/5);
  t_input.AddRow(std::vector<double>{1.0,67.31925746917527,66.58935317747915},(6*6)/5);
  t_input.AddRow(std::vector<double>{1.0,57.23870631569862,59.51428198012956},(5*5)/5);
  t_input.AddRow(std::vector<double>{1.0,80.36675600171273,90.96014789746954},(8*9)/5);
  t_input.AddRow(std::vector<double>{1.0,68.46852178591112,85.59430710452014},(6*8)/5);
  t_input.AddRow(std::vector<double>{1.0,42.0754545384731,78.84478600148043} ,(4*7)/5);
  t_input.AddRow(std::vector<double>{1.0,75.47770200533905,90.42453899753964},(7*9)/5);
  t_input.AddRow(std::vector<double>{1.0,78.63542434898018,96.64742716885644},(7*9)/5);
  t_input.AddRow(std::vector<double>{1.0,52.34800398794107,60.76950525602592},(5*6)/5);
  t_input.AddRow(std::vector<double>{1.0,94.09433112516793,77.15910509073893},(9*7)/5);
  t_input.AddRow(std::vector<double>{1.0,90.44855097096364,87.50879176484702},(9*8)/5);
  t_input.AddRow(std::vector<double>{1.0,55.48216114069585,35.57070347228866},(5*3)/5);
  t_input.AddRow(std::vector<double>{1.0,74.49269241843041,84.84513684930135},(7*8)/5);
  t_input.AddRow(std::vector<double>{1.0,89.84580670720979,45.35828361091658},(8*4)/5);
  t_input.AddRow(std::vector<double>{1.0,83.48916274498238,48.38028579728175},(8*4)/5);
  t_input.AddRow(std::vector<double>{1.0,42.2617008099817,87.10385094025457} ,(4*8)/5);
  t_input.AddRow(std::vector<double>{1.0,99.31500880510394,68.77540947206617},(9*6)/5);
  t_input.AddRow(std::vector<double>{1.0,55.34001756003703,64.9319380069486} ,(5*6)/5);
  t_input.AddRow(std::vector<double>{1.0,74.77589300092767,89.52981289513276},(7*8)/5);
  // }}}
  GuidedVectorData<double,size_t> t_samples(28,0,std::vector<double>(),std::vector<size_t>());
  // DATA {{{
  for (size_t instance=0; instance<t_input.Height(); ++instance)
  { double x1=(t_input.GetValue(instance,1)-50.0)/50.0;
    double x2=(t_input.GetValue(instance,2)-50.0)/50.0;
     t_samples.AddRow(vector<double>
      { t_input.GetValue(instance,0)   // 1.0
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
      }, t_input.GetResult(instance));
  }
  // }}}
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
  double alpha_inv=30.0;
  double lambda=0.0;
  mcmodel.FitParameters(t_samples,alpha_inv,lambda,200000,false); // Fit
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
  GuidedVectorData<double,size_t> t_test_points(3,0,std::vector<double>(),std::vector<size_t>());
  // DATA {{{
  t_test_points.AddRow(std::vector<double>{1.0,61.379289447425,72.80788731317097}  ,(6*7)/5);
  t_test_points.AddRow(std::vector<double>{1.0,85.40451939411645,57.05198397627122},(8*5)/5);
  t_test_points.AddRow(std::vector<double>{1.0,52.10797973193984,63.12762376881715},(5*6)/5);
  t_test_points.AddRow(std::vector<double>{1.0,52.04540476831827,79.43286012045222},(5*7)/5);
  t_test_points.AddRow(std::vector<double>{1.0,40.23689373545111,71.16774802184875},(4*7)/5);
  t_test_points.AddRow(std::vector<double>{1.0,54.63510555424817,52.21388588061123},(5*5)/5);
  // }}}
  GuidedVectorData<double,size_t> t_test(28,0,std::vector<double>(),std::vector<size_t>());
  // DATA {{{
  for (size_t instance=0; instance<t_test_points.Height(); ++instance)
  { double x1=(t_test_points.GetValue(instance,1)-50.0)/50.0;
    double x2=(t_test_points.GetValue(instance,2)-50.0)/50.0;
     t_test.AddRow(vector<double>
      { t_test_points.GetValue(instance,0)   // 1.0
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
      }, t_test_points.GetResult(instance));
  }
  // }}}
  bool result=true;
  for (size_t instance=0; instance<t_test.Height(); ++instance)
  {
    Label eval=mcmodel.Eval(t_test,instance);
    if (eval.Value()!=t_test.GetResult(instance))
    { cerr << "TestOVA1 failed - Predicted " << eval.Name() << "(" << eval.Value() << ")!=" << t_test.GetResult(instance) << " on test " << instance << endl;
      result=false;
    }
    else
    { cout << "TestOVA1 test " << instance << " succeeded: truth=" << t_test.GetResult(instance) << " guess=" << eval.Name() << "(" << eval.Value() << ")" << endl;
    }
  }
  return result;
} // }}}
int main()
{ try
  { 
    if (TestLinRM1())
      cout << "TestLinRM1 succeeded" << endl;
    else
      cout << "TestLinRM1 failed" << endl;
    if (TestLinRM2())
      cout << "TestLinRM2 succeeded" << endl;
    else
      cout << "TestLinRM2 failed" << endl;
    if (TestLogRM1())
      cout << "TestLogRM1 succeeded" << endl;
    else
      cout << "TestLogRM1 failed" << endl;
    if (TestNetwork0())
      cout << "TestNetwork0 succeeded" << endl;
    else
      cout << "TestNetwork0 failed" << endl;
    if (TestNetwork1())
      cout << "TestNetwork1 succeeded" << endl;
    else
      cout << "TestNetwork1 failed" << endl;
    //if (TestLogRM2())
    //  cout << "TestLogRM2 succeeded" << endl;
    //else
    //  cout << "TestLogRM2 failed" << endl;
    //if (TestOVA1())
    //  cout << "TestOVA1 succeeded" << endl;
    //else
    //  cout << "TestOVA1 failed" << endl;
  }
  catch (const std::string &error)
  { cerr << "Error: " << error << endl;
  }
}

