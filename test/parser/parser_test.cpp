/*****************************************************************************

    This file is part of SBG Library.

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

#include <cstdlib>
#include <gtest/gtest.h>
#include <fstream>

/// @file parser_test.cpp
///

/// @addtogroup SBGraphParserTests
/// @{

class ParserTests : public testing::TestWithParam<const char*> {
};

TEST_P(ParserTests, Parser)
{
  const std::string NAME = GetParam();
  std::cout << "Testing program: " << NAME << std::endl;
  const std::string PARSER_CMD = "./compile_and_run.sh " + NAME;
  const std::string RESULT_FILE = "./test_data/" + NAME + "/" + NAME + ".passed";
  const std::string TEST_CMD = "./results.sh " + NAME;

  std::system(PARSER_CMD.c_str());
  std::system(TEST_CMD.c_str());

  std::ifstream result(RESULT_FILE.c_str());
  EXPECT_TRUE(result.good());
}

const char* parser_program[] = {"arithmetic", "interval", "set", "lexp", "map"};

INSTANTIATE_TEST_SUITE_P(ParserInst, ParserTests, testing::ValuesIn(parser_program));

/// @}
