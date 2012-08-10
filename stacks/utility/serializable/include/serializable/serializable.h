#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <iostream>
#include <fstream>
#include <assert.h>
#include <vector>
#include <map>

/** \brief @b Serializable is an abstract base class which represents
 * objects that can be serialized and deserialized.
 */
class Serializable
{
public:
  virtual ~Serializable() {};
  virtual void serialize(std::ostream& out) const = 0;
  virtual void deserialize(std::istream& in) = 0;
  virtual void save(const std::string& filename) const;
  virtual void load(const std::string& filename);
};

std::ostream& operator<<(std::ostream& out, const Serializable& ser);
std::istream& operator>>(std::istream& in, Serializable& ser);

#endif // SERIALIZABLE_H
