/** @file pw_map.hpp

 @brief <b>Piecewise map implementation</b>

 A piecewise map (pw) <<m1, ..., mj>> is the implementation of compact functions
 using collections of domain-disjoint Maps, all sharing the same arity. Maps
 of the collection will be named "pieces" (i.e. m1, ..., mj). The domain of the
 function is the union of all domains of every map. Currently only one
 implementation is supported: UnorderedPW.

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

#include <cstdlib> // para std::exit y EXIT_FAILURE
#include <forward_list>
#include <iostream>
#include "sbg/af_map.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// PWMap Abstract Delegate -----------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

struct PWMapDelegate;

typedef std::unique_ptr<PWMapDelegate> PWMapDelegPtr;

struct PWMapDelegate {
  protected:
  const MapAF &fact_;

  public:
  virtual ~PWMapDelegate() = default;

  /**
   * @brief Constructs an empty pw.
   */
  PWMapDelegate(const MapAF &fact);

  /**
   * @brief Auxiliary function for defining the copy constructor of PWMap.
   */
  virtual PWMapDelegPtr clone() const = 0;

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
  virtual bool operator==(const PWMapDelegate &other) const = 0;
  virtual bool operator!=(const PWMapDelegate &other) const = 0;
  virtual std::ostream &print(std::ostream &out) const = 0;

  /**
   * @brief Sum of two pws.
   */
  virtual PWMapDelegPtr operator+(const PWMapDelegate &other) const = 0;

  /** 
   * @brief Bounded subtraction of pws. If for same value the result is
   * negative, it is replace by 0.
   */
  virtual PWMapDelegPtr operator-(const PWMapDelegate &other) const = 0;

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
  virtual PWMapDelegPtr restrict(const Set &subdom) const = 0;

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
  virtual PWMapDelegPtr inverse() const = 0;

  /**
   * @brief Calculate the composition of \p this with \p other, i.e.
   * \p this(\p other).
   */
  virtual PWMapDelegPtr composition(const PWMapDelegate &pw2) const = 0;

  /**
   * @brief First compose the pw with itself \p n times, obtaining pw'. Then,
   * compose pw' with itself up to convergence.
   */
  virtual PWMapDelegPtr mapInf(unsigned int n) const = 0;

  /**
   * @brief Compose a map with itself up to convergence.
   */
  virtual PWMapDelegPtr mapInf() const = 0;

  // Extra operations ----------------------------------------------------------

  /** 
   * @brief Concatenation of two pws.\n 
   * Precondition: pws should domain-disjoint.
   */
  virtual PWMapDelegPtr concatenation(const PWMapDelegate &other) const = 0;

  /**
   * @brief Extend the pw with exclusive elements in the domain of \p other.
   */
  virtual PWMapDelegPtr combine(const PWMapDelegate &other) const = 0;

  /** 
   * @brief Calculates (if possible) compactly the result of mapInf.\n  
   *
   * Currently, the only expressions that can be efficiently reduced are:
   *   - x+h
   *   - x-h
   *   - h
   */
  virtual PWMapDelegPtr reduce(const Interval &i, const LExp &e) const = 0;
  virtual PWMapDelegPtr reduce(const Map &sbgmap) const = 0;
  virtual PWMapDelegPtr reduce() const = 0;

  /**
   * @brief For every element in both domains assign the law that returns the
   * minimum value.
   */
  virtual PWMapDelegPtr minMap(const PWMapDelegate &other) const = 0;

  /**
   * @brief Given two maps pw1 (\p this) and pw2 (\p other), for every element y1
   * in the image of pw1 returns a pw res such that
   * res(y1) = {min(pw2(x)) : pw1(x) = y1}. In SBG algorithms it is used to
   * calculate for every vertex which of its adjacent vertices returns the
   * minimum value according to pw other.
   */
  virtual PWMapDelegPtr minAdjMap(const PWMapDelegate &other) const = 0;

  /** 
   * @brief Pseudo-inverse of a pw restricted to \p subdom. If a value is the
   * image of several elements of the original domain, it will mapped to any
   * of the possible candidates. 
   */
  virtual PWMapDelegPtr firstInv(const Set &subdom) const = 0;

  /** 
   * @brief Pseudo-inverse of a pw.
   */
  virtual PWMapDelegPtr firstInv() const = 0;

  /**
   * Returns a pw that keeps pieces of the original pw that satisfy the
   * predicate argument.
   */
  virtual PWMapDelegPtr filterMap(bool (*f)(const Map &)) const = 0;

  /** 
   * @brief Return elements in both domains, that have the same image in both
   * pws.
   */
  virtual Set equalImage(const PWMapDelegate &other) const = 0;

  /** 
   * @brief Given a map, return elements of the domain that share its image with
   * other values of the domain.
   */
  virtual Set sharedImage() const = 0;

  /**
   * @brief Sum a constant value to every element in the domain of the pw. The
   * law remains unchanged.
   */
  virtual PWMapDelegPtr offsetDom(const MD_NAT &off) const = 0;

  /**
   * @brief Sum the value indicated by pw \p off for every value in the domain
   * of the pw \p this. The law remains unchanged.
   */
  virtual PWMapDelegPtr offsetDom(const PWMapDelegate &off) const = 0;

  /**
   * @brief Sum a constant value to every element in the image of the pw, that
   * is, the law of the pw is modified without altering its domain.
   */
  virtual PWMapDelegPtr offsetImage(const MD_NAT &off) const = 0;

  /**
   * @brief Sum the expression \p off to every element in the image of the pw,
   * that is, the law is modified without altering its domain.
   */
  virtual PWMapDelegPtr offsetImage(const Exp &off) const = 0;

  /**
   * @brief Compact in the same piece all maps that have the same expression.
   */
  virtual PWMapDelegPtr compact() const = 0;
};

////////////////////////////////////////////////////////////////////////////////
// Unordered PWMap Implementation (concrete delegate) --------------------------
////////////////////////////////////////////////////////////////////////////////

typedef std::vector<Map> UnordMapCollection;

struct UnordPWMap : public PWMapDelegate {
  member_class(UnordMapCollection, pieces);

  ~UnordPWMap() = default;
  UnordPWMap(const MapAF &fact);
  UnordPWMap(const MapAF &fact, const Set &s);
  UnordPWMap(const MapAF &fact, const Map &m);
  UnordPWMap(const MapAF &fact, const UnordMapCollection &pieces);
  UnordPWMap(const UnordPWMap &pw);

  PWMapDelegPtr clone() const override;

  struct Iterator : public PWMapDelegate::Iterator {
    member_class(UnordMapCollection::const_iterator, it);

    Iterator(UnordMapCollection::const_iterator it);
    void operator++() override;
    bool operator!=(const PWMapDelegate::Iterator &other) const override;
    const Map &operator*() const override;
  };

  std::shared_ptr<PWMapDelegate::Iterator> begin() const override;
  std::shared_ptr<PWMapDelegate::Iterator> end() const override;

  void emplaceBack(const Map &m) override;

  bool operator==(const PWMapDelegate &other) const override;
  bool operator!=(const PWMapDelegate &other) const override;
  UnordPWMap &operator=(UnordPWMap &&other);
  std::ostream &print(std::ostream &out) const override;

  PWMapDelegPtr operator+(const PWMapDelegate &other) const override;
  PWMapDelegPtr operator-(const PWMapDelegate &other) const override;

  // Traditional map operations ------------------------------------------------

  std::size_t arity() const override;
  bool isEmpty() const override;
  Set dom() const override;
  PWMapDelegPtr restrict(const Set &subdom) const override;
  Set image() const override;
  Set image(const Set &subdom) const override;
  Set preImage(const Set &subcodom) const override;
  PWMapDelegPtr inverse() const override;
  PWMapDelegPtr composition(const PWMapDelegate &pw2) const override;

  PWMapDelegPtr mapInf(unsigned int n) const override;
  PWMapDelegPtr mapInf() const override;

  // Extra operations ----------------------------------------------------------

  PWMapDelegPtr concatenation(const PWMapDelegate &other) const override;
  PWMapDelegPtr combine(const PWMapDelegate &other) const override;
  PWMapDelegPtr reduce(const Interval &i, const LExp &e) const override;
  PWMapDelegPtr reduce(const Map &sbgmap) const override;
  PWMapDelegPtr reduce() const override;

  PWMapDelegPtr minMap(const PWMapDelegate &other) const override;
  PWMapDelegPtr minAdjMap(const PWMapDelegate &other) const override;

  PWMapDelegPtr firstInv(const Set &subdom) const override;
  PWMapDelegPtr firstInv() const override;

  PWMapDelegPtr filterMap(bool (*f)(const Map &)) const override;

  Set equalImage(const PWMapDelegate &other) const override;
  Set sharedImage() const override;

  PWMapDelegPtr offsetDom(const MD_NAT &off) const override;
  PWMapDelegPtr offsetDom(const PWMapDelegate &off) const override;
  PWMapDelegPtr offsetImage(const MD_NAT &off) const override;
  PWMapDelegPtr offsetImage(const Exp &off) const override;

  PWMapDelegPtr compact() const override;
};

typedef const UnordPWMap &UnordPWMapCRef;
typedef std::unique_ptr<UnordPWMap> UnordPWMapPtr;


////////////////////////////////////////////////////////////////////////////////
// Ordered PWMap Implementation (concrete delegate) ----------------------------
////////////////////////////////////////////////////////////////////////////////

typedef std::pair<MD_NAT, MD_NAT>  SetPerimeter;

typedef std::pair<Map, SetPerimeter>  MapEntry;

typedef std::vector<MapEntry> OrdMapCollection;

struct OrdPWMap : public PWMapDelegate {
  
  member_class(OrdMapCollection, pieces);

  ~OrdPWMap() = default;
  OrdPWMap(const MapAF &fact);
  OrdPWMap(const MapAF &fact, const Set &s);
  OrdPWMap(const MapAF &fact, const Map &m);
  OrdPWMap(const MapAF &fact, const OrdMapCollection &pieces);
  OrdPWMap(const OrdPWMap &pw);

  PWMapDelegPtr clone() const override;

  struct Iterator : public PWMapDelegate::Iterator {
    member_class(OrdMapCollection::const_iterator, it);

    Iterator(OrdMapCollection::const_iterator it);
    void operator++() override;
    bool operator!=(const PWMapDelegate::Iterator &other) const override;
    const Map &operator*() const override;
  };

  std::shared_ptr<PWMapDelegate::Iterator> begin() const override;
  std::shared_ptr<PWMapDelegate::Iterator> end() const override;

  void emplaceBack(const Map &m) override;

  bool operator==(const PWMapDelegate &other) const override;
  bool operator!=(const PWMapDelegate &other) const override;
  OrdPWMap &operator=(OrdPWMap &&other);
  std::ostream &print(std::ostream &out) const override;

  PWMapDelegPtr operator+(const PWMapDelegate &other) const override;
  PWMapDelegPtr operator-(const PWMapDelegate &other) const override;

  // Traditional map operations ------------------------------------------------

  std::size_t arity() const override;
  bool isEmpty() const override;
  Set dom() const override;
  PWMapDelegPtr restrict(const Set &subdom) const override;
  Set image() const override;
  Set image(const Set &subdom) const override;
  Set preImage(const Set &subcodom) const override;
  PWMapDelegPtr inverse() const override;
  PWMapDelegPtr composition(const PWMapDelegate &pw2) const override;

  PWMapDelegPtr mapInf(unsigned int n) const override;
  PWMapDelegPtr mapInf() const override;

  // Extra operations ----------------------------------------------------------

  PWMapDelegPtr concatenation(const PWMapDelegate &other) const override;
  PWMapDelegPtr combine(const PWMapDelegate &other) const override;
  PWMapDelegPtr reduce(const Interval &i, const LExp &e) const override;
  PWMapDelegPtr reduce(const Map &sbgmap) const override;
  PWMapDelegPtr reduce() const override;

  PWMapDelegPtr minMap(const PWMapDelegate &other) const override;
  PWMapDelegPtr minAdjMap(const PWMapDelegate &other) const override;

  PWMapDelegPtr firstInv(const Set &subdom) const override;
  PWMapDelegPtr firstInv() const override;

  PWMapDelegPtr filterMap(bool (*f)(const Map &)) const override;

  Set equalImage(const PWMapDelegate &other) const override;
  Set sharedImage() const override;

  PWMapDelegPtr offsetDom(const MD_NAT &off) const override;
  PWMapDelegPtr offsetDom(const PWMapDelegate &off) const override;
  PWMapDelegPtr offsetImage(const MD_NAT &off) const override;
  PWMapDelegPtr offsetImage(const Exp &off) const override;

  PWMapDelegPtr compact() const override;
  
  private:
  
  void processMinAdjMap(
  const Map &m1, 
  const Map &m2, 
  Set &set_in,
  Set &set_out, 
  OrdMapCollection  &ord_pwmap,
  unsigned int &global_pos) const;
  
  void processMinus(
  const Map &m1, 
  const Map &m2, 
  Set &set_in,
  Set &set_out, 
  OrdMapCollection  &ord_pwmap,
  unsigned int &global_pos) const; 
  
  void processAdd(
  const Map &m1, 
  const Map &m2, 
  Set &set_in,
  Set &set_out, 
  OrdMapCollection  &ord_pwmap,
  unsigned int &global_pos) const; 
  
  void processEqualImage(
  const Map &m1, 
  const Map &m2, 
  Set &set_in,
  Set &set_out,
  OrdMapCollection  &ord_pwmap,
  unsigned int &global_pos) const; 
  
  using ProcessFunc = void (OrdPWMap::*)(
  const Map &, const Map &, 
  Set &, Set &, OrdMapCollection &, 
  unsigned int&
  ) const;
  
  void processMapsOrd(
    const PWMapDelegate &other,
    Set &set_in,
    Set &set_out,
    OrdMapCollection  &ord_pwmap,
    ProcessFunc process,
    bool order_mts
  ) const;
};


typedef const OrdPWMap &OrdPWMapCRef;
typedef OrdPWMap &OrdPWMapRef;
typedef std::unique_ptr<OrdPWMap> OrdPWMapPtr;



////////////////////////////////////////////////////////////////////////////////
// PWMap Implementation (delegator) --------------------------------------------
////////////////////////////////////////////////////////////////////////////////

struct PWMap {
  private:
  PWMapDelegPtr delegate_;

  public:
  PWMap(PWMapDelegPtr deleg);
  PWMap(const PWMap &other);

  struct Iterator {
    private:
    std::shared_ptr<PWMapDelegate::Iterator> it_;

    public:
    Iterator(std::shared_ptr<PWMapDelegate::Iterator> it);
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
  PWMap operator-(const PWMap &other) const;

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
