/**
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

<<<<<<< HEAD
#include "util/defs.hpp"
#include "util/time_profiler.hpp"
=======
#include "defs.hpp"
#include "time_profiler.hpp"
>>>>>>> sb-graph-dev

namespace SBG {

namespace Util {


void time_profiler_results()
{
    Internal::TimeProfiler::print_execution_time();
}


namespace Internal{

std::unordered_map<std::string, double> TimeProfiler::_execution_time = {};

TimeProfiler::TimeProfiler(std::string&& function_name)
    : _function_name(move(function_name))
{
    if (time_profiler_enabled) {
        _start = std::chrono::high_resolution_clock::now();
    }
}


TimeProfiler::~TimeProfiler()
{
    if (time_profiler_enabled) {
        auto end = std::chrono::high_resolution_clock::now();
        auto exec_time = std::chrono::duration<double, std::milli>(end - _start).count();
        if (_execution_time.find(_function_name) == _execution_time.end()) {
            _execution_time.insert({_function_name, 0.0});
        }
        _execution_time[_function_name] += exec_time;
    }
}


void TimeProfiler::print_execution_time() {
    if (time_profiler_enabled) {
        for (auto&& pair : TimeProfiler::_execution_time) {
            std::cout << pair.first << ": " << pair.second << " ms" << std::endl;
        }
    }
}

}

}

<<<<<<< HEAD
}
=======
}
>>>>>>> sb-graph-dev
