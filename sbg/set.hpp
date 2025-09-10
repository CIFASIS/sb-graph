/** @file set.hpp

 @brief <b>SBG Set</b>

 A SBG Set is a structure that represents sets of multi-dimensional naturals
 (all with the same number of dimensions), using collections of disjoint MDIs.
 Currently three implementations are supported:
   - UnorderedSet that keeps no order, but supports multi-dimensional values.
   - OrderedSet that supports multi-dimensional values while also keeping an
     internal order that enhances performance.
   - OrdUnidimDenseSet that can represent sets of one dimensional naturals and
     that is optimized by keeping an ordered collection of MDIs.

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

#ifndef SBG_SET_HPP
#define SBG_SET_HPP

#include <memory>

#include "sbg/multidim_inter.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Set Abstract Strategy -------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

struct SetStrategy;

typedef std::unique_ptr<SetStrategy> SetStratPtr;

struct SetStrategy {
  virtual ~SetStrategy() = default;

  /**
   * @brief Constructs an empty set.
   */
  SetStrategy();

  /**
   * @brief Constructs a set with an unique element \p x.
   */
  SetStrategy(const MD_NAT &x);

  /**
   * @brief Constructs a one-dimensional set with the same elements as \p i.
   */
  SetStrategy(const Interval &i);

  /**
   * @brief Constructs a set with the same elements as \p mdi.
   */
  SetStrategy(const SetPiece &mdi);

  /**
   * @brief Auxiliary function for defining the copy constructor of Set.
   */
  virtual SetStratPtr clone() const = 0;

  struct Iterator {
    public:
    virtual ~Iterator() = default;
    virtual void operator++() = 0;
    virtual bool operator!=(const Iterator &other) const = 0;
    virtual const SetPiece &operator*() const = 0;
  };

  virtual std::shared_ptr<Iterator> begin() const = 0;
  virtual std::shared_ptr<Iterator> end() const = 0;

  /**
   * @brief Number of compact pieces in the set, i.e.
   * size({[1:1:10], [20:3:50]}) = 2.
   */
  virtual std::size_t size() const = 0;

  /**
   * @brief Adds a compact piece to the set.
   *
   * Complexity: O(size(\p this)) for ordered collections and O(1) for unordered
   * collections.
   */
  virtual void emplace(const SetPiece &mdi) = 0;

  /**
   * @brief Used specially by operations of ordered collections that keep
   * the result in order, that have to insert at the end. 
   *
   * Complexity: O(1) for ordered and unordered collections.
   */
  virtual void emplaceBack(const SetPiece &mdi) = 0;

  virtual bool operator==(const SetStrategy &other) const = 0;
  virtual bool operator!=(const SetStrategy &other) const = 0;
  virtual std::ostream &print(std::ostream &out) const = 0;

  // Traditional set operations ------------------------------------------------

  /**
   * @brief Number of elements contained in the set, i.e.
   * cardinal({[1:1:10]x[1:1:10], [101:1:200]x[201:1:300]}) = 10100. 
   */
  virtual unsigned int cardinal() const = 0;
  virtual bool isEmpty() const = 0;
  virtual MD_NAT minElem() const = 0;
  virtual MD_NAT maxElem() const = 0;
  virtual SetStratPtr intersection(const SetStrategy &other) const = 0;

  /**
   * @brief Calculates the union of two sets.
   */
  virtual SetStratPtr cup(const SetStrategy &other) const = 0;

  /**
   * @brief Calculates the complement of a set.\n 
   * Precondition: set must not be empty (undetermined arity).
   */
  virtual SetStratPtr complement() const = 0;
  virtual SetStratPtr difference(const SetStrategy &other) const = 0;

  // Extra operations ----------------------------------------------------------

  /**
   * @brief Number of dimensions of the elements that compose the set. For
   * example, arity([1:1:10]x[1:1:10]) = 2.
   */
  virtual std::size_t arity() const = 0;

  /**
   * @brief Calculates the union of two disjoint sets. \n 
   * Precondition: this->intersection(other) = {}.
   */
  virtual SetStratPtr disjointCup(const SetStrategy &other) const = 0;

  /**
   * @brief Returns a set that keeps pieces of the original set that satisfy
   * the predicate argument.
   */
  virtual SetStratPtr filterSet(bool (*f)(const SetPiece &mdi)) const = 0;

  /**
   * @brief Sum a constant value to every element of the set.
   */
  virtual SetStratPtr offset(const MD_NAT &off) const = 0;

  /**
   * @brief Merge as many pieces of the set as possible. Heuristic guided.
   */
  virtual SetStratPtr compact() const = 0;
};

////////////////////////////////////////////////////////////////////////////////
// Set Interface (context) -----------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

struct Set {
  private:
  SetStratPtr strategy_;

  public:
  Set(SetStratPtr strat);
  Set(const Set &other);

  struct Iterator {
    private:
    std::shared_ptr<SetStrategy::Iterator> it_;

    public:
    Iterator(std::shared_ptr<SetStrategy::Iterator> it);
    void operator++();
    bool operator!=(const Iterator& other) const;
    SetPiece operator*() const;
  };

  Iterator begin() const;
  Iterator end() const;

  std::size_t size() const;
  void emplace(SetPiece mdi);
  void emplaceBack(SetPiece mdi);

  bool operator==(const Set &other) const;
  bool operator!=(const Set &other) const;
  Set &operator=(const Set &other);
  Set &operator=(Set &&other);
  std::ostream &print(std::ostream &out) const;

  // Traditional set operations ------------------------------------------------

  unsigned int cardinal() const;
  bool isEmpty() const;
  MD_NAT minElem() const;
  MD_NAT maxElem() const;
  Set intersection(const Set &other) const;
  Set cup(const Set &other) const;
  Set complement() const;
  Set difference(const Set &other) const;

  // Extra operations ----------------------------------------------------------

  std::size_t arity() const;
  Set disjointCup(const Set &other) const;
  Set filterSet(bool (*f)(const SetPiece &mdi)) const;
  Set offset(const MD_NAT &off) const;
  Set compact() const;
};
std::ostream &operator<<(std::ostream &out, const Set &s);


} // namespace LIB

}  // namespace SBG

#endif
