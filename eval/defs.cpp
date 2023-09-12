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

#include "eval/defs.hpp"

namespace SBG {

namespace Eval {

// Type definitions ------------------------------------------------------------

Util::NAT toNat(ExprBaseType v) 
{ 
  if (std::holds_alternative<Util::NAT>(v))
    return std::get<Util::NAT>(v);

  if (std::holds_alternative<Util::RATIONAL>(v)) {
    Util::RATIONAL v_rat = std::get<Util::RATIONAL>(v);
    if (v_rat.denominator() == 1 && v_rat.numerator() >= 0)
      return v_rat.numerator();
  }

  Util::ERROR("toNat: expression is not integer");
  return 0; 
}

// Environments ----------------------------------------------------------------

VarEnv::VarEnv() : mapping_() {}

void VarEnv::insert(VKey k, VValue v)
{
  mapping_.erase(k);
  mapping_.insert(std::pair<VKey, VValue>(k, v));
}

MaybeVValue VarEnv::operator[](VKey k) const
{
  if (mapping_.find(k) == mapping_.end()) return std::nullopt;
  return mapping_.at(k);
}

FuncEnv::FuncEnv() {}
FuncEnvType FuncEnv::mapping_ = {{"isEmpty", 0}, {"isMember", 1}, {"minElem", 2}, {"maxElem", 3}, {"lt", 4},
  {"compose", 5}, {"inv", 6}, {"image", 7}, {"preImage", 8}, {"dom", 9}, {"minAdj", 10}};

MaybeFValue FuncEnv::operator[](FKey k) const
{
  if (mapping_.find(k) == mapping_.end()) return std::nullopt;
  return mapping_.at(k);
}

// Classes for pretty printing ------------------------------------------------

std::ostream &operator<<(std::ostream &out, const ExprEval &e)
{
  out << std::get<0>(e) << "\n  --> "; 
  ExprBaseType ebt = std::get<1>(e);
  std::visit(
    Util::Overload {
      [&](auto v) { out << v; }
    }
    , ebt
  );
  out << "\n";

  return out;
}

std::ostream &operator<<(std::ostream &out, const ExprEvalList &ee)
{
  BOOST_FOREACH (ExprEval e, ee)
    out << e << "\n";

  return out;
}

ProgramIO::ProgramIO() : nmbr_dims_(1), stms_(), exprs_() {}
ProgramIO::ProgramIO(AST::StatementList stms, ExprEvalList exprs) : nmbr_dims_(1), stms_(stms), exprs_(exprs) {}
ProgramIO::ProgramIO(Util::NAT nmbr_dims, AST::StatementList stms, ExprEvalList exprs) : nmbr_dims_(nmbr_dims), stms_(stms), exprs_(exprs) {}

member_imp(ProgramIO, Util::NAT, nmbr_dims);
member_imp(ProgramIO, AST::StatementList, stms);
member_imp(ProgramIO, ExprEvalList, exprs);

std::ostream &operator<<(std::ostream &out, const ProgramIO &p)
{
  out << p.stms() << "\n" << p.exprs();

  return out;
}

} // namespace Eval

} // namespace SBG
