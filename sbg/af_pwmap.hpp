/** @file af_pwmap.hpp

 @brief <b>PWMap Abstract Factory</b>

 Currently only one implementation of PWMaps is supported: unordered. In the
 future an ordered implementation might be added.

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

#ifndef SBG_AF_PWMAP_HPP
#define SBG_AF_PWMAP_HPP

#include "pw_map.hpp"

namespace SBG {

namespace LIB {

#define PW_FACT PWFactory::instance().pw_fact()

struct PWMapAF {
  public:
  virtual ~PWMapAF() = default;
  PWMapAF();

  virtual PWMap createPWMap() const = 0;
  virtual PWMap createPWMap(const Set &s) const = 0;
  virtual PWMap createPWMap(const Map &m) const = 0;
  virtual std::string prettyPrint() const = 0;
};

struct UnordPWMapAF : public PWMapAF {
  public:
  UnordPWMapAF();

  PWMap createPWMap() const override;
  PWMap createPWMap(const Set &s) const override;
  PWMap createPWMap(const Map &m) const override;
  std::string prettyPrint() const override;
};

using PWMapFactPtr = std::unique_ptr<PWMapAF>;

/**
 * @brief Single instance of pw factory to be used by clients in need of
 * creating pws. A client includes this file and calls
 * PW_FACT.createPWMap(args).
 */
class PWFactory {
  public:
  ~PWFactory() = default;
  static PWFactory& instance() {
    static PWFactory instance_;
    return instance_;
  }

  PWMapAF& pw_fact();
  void set_pw_fact(PWMapFactPtr set_fact);

  private:
  PWFactory();
  PWMapFactPtr pw_fact_;
};

} // namespace LIB

}  // namespace SBG

#endif
