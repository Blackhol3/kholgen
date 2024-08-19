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

if [ -d package/ ]; then
	echo "The whole ${PWD}/package folder will be emptied. Continue?"
	select yn in "Yes" "No"; do
		case $yn in
			Yes) break;;
			No) exit;;
		esac
	done
fi

rm -rf package/
mkdir package/
cd package/

cp -r "../${SOLVER_BUILD_PATH}/package" solver/
cp -r "../user-interface/browser" user-interface/
cp "../user-interface/3rdpartylicenses.txt" user-interface/
create-shortcut "${PWD}/solver/KholGen.exe" "${PWD}/KholGen.lnk"
