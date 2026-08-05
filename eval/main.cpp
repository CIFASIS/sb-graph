/** @file main.hpp

 @brief <b>SBG program evaluator</b>

 These modules allows the user to test the SBG modules. To do so the user should
 provide a SBG program file. The file will be parser, and next the visitors
 will be used to return a result.

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

#include <iostream>

#include "eval/eval_exec.hpp"

int main(int argc, char* argv[])
{
  std::cout << std::boolalpha;

  SBG::Eval::EvalExecutor eval_exec;
  eval_exec.execute(argc, argv);

  return 0;
}
