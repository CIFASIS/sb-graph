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

#include "algorithms/scc/scc_fact.hpp"
#include "eval/user_impl_map.hpp"
#include "sbg/pw_map.hpp"
#include "sbg/pwmap_fact.hpp"
#include "sbg/set.hpp"
#include "sbg/set_fact.hpp"
#include "util/debug.hpp"

namespace SBG {

namespace Eval {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Single Structure Implementation Map -----------------------------------------
////////////////////////////////////////////////////////////////////////////////

UserImplMap::StructImplMap::StructImplMap() : _is_frozen(false) {}

UserImplMap::StructImplMap::SValue& UserImplMap::StructImplMap::operator[]
  (const SKey& key)
{
  Util::ERROR_UNLESS(!_is_frozen, "StructImplMap: map is frozen, insertion is"
    " prohibited.\n");
  return _struct_impls[key];
}

const UserImplMap::StructImplMap::SValue& UserImplMap::StructImplMap::operator[]
  (const SKey& key) const
{
  Util::ERROR_UNLESS(_struct_impls.find(key) != _struct_impls.end()
    , "StructImplMap: undefined implementation.\n");
  return _struct_impls.at(key); 
}

void UserImplMap::StructImplMap::freeze()
{
  _is_frozen = true;
}

UserImplMap::StructImplMap setMap()
{
  UserImplMap::StructImplMap set_mapping;
  set_mapping[0] = LIB::SetKind::kUnordered;
  set_mapping[1] = LIB::SetKind::kOrdered;
  set_mapping[2] = LIB::SetKind::kOrdUnidimDense;
  set_mapping.freeze();
  return set_mapping;
}

UserImplMap::StructImplMap pwMap()
{
  UserImplMap::StructImplMap pwmap_mapping;
  pwmap_mapping[0] = LIB::PWMapKind::kUnordered;
  pwmap_mapping[1] = LIB::PWMapKind::kOrdered;
  pwmap_mapping[2] = LIB::PWMapKind::kDomOrdered;
  pwmap_mapping.freeze();
  return pwmap_mapping;
}

UserImplMap::StructImplMap matchMap()
{
  UserImplMap::StructImplMap match_mapping;
  match_mapping[0] = LIB::MatchKind::kBFSPaths;
  match_mapping.freeze();
  return match_mapping;
}

UserImplMap::StructImplMap sccMap()
{
  UserImplMap::StructImplMap scc_mapping;
  scc_mapping[0] = LIB::SCCKind::kMinReachV1;
  scc_mapping[1] = LIB::SCCKind::kMinReachV2;
  scc_mapping.freeze();
  return scc_mapping;
}

UserImplMap::StructImplMap mfvsMap()
{
  UserImplMap::StructImplMap mfvs_mapping;
  mfvs_mapping[0] = LIB::MFVSKind::kGreedy;
  mfvs_mapping.freeze();
  return mfvs_mapping;
}

UserImplMap::StructImplMap tsMap()
{
  UserImplMap::StructImplMap ts_mapping;
  ts_mapping[0] = LIB::TSKind::kMinVertex;
  ts_mapping.freeze();
  return ts_mapping;
}

////////////////////////////////////////////////////////////////////////////////
// User Implementation Map -----------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

UserImplMap::UserImplMap()
{
  implementations_["set"] = setMap();
  implementations_["pwmap"] = pwMap();
  implementations_["match"] = matchMap();
  implementations_["scc"] = sccMap();
  implementations_["mfvs"] = mfvsMap();
  implementations_["ts"] = tsMap();
}

Kind UserImplMap::getFactory(std::string strct, int impl)
{
  const StructImplMap& struct_map = implementations_[strct];
  return struct_map[impl];
}

} // namespace detail

////////////////////////////////////////////////////////////////////////////////
// Extra operations ------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

void setSetFactory(int set_impl)
{
  LIB::SetKind set_fact = std::get<LIB::SetKind>(detail::IMPL_MAP.getFactory(
    "set", set_impl));
  LIB::SET_FACT.set_set_fact(set_fact);
}

void setPWFactory(int pw_impl)
{
  LIB::PWMapKind pwmap_fact = std::get<LIB::PWMapKind>(
    detail::IMPL_MAP.getFactory("pwmap", pw_impl));
  LIB::PWMAP_FACT.set_pwmap_fact(pwmap_fact);
}

void setSCCFactory(int scc_impl)
{
  LIB::SCCKind scc_fact = std::get<LIB::SCCKind>(
    detail::IMPL_MAP.getFactory("scc", scc_impl));
  LIB::SCC_FACT.set_scc_fact(scc_fact);

  return;
}

} // namespace Eval

} // namespace SBG
