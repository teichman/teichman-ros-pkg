#include <eigen_extensions/eigen_extensions.h>
#include <gtest/gtest.h>

using namespace std;
using namespace Eigen;

int size = 3;

TEST(EigenExtensions, MatrixXd_serialization) {
  MatrixXd mat = MatrixXd::Random(size, size);
  eigen_extensions::save(mat, "matxd.eig");
  MatrixXd mat2;
  eigen_extensions::load("matxd.eig", &mat2);
  EXPECT_TRUE(mat.isApprox(mat2));

  eigen_extensions::save(mat, "matxd.eig.gz");
  eigen_extensions::load("matxd.eig.gz", &mat2);
  EXPECT_TRUE(mat.isApprox(mat2));
}

TEST(EigenExtensions, MatrixXf_serialization) {
  MatrixXf mat = MatrixXf::Random(size, size);
  eigen_extensions::save(mat, "matxf.eig");
  MatrixXf mat2;
  eigen_extensions::load("matxf.eig", &mat2);
  EXPECT_TRUE(mat.isApprox(mat2));

  eigen_extensions::save(mat, "matxf.eig.gz");
  eigen_extensions::load("matxf.eig.gz", &mat2);
  EXPECT_TRUE(mat.isApprox(mat2));
}

TEST(EigenExtensions, VectorXd_serialization) {
  VectorXd mat = VectorXd::Random(size);
  eigen_extensions::save(mat, "vecxd.eig");
  VectorXd mat2;
  eigen_extensions::load("vecxd.eig", &mat2);
  EXPECT_TRUE(mat.isApprox(mat2));
  MatrixXd mat3;
  eigen_extensions::load("vecxd.eig", &mat3);
  EXPECT_TRUE(mat.isApprox(mat3));
}

TEST(EigenExtensions, VectorXf_serialization) {
  VectorXf mat = VectorXf::Random(size);
  eigen_extensions::save(mat, "vecxf.eig");
  VectorXf mat2;
  eigen_extensions::load("vecxf.eig", &mat2);
  EXPECT_TRUE(mat.isApprox(mat2));
  MatrixXf mat3;
  eigen_extensions::load("vecxf.eig", &mat3);
  EXPECT_TRUE(mat.isApprox(mat3));
}

TEST(EigenExtensions, Vector3i_serialization) {
  Vector3i mat = Vector3i::Random(3);
  eigen_extensions::save(mat, "vec3i.eig");
  Vector3i mat2;
  eigen_extensions::load("vec3i.eig", &mat2);
  EXPECT_TRUE(mat.isApprox(mat2));
  cout << mat.transpose() << endl;
  cout << mat2.transpose() << endl;
}

TEST(EigenExtensions, MatrixXd_serialization_ascii) {
  MatrixXd mat = MatrixXd::Random(5, 20);
  eigen_extensions::saveASCII(mat, "matxd.eig.txt");
  MatrixXd mat2;
  eigen_extensions::loadASCII("matxd.eig.txt", &mat2);
  cout << mat << endl;
  cout << "----------" << endl;
  cout << mat2 << endl;
  cout << "fnorm: " << (mat - mat2).norm() << endl;
  
  EXPECT_TRUE((mat - mat2).norm() < 1e-3);
}

TEST(EigenExtensions, Vector3i_serialization_ascii) {
  Vector3i mat = Vector3i::Random(3);
  eigen_extensions::saveASCII(mat, "vec3i.eig.txt");
  Vector3i mat2;
  eigen_extensions::loadASCII("vec3i.eig.txt", &mat2);
  EXPECT_TRUE(mat.isApprox(mat2));
  cout << mat.transpose() << endl;
  cout << mat2.transpose() << endl;
}

TEST(EigenExtensions, Compression)
{
  MatrixXd mat = MatrixXd::Identity(1000, 1000);
  eigen_extensions::save(mat, "large.eig");
  MatrixXd mat2;
  eigen_extensions::load("large.eig", &mat2);
  EXPECT_TRUE(mat.isApprox(mat2));

  eigen_extensions::save(mat, "large.eig.gz");
  eigen_extensions::load("large.eig.gz", &mat2);
  EXPECT_TRUE(mat.isApprox(mat2));
}

TEST(EigenExtensions, serialization_multi_ascii) {
  Vector3i vec = Vector3i::Random(3);
  MatrixXd mat = MatrixXd::Random(3, 5);

  std::ofstream ofile;
  ofile.open("multi_serialize_ascii");
  assert(ofile);
  eigen_extensions::serializeASCII(vec, ofile);
  eigen_extensions::serializeASCII(mat, ofile);
  ofile.close();

  Vector3i vec2;
  MatrixXd mat2;

  std::ifstream ifile;
  ifile.open("multi_serialize_ascii");
  assert(ifile);
  eigen_extensions::deserializeASCII(ifile, &vec2);
  eigen_extensions::deserializeASCII(ifile, &mat2);
  ifile.close();

  EXPECT_TRUE(mat2.isApprox(mat));
  EXPECT_TRUE(vec2.isApprox(vec));
}

TEST(EigenExtensions, SparseSerialization)
{
  SparseMatrix<double> mat(5, 3);
  for(int i = 0; i < mat.outerSize(); ++i) {
    mat.startVec(i);
    mat.insertBack(i, i) = 1;
    if(i == 0)
      mat.insertBack(i+1, i) = 2;
    if(i == 2)
      mat.insertBack(i+2, i) = 3;
  }
  mat.finalize();
  
  eigen_extensions::save(mat, "sparse.eig");
  SparseMatrix<double> mat2;
  eigen_extensions::load("sparse.eig", &mat2);
  EXPECT_TRUE(mat.isApprox(mat2));
}

TEST(EigenExtensions, SparseSerializationRowMajor)
{
  SparseMatrix<double, RowMajor> mat(3, 5);
  for(int i = 0; i < mat.outerSize(); ++i) {
    mat.startVec(i);
    mat.insertBack(i, i) = 1;
    if(i == 0)
      mat.insertBack(i, i+1) = 2;
    if(i == 2)
      mat.insertBack(i, i+2) = 3;
  }
  mat.finalize();
  
  eigen_extensions::save(mat, "sparse.eig");
  SparseMatrix<double, RowMajor> mat2;
  eigen_extensions::load("sparse.eig", &mat2);
  cout << mat << endl;
  cout << mat2 << endl;
  
  EXPECT_TRUE(mat.isApprox(mat2));
}

// TEST(EigenExtensions, SparseVecSerialization)
// {
//   SparseVector<double> vec(5);
//   vec.coeffRef(2) = 1;
//   vec.coeffRef(4) = 2;
  
//   eigen_extensions::save(vec, "sparse.eig");
//   SparseVector<double> vec2;
//   eigen_extensions::load("sparse.eig", &vec2);
//   EXPECT_TRUE(vec.isApprox(vec2));
// }

// TEST(EigenExtensions, bad_load) {
//   MatrixXd mat;
//   eigen_extensions::loadASCII("bad.eig.txt", &mat);
// }

TEST(cpp, cpp)
{
  double x = 13.13;
  size_t y = 13;
  cout << x / y << endl;
  cout << y / x << endl;
  cout << x * y << endl;
  cout << y * x << endl;
  cout << rand() / RAND_MAX << endl;
  cout << rand() / (double)RAND_MAX << endl;
  cout << (double)rand() / RAND_MAX << endl;
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
