/** @file map.hpp

 @brief <b>Map delegate pattern</b>

 A SBG map dom -> exp is an entity composed by a set (domain) and a law.
 Currently only linear expressions are supported. Both the domain and law should
 share the same arity. As such, function notation will be used, i.e. given a map
 m writing m(x) calculates the result of applying m to value x.

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

#ifndef SBG_MAP_HPP
#define SBG_MAP_HPP

#include "sbg/af_set.hpp"
#include "sbg/multidim_lexp.hpp"

namespace SBG {

namespace LIB {

struct Map;

typedef std::optional<Map> MaybeMap;

/**
 * @brief Implementation of maps. Every map has as member a SetAF that keeps
 * track of the chosen implementation for Sets.
 */
struct Map {
  private:
  const SetAF &fact_;

  public:
  member_class(Set, dom);
  member_class(Exp, exp);

  ~Map();

  /**
   * @brief Construct a map with empty domain and expression.
   */
  Map(const SetAF &fact);

  /**
   * @brief Construct a map with a single element \p x in its domain, and with
   * \p exp as its law.
   */
  Map(const SetAF &fact, MD_NAT x, Exp exp);

  /**
   * @brief Construct a map with all the elements of \p i in its domain, and law
   * \p le.
   */
  Map(const SetAF &fact, Interval i, LExp le);

  /**
   * @brief Construct a map with all the elements of \p mdi in its domain, and
   * law \p exp. 
   */
  Map(const SetAF &fact, SetPiece mdi, Exp exp);

  /**
   * @brief Construct a map defining its domain as \p s and law as \p exp.
   */
  Map(const SetAF &fact, Set s, Exp exp);

  bool operator==(const Map &other) const;
  bool operator!=(const Map &other) const;
  Map &operator=(const Map &other);

  /**
   * @brief Calculates the sum of both maps for elements that belong to both
   * domains.
   */
  Map operator+(const Map &other) const;

  // Traditional map operations ------------------------------------------------

  /**
   * @brief Number of dimensions of the elements that compose the mdi. For
   * example, arity([1:1:10]x[1:1:10]) = 2.
   */
  std::size_t arity() const;

  bool isEmpty() const;

  /**
   * @brief Restrict the domain of the map to \p subdom.
   */
  Map restrict(const Set &subdom) const;

  /**
   * @brief Return all the elements that are the image of a value in the domain.
   */
  Set image() const;

  /**
   * @brief Return the image of the map with its domain restricted to \p subdom.
   */
  Set image(const Set &subdom) const;

  /**
   * @brief Calculate the pre image of certain elements of the image.
   *
   * @param subcodom Set of elements in the image of the map for which the
   * pre image will be calculated.
   */
  Set preImage(const Set &subcodom) const;

  /**
   * @brief Calculate the composition of \p this with \p other, i.e.
   * \p this(\p other).
   */
  Map composition(const Map &other) const;

  // Extra operations ----------------------------------------------------------

  /**
   * @brief Calculate pseudo-inverse of a map. If the map is bijective, it
   * returns its inverse. If it's constant, i.e. s -> 0*x+h it returns a new map
   * {h} -> min(s), that is a map from the image to the minimum element of the
   * domain.
   */
  Map minInv() const;

  bool isId() const;

  /**
   * @brief Compact the domain of two maps if both share the same expression. If
   * not, the result isn't a map, so no value is returned.
   */
  MaybeMap compact(const Map &other) const;
};
std::ostream &operator<<(std::ostream &out, const Map &s);

} // namespace LIB

}  // namespace SBG

#endif
