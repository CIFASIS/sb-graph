/*****************************************************************************

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
#include "boost/variant.hpp"

#include <parser/parser.hpp>
#include <sbg/sbg.hpp>

typedef boost::variant<SBG::SBGraph, SBG::DSBGraph> Grph;

struct Converter{
  member_class(SBG::OrdCT<SBG::INT>, offset);
  member_class(Parser::SetGraph, sg);

  Converter();
  Converter(Parser::SetGraph sg);

  SBG::MultiInterval makeDom(SBG::MultiInterval mi1, SBG::MultiInterval mi2);
  SBG::LMap makeExp(SBG::MultiInterval dom, SBG::MultiInterval mi);
  SBG::SetEdge convertEdge(Parser::SetEdge se);
  SBG::DSetEdge convertDirectedEdge(Parser::SetEdge se);

  SBG::SBGraph convertUndirectedGraph();
  SBG::DSBGraph convertDirectedGraph();
  Grph convertGraph();
};
