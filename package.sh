#!/bin/bash
# Package the whole application in the build/package/ folder.
# The `create-shortcut` command must be available, for instance through Git Bash.

set -e
mkdir -p build/
cd build/

SOLVER_BUILD_PATH=$(find solver/ -maxdepth 1 -type d -iname '*Release*')
if [ -z "$SOLVER_BUILD_PATH" ]; then
	echo "Solver build folder not found"
	exit
fi

rm -rf package/
mkdir package/
cd package/

cp -rp "../${SOLVER_BUILD_PATH}/package" solver/
cp -rp "../user-interface/browser" user-interface/
cp -p "../user-interface/3rdpartylicenses.txt" user-interface/
create-shortcut "${PWD}/solver/KholGen.exe" "${PWD}/KholGen.lnk"
