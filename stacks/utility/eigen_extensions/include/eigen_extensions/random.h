#ifndef EIGEN_EXTENSIONS_RANDOM_H
#define EIGEN_EXTENSIONS_RANDOM_H

#include <stdint.h>
#include <tr1/random>
#define EIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET
#include <Eigen/Eigen>
#include <Eigen/Sparse>

namespace eigen_extensions
{
  
  class Sampler
  {
  public:
    virtual ~Sampler() {}
    virtual double sample() = 0;
  };
  
  class UniformSampler : public Sampler
  {
  public:
    UniformSampler(uint64_t seed = 0);
    double sample();
    
  protected:
    std::tr1::mt19937 mersenne_;
  };
  
  class GaussianSampler : public Sampler
  {
  public:
    GaussianSampler(double mean = 0, double variance = 1, uint64_t seed = 0);
    double sample();
    template<class S, int T, int U> void sample(Eigen::Matrix<S, T, U>* mat);
        
  protected:
    std::tr1::mt19937 mersenne_;
    std::tr1::normal_distribution<double> normal_;
    std::tr1::variate_generator<std::tr1::mt19937, std::tr1::normal_distribution<double> > vg_;
  };

  template<class S, int T, int U>
  void GaussianSampler::sample(Eigen::Matrix<S, T, U>* mat)
  {
    for(int i = 0; i < mat->cols(); ++i)
      for(int j = 0; j < mat->rows(); ++j)
	mat->coeffRef(j, i) = sample();
  }
  
  template<class S, int T, int U>
  void sampleGaussian(Eigen::Matrix<S, T, U>* mat)
  {
    GaussianSampler gs;
    gs.sample(mat);
  }
  
  void sampleSparseGaussianVector(int rows, int nnz, Eigen::SparseVector<double>* vec);
  int weightedSample(Eigen::VectorXd weights);
  //! Fills indices with samples from the weights vector, with replacement.
  void weightedSample(Eigen::VectorXd weights, Eigen::VectorXi* indices);
  void weightedSampleLowVariance(Eigen::VectorXd weights, Eigen::VectorXi* indices);
}


#endif // EIGEN_EXTENSIONS_RANDOM_H
