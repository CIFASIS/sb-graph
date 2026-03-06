/** @file map_detail.hpp

 @brief <b>Map implementation details</b>

 This module defines some functions that work both with the implementation of
 Interval, SetPiece, LinearExpr (that don't belong to the public interface),
 and also Expression, which belongs to the public interface, but exposes its
 implementations details only to these functions.

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

#ifndef SBGRAPH_SBG_MAP_DETAIL_HPP_
#define SBGRAPH_SBG_MAP_DETAIL_HPP_

#include "sbg/expression_impl.hpp"
#include "sbg/interval.hpp"
#include "sbg/linear_expr.hpp"
#include "sbg/map.hpp"
#include "sbg/multidim_inter.hpp"

namespace SBG {

namespace LIB {

namespace detail {

using AtomicMap = std::pair<Interval, LinearExpr>;
using AtomicMapVector = std::vector<AtomicMap>;
using MapVector = std::vector<Map>;

class MapDetail {
public:
  static Set image(const Set& s, const Expression& expr);
  static Set preImage(const Set& s, const Expression& expr);
  static Set lessImage(const Expression& expr1, const Expression& expr2);
  static MapVector reduce(const Map& m);

private:
  template<typename SetMDIImpl>
  static MapVector MDICollectionReduce(const SetMDIImpl& s
    , const ExpressionImpl& expr);
  static MapVector reduce(const OrdUnidimDenseSet& s
    , const ExpressionImpl& expr);
};

template<class... Ts>
class Overload : Ts... {
public:
  using Ts::operator()...;
  Overload(Ts... ts) : Ts(ts)... {};
};

template<class... Ts>
Overload(Ts...) -> Overload<Ts...>;

} // namespace detail

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_SBG_MAP_DETAIL_HPP_
