/** @file af_set.hpp

 @brief <b>Set Abstract Factory</b>

 While developing the SBG library several implementations were proposed. First,
 two implementations were proposed for sets: unordered and ordered sets. For
 this reason, it was decided that delegation should be used to provide
 different implementations, and also an abstract factory was needed to create
 objects with the desired implementation.

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

#ifndef SBG_AF_SET_HPP
#define SBG_AF_SET_HPP

#include "set.hpp"

namespace SBG {

namespace LIB {

#define SET_FACT SetFactory::instance().set_fact()

class SetAF {
  public:
  virtual ~SetAF() = default;

  virtual Set createSet() const = 0;
  virtual Set createSet(const MD_NAT &x) const = 0;
  virtual Set createSet(const Interval &i) const = 0;
  virtual Set createSet(const SetPiece &mdi) const = 0;
  virtual std::string prettyPrint() const = 0;
};

class UnordAF : public SetAF {
  public:
  Set createSet() const override;
  Set createSet(const MD_NAT &x) const override;
  Set createSet(const Interval &i) const override;
  Set createSet(const SetPiece &mdi) const override;
  std::string prettyPrint() const override;
};

class OrdDenseAF : public SetAF {
  public:
  Set createSet()  const override;
  Set createSet(const MD_NAT &x) const override;
  Set createSet(const Interval &i) const override;
  Set createSet(const SetPiece &mdi) const override;
  std::string prettyPrint() const override;
};

using SetFactPtr = std::unique_ptr<SetAF>;

/**
 * @brief Single instance of set factory to be used by clients in need of
 * creating sets. A client includes this file and calls
 * SET_FACT.createSet(args).
 */
class SetFactory {
  public:
  ~SetFactory() = default;
  static SetFactory& instance() {
    static SetFactory instance_;
    return instance_;
  }

  SetAF& set_fact();
  void set_set_fact(SetFactPtr set_fact);

  private:
  SetFactory();
  SetFactPtr set_fact_;
};

} // namespace LIB

}  // namespace SBG

#endif
