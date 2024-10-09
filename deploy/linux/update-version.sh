#!/bin/bash
#===================================================================================
#
# 				 FILE: update-version.sh  
#
# 				USAGE: update-version.sh
#
# 	DESCRIPTION: Helper script to update version info in all project related files.
#
#    PARAMETERS: ---
#       OPTIONS: ---
#  REQUIREMENTS: ---
#         NOTES: --- 
#        AUTHOR: Joaquin Fernandez, joaquin.f.fernandez@gmail.com
#       PROJECT: Set Based Graph Library
#===================================================================================

VER=`cat ./version`

# Update deploy files.
sed -i "s/VERSION:.*/VERSION: $VER/" ./build.sh

# Update doxyfiles.
sed -i "s/PROJECT_NUMBER =.*/PROJECT_NUMBER = $VER/" ../../sbg/SBG.doxyfile
sed -i "s/PROJECT_NUMBER =.*/PROJECT_NUMBER = $VER/" ../../util/UTIL.doxyfile

# Update main bynary files version.
sed -i "s/SBG library v.*/SBG library v$VER\" << std::endl;/" ../../parser/main.cpp
sed -i "s/SBG library v.*/SBG library v$VER\" << std::endl;/" ../../eval/main.cpp