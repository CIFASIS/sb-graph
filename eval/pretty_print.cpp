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

#include "eval/pretty_print.hpp"

namespace SBG {

namespace Eval {

template<typename T, typename... Ts>
std::ostream &operator<<(std::ostream &out, const std::variant<T, Ts...> &v)
{
  std::visit([&out](auto&& arg) {
    out << arg;
  }, v);

  return out;
}

template std::ostream &operator<<(std::ostream &out, const ExprBaseType &v);

std::ostream &operator<<(std::ostream &out, const ExprResult &e)
{
  out << std::get<0>(e) << "\n  --> " << std::get<1>(e) << "\n"; 

  return out;
}

std::ostream &operator<<(std::ostream &out, const ExprResultList &ee)
{
  for (ExprResult e : ee)
    out << e << "\n";

  return out;
}

std::ostream &operator<<(std::ostream &out, const StmResult &s)
{
  out << std::get<0>(s) << " = " << std::get<1>(s) << ";";

  return out;
}

std::ostream &operator<<(std::ostream &out, const StmResultList &ss)
{
  for (StmResult s : ss)
    out << s << "\n";

  return out;
}

ProgramIO::ProgramIO(StmResultList stms, ExprResultList exprs) 
  : nmbr_dims_(1), stms_(stms), exprs_(exprs) {}
ProgramIO::ProgramIO(unsigned int nmbr_dims, StmResultList stms
  , ExprResultList exprs)
  : nmbr_dims_(nmbr_dims), stms_(stms), exprs_(exprs) {}

member_imp(ProgramIO, unsigned int, nmbr_dims);
member_imp(ProgramIO, StmResultList, stms);
member_imp(ProgramIO, ExprResultList, exprs);

std::ostream &operator<<(std::ostream &out, const ProgramIO &p)
{
  out << p.stms() << "\n";
  out << p.exprs();

  return out;
}

} // namespace Eval

} // namespace SBG
