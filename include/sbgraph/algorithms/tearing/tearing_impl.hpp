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

#ifndef SBGRAPH_ALGORITHMS_TEARING_TEARING_IMPL_HPP_
#define SBGRAPH_ALGORITHMS_TEARING_TEARING_IMPL_HPP_

#include <iosfwd>

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Tearing implementations -----------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

enum class TearingKind { kTearingV1 };

std::ostream& operator<<(std::ostream& out, const TearingKind kind);

#define TEARING_IMPL TearingImplementation::instance()

/**
 * @brief Singleton that keeps record of the chosen SCC implementation. 
 */
class TearingImplementation {
public:
  ~TearingImplementation() = default;

  static TearingImplementation& instance();

  const TearingKind& kind() const;
  void set_tearing_fact(TearingKind kind);

private:
  TearingImplementation();

  TearingKind _kind;
};

} // namespace LIB

}  // namespace SBG

#endif // SBGRAPH_ALGORITHMS_TEARING_TEARING_IMPL_HPP_
