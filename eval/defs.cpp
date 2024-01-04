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
  {"compose", 5}, {"inv", 6}, {"image", 7}, {"preImage", 8}, {"dom", 9}, {"combine", 10}, {"minMap", 11}, 
  {"reduce", 12}, {"minAdj", 13}, {"CC", 14}, {"minReach", 15}, {"matching", 16}, {"scc", 17},
  {"sort", 18}};

MaybeFValue FuncEnv::operator[](FKey k) const
{
  if (mapping_.find(k) == mapping_.end()) return std::nullopt;
  return mapping_.at(k);
}

// Classes for pretty printing ------------------------------------------------

template<typename T, typename... Ts>
std::ostream& operator<<(std::ostream& os, const std::variant<T, Ts...>& v)
{
    std::visit([&os](auto&& arg) {
        os << arg;
    }, v);
    return os;
}

std::ostream &operator<<(std::ostream &out, const ExprEval &e)
{
  out << std::get<0>(e) << "\n  --> "; 
  ExprBaseType ebt = std::get<1>(e);
  auto printer = [&](auto v) { out << v; };
  std::visit(printer, ebt);
  out << "\n";

  return out;
}

std::ostream &operator<<(std::ostream &out, const ExprEvalList &ee)
{
  for (ExprEval e : ee)
    out << e << "\n";

  return out;
}

ProgramIO::ProgramIO() : nmbr_dims_(1), stms_(), exprs_() {}
ProgramIO::ProgramIO(AST::StatementList stms, ExprEvalList exprs) 
  : nmbr_dims_(1), stms_(stms), exprs_(exprs) {}
ProgramIO::ProgramIO(unsigned int nmbr_dims, AST::StatementList stms, ExprEvalList exprs)
  : nmbr_dims_(nmbr_dims), stms_(stms), exprs_(exprs) {}

member_imp(ProgramIO, unsigned int, nmbr_dims);
member_imp(ProgramIO, AST::StatementList, stms);
member_imp(ProgramIO, ExprEvalList, exprs);

std::ostream &operator<<(std::ostream &out, const ProgramIO &p)
{
  if (!p.stms().empty())
    out << p.stms() << "\n";
  out << p.exprs();

  return out;
}

} // namespace Eval

} // namespace SBG
