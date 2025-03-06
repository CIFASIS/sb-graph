/** @file set.hpp

 @brief <b>Set delegate pattern</b>

 A piecewise multi-dimensional interval is a collection of non-empty disjoint
 intervals, and its corresponding set is the union of all composing mdis.
 As multiple implementations were developed, a need to pick the desired
 implementation arised. It was then decided that a Delegate pattern was the
 right choice to implement this feature. This file contains the interface
 classes, while each concrete implementation is in a separate file.

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

#include <algorithm>
#include <memory>
#include <unordered_set>

#include <boost/container/flat_set.hpp>

#include "sbg/multidim_inter.hpp"

namespace SBG {

namespace LIB {

// Set Abstract Delegate -------------------------------------------------------

struct SetDelegate;

typedef std::unique_ptr<SetDelegate> SetDelegPtr;

struct SetDelegate {
  virtual ~SetDelegate() = default;

  struct Iterator {
    public:
    virtual ~Iterator() = default;
    virtual void operator++() = 0;
    virtual bool operator!=(const Iterator &other) const = 0;
    virtual const SetPiece &operator*() const = 0;
  };

  virtual std::shared_ptr<Iterator> begin() const = 0;
  virtual std::shared_ptr<Iterator> end() const = 0;

  virtual std::size_t size() const = 0;
  virtual void emplace(const SetPiece &mdi) = 0;
  virtual void emplaceBack(const SetPiece &mdi) = 0;

  virtual bool operator==(const SetDelegate &other) const = 0;
  virtual bool operator!=(const SetDelegate &other) const = 0;
  virtual bool operator<(const SetDelegate &other) const = 0;
  virtual std::ostream &print(std::ostream &out) const = 0;

  /**
   * @brief Traditional set operations.
   */
  virtual unsigned int cardinal() const = 0;
  virtual bool isEmpty() const = 0;
  virtual Util::MD_NAT minElem() const = 0;
  virtual Util::MD_NAT maxElem() const = 0;
  virtual SetDelegPtr intersection(const SetDelegate &other) const = 0;
  virtual SetDelegPtr cup(const SetDelegate &other) const = 0;
  virtual SetDelegPtr complement() const = 0;
  virtual SetDelegPtr difference(const SetDelegate &other) const = 0;

  /**
   * @brief Extra operations.
   */
  virtual std::size_t arity() const = 0;
  virtual SetDelegPtr disjointCup(const SetDelegate &other) const = 0;
  virtual SetDelegPtr filterSet(bool (*f)(const SetPiece &mdi)) const = 0;
  virtual SetDelegPtr offset(const Util::MD_NAT &off) const = 0;
  virtual SetDelegPtr compact() const = 0;
};

// Unordered Set Implementation (concrete delegate) ----------------------------

typedef std::vector<SetPiece> MDIUnordSet;

struct UnorderedSet : public SetDelegate {
  member_class(MDIUnordSet, pieces);

  ~UnorderedSet();
  UnorderedSet();
  UnorderedSet(const Util::MD_NAT &x);
  UnorderedSet(const Interval &i);
  UnorderedSet(const SetPiece &mdi);
  UnorderedSet(const MDIUnordSet &pieces);

  struct Iterator : public SetDelegate::Iterator {
    member_class(MDIUnordSet::const_iterator, it);

    Iterator(MDIUnordSet::const_iterator it);
    void operator++() override;
    bool operator!=(const SetDelegate::Iterator &other) const override;
    const SetPiece &operator*() const override;
  };

  std::shared_ptr<SetDelegate::Iterator> begin() const override;
  std::shared_ptr<SetDelegate::Iterator> end() const override;

  std::size_t size() const override;
  void emplace(const SetPiece &mdi) override;
  void emplaceBack(const SetPiece &mdi) override;

  bool operator==(const SetDelegate &other) const override;
  bool operator!=(const SetDelegate &other) const override;
  bool operator<(const SetDelegate &other) const override;
  std::ostream &print(std::ostream &out) const override;

  /**
   * @brief Traditional set operations.
   */
  unsigned int cardinal() const override;
  bool isEmpty() const override;
  Util::MD_NAT minElem() const override;
  Util::MD_NAT maxElem() const override;
  SetDelegPtr intersection(const SetDelegate &other) const override;
  SetDelegPtr cup(const SetDelegate &other) const override;
  SetDelegPtr complement() const;
  SetDelegPtr difference(const SetDelegate &other) const override;

  /**
   * @brief Extra operations.
   */
  std::size_t arity() const override;
  SetDelegPtr disjointCup(const SetDelegate &other) const override;
  SetDelegPtr filterSet(bool (*f)(const SetPiece &mdi)) const override;
  SetDelegPtr offset(const Util::MD_NAT &off) const override;
  SetDelegPtr compact() const override;

  private:
  UnorderedSet complementAtom() const;
};

typedef const UnorderedSet &UnordSetCRef;

// Ordered Dense Set Implementation (concrete delegate) -----------------------

typedef boost::container::flat_set<SetPiece> MDIOrdSet;

struct OrderedDenseSet : public SetDelegate {
  member_class(MDIOrdSet, pieces);

  ~OrderedDenseSet();
  OrderedDenseSet();
  OrderedDenseSet(Util::MD_NAT x);
  OrderedDenseSet(Interval i);
  OrderedDenseSet(SetPiece mdi);
  OrderedDenseSet(MDIOrdSet pieces);

  struct Iterator : public SetDelegate::Iterator {
    member_class(MDIOrdSet::const_iterator, it);

    Iterator(MDIOrdSet::const_iterator it);
    void operator++() override;
    bool operator!=(const SetDelegate::Iterator &other) const override;
    const SetPiece &operator*() const override;
  };

  std::shared_ptr<SetDelegate::Iterator> begin() const override;
  std::shared_ptr<SetDelegate::Iterator> end() const override;

  std::size_t size() const override;
  void emplace(const SetPiece &mdi) override;
  void emplaceBack(const SetPiece &mdi) override;

  bool operator==(const SetDelegate &other) const override;
  bool operator!=(const SetDelegate &other) const override;
  bool operator<(const SetDelegate &other) const override;
  std::ostream &print(std::ostream &out) const override;

  /**
   * @brief Traditional set operations.
   */
  unsigned int cardinal() const override;
  bool isEmpty() const override;
  Util::MD_NAT minElem() const override;
  Util::MD_NAT maxElem() const override;
  SetDelegPtr intersection(const SetDelegate &other) const override;
  SetDelegPtr cup(const SetDelegate &other) const override;
  SetDelegPtr complement() const;
  SetDelegPtr difference(const SetDelegate &other) const override;

  /**
   * @brief Extra operations.
   */
  std::size_t arity() const override;
  SetDelegPtr disjointCup(const SetDelegate &other) const override;
  SetDelegPtr filterSet(bool (*f)(const SetPiece &mdi)) const override;
  SetDelegPtr offset(const Util::MD_NAT &off) const override;
  SetDelegPtr compact() const override;

  private:
  MDIOrdSet boundedTraverse(SetPiece (SetPiece::*f)(const SetPiece &) const
    , const MDIOrdSet &other) const;
  MDIOrdSet traverse(SetPiece (SetPiece::*f)(const SetPiece &) const
    , const MDIOrdSet &other) const;
};

typedef const OrderedDenseSet &OrdDenseSetCRef;

// Set Implementation (delegator) --------------------------------------

struct Set {
  private:
  SetDelegPtr delegate_;

  public:
  Set(SetDelegPtr deleg);

  struct Iterator {
    private:
    std::shared_ptr<SetDelegate::Iterator> it_;
    public:
    Iterator(std::shared_ptr<SetDelegate::Iterator> it);
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
  bool operator<(const Set &other) const;
  std::ostream &print(std::ostream &out) const;

  /**
   * @brief Traditional set operations.
   */
  unsigned int cardinal() const;
  bool isEmpty() const;
  Util::MD_NAT minElem() const;
  Util::MD_NAT maxElem() const;
  Set intersection(const Set &other) const;
  Set cup(const Set &other) const;
  Set complement() const;
  Set difference(const Set &other) const;

  /**
   * @brief Extra operations.
   */
  std::size_t arity() const;
  Set disjointCup(const Set &other) const;
  Set filterSet(bool (*f)(const SetPiece &mdi)) const;
  Set offset(const Util::MD_NAT &off) const;
  Set compact() const;
};
std::ostream &operator<<(std::ostream &out, const Set &s);


} // namespace LIB

}  // namespace SBG

#endif
