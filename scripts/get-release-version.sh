#!/bin/sh

# Get string representation of the current CAF version.
caf_version=`grep "define CAF_VERSION" libcaf_core/caf/config.hpp | awk '{ printf "%d.%d.%d", int($3 / 10000), int($3 / 100) % 100, $3 % 100 }'`

# Get string representation of the CAF version in the previous commit.
prev_caf_version=`git show HEAD~1:libcaf_core/caf/config.hpp | grep "define CAF_VERSION" | awk '{ printf "%d.%d.%d", int($3 / 10000), int($3 / 100) % 100, $3 % 100 }'`

# Check whether this commit is a version change.
if [ "$caf_version" != "$prev_caf_version" ] ; then
  # Version changes mark a stable release.
  echo "build stable release version: $caf_version"
  echo "$caf_version" >release.txt
else
  # Get SHA from Git and generate '+exp.sha' version for this commit.
  git_sha=`git log --pretty=format:%h -n 1`
  caf_release_version="$caf_version+exp.sha.$git_sha"
  echo "build commit version: $caf_release_version"
  echo "$caf_release_version" >release.txt
fi
