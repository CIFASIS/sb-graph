/** @file env.hpp

 @brief <b>Environments</b>

 Environment to save values of variables.

 <hr>

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

#ifndef ENV_UTIL_HPP
#define ENV_UTIL_HPP

#include <iostream>
#include <map>
#include <string>

#include <util/defs.hpp>

namespace SBG {

namespace Util {

template <typename Key, typename Value>
struct SymbolTable : public std::map<Key, Value> {
  void insert(Key k, Value v)
  {
    std::map<Key, Value>::erase(k);
    std::map<Key, Value>::insert(std::pair<Key, Value>(k, v));
  }
  Option<Value> operator[](Key k) const
  {
    if (std::map<Key, Value>::find(k) == std::map<Key, Value>::end()) return Option<Value>();
    return std::map<Key, Value>::at(k);
  }
  void remove(Key k) { std::map<Key, Value>::erase(k); }
  void dump()
  {
    typename std::map<Key, Value>::iterator it;
    for (it = std::map<Key, Value>::begin(); it != std::map<Key, Value>::end(); it++) {
      std::cerr << it->first << ":" /*<< it->second */ << "\n";
    }
  }
};

} // namespace Util

} // namespace SBG

#endif
