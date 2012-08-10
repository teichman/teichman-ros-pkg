#include <timer/timer.h>
#include <eigen_extensions/random.h>
#include <gtest/gtest.h>

using namespace std;
using namespace Eigen;
using namespace eigen_extensions;

TEST(EigenExtensions, Random)
{
  MatrixXd mat(5, 5);
  sampleGaussian(&mat);
  cout << mat << endl;
  cout << mat.sum() << endl;
}

TEST(EigenExtensions, SparseRandom)
{
  SparseVector<double> vec;
  sampleSparseGaussianVector(100, 10, &vec);
  cout << vec.transpose() << endl;
}

TEST(EigenExtensions, WeightedSample)
{
  VectorXd weights(10);
  sampleGaussian(&weights);
  weights = (weights.array() * weights.array()).matrix();

  VectorXi indices(100);
  weightedSample(weights, &indices);
  cout << weights.transpose() << endl;
  cout << indices.transpose() << endl;
}

TEST(EigenExtensions, weightedSampleLowVariance)
{
  int chunk_size = 5e5;
  int buffer_size = 5e5;
  int total_num_seen = 5e7;
  VectorXd weights(chunk_size + buffer_size);
  weights.head(buffer_size).setConstant(total_num_seen);
  weights.tail(chunk_size).setConstant(chunk_size);

  VectorXi indices(buffer_size);
  srand(time(NULL));
  cout << "random number: " << rand() << endl;
  weightedSampleLowVariance(weights, &indices);

  int num_new = 0;
  int num_old = 0;
  for(int i = 0; i < indices.rows(); ++i) {
    if(indices(i) > buffer_size)
      ++num_new;
    else
      ++num_old;
  }
  cout << "Got " << num_new << " new and " << num_old << " old." << endl;

  // cout << weights.transpose() << endl;
  // cout << indices.transpose() << endl;
  // int idx = 0;
  // for(int i = 0; i < weights.rows(); ++i) {
  //   cout << i << " " << weights(i) << " ";
  //   while(idx < indices.rows() && indices(idx) < i)
  //     ++idx;
  //   while(idx < indices.rows() && indices(idx) == i) {
  //     cout << "*";
  //     ++idx;
  //   }
  //   cout << endl;
  // }
    
}

TEST(EigenExtensions, BigWeightedSample)
{

  VectorXd weights(1e3);
  sampleGaussian(&weights);
  weights = (weights.array() * weights.array()).matrix();

  VectorXi indices(1e3);
  HighResTimer hrt("weighted sample");
  hrt.start();
  weightedSample(weights, &indices);
  hrt.stop();
  cout << hrt.report() << endl;
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
