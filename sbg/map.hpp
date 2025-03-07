/** @file map.hpp

 @brief <b>Map delegate pattern</b>

 A SBG map is an entity composed by a domain and a law.
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

#ifndef SBG_MAP_HPP
#define SBG_MAP_HPP

#include "sbg/af_set.hpp"
#include "sbg/multidim_lexp.hpp"
#include "sbg/set.hpp"

namespace SBG {

namespace LIB {

// Map Abstract Delegate -------------------------------------------------------

struct MapDelegate;

typedef std::unique_ptr<MapDelegate> MapDelegPtr;

struct MapDelegate {
  protected:
  const SetAF &fact_;
  Set dom_;
  Exp exp_;

  public:
  virtual ~MapDelegate() = default;
  MapDelegate(const SetAF &fact);
  MapDelegate(const SetAF &fact, Util::MD_NAT x, Exp exp);
  MapDelegate(const SetAF &fact, Interval i, LExp le);
  MapDelegate(const SetAF &fact, SetPiece mdi, Exp exp);
  MapDelegate(const SetAF &fact, Set s, Exp exp);

  virtual bool operator==(const MapDelegate &other) const = 0;
  virtual bool operator!=(const MapDelegate &other) const = 0;
  virtual MapDelegPtr operator+(const MapDelegate &other) = 0;
  virtual std::ostream &print(std::ostream &out) const = 0;

  /**
   * @brief Traditional map operations.
   */
  virtual Set dom() const = 0;
  virtual Exp exp() const = 0;
  virtual std::size_t arity() const = 0;
  virtual bool isEmpty() const = 0;
  virtual MapDelegPtr restrict(const Set &subdom) = 0;
  virtual Set image() const = 0;
  virtual Set image(const Set &subdom) const  = 0;
  virtual Set preImage() const = 0;
  virtual Set preImage(const Set &subdom) const = 0;
  virtual MapDelegPtr composition(const MapDelegate &other) = 0;

  /**
   * @brief Extra operations.
   */
  virtual MapDelegPtr minInv() = 0;
  virtual bool isId() const = 0;
  virtual MapDelegPtr compact(const MapDelegate &other) = 0;
};

// Map Set Implementation (concrete delegate) ----------------------------

struct MapSetDeleg : public MapDelegate {

  public:
  ~MapSetDeleg();
  MapSetDeleg(const SetAF &fact);
  MapSetDeleg(const SetAF &fact, Util::MD_NAT x, Exp exp);
  MapSetDeleg(const SetAF &fact, Interval i, LExp le);
  MapSetDeleg(const SetAF &fact, SetPiece mdi, Exp exp);
  MapSetDeleg(const SetAF &fact, Set s, Exp exp);

  bool operator==(const MapDelegate &other) const override;
  bool operator!=(const MapDelegate &other) const override;
  MapDelegPtr operator+(const MapDelegate &other) override;
  std::ostream &print(std::ostream &out) const override;

  /**
   * @brief Traditional map operations.
   */
  Set dom() const override;
  Exp exp() const override;
  std::size_t arity() const override;
  bool isEmpty() const override;
  MapDelegPtr restrict(const Set &subdom) override;
  Set image() const override;
  Set image(const Set &subdom) const override;
  Set preImage() const override;
  Set preImage(const Set &subdom) const override;
  MapDelegPtr composition(const MapDelegate &other) override;

  /**
   * @brief Extra operations.
   */
  MapDelegPtr minInv() override;
  bool isId() const override;
  MapDelegPtr compact(const MapDelegate &other) override;
};

typedef const MapSetDeleg &MapSetDelegCRef;

// Map Implementation (delegator) --------------------------------------

typedef std::unique_ptr<MapDelegate> MapDelegPtr;

struct Map {
  private:
  MapDelegPtr delegate_;

  public:
  Map(MapDelegPtr deleg);

  bool operator==(const Map &other) const;
  bool operator!=(const Map &other) const;
  Map operator+(const Map &other) const;
  std::ostream &print(std::ostream &out) const;

  /**
   * @brief Traditional map operations.
   */
  Set dom() const;
  Exp exp() const;
  std::size_t arity() const;
  bool isEmpty() const;
  Map restrict(const Set &subdom) const;
  Set image() const;
  Set image(const Set &subdom) const;
  Set preImage() const;
  Set preImage(const Set &subdom) const;
  Map composition(const Map &other) const;

  /**
   * @brief Extra operations.
   */
  Map minInv() const;
  bool isId() const;
  Map compact(Map &other) const;
};
std::ostream &operator<<(std::ostream &out, const Map &s);


} // namespace LIB

}  // namespace SBG

#endif
