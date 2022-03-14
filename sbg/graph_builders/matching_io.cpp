/*****************************************************************************

 This file is part of Set--Based Graph Library.

 SBG Library is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 SBG Library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with SBG Library.  If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#include <sbg/graph_builders/matching_io.hpp>

namespace SBG {

namespace IO {

LinearMap::LinearMap() : constant_(0), slope_(1), iterator_() {}
LinearMap::LinearMap(int constant, int slope, std::string iterator) : constant_(constant), slope_(slope), iterator_(iterator) {}

member_imp(LinearMap, std::string, iterator);
member_imp(LinearMap, int, constant);
member_imp(LinearMap, int, slope);

bool LinearMap::operator<(const LinearMap &other) const
{
  if (constant_ == other.constant_) {
    return slope_ < other.slope_;
  }
  return constant_ < other.constant_;
}

std::ostream &operator<<(std::ostream &out, LinearMap &lm)
{
  if (lm.slope_ == 1) {
    out << lm.iterator_;
  } else if (lm.slope_ != 0) {
    out << lm.slope_ << " * " << lm.iterator_;
  }
  if (lm.constant_ > 0) {
    out << " + " << lm.constant_;
  } else if (lm.constant_ < 0) {
    out << lm.constant_;
  }
  return out;
}

std::ostream &operator<<(std::ostream &out, LinearMaps &lms)
{
  if (lms.empty()) {
    return out;
  }
  unsigned int size = lms.size() - 1;
  unsigned int i = 0;
  out << "[";
  while (i < size) {
    out << lms[i] << ",";
    i++;
  }
  out << lms[size] << "]";
  return out;
}

VariableUsage::VariableUsage() : name_(), usage_() {}
VariableUsage::VariableUsage(std::string name, LinearMaps usage) : name_(name), usage_(usage) {}

member_imp(VariableUsage, std::string, name);
member_imp(VariableUsage, LinearMaps, usage);

bool VariableUsage::operator<(const VariableUsage &other) const
{
  if (name() == other.name()) {
    for (unsigned int dim = 0; dim < usage_.size(); dim++) {
      if (usage_[dim] < other.usage_[dim]) return true;
    }
    return false;
  }
  return name() < other.name();
}

std::ostream &operator<<(std::ostream &out, VariableUsage &var_usage)
{
  out << var_usage.name() << var_usage.usage_ref();
  return out;
}

RangeDef::RangeDef() : iterator_(), begin_(1), step_(1), end_(1) {}
RangeDef::RangeDef(std::string iterator, int begin, int step, int end) : iterator_(iterator), begin_(begin), step_(step), end_(end) {}

member_imp(RangeDef, std::string, iterator);
member_imp(RangeDef, int, begin);
member_imp(RangeDef, int, step);
member_imp(RangeDef, int, end);

EquationInfo::EquationInfo() : size_(), var_usage_() {}
EquationInfo::EquationInfo(std::vector<RangeDef> size, std::set<VariableUsage> var_usage) : size_(size), var_usage_(var_usage) {}

member_imp(EquationInfo, std::vector<RangeDef>, size);
member_imp(EquationInfo, std::set<VariableUsage>, var_usage);

VariableInfo::VariableInfo() : name_(), size_(), is_state_(true) {}
VariableInfo::VariableInfo(std::string name, std::vector<int> size, bool is_state) : name_(name), size_(size), is_state_(is_state) {}

member_imp(VariableInfo, std::string, name);
member_imp(VariableInfo, std::vector<int>, size);
member_imp(VariableInfo, bool, is_state);

MatchingInfo::MatchingInfo() : eq_id_(0), eq_range_(), var_id_(0), var_range_() {}
MatchingInfo::MatchingInfo(int eq_id, std::vector<RangeDef> eq_range, int var_id, std::vector<RangeDef> var_range)
    : eq_id_(eq_id), eq_range_(eq_range), var_id_(var_id), var_range_(var_range)
{
}

member_imp(MatchingInfo, int, eq_id);
member_imp(MatchingInfo, std::vector<RangeDef>, eq_range);
member_imp(MatchingInfo, int, var_id);
member_imp(MatchingInfo, std::vector<RangeDef>, var_range);

std::ostream &operator<<(std::ostream &out, MatchingInfo &matching_info)
{
  out << "Equation: " << matching_info.eq_id() << std::endl;
  out << "Range: " << matching_info.eq_range_ref() << std::endl;
  out << "Solves Variable: " << matching_info.var_id() << std::endl;
  out << "Range: " << matching_info.var_range_ref() << std::endl;
  return out;
}

std::ostream &operator<<(std::ostream &out, Matching &matching)
{
  out << "Matching:" << std::endl;
  for (auto m : matching) {
    out << m << std::endl;
  }
  return out;
}

std::ostream &operator<<(std::ostream &out, RangeDef &range)
{
  out << "[" << range.begin() << ", ";
  if (range.step() != 1) {
    out << range.step() << ", ";
  }
  out << range.end() << "]";
  return out;
}

std::ostream &operator<<(std::ostream &out, std::vector<RangeDef> &ranges)
{
  int end = ranges.size(), c = 1;
  for (auto m : ranges) {
    out << m;
    out << ((c++ < end) ? " x " : "");
  }
  return out;
}

}  // namespace IO

}  // namespace SBG
