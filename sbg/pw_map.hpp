/** @file pw_map.hpp

 @brief <b>SBG Piecewise Map</b>

 A piecewise map (pw) <<m1, ..., mj>> is a representation for functions
 using collections of domain-disjoint Maps, all sharing the same arity. Maps
 of the collection will be named "pieces" (i.e. m1, ..., mj). The domain of the
 function is the union of all domains of every map. Currently two
 implementations are supported:
   - Unordered PWMaps.
   - Ordered PWMaps, allowing different optimizations.

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

#ifndef SBG_PWMAP_HPP
#define SBG_PWMAP_HPP

#include <forward_list>

#include "sbg/map.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// PWMap Abstract Strategy -----------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

struct PWMapStrategy;

typedef std::unique_ptr<PWMapStrategy> PWMapStratPtr;

struct PWMapStrategy {
  public:
  virtual ~PWMapStrategy() = default;

  /**
   * @brief Constructs an empty pw.
   */
  PWMapStrategy();

  /**
   * @brief Auxiliary function for defining the copy constructor of PWMap.
   */
  virtual PWMapStratPtr clone() const = 0;

  struct Iterator {
    public:
    virtual ~Iterator() = default;
    virtual void operator++() = 0;
    virtual bool operator!=(const Iterator &other) const = 0;
    virtual const Map &operator*() const = 0;
  };

  virtual std::shared_ptr<Iterator> begin() const = 0;
  virtual std::shared_ptr<Iterator> end() const = 0;

  /**
   * @brief Adds a piece to the pw.
   */
  virtual void emplaceBack(const Map &m) = 0;

  /**
   * @brief Two pws are equal if they satisfy the function extensionality
   * principle.
   */
  virtual bool operator==(const PWMapStrategy &other) const = 0;
  virtual bool operator!=(const PWMapStrategy &other) const = 0;
  virtual std::ostream &print(std::ostream &out) const = 0;

  /**
   * @brief Sum of two pws.
   */
  virtual PWMapStratPtr operator+(const PWMapStrategy &other) const = 0;

  // Traditional maps operations -----------------------------------------------

  /**
   * @brief Number of dimensions of the elements that compose the pw. For
   * example, arity(<<{[1:1:10] x [1:1:10]} -> 1*x+0|1*x+0>>) = 2.
   */
  virtual std::size_t arity() const = 0;

  /**
   * @brief Determines if the collection is empty or not.
   */
  virtual bool isEmpty() const = 0;

  /**
   * @brief Domain of the pw, i.e. the union of all domains of every map in the
   * collection.
   */
  virtual Set dom() const = 0;

  /**
   * @brief Restrict the domain of the pw to \p subdom.
   */
  virtual PWMapStratPtr restrict(const Set &subdom) const = 0;

  /**
   * @brief Calculates all possible images for all elements in the domain of
   * the pw.
   */
  virtual Set image() const = 0;

  /**
   * @brief Calculates all possible images for all elements in the domain of
   * the pw restricted to \p subdom.
   */
  virtual Set image(const Set &subdom) const = 0;

  /**
   * @brief Calculates the pre image of certain elements of the image.
   *
   * @param subcodom Set of elements in the image of the map for which the pre
   * image will be calculated.
   */
  virtual Set preImage(const Set &subcodom) const = 0;

  /** 
   * @brief Calculate the inverse of a bijective pw.\n  
   * Precondition: the pw must be bijective.
   */
  virtual PWMapStratPtr inverse() const = 0;

  /**
   * @brief Calculate the composition of \p this with \p other, i.e.
   * \p this(\p other).
   */
  virtual PWMapStratPtr composition(const PWMapStrategy &pw2) const = 0;

  /**
   * @brief First compose the pw with itself \p n times, obtaining pw'. Then,
   * compose pw' with itself up to convergence.
   */
  virtual PWMapStratPtr mapInf(unsigned int n) const = 0;

  /**
   * @brief Compose a map with itself up to convergence.
   */
  virtual PWMapStratPtr mapInf() const = 0;

  /**
   * @brief Calculates the set of elements in the domain such that f(x) = x.
   */
  virtual Set fixedPoints() const = 0;

  // Extra operations ----------------------------------------------------------

  /** 
   * @brief Concatenation of two pws.\n 
   * Precondition: pws should domain-disjoint.
   */
  virtual PWMapStratPtr concatenation(const PWMapStrategy &other) const = 0;

  /**
   * @brief Extend the pw with exclusive elements in the domain of \p other.
   */
  virtual PWMapStratPtr combine(const PWMapStrategy &other) const = 0;

  /** 
   * @brief Calculates (if possible) compactly the result of mapInf.\n  
   *
   * Currently, the only expressions that can be efficiently reduced are:
   *   - x+h
   *   - x-h
   *   - h
   */
  virtual PWMapStratPtr reduce(const Interval &i, const LExp &e) const = 0;
  virtual PWMapStratPtr reduce(const Map &sbgmap) const = 0;
  virtual PWMapStratPtr reduce() const = 0;

  /**
   * @brief For every element in both domains assign the law that returns the
   * minimum value.
   */
  virtual PWMapStratPtr minMap(const PWMapStrategy &other) const = 0;

  /**
   * @brief Given two maps pw1 (\p this) and pw2 (\p other), for every element y1
   * in the image of pw1 returns a pw res such that
   * res(y1) = {min(pw2(x)) : pw1(x) = y1}. In SBG algorithms it is used to
   * calculate for every vertex which of its adjacent vertices returns the
   * minimum value according to pw other.
   */
  virtual PWMapStratPtr minAdjMap(const PWMapStrategy &other) const = 0;

  /** 
   * @brief Pseudo-inverse of a pw restricted to \p subdom. If a value is the
   * image of several elements of the original domain, it will mapped to any
   * of the possible candidates. 
   */
  virtual PWMapStratPtr firstInv(const Set &subdom) const = 0;

  /** 
   * @brief Pseudo-inverse of a pw.
   */
  virtual PWMapStratPtr firstInv() const = 0;

  /**
   * Returns a pw that keeps pieces of the original pw that satisfy the
   * predicate argument.
   */
  virtual PWMapStratPtr filterMap(bool (*f)(const Map &)) const = 0;

  /** 
   * @brief Return elements in both domains, that have the same image in both
   * pws.
   */
  virtual Set equalImage(const PWMapStrategy &other) const = 0;

  /**
   * @brief Returns the set of elements of the domain that have a lesser or
   * equal image in the first argument.
   * For example: lessEqImage({[1:100]} -> x, {[1:100]} -> -x+100) = {[1:50]}.
   */
  virtual Set lessEqImage(const PWMapStrategy& other) const = 0;

  /** 
   * @brief Given a map, return elements of the domain that share its image with
   * other values of the domain.
   */
  virtual Set sharedImage() const = 0;

  /**
   * @brief Sum a constant value to every element in the domain of the pw. The
   * law remains unchanged.
   */
  virtual PWMapStratPtr offsetDom(const MD_NAT &off) const = 0;

  /**
   * @brief Sum the value indicated by pw \p off for every value in the domain
   * of the pw \p this. The law remains unchanged.
   */
  virtual PWMapStratPtr offsetDom(const PWMapStrategy &off) const = 0;

  /**
   * @brief Sum a constant value to every element in the image of the pw, that
   * is, the law of the pw is modified without altering its domain.
   */
  virtual PWMapStratPtr offsetImage(const MD_NAT &off) const = 0;

  /**
   * @brief Sum the expression \p off to every element in the image of the pw,
   * that is, the law is modified without altering its domain.
   */
  virtual PWMapStratPtr offsetImage(const Exp &off) const = 0;

  /**
   * @brief Compact in the same piece all maps that have the same expression.
   */
  virtual PWMapStratPtr compact() const = 0;
};

////////////////////////////////////////////////////////////////////////////////
// PWMap Interface (context) ---------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

struct PWMap {
  private:
  PWMapStratPtr strategy_;

  public:
  PWMap(PWMapStratPtr strat);
  PWMap(const PWMap &other);

  struct Iterator {
    private:
    std::shared_ptr<PWMapStrategy::Iterator> it_;

    public:
    Iterator(std::shared_ptr<PWMapStrategy::Iterator> it);
    void operator++();
    bool operator!=(const PWMap::Iterator &other) const;
    Map operator*() const;
  };

  Iterator begin() const;
  Iterator end() const;

  void emplaceBack(const Map &m);

  bool operator==(const PWMap &other) const;
  bool operator!=(const PWMap &other) const;
  PWMap &operator=(const PWMap &other);
  PWMap &operator=(PWMap &&other);
  std::ostream &print(std::ostream &out) const;

  PWMap operator+(const PWMap &other) const;

  // Traditional map operations ------------------------------------------------

  std::size_t arity() const;
  bool isEmpty() const;
  Set dom() const;
  PWMap restrict(const Set &subdom) const;
  Set image() const;
  Set image(const Set &subdom) const;
  Set preImage(const Set &subcodom) const;
  PWMap inverse() const;
  PWMap composition(const PWMap &other) const;

  PWMap mapInf(unsigned int n) const;
  PWMap mapInf() const;
  Set fixedPoints() const;

  // Extra operations ----------------------------------------------------------

  PWMap concatenation(const PWMap &other) const;
  PWMap combine(const PWMap &other) const;
  PWMap reduce() const;

  PWMap minMap(const PWMap &other) const;
  PWMap minAdjMap(const PWMap &other) const;

  PWMap firstInv(const Set &subdom) const;
  PWMap firstInv() const;

  PWMap filterMap(bool (*f)(const Map &)) const;

  Set equalImage(const PWMap &other) const;
  Set lessEqImage(const PWMap& other) const;
  Set sharedImage() const;

  PWMap offsetDom(const MD_NAT &off) const;
  PWMap offsetDom(const PWMap &off) const;
  PWMap offsetImage(const MD_NAT &off) const;
  PWMap offsetImage(const Exp &off) const;

  PWMap compact() const;
};
std::ostream &operator<<(std::ostream &out, const PWMap &pw);

} // namespace LIB

}  // namespace SBG

#endif
