/** @file user_impl_map.hpp

 @brief <b>Mapping from the user input to implementations</b>

 This module keeps track of the method to map the user input to the selected
 implementation. So, it doesn't keep track of the state of the user input, it
 just indicates what implementation will be used for each possible value the
 user can input. 

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

#ifndef EVAL_USER_IMPL_MAP_HPP
#define EVAL_USER_IMPL_MAP_HPP

#include <unordered_map>
#include <variant>

#include "algorithms/cutvertex/cv_fact.hpp"
#include "algorithms/matching/matching_fact.hpp"
#include "algorithms/scc/scc_fact.hpp"
#include "algorithms/toposort/ts_fact.hpp"
#include "sbg/af_pwmap.hpp"
#include "sbg/af_set.hpp"

namespace SBG {

namespace Eval {

////////////////////////////////////////////////////////////////////////////////
// User input to implementation map --------------------------------------------
////////////////////////////////////////////////////////////////////////////////

#define IMPL_MAP UserImplMap::instance()

/**
 * @brief Type defined to be used in ImplEnv.
 */
using ImplFactory = std::variant<LIB::SetFactPtr, LIB::PWMapFactPtr
  , LIB::MatchFactPtr, LIB::SCCFactPtr, LIB::TSFactPtr, LIB::CVFactPtr>;

/**
 * @brief Mapping for all structures such as Set, PW and algorithms.
 */
class UserImplMap {
  public:
  /**
   * @brief Mapping of a single structure. For example, it maps numbers to the
   * different Set implementations.
   */
  class StructImplMap {
    public:
    using SKey = int;
    using SValue = std::function<ImplFactory()>;
    using SType = std::unordered_map<SKey, SValue>;
  
    StructImplMap();
  
    SValue& operator[](const SKey& key);
    const SValue& operator[](const SKey& key) const;
    void freeze();
  
    private:
    bool is_frozen_;
    SType struct_impls_;
  };

  using IKey = std::string;
  using IValue = StructImplMap;
  using IType = std::unordered_map<IKey, IValue>;

  static UserImplMap& instance() {
    static UserImplMap instance_;
    return instance_;
  }
  ImplFactory getFactory(std::string strct, int impl);

  private: 
  UserImplMap();

  IType implementations_;
};

////////////////////////////////////////////////////////////////////////////////
// Extra operations ------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

void setSetFactory(int set_impl);

} // namespace Eval

} // namespace SBG

#endif
