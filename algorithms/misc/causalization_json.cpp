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

#include "rapidjson/document.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"

#include "algorithms/misc/causalization_json.hpp"

namespace MISC {

using namespace SBG::LIB;

rapidjson::Value setJson(const Set &s
  , rapidjson::Document::AllocatorType &alloc)
{
  rapidjson::Value result(rapidjson::kArrayType);

  for (const SetPiece &mdi : s) {
    rapidjson::Value inter_array(rapidjson::kArrayType);
    for (const Interval &i : mdi) {
      rapidjson::Value inter(rapidjson::kArrayType);

      rapidjson::Value beg;
      beg.SetInt(i.begin());
      inter.PushBack(beg, alloc);
      rapidjson::Value st;
      st.SetInt(i.step());
      inter.PushBack(st, alloc);
      rapidjson::Value end;
      end.SetInt(i.end());
      inter.PushBack(end, alloc);

      inter_array.PushBack(inter, alloc);
    }
    rapidjson::Value mdi_obj(rapidjson::kObjectType);
    mdi_obj.AddMember("interval", inter_array, alloc);
    result.PushBack(mdi_obj, alloc);
  }

  return result;
}

rapidjson::Value expJson(Exp exp, rapidjson::Document::AllocatorType &alloc)
{
  rapidjson::Value result(rapidjson::kArrayType);

  for (const LExp &le : exp) {
    rapidjson::Value le_array(rapidjson::kArrayType);

    std::stringstream ssm;
    ssm << le.slope();
    rapidjson::Value m;
    m.SetString(ssm.str().c_str(), strlen(ssm.str().c_str()), alloc);
    le_array.PushBack(m, alloc);

    std::stringstream ssh;
    ssh << le.offset();
    rapidjson::Value h;
    h.SetString(ssh.str().c_str(), strlen(ssh.str().c_str()), alloc);
    le_array.PushBack(h, alloc);

    result.PushBack(le_array, alloc);
  }

  return result;
}

rapidjson::Value mapJson(
  const PWMap &pw, rapidjson::Document::AllocatorType &alloc
)
{
  rapidjson::Value result(rapidjson::kArrayType);

  for (const Map &map : pw) {
    rapidjson::Value ith(rapidjson::kObjectType);

    ith.AddMember("dom", setJson(map.dom(), alloc), alloc);
    ith.AddMember("exp", expJson(map.exp(), alloc), alloc);

    result.PushBack(ith, alloc);
  }

  return result;
}

void buildJson(const Set &matching, const PWMap &scc, const PWMap &order)
{
  rapidjson::Document d;
  d.SetObject();
  rapidjson::Document::AllocatorType& alloc = d.GetAllocator();

  // Create matching information
  rapidjson::Value edges = setJson(matching, alloc);
  d.AddMember("matching", edges, alloc);

  // Create SCC information
  rapidjson::Value scc_rmap = mapJson(scc, alloc);
  d.AddMember("scc", scc_rmap, alloc);

  // Create sort information
  rapidjson::Value order_rmap = mapJson(order, alloc);
  d.AddMember("sort", order_rmap, alloc);

  FILE *fp = fopen("output.json", "w");
  char write_buffer[65536];
  rapidjson::FileWriteStream os(fp, write_buffer, sizeof(write_buffer));
  rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
  rapidjson::PrettyFormatOptions opt = rapidjson::kFormatSingleLineArray;
  writer.SetFormatOptions(opt);
  d.Accept(writer);

  fclose(fp);
}

} // namespace MISC

