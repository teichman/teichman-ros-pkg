#include <name_mapping/name_mapping.h>

using namespace std;

NameMapping::NameMapping()
{
}

NameMapping::NameMapping(const NameMapping& other) :
  names_(other.names_),
  indices_(other.indices_)
{
} 

NameMapping& NameMapping::operator=(const NameMapping& other)
{
  names_ = other.names_;
  indices_ = other.indices_;

  return *this;
}

NameMapping& NameMapping::operator+=(const NameMapping& other)
{
  for(size_t i = 0; i < other.names_.size(); ++i) {
    if(hasName(other.names_[i]))
      continue;

    addName(other.names_[i]);
  }

  return *this;
}

NameMapping NameMapping::operator+(const NameMapping& other)
{
  NameMapping new_mapping = *this;
  new_mapping += other;
  return new_mapping;
}

void NameMapping::addName(const string& name)
{
  // No spaces.
  if(name.find(" ") != string::npos)
    ROS_FATAL_STREAM("Space found in NameMapping entry " << name << ", dying horribly.");

   // No duplicates.
  if(indices_.find(name) != indices_.end())
    ROS_FATAL_STREAM("Duplicate name " << name << " added to NameMapping, dying horribly.");

  indices_[name] = names_.size();
  names_.push_back(name);
}

void NameMapping::addNames(const vector<string>& names)
{
  for(size_t i = 0; i < names.size(); ++i)
    addName(names[i]);
}

bool NameMapping::hasName(std::string name) const
{
  return indices_.count(name);
}

void NameMapping::augment(const NameMapping & other) {
  set<string> uniq;
  for(size_t i = 0; i < names_.size(); ++i)
    uniq.insert(names_[i]);

  for(size_t i = 0; i < other.size(); ++i) { 
    if(uniq.count(other.toName(i)) == 0)
      addName(other.toName(i));
  }
}

string NameMapping::toName(size_t id) const {
  ROS_ASSERT(id < names_.size());
  return names_[id];
}

size_t NameMapping::toId(string name) const {
  map<string, size_t>::const_iterator it;
  it = indices_.find(name);
  ROS_FATAL_STREAM_COND(it == indices_.end(), "Could not find name " << name << " in the following NameMapping: " << endl << status());

  return it->second;
}

#define SERIALIZATION_VERSION 2
void NameMapping::serialize(std::ostream& out) const
{
  out << "NameMapping" << endl;
  out << "SERIALIZATION_VERSION " << SERIALIZATION_VERSION << endl;
  out << "num_names " << names_.size() << endl;
  for(size_t i=0; i<names_.size(); ++i)
    out << names_[i] << endl;
}

void NameMapping::deserialize(std::istream& in)
{
  string buf;
  in >> buf;
  if(buf.compare("NameMapping") != 0) {
    ROS_FATAL_STREAM("Expected \"NameMapping\", got \"" << buf << "\"" << endl);
    abort();
  }
  in >> buf;
  int serialization_version;
  in >> serialization_version;
  if(serialization_version != SERIALIZATION_VERSION) { 
    ROS_FATAL_STREAM("NameMapping serialization version is " << serialization_version << ", expected " << SERIALIZATION_VERSION);
    abort();
  }

  in >> buf;
  size_t num_names;
  in >> num_names;

  for(size_t i = 0; i < num_names; ++i) { 
    in >> buf;
    addName(buf);
  }

  getline(in, buf); // Eat the final newline.
}

bool NameMapping::operator==(const NameMapping& other) const
{
  ROS_DEBUG("Comparing NameMappings.");
  
  if(size() != other.size()) {
    ROS_DEBUG("Size did not match.");
    return false;
  }

  for(size_t i = 0; i < names_.size(); ++i) {
    if(!other.hasName(names_[i])) { 
      ROS_DEBUG_STREAM("Other does not have name: " << names_[i] << ".");
      return false;
    }
    
    int id = other.toId(names_[i]);
    ROS_ASSERT(id >= 0);
    if(i != (size_t)(id)) {
      ROS_DEBUG_STREAM("Name mismatch: " << i);
      return false;
    }
  }

  for(size_t i = 0; i < other.size(); ++i) {
    string name = other.toName(i);
    if(!hasName(name)) {
      ROS_DEBUG_STREAM("*this does not have name: " << name << ".");
      return false;
    }

    if(i != toId(name)) {
      ROS_DEBUG_STREAM("Name mismatch: " << i);
      return false;
    }
  }
		  
  return true;
}

bool NameMapping::operator!=(const NameMapping& other) const
{
  return !(*this == other);
}

bool NameMapping::isPermutation(const NameMapping& other) const
{
  if(size() != other.size())
    return false;

  for(size_t i = 0; i < names_.size(); ++i)
    if(!other.hasName(names_[i]))
      return false;

  for(size_t i = 0; i < other.size(); ++i)
    if(!hasName(other.toName(i)))
      return false;
		  
  return true;
}

std::string NameMapping::diff(const NameMapping& other) const
{
  vector<string> here_but_not_there;
  vector<string> there_but_not_here;
  diff(other, &here_but_not_there, &there_but_not_here);

  ostringstream oss;
  oss << "here_but_not_there: " << endl;
  for(size_t i = 0; i < here_but_not_there.size(); ++i)
    oss << here_but_not_there[i] << endl;
  oss << "there_but_not_here: " << endl;
  for(size_t i = 0; i < there_but_not_here.size(); ++i)
    oss << there_but_not_here[i] << endl;

  return oss.str();
}

void NameMapping::diff(const NameMapping& other,
		       vector<string>* here_but_not_there,
		       vector<string>* there_but_not_here) const
{
  ROS_ASSERT(there_but_not_here->empty());
  ROS_ASSERT(here_but_not_there->empty());
  
  for(size_t i = 0; i < names_.size(); ++i) {
    if(other.indices_.find(names_[i]) == other.indices_.end())
      here_but_not_there->push_back(names_[i]);
  }

  for(size_t i = 0; i < other.names_.size(); ++i) {
    if(indices_.find(other.names_[i]) == indices_.end())
      there_but_not_here->push_back(other.names_[i]);
  }
}

size_t NameMapping::size() const
{
  return names_.size();
}

bool NameMapping::empty() const
{
  return names_.empty();
}

std::string NameMapping::status(const std::string& prefix) const
{
  ostringstream oss;
  for(size_t i = 0; i < names_.size(); ++i)
    oss << prefix << (int)i << ": " << names_[i] << endl;
  return oss.str();
}


/***********************************************************
 * NameTranslator2
 ************************************************************/

NameTranslator2::NameTranslator2(const NameMapping& old_mapping, const NameMapping& new_mapping) :
  old_mapping_(old_mapping),
  new_mapping_(new_mapping)
{
  initialize();
}

std::string NameTranslator2::status(const std::string& prefix) const
{
  ostringstream oss;
  oss << prefix << "Old mapping: " << endl;
  oss << old_mapping_.status(prefix);
  oss << prefix << "New mapping: " << endl;
  oss << new_mapping_.status(prefix);
  for(size_t i = 0; i < old_to_new_.size(); ++i) {
    oss << prefix << i << " <-> ";
    if(toNew(i) != NO_ID)
      oss << toNew(i);
    oss << "\t\t" << old_mapping_.toName(i) << endl;
  }
  oss << prefix << "raw" << endl;
  for(size_t i = 0; i < old_to_new_.size(); ++i)
    oss << prefix << i << " - " << old_to_new_[i] << endl;

  return oss.str();
}

void NameTranslator2::initialize()
{
  old_to_new_.resize(old_mapping_.size());
  for(size_t i = 0; i < old_mapping_.size(); ++i) {
    string old_name = old_mapping_.toName(i);
    if(!new_mapping_.hasName(old_name))
      old_to_new_[i] = NO_ID;
    else
      old_to_new_[i] = new_mapping_.indices_[old_name];
  }
}

int NameTranslator2::toNew(size_t old) const
{
  ROS_ASSERT(old < old_to_new_.size());
  return old_to_new_[old];
}

size_t NameTranslator2::newSize() const
{
  return new_mapping_.size();
}

size_t NameTranslator2::oldSize() const
{
  return old_mapping_.size();
}

void NameTranslator2::translate(int* target) const
{
  ROS_ASSERT(target && (*target == NO_ID || old_mapping_.hasId(*target)));
  if(*target == NO_ID)
    return;
  *target = old_to_new_[*target];
}


/************************************************************
 * NameMappable
 ************************************************************/

void NameTranslatable::applyNameTranslator(const std::string& nmid, const NameTranslator2& translator)
{
  _applyNameTranslator(nmid, translator);
}


/************************************************************
 * NameMappable
 ************************************************************/

void NameMappable::applyNameTranslator(const std::string& nmid, const NameTranslator2& translator)
{
  name_mappings_[nmid] = translator.newMapping();
  NameTranslatable::applyNameTranslator(nmid, translator);
}

void NameMappable::applyNameMapping(const std::string& nmid, const NameMapping& new_mapping)
{
  NameMapping old_mapping;
  if(name_mappings_.count(nmid))
    old_mapping = name_mappings_[nmid];
  name_mappings_[nmid] = new_mapping;

  // Sometimes you want to apply an identical mapping just to get the allocation and initialization
  // to happen automatically.
  // if(old_mapping == new_mapping)
  //   return;

  NameTranslator2 translator(old_mapping, new_mapping);
  _applyNameTranslator(nmid, translator);
}

void NameMappable::applyNameMappings(const NameMappable& other)
{
  map<string, NameMapping>::const_iterator it;
  for(it = other.nameMappings().begin(); it != other.nameMappings().end(); ++it)
    applyNameMapping(it->first, it->second);
}

const NameMapping& NameMappable::nameMapping(const std::string& nmid) const
{
  ROS_ASSERT(name_mappings_.count(nmid));
  return name_mappings_.find(nmid)->second;
}

void NameMappable::serializeNameMappings(std::ostream& out) const
{
  out << "NameMappable" << endl;
  out << name_mappings_.size() << endl;
  map<string, NameMapping>::const_iterator it;
  for(it = name_mappings_.begin(); it != name_mappings_.end(); ++it) {
    string nmid = it->first;
    const NameMapping& mapping = it->second;
    out << nmid << endl;
    out << mapping;
  }
}

void NameMappable::deserializeNameMappings(std::istream& in)
{
  string line;
  getline(in, line);
  ROS_ASSERT(line.compare("NameMappable") == 0);
  size_t num_mappings;
  in >> num_mappings;
  getline(in, line);
  for(size_t i = 0; i < num_mappings; ++i) {
    string nmid;
    getline(in, nmid);
    NameMapping mapping;
    in >> mapping;
    name_mappings_[nmid] = mapping;
  }
}

bool NameMappable::nameMappingsAreEqual(const NameMappable& other) const
{
  map<string, NameMapping> other_mappings = other.nameMappings();
  if(other_mappings.size() != name_mappings_.size())
    return false;

  map<string, NameMapping>::const_iterator it;
  for(it = name_mappings_.begin(); it != name_mappings_.end(); ++it) {
    const string& nmid = it->first;
    if(!other_mappings.count(nmid))
      return false;
    if(other_mappings.find(nmid)->second != name_mappings_.find(nmid)->second)
      return false;
  }
  return true;
}

bool NameMappable::nameMappingsArePermutations(const NameMappable& other) const
{
  map<string, NameMapping> other_mappings = other.nameMappings();
  if(other_mappings.size() != name_mappings_.size())
    return false;

  map<string, NameMapping>::const_iterator it;
  for(it = name_mappings_.begin(); it != name_mappings_.end(); ++it) {
    const string& nmid = it->first;
    if(!other_mappings.count(nmid))
      return false;
    if(!other_mappings.find(nmid)->second.isPermutation(it->second))
      return false;
  }
  return true;
}

std::string NameMappable::nameMappingStatus(const std::string& prefix) const
{
  ostringstream oss;
  map<string, NameMapping>::const_iterator it;
  for(it = name_mappings_.begin(); it != name_mappings_.end(); ++it) {
    const string& nmid = it->first;
    oss << prefix << "NameMapping \"" << nmid << "\"" << endl;
    oss << it->second.status(prefix);
  }
  return oss.str();
}

