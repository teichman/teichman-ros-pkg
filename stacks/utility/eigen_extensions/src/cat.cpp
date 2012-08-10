#include <eigen_extensions/eigen_extensions.h>

using namespace std;
using namespace Eigen;

template<class T>
void catMat(const string& filename) {
  Matrix<T, Eigen::Dynamic, Eigen::Dynamic> mat;
  eigen_extensions::load(filename, &mat);
  if(mat.cols() == 1)
    cout << mat.transpose() << endl;
  else
    cout << mat.transpose() << endl;
}
  
void die() {
  cout << "Usage: cat TYPE FILE" << endl;
  cout << "  where TYPE is one of --int, --float, or --double and " << endl;
  cout << "  FILE is a .eig file saved with eigen_extensions." << endl;
  exit(0);
}

int main(int argc, char** argv) {
  if(argc != 3)
    die();

  string type(argv[1]);
  string filename(argv[2]);
  if(type.compare("--int") == 0)
    catMat<int>(filename);
  else if(type.compare("--float") == 0)
    catMat<float>(filename);
  else if(type.compare("--double") == 0)
    catMat<double>(filename);
  else
    die();

  return 0;
}
