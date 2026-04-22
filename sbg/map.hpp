/** @file map.hpp

 @brief <b>SBG Map</b>

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

#ifndef SBGRAPH_SBG_MAP_HPP_
#define SBGRAPH_SBG_MAP_HPP_

#include "sbg/natural.hpp"
#include "sbg/expression.hpp"
#include "sbg/set.hpp"

#include <iosfwd>
#include <optional>
#include <vector>

namespace SBG {

namespace LIB {

class Map;

using MaybeMap = std::optional<Map>;

class Map {
public:
  /**
   * @brief Construct a map with empty domain and expression.
   */
  Map();

  /**
   * @brief Construct a map with a single element \p x in its domain, and with
   * \p exp as its law.
   */
  Map(const MD_NAT& x, const Expression& expr);
  Map(MD_NAT&& x, Expression&& expr);

  /**
   * @brief Construct a map defining its domain as \p s and law as \p exp.
   */
  Map(const Set& s, const Expression& expr);
  Map(Set&& s, Expression&& expr);

  const Set& domain() const &;
  Set domain() &&;
  const Expression& law() const;

  bool operator==(const Map& other) const;
  bool operator!=(const Map& other) const;
  
  /**
   * @brief Calculates the sum of both maps for elements that belong to both
   * domains.
   */
  Map operator+(const Map& other) const;

  // Traditional map operations ------------------------------------------------

  /**
   * @brief Number of dimensions of the elements that compose the domain of the
   * map. For example, arity([1:1:10]x[1:1:10]) = 2.
   */
  std::size_t arity() const;

  bool isEmpty() const;

  /**
   * @brief Restrict the domain of the map to \p subdom.
   */
  Map restrict(const Set& subdom) const &;
  Map restrict(const Set& subdom) &&;
  Map restrict(Set&& subdom) const &;

  /**
   * @brief Return all the elements that are the image of a value in the domain.
   */
  Set image() const;

  /**
   * @brief Return the image of the map with its domain restricted to \p subdom.
   */
  Set image(const Set& subdom) const;

  /**
   * @brief Calculate the pre image of certain elements of the image.
   *
   * @param subcodom Set of elements in the image of the map for which the
   * pre image will be calculated.
   */
  Set preImage(const Set& subcodom) const; // opt

  /**
   * @brief Calculate the composition of \p this with \p other, i.e.
   * \p this(\p other).
   */
  Map composition(const Map& other) const;

  /**
   * @brief Calculates the set of elements in the domain such that f(x) = x.
   */
  Set fixedPoints() const;

  // Extra operations ----------------------------------------------------------

  bool isId() const;

  /**
   * @brief Returns the set of elements of the domain that have a lesser
   * image in the first argument.
   * For example: lessImage({[1:100]} -> x, {[1:100]} -> -x+100) = {[1:49]}.
   */
  Set lessImage(const Map& other) const;

  /**
   * @brief Given two maps m1 (\p this) and m2 (\p other), for every element
   * y in the image of m1 returns a map result such that
   * result(y) = {min(m2(x)) : m1(x) = y}.
   */
  Map minAdj(const Map& other) const;

  /**
   * @brief If it is convenient calculates the result of composing the map with
   * itself until the image is out of the domain (without actually composing the
   * map). For example, reduce({[1:1:100]} -> x+1) = {[1:1:100]} -> 101.
   */
  std::vector<Map> reduce() const;

  /**
   * @brief Calculates the multiplicity for each element of the image, i.e. the
   * number of pre-images of each one.
   */
  std::vector<Map> imageMultiplicity() const;

  /**
   * @brief Minimize internal representation cost. Heuristic guided.
   * Precondition: maps should domain disjoint. 
   */
  MaybeMap compact(const Map& other) const;

private:  
  Set _domain;
  Expression _law;
};
std::ostream& operator<<(std::ostream& out, const Map& s);

} // namespace LIB

}  // namespace SBG

#endif // SBGRAPH_SBG_MAP_HPP_
