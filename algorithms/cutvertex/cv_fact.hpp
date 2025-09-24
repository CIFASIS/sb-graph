/** @file cv_fact.hpp

 @brief <b>Vertex Cut Set Algorithm Factory</b>

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

#ifndef SBG_CV_FACT_HPP
#define SBG_CV_FACT_HPP

#include "cut_vertex.hpp"

namespace SBG {

namespace LIB {

#define CV_FACT CVFactory::instance().getCVFactory()

class CVFact {
  public:
  virtual ~CVFact() = default;
  CVFact() = default;

  virtual CutVertex createCVAlgorithm() const = 0;
};

class MaxDegCVFact : public CVFact {
  public:
  MaxDegCVFact() = default;

  CutVertex createCVAlgorithm() const override;
};

/**
 * @brief Single instance of cv factory to be used by clients in need of
 * creating an instance of a cv algorithm. A client includes this file and
 * calls CV_FACT.createCVAlgorithm(args).
 */
class CVFactory {
  public:
  ~CVFactory() = default;
  static CVFactory& instance() {
    static CVFactory instance_;
    return instance_;
  }

  CVFact& getCVFactory();
  void setCVFactory(std::unique_ptr<CVFact> cv_fact);

  private:
  CVFactory();
  std::unique_ptr<CVFact> cv_fact_;
};

} // namespace LIB

}  // namespace SBG

#endif
