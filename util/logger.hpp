/** @file logger.hpp

 @brief <b>Logging</b>

 Creates an SBGLogger that can be used by all modules in the same way of an
 ostream object. It saves the desired contents in an "SBG.log" file.

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

#ifndef SBG_LOGGER_HPP
#define SBG_LOGGER_HPP

#include <fstream>
#include <string>

namespace SBG {

namespace Util {

enum class LogLevel { Info = 0, Debug = 1, Warning = 2 };

class NullBuffer : public std::streambuf {
  int overflow(int c) override { return c; }
};

static NullBuffer null_buffer;
static std::ostream null_stream(&null_buffer);

#define SBG_LOG SBGLogger::instance().log(::SBG::Util::LogLevel::Info)
#define DEBUG_LOG SBGLogger::instance().log(::SBG::Util::LogLevel::Debug)

class SBGLogger {
  public:
  static SBGLogger& instance()
  {
    static SBGLogger _instance;
    return _instance;
  }

  ~SBGLogger();

  void setLevel(LogLevel lvl);
  std::ostream& log(LogLevel msgLevel);

  private:
  SBGLogger();
  std::ofstream file_;
  LogLevel level_{LogLevel::Info};
};

} // namespace Util

} // namespace SBG

#endif
