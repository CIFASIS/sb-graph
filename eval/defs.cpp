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

// Types -----------------------------------------------------------------------

Boolean::Boolean() : b_() {}
Boolean::Boolean(bool b) : b_(b) {}

std::ostream &operator<<(std::ostream &out, const Boolean &b)
{
  if (b.b_)
    out << "true";

  else
    out << "false";

  return out;
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
FuncEnvType FuncEnv::mapping_ = {
  {"isEmpty", 0}, {"minElem", 1}, {"maxElem", 2}
  , {"compose", 3}, {"inv", 4}, {"image", 5}, {"preImage", 6}, {"dom", 7}
  , {"combine", 8}, {"firstInv", 9}, {"minMap", 10}, {"reduce", 11}
  , {"minAdj", 12}, {"mapInf", 13}, {"CC", 14}, {"matching", 15}, {"scc", 16}
  , {"sort", 17}, {"matchSCC", 18}, {"matchSCCTS", 19}, {"cut", 20}
};

MaybeFValue FuncEnv::operator[](FKey k) const
{
  if (mapping_.find(k) == mapping_.end()) return std::nullopt;
  return mapping_.at(k);
}

// Classes for pretty printing ------------------------------------------------

template<typename T, typename... Ts>
std::ostream &operator<<(std::ostream &out, const std::variant<T, Ts...> &v)
{
  std::visit([&out](auto&& arg) {
    out << arg;
  }, v);

  return out;
}

template std::ostream &operator<<(std::ostream &out, const ExprBaseType &v);

std::ostream &operator<<(std::ostream &out, const ExprEval &e)
{
  out << std::get<0>(e) << "\n  --> "; 
  ExprBaseType ebt = std::get<1>(e);
  out << ebt << "\n";

  return out;
}

std::ostream &operator<<(std::ostream &out, const ExprEvalList &ee)
{
  for (ExprEval e : ee)
    out << e << "\n";

  return out;
}

ProgramIO::ProgramIO(AST::StatementList stms, ExprEvalList exprs) 
  : nmbr_dims_(1), stms_(stms), exprs_(exprs) {}
ProgramIO::ProgramIO(unsigned int nmbr_dims, AST::StatementList stms, ExprEvalList exprs)
  : nmbr_dims_(nmbr_dims), stms_(stms), exprs_(exprs) {}

member_imp(ProgramIO, unsigned int, nmbr_dims);
member_imp(ProgramIO, AST::StatementList, stms);

std::ostream &operator<<(std::ostream &out, const ProgramIO &p)
{
  if (!p.stms().empty())
    out << p.stms() << "\n";
  out << p.exprs_;

  return out;
}

} // namespace Eval

} // namespace SBG
