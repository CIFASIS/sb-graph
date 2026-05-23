/** @file scc_impl.hpp

 @brief <b>SCC Algorithm Implementation</b>

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

#ifndef SBGRAPH_ALGORITHMS_SCC_SCC_IMPL_HPP_
#define SBGRAPH_ALGORITHMS_SCC_SCC_IMPL_HPP_

#include <iosfwd>

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// SCC implementations ---------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

enum class SCCKind { kMinReachV1, kMinReachV2 };

std::ostream& operator<<(std::ostream& out, const SCCKind kind);

#define SCC_IMPL SCCImplementation::instance()

/**
 * @brief Singleton that keeps record of the chosen SCC implementation. 
 */
class SCCImplementation {
public:
  ~SCCImplementation() = default;

  static SCCImplementation& instance();

  const SCCKind& kind() const;
  void set_scc_fact(SCCKind kind);

private:
  SCCImplementation();

  SCCKind _kind;
};

} // namespace LIB

}  // namespace SBG

#endif // SBGRAPH_ALGORITHMS_SCC_SCC_IMPL_HPP_
