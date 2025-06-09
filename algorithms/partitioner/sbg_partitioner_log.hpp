/*****************************************************************************

 This file is part of SBG Partitioner.

 SBG Partitioner is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 SBG Partitioner is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with SBG Partitioner.  If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#pragma once

#include <iostream>


namespace sbg_partitioner {

namespace logging {


class SBGPartitionerLogger {

public:

    static SBGPartitionerLogger& instance()
    {
        static SBGPartitionerLogger _instance;
        return _instance;
    }

    template<typename T>
    SBGPartitionerLogger& operator << (const T& x)
    {
#ifdef SBG_PARTITIONER_LOGGING
        *log << x;
#endif
        return *this;
    }

    SBGPartitionerLogger& operator <<(std::ostream& (*os)(std::ostream&))
    {
#ifdef SBG_PARTITIONER_LOGGING
        *log << os;
#endif
        return *this;
    }

private:
    SBGPartitionerLogger()
    {
#ifdef SBG_PARTITIONER_LOGGING
        log = &std::cout;
#endif
    }

    ~SBGPartitionerLogger() = default;

    std::ostream* log;
};

static SBGPartitionerLogger& sbg_log = SBGPartitionerLogger::instance();

}
}