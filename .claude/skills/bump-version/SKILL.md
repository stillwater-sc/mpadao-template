---
name: bump-version
description: Bump the MPADAO semantic version (major, minor, or patch)
argument-hint: "<major|minor|patch>"
disable-model-invocation: true
allowed-tools: Read, Edit, Bash
---

Bump the project version in CMakeLists.txt.

1. Read the root CMakeLists.txt to find current MPADAO_MAJOR, MPADAO_MINOR, MPADAO_PATCH
2. Based on $ARGUMENTS:
   - "major": increment major, reset minor and patch to 0
   - "minor": increment minor, reset patch to 0
   - "patch": increment patch
3. Edit CMakeLists.txt with the new values
4. Rebuild to verify version propagates: cd build && cmake .. && make semver && ./bin/semver
5. Report the old and new version numbers
