/** @file matching_impl.hpp

 @brief <b>Matching Algorithm Implementation</b>

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

#ifndef SBGRAPH_ALGORITHMS_MATCHING_MATCHING_IMPL_HPP_
#define SBGRAPH_ALGORITHMS_MATCHING_MATCHING_IMPL_HPP_

#include <iosfwd>

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Matching implementations ----------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

enum class MatchKind { kBFSPaths };

std::ostream& operator<<(std::ostream& out, const MatchKind kind);

#define MATCH_IMPL MatchImplementation::instance()

/**
 * @brief Singleton that keeps record of the chosen matching implementation.
 */
class MatchImplementation {
public:
  ~MatchImplementation() = default;

  static MatchImplementation& instance();

  const MatchKind& kind() const;
  void set_match_fact(MatchKind kind);

private:
  MatchImplementation();

  MatchKind _kind;
};

} // namespace LIB

}  // namespace SBG

#endif // SBGRAPH_ALGORITHMS_MATCHING_MATCHING_IMPL_HPP_
