#include <name_mapping/name_mapping.h>
#include <gtest/gtest.h>

using namespace std;
using namespace Eigen;

TEST(NameMapping, Assigment)
{
  NameMapping nm;
  nm.addName("foo");
  nm.addName("bar");
  nm.addName("baz");

  cout << nm.status() << endl;
  NameMapping nm2 = nm;
  cout << nm2.status() << endl;
  
  EXPECT_TRUE(nm == nm2);

  NameMapping nm3;
  nm3 = nm;
  EXPECT_TRUE(nm == nm3);

  nm3.addName("aoeu");
  EXPECT_FALSE(nm == nm3);
}

TEST(NameMapping, Serialization)
{
  NameMapping nm;
  nm.addName("foo");
  nm.addName("bar");
  nm.addName("baz");

  string filename = ".test-18923748736458927397653.nm";
  nm.save(filename);

  NameMapping nm2;
  nm2.load(filename);
  cout << nm.status() << endl;
  cout << nm << endl;
  cout << nm2.status() << endl;
  cout << nm2 << endl;
  EXPECT_TRUE(nm == nm2);

  int retval = system(("rm " + filename).c_str()); retval--;
}

TEST(NameMapping, Addition)
{
  NameMapping initial;
  initial.addName("car");
  initial.addName("pedestrian");
  initial.addName("bicyclist");

  NameMapping supplemental;
  supplemental.addName("car");
  supplemental.addName("motorcyclist");

  NameMapping final = initial + supplemental;
  cout << final.status() << endl;
  cout << final << endl;
  EXPECT_TRUE(final.size() == 4);
  EXPECT_TRUE(final.toName(3).compare("motorcyclist") == 0);
}

TEST(NameTranslator2, Translation)
{
  NameMapping initial;
  initial.addName("car");
  initial.addName("pedestrian");
  initial.addName("bicyclist");

  NameMapping final;
  final.addName("bicyclist");
  final.addName("car");
  
  NameTranslator2 translator(initial, final);
  cout << translator.status() << endl;

  EXPECT_TRUE(translator.toNew(0) == 1);
  EXPECT_TRUE(translator.toNew(1) == NameTranslator2::NO_ID);
}

class Foo : public NameMappable, public Serializable
{
public:
  std::vector<int> labels_;
  std::vector<double> std_weights_;
  Eigen::VectorXf eigen_weights_;
  Eigen::MatrixXi eigen_labels_;
  Eigen::MatrixXi eigen_labels_transposed_;
  
  static Foo getTestMBObject();
  std::string status() const;

  void serialize(std::ostream& out) const;
  void deserialize(std::istream& in);

protected:
  void _applyNameTranslator(const std::string& id, const NameTranslator2& translator);
};

void Foo::_applyNameTranslator(const std::string& id, const NameTranslator2& translator)
{
  if(id == "cmap") {
    translator.translateRows(&eigen_labels_);
    translator.translateCols(&eigen_labels_transposed_);

    for(size_t i = 0; i < labels_.size(); ++i) {
      if(labels_[i] == -1)
	continue;
      if(translator.toNew(labels_[i]) == NameTranslator2::NO_ID)
	labels_[i] = -1;
      else
	labels_[i] = translator.toNew(labels_[i]);
    }
  }
  else if(id == "dmap") {
    translator.translate(&eigen_weights_);
    translator.translate(&std_weights_, 0.0);
  }
}

string Foo::status() const
{
  ostringstream oss;
  oss << "--- Foo status ---" << endl;
  oss << nameMappingStatus() << endl;
  for(size_t i = 0; i < labels_.size(); ++i) {
    oss << i << " " << labels_[i] << " ";
    if(labels_[i] == -1)
      oss << " bg." << endl;
    else
      oss << nameMapping("cmap").toName(labels_[i]) << endl;
  }
  oss << "Weights (eigen, then std): " << endl;
  for(size_t i = 0; i < nameMapping("dmap").size(); ++i)
    oss << i << ": " << eigen_weights_(i) << "\t" << std_weights_[i] << " \t --- " << nameMapping("dmap").toName(i) << endl;
  oss << "Eigen labels: " << endl;
  oss << eigen_labels_.transpose() << endl;
  
  return oss.str();
}

Foo Foo::getTestMBObject()
{
  Foo foo;

  NameMapping cmap;
  cmap.addName("car");
  cmap.addName("pedestrian");
  cmap.addName("bicyclist");
  foo.applyNameMapping("cmap", cmap);
  foo.labels_.push_back(0);
  foo.labels_.push_back(0);
  foo.labels_.push_back(2);
  foo.labels_.push_back(1);
  foo.eigen_labels_ = MatrixXi::Zero(cmap.size(), foo.labels_.size());
  for(size_t i = 0; i < foo.labels_.size(); ++i) {
    for(int j = 0; j < foo.eigen_labels_.rows(); ++j) {
      if(j == foo.labels_[i])
	foo.eigen_labels_(j, i) = 1;
      else
	foo.eigen_labels_(j, i) = -1;
    }
  }
  foo.eigen_labels_transposed_ = foo.eigen_labels_.transpose();
  
  NameMapping dmap;
  dmap.addName("SpinImage");
  dmap.addName("HOG");
  dmap.addName("Size");
  dmap.addName("Fury");
  foo.applyNameMapping("dmap", dmap);
  foo.eigen_weights_(0) = 10;
  foo.eigen_weights_(1) = 1;
  foo.eigen_weights_(2) = 2;
  foo.eigen_weights_(3) = 3;
  foo.std_weights_[0] = 10;
  foo.std_weights_[1] = 1;
  foo.std_weights_[2] = 2;
  foo.std_weights_[3] = 3;

  return foo;
}

void Foo::serialize(std::ostream& out) const
{
  serializeNameMappings(out);
}

void Foo::deserialize(std::istream& in)
{
  deserializeNameMappings(in);
}

TEST(NameMappable, Translation)
{
  Foo foo = Foo::getTestMBObject();
  cout << "Foo object: " << endl << foo.status() << endl;

  NameMapping new_cmap;
  new_cmap.addName("pedestrian");
  new_cmap.addName("bicyclist");
  new_cmap.addName("motorcyclist");
  foo.applyNameMapping("cmap", new_cmap);
  EXPECT_TRUE(foo.labels_[0] == -1);
  EXPECT_TRUE(foo.labels_[1] == -1);
  EXPECT_TRUE(foo.labels_[2] == 1);
  EXPECT_TRUE(foo.labels_[3] == 0);
  Eigen::MatrixXi expected(new_cmap.size(), foo.labels_.size());
  expected << -1, -1, -1, +1,
              -1, -1, +1, -1,
               0,  0,  0,  0;
  for(int i = 0; i < foo.eigen_labels_.cols(); ++i) {
    for(int j = 0; j < foo.eigen_labels_.rows(); ++j) {
      EXPECT_TRUE(expected(j, i) == foo.eigen_labels_(j, i));
      EXPECT_TRUE(expected(j, i) == foo.eigen_labels_transposed_(i, j));
    }
  }

  cout << foo.status() << endl;

  NameMapping new_dmap;
  new_dmap.addName("Fury");
  new_dmap.addName("Size");
  new_dmap.addName("SomethingNew");
  new_dmap.addName("HOG");
  foo.applyNameMapping("dmap", new_dmap);
  cout << foo.status() << endl;
  EXPECT_FLOAT_EQ(foo.eigen_weights_(0), 3);
  EXPECT_FLOAT_EQ(foo.eigen_weights_(1), 2);
  EXPECT_FLOAT_EQ(foo.eigen_weights_(2), 0);
  EXPECT_FLOAT_EQ(foo.eigen_weights_(3), 1);
  EXPECT_FLOAT_EQ(foo.std_weights_[0], 3);
  EXPECT_FLOAT_EQ(foo.std_weights_[1], 2);
  EXPECT_FLOAT_EQ(foo.std_weights_[2], 0);
  EXPECT_FLOAT_EQ(foo.std_weights_[3], 1);
}

TEST(NameMappable, Serialization)
{
  Foo foo = Foo::getTestMBObject();
  cout << "Foo object: " << endl << foo.status() << endl;
  string path = "name_mappable_test";
  foo.save(path);

  Foo foo2;
  foo2.load(path);
  EXPECT_TRUE(foo.nameMappingsAreEqual(foo2));
  EXPECT_TRUE(foo2.nameMappingsAreEqual(foo));
  EXPECT_TRUE(foo.nameMappingsArePermutations(foo2));
  EXPECT_TRUE(foo2.nameMappingsArePermutations(foo));

  NameMapping new_cmap;
  new_cmap.addName("pedestrian");
  new_cmap.addName("bicyclist");
  new_cmap.addName("car");
  foo.applyNameMapping("cmap", new_cmap);
  EXPECT_TRUE(!foo.nameMappingsAreEqual(foo2));
  EXPECT_TRUE(!foo2.nameMappingsAreEqual(foo));
  EXPECT_TRUE(foo.nameMappingsArePermutations(foo2));
  EXPECT_TRUE(foo2.nameMappingsArePermutations(foo));

  new_cmap.addName("motorcyclist");
  foo.applyNameMapping("cmap", new_cmap);
  EXPECT_TRUE(!foo.nameMappingsAreEqual(foo2));
  EXPECT_TRUE(!foo2.nameMappingsAreEqual(foo));
  EXPECT_TRUE(!foo.nameMappingsArePermutations(foo2));
  EXPECT_TRUE(!foo2.nameMappingsArePermutations(foo));

}

TEST(StringEquals, StringEquals)
{
  string foo = "foo";
  string foo2 = "foo";
  string bar = "bar";
  EXPECT_TRUE(foo == "foo");
  EXPECT_TRUE(foo == foo2);
  EXPECT_TRUE(foo != bar);
  EXPECT_TRUE(foo != "bar");
}

TEST(Cast, Cast)
{
  // Want to ensure that vec[translator.toNew(nonexistent)] will produce segfaults.
  size_t id = (size_t)(-1);
  EXPECT_TRUE(id == numeric_limits<size_t>::max());
}


class Label : public Eigen::VectorXf, public NameTranslatable
{
protected:
  void _applyNameTranslator(const std::string& id, const NameTranslator2& translator)
  {
    translator.translate(this);
  }
};

class Dataset : public NameMappable
{
public:
  std::vector<Label> labels_;
  std::string status() const
  {
    std::ostringstream oss;
    oss << "Dataset: " << endl;
    oss << nameMappingStatus(" ");
    oss << " Labels: " << endl;
    for(size_t i = 0; i < labels_.size(); ++i)
      oss << "  " << i << ": " << labels_[i].transpose() << endl;

    return oss.str();
  }
  
protected:
  void _applyNameTranslator(const std::string& id, const NameTranslator2& translator)
  {
    ROS_ASSERT(id == "cmap");
    for(size_t i = 0; i < labels_.size(); ++i)
      labels_[i].applyNameTranslator(id, translator);
  }
};

TEST(NameTranslatable, NameTranslatable)
{
  Dataset dataset;
  dataset.labels_.resize(3);
  EXPECT_TRUE(dataset.labels_[0].rows() == 0);
  NameMapping cmap;
  cmap.addName("car");
  cmap.addName("ped");
  cmap.addName("bike");
  dataset.applyNameMapping("cmap", cmap);
  EXPECT_TRUE(dataset.labels_[0].rows() == (int)cmap.size());
  EXPECT_TRUE(dataset.labels_[0](0) == 0);
  EXPECT_TRUE(dataset.nameMapping("cmap") == cmap);  // This ensures the correct applyNameTranslator method is being called.

  dataset.labels_[0](1) = 1;
  cout << dataset.status() << endl;
  NameMapping cmap2;
  cmap2.addName("car");
  cmap2.addName("bike");
  cmap2.addName("train");
  cmap2.addName("ped");
  NameTranslator2 translator(cmap, cmap2);
  dataset.applyNameTranslator("cmap", translator);  // Make sure calling it directly also gets you the right implementation.
  EXPECT_TRUE(dataset.labels_[0].rows() == (int)cmap2.size());
  EXPECT_TRUE(dataset.labels_[0](2) == 0);
  EXPECT_TRUE(dataset.labels_[0](3) == 1);
  EXPECT_TRUE(dataset.nameMapping("cmap") == cmap2);
  cout << "------------------------------------------------------------" << endl;
  cout << dataset.status() << endl;
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
