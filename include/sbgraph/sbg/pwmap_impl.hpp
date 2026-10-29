/** @file pwmap_impl.hpp

 @brief <b>PWMap Implementation</b>

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

#ifndef SBGRAPH_SBG_PWMAP_IMPL_HPP_
#define SBGRAPH_SBG_PWMAP_IMPL_HPP_

#include <iosfwd>

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// PWMap implementations -------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

enum class PWMapKind { kUnordered, kOrdered, kDomOrdered };

std::ostream& operator<<(std::ostream& out, PWMapKind kind);

#define PWMAP_IMPL PWMapImplementation::instance()

/**
 * @brief Singleton that keeps record of the chosen PWMap implementation.
 */
class PWMapImplementation {
public:
  ~PWMapImplementation() = default;

  static PWMapImplementation& instance();

  const PWMapKind& kind() const;  
  void set_pwmap_fact(PWMapKind kind);

private:
  PWMapImplementation();

  PWMapKind _kind;
};

} // namespace LIB

}  // namespace SBG

#endif // SBGRAPH_SBG_PWMAP_IMPL_HPP_
