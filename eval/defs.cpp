/*******************************************************************************

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

#include <eval/defs.hpp>

namespace SBG {

namespace Eval {

// Type definitions ------------------------------------------------------------

Util::INT toInt(ExprBaseType v) 
{ 
  if (std::holds_alternative<Util::INT>(v))
    return std::get<Util::INT>(v);

  if (std::holds_alternative<Util::RATIONAL>(v)) {
    Util::RATIONAL v_rat = std::get<Util::RATIONAL>(v);
    if (v_rat.denominator() == 1)
      return v_rat.numerator();
  }

  Util::ERROR("toInt: expression is not integer");
  return 0; 
}

// Environments ----------------------------------------------------------------

VarEnv::VarEnv() {}

FuncEnv::FuncEnv() {
  insert("isEmpty", 0);
  insert("isMember", 1);
  insert("min", 2);
  insert("max", 3);
}

// Classes for pretty printing ------------------------------------------------

std::ostream &operator<<(std::ostream &out, const ExprEval &e)
{
  out << std::get<0>(e) << "\n  --> " << streamer{std::get<1>(e)} << "\n";

  return out;
}

std::ostream &operator<<(std::ostream &out, const ExprEvalList &ee)
{
  BOOST_FOREACH (ExprEval e, ee)
    out << e << "\n";

  return out;
}

ProgramIO::ProgramIO() : stms_(), exprs_() {}
ProgramIO::ProgramIO(AST::StatementList stms, ExprEvalList exprs) : stms_(stms), exprs_(exprs) {}

member_imp(ProgramIO, AST::StatementList, stms);
member_imp(ProgramIO, ExprEvalList, exprs);

std::ostream &operator<<(std::ostream &out, const ProgramIO &p)
{
  out << p.stms() << "\n" << p.exprs();

  return out;
}

} // namespace Eval

} // namespace SBG
