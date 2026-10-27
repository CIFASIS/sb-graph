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

#include "eval/user_impl_map.hpp"

namespace SBG {

namespace Eval {

////////////////////////////////////////////////////////////////////////////////
// Single Structure Implementation Map -----------------------------------------
////////////////////////////////////////////////////////////////////////////////

UserImplMap::StructImplMap::StructImplMap() : is_frozen_(false) {}

UserImplMap::StructImplMap::SValue& UserImplMap::StructImplMap::operator[]
  (const SKey& key)
{
  Util::ERROR_UNLESS(!is_frozen_, "StructImplMap: map is frozen, insertion is"
    " prohibited.\n");
  return struct_impls_[key];
}

const UserImplMap::StructImplMap::SValue& UserImplMap::StructImplMap::operator[]
  (const SKey& key) const
{
  Util::ERROR_UNLESS(struct_impls_.find(key) != struct_impls_.end()
    , "StructImplMap: undefined implementation.\n");
  return struct_impls_.at(key); 
}

void UserImplMap::StructImplMap::freeze()
{
  is_frozen_ = true;
}

UserImplMap::StructImplMap setMap()
{
  UserImplMap::StructImplMap set_mapping;
  set_mapping[0] = []() { return std::make_unique<LIB::UnordSetFact>(); };
  set_mapping[1] = []() { return std::make_unique<LIB::OrdSetFact>(); };
  set_mapping[2] = []() {
    return std::make_unique<LIB::OrdUnidimDenseSetFact>();
  };
  set_mapping.freeze();
  return set_mapping;
}

UserImplMap::StructImplMap pwMap()
{
  UserImplMap::StructImplMap pw_mapping;
  pw_mapping[0] = []() { return std::make_unique<LIB::UnordPWMapFact>(); };
  pw_mapping[1] = []() { return std::make_unique<LIB::OrdPWMapFact>(); };
  pw_mapping[2] = []() { return std::make_unique<LIB::DomOrdPWMapFact>(); };
  pw_mapping.freeze();
  return pw_mapping;
}

UserImplMap::StructImplMap matchMap()
{
  UserImplMap::StructImplMap match_mapping;
  match_mapping[0] = []() { return std::make_unique<LIB::BFSMatchingFact>(); };
  match_mapping.freeze();
  return match_mapping;
}

UserImplMap::StructImplMap sccMap()
{
  UserImplMap::StructImplMap scc_mapping;
  scc_mapping[0] = []() { return std::make_unique<LIB::MinReachSCCV1Fact>(); };
  scc_mapping[1] = []() { return std::make_unique<LIB::MinReachSCCV2Fact>(); };
  scc_mapping.freeze();
  return scc_mapping;
}

UserImplMap::StructImplMap tsMap()
{
  UserImplMap::StructImplMap ts_mapping;
  ts_mapping[0] = []() { return std::make_unique<LIB::MinVertexTSFact>(); };
  ts_mapping.freeze();
  return ts_mapping;
}

UserImplMap::StructImplMap cvMap()
{
  UserImplMap::StructImplMap cv_mapping;
  cv_mapping[0] = []() { return std::make_unique<LIB::MaxDegCVFact>(); };
  cv_mapping.freeze();
  return cv_mapping;
}

////////////////////////////////////////////////////////////////////////////////
// User Implementation Map -----------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

UserImplMap::UserImplMap()
{
  implementations_["set"] = setMap();
  implementations_["pw"] = pwMap();
  implementations_["match"] = matchMap();
  implementations_["scc"] = sccMap();
  implementations_["ts"] = tsMap();
  implementations_["cv"] = cvMap();
}

ImplFactory UserImplMap::getFactory(std::string strct, int impl)
{
  const StructImplMap& struct_map = implementations_[strct];
  return struct_map[impl]();
}

////////////////////////////////////////////////////////////////////////////////
// Extra operations ------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

void setSetFactory(int set_impl)
{
  LIB::SetFactPtr set_fact = std::get<LIB::SetFactPtr>(IMPL_MAP.getFactory("set"
    , set_impl));
  LIB::SetFactory::instance().set_set_fact(std::move(set_fact));

  return;
}

void setPWFactory(int pw_impl)
{
  LIB::PWMapFactPtr pw_fact = std::get<LIB::PWMapFactPtr>(
    IMPL_MAP.getFactory("pw", pw_impl));
  LIB::PWFactory::instance().set_pw_fact(std::move(pw_fact));

  return;
}

void setSCCFactory(int scc_impl)
{
  LIB::SCCFactPtr scc_fact = std::get<LIB::SCCFactPtr>(IMPL_MAP.getFactory("scc"
    , scc_impl));
  LIB::SCCFactory::instance().set_scc_fact(std::move(scc_fact));

  return;
}

} // namespace Eval

} // namespace SBG
