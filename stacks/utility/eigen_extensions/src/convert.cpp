#include <eigen_extensions/eigen_extensions.h>

using namespace std;
using namespace Eigen;

void deserializeVector(std::istream& is, Eigen::VectorXd* target) {
  int rows;
  string str;
  is >> rows;
  getline(is, str);
  double* buf = (double*)malloc(sizeof(double)*rows);
  is.read((char*)buf, sizeof(double)*rows);
  VectorXd tmp = Eigen::Map<VectorXd>(buf, rows);
  *target = tmp;
  free(buf);
  getline(is, str);
}

void deserializeVector(std::string filename, Eigen::VectorXd* target) {
  ifstream file(filename.c_str());
  if(!file.is_open()) {
    cerr << "Unable to open " << filename << endl;
    file.close();
    return;
  }

  deserializeVector(file, target);
  file.close();
}


int main(int argc, char** argv)
{
  if(argc != 3) {
    cout << "Usage: convert OLD NEW" << endl;
    cout << "  where OLD is an old format serialized Eigen::VectorXd and NEW is the eigen_extensions version." << endl;
    return 1;
  }

  VectorXd vec;
  deserializeVector(argv[1], &vec);
  eigen_extensions::save(vec, argv[2]);

  cout << vec.transpose() << endl;
  cout << "Saved new vector into " << argv[2] << endl;
  
  return 0;
}
