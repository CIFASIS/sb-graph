/*******************************************************************************

 This file is part of Set--Based Graph Library.

 SBG Library is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 ******************************************************************************/

// Translation unit kept to (a) include the header so the template definitions
// are syntactically checked at library build time, and (b) give CMake a
// non-empty target_sources entry. ESBG is a class template, so the actual
// instantiations live wherever clients use it.

#include "algorithms/structure/esbg.hpp"
