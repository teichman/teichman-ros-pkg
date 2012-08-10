#ifndef NAME_MAPPING2_H
#define NAME_MAPPING2_H

#include <assert.h>
#include <stdarg.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <sstream>
#include <limits.h>

#include <ros/console.h>
#include <ros/assert.h>
#include <Eigen/Eigen>
#include <serializable/serializable.h>
#include <name_mapping/name_mapping.h>

class NameMapping : public Serializable
{
public:
  //! Default constructor: empty NameMapping.
  NameMapping();
  NameMapping(const NameMapping& other);
  
  /************************************************************
   * Modify
   ************************************************************/

  void addName(const std::string& name);
  void addNames(const std::vector<std::string>& names);
  void augment(const NameMapping& other);
  NameMapping& operator=(const NameMapping& other);
  NameMapping& operator+=(const NameMapping& other);
  NameMapping operator+(const NameMapping& other);

  
  /************************************************************
   * Access
   ************************************************************/

  std::string toName(size_t id) const;
  size_t toId(std::string name) const;
  bool hasName(std::string name) const;
  bool hasId(size_t id) const { return (id < names_.size()); }
  const std::vector<std::string>& names() const { return names_; }
  size_t size() const;
  bool empty() const;
  std::string status(const std::string& prefix = "") const;

  
  /************************************************************
   * Compare
   ************************************************************/
  
  //! Returns a human-readable diff.
  std::string diff(const NameMapping& other) const;
  void diff(const NameMapping& other,
  	    std::vector<std::string>* here_but_not_there,
  	    std::vector<std::string>* there_but_not_here) const;
  //! Returns true if this NameMapping is equal to other.  (Permutations are different.)
  bool operator==(const NameMapping& other) const;
  bool operator!=(const NameMapping& other) const;
  bool isPermutation(const NameMapping& other) const;
  
  /************************************************************
   * Serialize
   ************************************************************/

  void serialize(std::ostream& out) const;
  void deserialize(std::istream& in);
  
private:
  std::vector<std::string> names_;
  std::map<std::string, size_t> indices_;

  friend class NameTranslator2;
};

class NameTranslator2
{
public:
  static const int NO_ID = -1;
  
  NameTranslator2(const NameMapping& old_mapping, const NameMapping& new_mapping);
  int toNew(size_t old) const;
  size_t newSize() const;
  size_t oldSize() const;
  std::string status(const std::string& prefix = "") const;
  const NameMapping& oldMapping() const { return old_mapping_; }
  const NameMapping& newMapping() const { return new_mapping_; }
  
  /************************************************************
   * Functions for common translations
   ************************************************************/

  //! Assumes target is an index into the name map.
  //! If the name no longer exists, target will be set to NO_ID.
  //! If target is NO_ID, then nothing will change.
  void translate(int* target) const;
  //! Generic translation function for std::vectors.
  //! Makes a copy.  Newly-added elements are set to init.
  //! The default ctor is a reasonable choice for init, but for POD
  //! types no complaint is made about the lack of initialization.
  //! This makes me uncomfortable.  Uninitialized data could result
  //! in really nasty bugs that are hard to track down.
  //! So, you are forced to provide the default value for new elements.
  template<typename T>
  void translate(std::vector<T>* target, T init) const;
  //! Generic translation function for Eigen column vectors.
  //! Makes a copy.  Newly-added elements are set to init.
  template<typename T, int R>
  void translate(Eigen::Matrix<T, R, 1>* target, T init = 0) const;
  //! Translates a matrix by translating each column, one at a time.
  //! Rows will end up being swapped.
  //! Makes a copy.  Newly-added elements are set to init.
  template<typename T, int R, int C>
  void translateRows(Eigen::Matrix<T, R, C>* target, T init = 0) const;
  //! Translates a matrix by translating each row, one at a time.
  //! Cols will end up being swapped.
  //! Makes a copy.  Newly-added elements are set to init.
  template<typename T, int R, int C>
  void translateCols(Eigen::Matrix<T, R, C>* target, T init = 0) const;
  
private:
  NameMapping old_mapping_;
  NameMapping new_mapping_;
  std::vector<int> old_to_new_;

  void initialize();
};

//! Abstract base class for classes that can have a NameTranslator2 applied to them.
//! These classes do not necessarily store their NameMappings.
//! For example, a Classification object is very lightweight - just a VectorXf with
//! some convenience functions - and you wouldn't want to copy a NameMapping into it,
//! but you do want to be able to translate it when it's containing object is translated.
//! It's up to the programmer to make sure the correct translator is applied.
class NameTranslatable
{
public:
  virtual void applyNameTranslator(const std::string& nmid, const NameTranslator2& translator);
  
protected:
  //! Preceeding underscore implies this method is a custom implementation which is called
  //! by the corresponding method without the underscore.
  //! This method should remain protected in derived classes.  (Is it possible to force this?)
  //!
  //! Translation is allocation: Assume that this method will be called with an empty old mapping
  //! and a filled new mapping.  The derived object should allocate as necessary.
  //! See the translation convenience functions for std::vector and Eigen objects.
  virtual void _applyNameTranslator(const std::string& nmid, const NameTranslator2& translator) = 0;
};

//! Abstract base class for classes that should be NameTranslatable and should
//! store their NameMappings.
class NameMappable : public NameTranslatable
{
public:
  virtual ~NameMappable() {};

  void applyNameMapping(const std::string& nmid, const NameMapping& new_mapping);
  void applyNameMappings(const NameMappable& other);
  //! Unlike the NameTranslatable version, this will also copy in the new NameMapping.
  void applyNameTranslator(const std::string& nmid, const NameTranslator2& translator);
  
  const NameMapping& nameMapping(const std::string& nmid) const;
  const std::map<std::string, NameMapping>& nameMappings() const { return name_mappings_; }
  std::string nameMappingStatus(const std::string& prefix = "") const;
  bool hasNameMapping(const std::string& nmid) const { return name_mappings_.count(nmid); }
  
  bool nameMappingsAreEqual(const NameMappable& other) const;
  bool nameMappingsArePermutations(const NameMappable& other) const;
  
  void serializeNameMappings(std::ostream& out) const;
  //! Does not apply the name mappings.  It is assumed that the user
  //! will load name-mapped data that is consistent with the name mappings
  //! that are deserialized.
  void deserializeNameMappings(std::istream& in);

private:
  std::map<std::string, NameMapping> name_mappings_;
};

template<typename T>
void NameTranslator2::translate(std::vector<T>* target, T init) const
{
  ROS_ASSERT(target->size() == 0 || target->size() == old_mapping_.size());  // Allow uninitialized data to become initialized.
  if(target->size() == 0) {
    target->resize(newSize(), init);
    return;
  }
  
  std::vector<T> tmp(newSize(), init);
  for(size_t i = 0; i < oldSize(); ++i)
    if(toNew(i) != NO_ID)
      tmp[toNew(i)] = target->at(i);
  *target = tmp;
}

template<typename T, int R>
void NameTranslator2::translate(Eigen::Matrix<T, R, 1>* target, T init) const
{
  ROS_ASSERT(newSize() > 0);
  ROS_ASSERT(target->rows() == 0 || target->rows() == (int)old_mapping_.size());  // Allow uninitialized data to become initialized.
  if(target->rows() == 0) {
    target->resize(newSize());
    target->setConstant(init);
    return;
  }
  
  Eigen::Matrix<T, R, 1> tmp = Eigen::Matrix<T, R, 1>(newSize());
  tmp.setConstant(init);
  for(size_t i = 0; i < oldSize(); ++i)
    if(toNew(i) != NO_ID)
      tmp(toNew(i)) = target->coeffRef(i);
  *target = tmp;
}

template<typename T, int R, int C>
void NameTranslator2::translateRows(Eigen::Matrix<T, R, C>* target, T init) const
{
  ROS_ASSERT(newSize() > 0);
  ROS_ASSERT(target->rows() == (int)old_mapping_.size());
  Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> tmp(newSize(), target->cols());
  Eigen::Matrix<T, Eigen::Dynamic, 1> tmpvec;
  for(int i = 0; i < target->cols(); ++i) {
    tmpvec = target->col(i);  
    translate(&tmpvec, init);
    tmp.col(i) = tmpvec;
  }
  *target = tmp;
}

template<typename T, int R, int C>
void NameTranslator2::translateCols(Eigen::Matrix<T, R, C>* target, T init) const
{
  ROS_ASSERT(newSize() > 0);
  ROS_ASSERT(target->cols() == (int)old_mapping_.size());
  Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> tmp(target->rows(), newSize());
  Eigen::Matrix<T, Eigen::Dynamic, 1> tmpvec;
  for(int i = 0; i < target->rows(); ++i) {
    tmpvec = target->row(i).transpose();  
    translate(&tmpvec, init);
    tmp.row(i) = tmpvec.transpose();
  }
  *target = tmp;
}

#endif // NAME_MAPPING2_H
