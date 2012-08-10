#include <matplotlib_interface/matplotlib_interface.h>

using namespace std;
using namespace Eigen;

void mpliExecuteFile(const string& filename) {
  FILE *fp = fopen(filename.c_str(), "r");
  PyRun_SimpleFile(fp, filename.c_str());
  fclose(fp);
}

void mpli(const string& str) {
  PyRun_SimpleString(str.c_str());
}

void mpliBegin() {
  Py_Initialize();
  PyRun_SimpleString("import numpy as np");
  PyRun_SimpleString("import matplotlib");
  PyRun_SimpleString("import matplotlib.pyplot as plt");
  PyRun_SimpleString("import warnings");
  PyRun_SimpleString("warnings.simplefilter('ignore')"); // waitforbuttonpress causes a warning.  Is there a better function to use?  Warnings should be on..
}

void mpliEnd() {
  Py_Finalize();
}

string mpliToPython(const string& str) {
  return "'" + str + "'";
}

string mpliToPython(int val) {
  ostringstream oss;
  oss << val;
  return oss.str();
}

string mpliToPython(size_t val) {
  ostringstream oss;
  oss << val;
  return oss.str();
}

string mpliToPython(double val) {
  ostringstream oss;
  oss << val;
  return oss.str();
}

string mpliToPython(const VectorXd& vec) {
  ostringstream oss;
  oss << "np.array( [";
  for(int i = 0; i < vec.rows(); ++i) {
    oss << vec(i);
    if(i != vec.rows() - 1)
      oss << ", ";
  }
  oss << "] )";
  return oss.str();
}

string mpliToPython(const std::vector<double>& vec)
{
  ostringstream oss;
  oss << "np.array( [";
  for(size_t i = 0; i < vec.size(); ++i) {
    oss << vec[i];
    if(i != vec.size() - 1)
      oss << ", ";
  }
  oss << "] )";
  return oss.str();
}

string mpliToPython(const MatrixXd& mat) {
  ostringstream oss;
  oss << "np.array([";
  for(int i = 0; i < mat.rows(); ++i) {
    oss << "[";
    for(int j = 0; j < mat.cols(); ++j) { 
      oss << mat(i, j);
      if(j != mat.cols() - 1)
	oss << ", ";
    }
    oss << "]";
    if(i != mat.rows() - 1)
      oss << ",";
  }
  oss << "] )";
  return oss.str();
}

std::string mpliToPython(const Eigen::ArrayXXd& arr)
{
  return mpliToPython((MatrixXd)arr.matrix());
}

std::string mpliToPython(const Eigen::ArrayXd& arr)
{
  return mpliToPython((VectorXd)arr.matrix());
}

void mpliPrintSize() {
  mpli("matplotlib.rcParams['xtick.labelsize'] = 14");
  mpli("matplotlib.rcParams['ytick.labelsize'] = 14");
  mpli("matplotlib.rcParams['lines.linewidth'] = 4");
  mpli("matplotlib.rcParams['axes.labelsize'] = 18");
  mpli("matplotlib.rcParams['axes.titlesize'] = 20");
  mpli("matplotlib.rcParams['axes.formatter.limits'] = (-4, 5)");
  
  // To prevent Type-3 fonts from being embedded:
  mpli("matplotlib.rcParams['text.usetex'] = True");
}
