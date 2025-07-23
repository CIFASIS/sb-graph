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

 #include <chrono>
 #include <iostream>
 #include <string>
 #include <unordered_map>

 #pragma once

namespace sbg_partitioner {

constexpr bool time_profiler_enabled = false;

void time_profiler_results();

namespace internal {

struct TimeProfiler {
    TimeProfiler(std::string&& function_name);

    ~TimeProfiler();

    static void print_execution_time();

private:
    std::string _function_name;
    std::chrono::_V2::system_clock::time_point _start;
    static std::unordered_map<std::string, double> _execution_time;
};

}

}