/** @file set.hpp

 @brief <b>Set delegate pattern</b>

 Compact implementation of sets of multi-dimensional naturals (all with the same
 number of dimensions), using collections of disjoint MDIs. Currently two
 implementations are supported: OrderedDenseSet that can represent sets of
 one dimensional naturals and that is optimized by keeping an ordered collection
 of MDIs; and UnorderedSet that keeps no order, but supports multi-dimensional
 values.

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
// Set Abstract Delegate -------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

struct SetDelegate;

typedef std::unique_ptr<SetDelegate> SetDelegPtr;

struct SetDelegate {
  virtual ~SetDelegate() = default;

  /**
   * @brief Constructs an empty set.
   */
  SetDelegate();

  /**
   * @brief Constructs a set with an unique element \p x.
   */
  SetDelegate(const MD_NAT &x);

  /**
   * @brief Constructs a one-dimensional set with the same elements as \p i.
   */
  SetDelegate(const Interval &i);

  /**
   * @brief Constructs a set with the same elements as \p mdi.
   */
  SetDelegate(const SetPiece &mdi);

  /**
   * @brief Auxiliary function for defining the copy constructor of Set.
   */
  virtual SetDelegPtr clone() const = 0;

  struct Iterator {
    public:
    virtual ~Iterator() = default;
    virtual void operator++() = 0;
    virtual bool operator!=(const Iterator &other) const = 0;
    virtual bool operator==(const Iterator &other) const = 0;
    virtual bool operator<(const Iterator &other) const = 0;
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

  virtual bool operator==(const SetDelegate &other) const = 0;
  virtual bool operator!=(const SetDelegate &other) const = 0;
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
  virtual SetDelegPtr intersection(const SetDelegate &other) const = 0;

  /**
   * @brief Calculates the union of two sets.
   */
  virtual SetDelegPtr cup(const SetDelegate &other) const = 0;

  /**
   * @brief Calculates the complement of a set.\n 
   * Precondition: set must not be empty (undetermined arity).
   */
  virtual SetDelegPtr complement() const = 0;
  virtual SetDelegPtr difference(const SetDelegate &other) const = 0;

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
  virtual SetDelegPtr disjointCup(const SetDelegate &other) const = 0;

  /**
   * @brief Returns a set that keeps pieces of the original set that satisfy
   * the predicate argument.
   */
  virtual SetDelegPtr filterSet(bool (*f)(const SetPiece &mdi)) const = 0;

  /**
   * @brief Sum a constant value to every element of the set.
   */
  virtual SetDelegPtr offset(const MD_NAT &off) const = 0;

  /**
   * @brief Merge as many pieces of the set as possible. Heuristic guided.
   */
  virtual SetDelegPtr compact() const = 0;
};

////////////////////////////////////////////////////////////////////////////////
// Unordered Set Implementation (concrete delegate) ----------------------------
////////////////////////////////////////////////////////////////////////////////

typedef std::vector<SetPiece> MDIUnordSet;

struct UnorderedSet : public SetDelegate {
  member_class(MDIUnordSet, pieces);

  ~UnorderedSet();
  UnorderedSet();
  UnorderedSet(const MD_NAT &x);
  UnorderedSet(const Interval &i);
  UnorderedSet(const SetPiece &mdi);
  UnorderedSet(const MDIUnordSet &pieces);

  SetDelegPtr clone() const override;

  struct Iterator : public SetDelegate::Iterator {
    member_class(MDIUnordSet::const_iterator, it);

    Iterator(MDIUnordSet::const_iterator it);
    void operator++() override;
    bool operator!=(const SetDelegate::Iterator &other) const override;
    bool operator==(const SetDelegate::Iterator &other) const override;
    bool operator<(const SetDelegate::Iterator& other) const override;
    const SetPiece &operator*() const override;
  };

  std::shared_ptr<SetDelegate::Iterator> begin() const override;
  std::shared_ptr<SetDelegate::Iterator> end() const override;

  std::size_t size() const override;
  void emplace(const SetPiece &mdi) override;
  void emplaceBack(const SetPiece &mdi) override;

  bool operator==(const SetDelegate &other) const override;
  bool operator!=(const SetDelegate &other) const override;
  std::ostream &print(std::ostream &out) const override;

  // Traditional set operations ------------------------------------------------

  unsigned int cardinal() const override;
  bool isEmpty() const override;
  MD_NAT minElem() const override;
  MD_NAT maxElem() const override;
  SetDelegPtr intersection(const SetDelegate &other) const override;
  SetDelegPtr cup(const SetDelegate &other) const override;
  SetDelegPtr complement() const;
  SetDelegPtr difference(const SetDelegate &other) const override;

  // Extra operations ----------------------------------------------------------

  std::size_t arity() const override;
  SetDelegPtr disjointCup(const SetDelegate &other) const override;
  SetDelegPtr filterSet(bool (*f)(const SetPiece &mdi)) const override;
  SetDelegPtr offset(const MD_NAT &off) const override;
  SetDelegPtr compact() const override;

  private:
  /**
   * @brief Calculate the complement of an unordered set with a single piece.
   */
  SetDelegPtr complementAtom() const;
};

typedef const UnorderedSet &UnordSetCRef;

////////////////////////////////////////////////////////////////////////////////
// Ordered Dense Set Implementation (concrete delegate) -----------------------
////////////////////////////////////////////////////////////////////////////////

typedef std::vector<SetPiece> MDIOrdSet;

struct OrderedDenseSet : public SetDelegate {
  member_class(MDIOrdSet, pieces);

  ~OrderedDenseSet();
  OrderedDenseSet();
  OrderedDenseSet(MD_NAT x);
  OrderedDenseSet(Interval i);
  OrderedDenseSet(SetPiece mdi);
  OrderedDenseSet(MDIOrdSet pieces);

  SetDelegPtr clone() const override;

  struct Iterator : public SetDelegate::Iterator {
    member_class(MDIOrdSet::const_iterator, it);

    Iterator(MDIOrdSet::const_iterator it);
    void operator++() override;
    bool operator!=(const SetDelegate::Iterator &other) const override;
    bool operator==(const SetDelegate::Iterator &other) const override;
    bool operator<(const SetDelegate::Iterator &other) const override;
    const SetPiece &operator*() const override;
  };

  std::shared_ptr<SetDelegate::Iterator> begin() const override;
  std::shared_ptr<SetDelegate::Iterator> end() const override;

  std::size_t size() const override;
  void emplace(const SetPiece &mdi) override;
  void emplaceBack(const SetPiece &mdi) override;

  bool operator==(const SetDelegate &other) const override;
  bool operator!=(const SetDelegate &other) const override;
  std::ostream &print(std::ostream &out) const override;

  // Traditional set operations ------------------------------------------------

  unsigned int cardinal() const override;
  bool isEmpty() const override;
  MD_NAT minElem() const override;
  MD_NAT maxElem() const override;
  SetDelegPtr intersection(const SetDelegate &other) const override;
  SetDelegPtr cup(const SetDelegate &other) const override;
  SetDelegPtr complement() const;
  SetDelegPtr difference(const SetDelegate &other) const override;

  // Extra operations ----------------------------------------------------------

  std::size_t arity() const override;
  SetDelegPtr disjointCup(const SetDelegate &other) const override;
  SetDelegPtr filterSet(bool (*f)(const SetPiece &mdi)) const override;
  SetDelegPtr offset(const MD_NAT &off) const override;
  SetDelegPtr compact() const override;

  private:
  /**
   * @brief Performs operation f between a piece of s1 and a piece of s2. At the
   * start begins with both minimum elements, and advances the iterator of the
   * set with the piece that has the minimum end. This is repeated until one of
   * the two collections is consumed.
   */
  MDIOrdSet boundedTraverse(SetPiece (SetPiece::*f)(const SetPiece &) const
    , const MDIOrdSet &other) const;

  /**
   * @brief Performs operation f between a piece of s1 and a piece of s2. At the
   * start begins with both minimum elements, and advances the iterator of the
   * set with the piece that has the minimum end. This is repeated until one of
   * the two collections is consumed. Then, all the remaining pieces of the
   * other set are also inserted.
   */
  MDIOrdSet traverse(SetPiece (SetPiece::*f)(const SetPiece &) const
    , const MDIOrdSet &other) const;
};

typedef const OrderedDenseSet &OrdDenseSetCRef;

////////////////////////////////////////////////////////////////////////////////
// Set Implementation (delegator) --------------------------------------
////////////////////////////////////////////////////////////////////////////////

struct Set {
  private:
  SetDelegPtr delegate_;

  public:
  Set(SetDelegPtr deleg);
  Set(const Set &other);

  struct Iterator {
    private:
    std::shared_ptr<SetDelegate::Iterator> it_;

    public:
    Iterator(std::shared_ptr<SetDelegate::Iterator> it);
    void operator++();
    bool operator!=(const Iterator& other) const;
    bool operator==(const Iterator& other) const;
    bool operator<(const Iterator& other) const;
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


////////////////////////////////////////////////////////////////////////////////
// Set Hashes ------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
struct SetPieceHash {
  static std::size_t set_piece_hash(const SetPiece& set_piece);  
  std::size_t operator()(const SetPiece& set_piece) const;
};

struct SetHash {
  std::size_t operator()(const Set& set) const;
};


} // namespace LIB

}  // namespace SBG

#endif
