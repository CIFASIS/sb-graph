/** @file pw_map.hpp

 @brief <b>SBG Piecewise Map</b>

 A piecewise map (pw) <<m1, ..., mj>> is a representation for functions
 using collections of domain-disjoint Maps, all sharing the same arity. Maps
 of the collection will be named "pieces" (i.e. m1, ..., mj). The domain of the
 function is the union of all domains of every map. Currently three
 implementations are supported:
   - Unordered PWMaps.
   - Ordered PWMaps, that saves comparisons between maps in some operations.
   - Domain ordered PWMaps, that have an ordered set implementation as domain,
     and also order their collection of maps accordingly.

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

#ifndef SBGRAPH_SBG_PW_MAP_HPP_
#define SBGRAPH_SBG_PW_MAP_HPP_

#include "sbg/map.hpp"
#include "sbg/set.hpp"
#include "sbg/unord_pwmap.hpp"

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// PWMaps implementations ------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

using PWMapImpl = std::variant<UnordPWMap>;

} // namespace detail

enum class PWMapKind { kUnordered, kOrdered, kDomOrdered };

std::ostream& operator<<(std::ostream& out, PWMapKind kind);

////////////////////////////////////////////////////////////////////////////////
// PWMap -----------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

class PWMap {
public:
  using ConstIt = std::vector<Map>::const_iterator;

  /**
   * @brief Constructs an empty domain pw.
   */
  PWMap(const PWMapKind kind);
  PWMap(const PWMapKind kind, Set s);
  PWMap(const PWMapKind kind, Map m);

  ConstIt begin();
  ConstIt end();

  template<typename... Args>
  void emplace(Args&&... args);
  /**
   * @brief Adds a piece to the pw.
   * Precondition: \p m domain should not be empty, and should have no
   * intersection with the current domain of the PWMap.
   */
  void insert(const Map& m);
  void insert(Map&& m);

  /**
   * @brief Two pws are equal if they satisfy the function extensionality
   * principle.
   */
  bool operator==(const PWMap& other) const;
  bool operator!=(const PWMap& other) const;
  PWMap operator+(const PWMap& other) const;
  std::ostream& print(std::ostream& out) const;

  // Traditional maps operations -----------------------------------------------

  /**
   * @brief Number of dimensions of the elements that compose the pw. For
   * example, arity(<<{[1:1:10] x [1:1:10]} -> 1*x+0|1*x+0>>) = 2.
   */
  std::size_t arity() const;

  /**
   * @brief Determines if the collection is empty or not.
   */
  bool isEmpty() const;

  /**
   * @brief Domain of the pw, i.e. the union of all domains of every map in the
   * collection.
   */
  Set domain() const &;
  Set domain() &&;

  /**
   * @brief Restrict the domain of the pw to \p subdom.
   */
  PWMap restrict(const Set& subdom) const;

  /**
   * @brief Calculates all possible images for all elements in the domain of
   * the pw.
   */
  Set image() const;

  /**
   * @brief Calculates all possible images for all elements in the domain of
   * the pw restricted to \p subdom.
   */
  Set image(const Set& subdom) const;

  /**
   * @brief Calculates the pre image of certain elements of the image.
   *
   * @param subcodom Set of elements in the image of the map for which the pre
   * image will be calculated.
   */
  Set preImage(const Set& subcodom) const;

  /** 
   * @brief Calculate the inverse of a bijective pw.\n  
   * Precondition: the pw must be bijective.
   */
  PWMap inverse() const;

  /**
   * @brief Calculate the composition of \p this with \p other, i.e.
   * \p this(\p other).
   */
  PWMap composition(const PWMap& pw2) const;

  /**
   * @brief Compose a map with itself up to convergence.
   */
  PWMap mapInf() const;

  /**
   * @brief Calculates the set of elements in the domain such that f(x) = x.
   */
  Set fixedPoints() const;

  // Extra operations ----------------------------------------------------------

  /** 
   * @brief Concatenation of two pws.\n 
   * Precondition: pws should domain-disjoint.
   */
  PWMap concatenation(const PWMap& other) const &;
  PWMap concatenation(const PWMap& other) &&;
  PWMap concatenation(PWMap&& other) const &;
  PWMap concatenation(PWMap&& other) &&;

  /**
   * @brief Extend the pw with exclusive elements in the domain of \p other.
   */
  PWMap combine(const PWMap& other) const &;
  PWMap combine(const PWMap& other) &&;
  PWMap combine(PWMap&& other) const &;
  PWMap combine(PWMap&& other) &&;

  /**
   * @brief For every element in both domains assign the law that returns the
   * minimum value.
   */
  PWMap min(const PWMap& other) const;

  /**
   * @brief Given two maps pw1 (\p this) and pw2 (\p other), for every element y1
   * in the image of pw1 returns a pw res such that
   * res(y1) = {min(pw2(x)) : pw1(x) = y1}. In SBG algorithms it is used to
   * calculate for every vertex which of its adjacent vertices returns the
   * minimum value according to pw \p other.
   */
  PWMap minAdj(const PWMap& other) const;

  /** 
   * @brief Given a map, return elements of the domain that share its image with
   * other values of the domain.
   */
  Set sharedImage() const;

  /** 
   * @brief Return elements in both domains, that have the same image in both
   * pws.
   */
  Set equalImage(const PWMap& other) const;

  /**
   * @brief Returns the set of elements of the domain that have a lesser
   * image in the first argument.
   * For example: lessImage({[1:100]} -> x, {[1:100]} -> -x+100) = {[1:49]}.
   */
  Set lessImage(const PWMap& other) const;

  /**
   * @brief Minimize internal representation cost. Heuristic guided.
   */
  void compact();

private:
  PWMap(const detail::PWMapImpl& impl);
  PWMap(detail::PWMapImpl&& impl);

  detail::PWMapImpl _impl;
};

std::ostream& operator<<(std::ostream& out, const PWMap& pw);

template<typename... Args>
inline void PWMap::emplace(Args&&... args)
{
  std::visit([&](auto& a) { a.emplace(std::forward<Args>(args)...); } , _impl);
}

} // namespace LIB

}  // namespace SBG

#endif // SBGRAPH_SBG_PW_MAP_HPP_
