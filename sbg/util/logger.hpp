/*****************************************************************************

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

#ifndef SBG_LOGGER_HPP
#define SBG_LOGGER_HPP

#include <fstream>
#include <string>

namespace SBG {

#define SBG_LOG SBGLogger::instance().log

class SBGLogger {
  public:
  static SBGLogger& instance()
  {
    static SBGLogger _instance;
    return _instance;
  }

  ~SBGLogger();

  std::ofstream log;

  private:
  SBGLogger();
};

}  // namespace SBG

#endif
