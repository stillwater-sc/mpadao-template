---
name: fix-issue
description: Analyze a GitHub issue and propose or implement a fix
argument-hint: "<issue number>"
disable-model-invocation: true
---

Workflow for fixing GitHub issue #$ARGUMENTS:

1. Fetch issue details: gh issue view $ARGUMENTS
2. Read all comments: gh api repos/{owner}/{repo}/issues/$ARGUMENTS/comments
3. Understand the problem:
   - What is the expected behavior?
   - What is the actual behavior?
   - What components are affected?
4. Search the codebase for relevant code using Grep and Glob
5. Identify the root cause
6. Present a fix plan to the user — do NOT implement without approval
7. After user approval:
   - Create a feature branch: git checkout -b fix/issue-$ARGUMENTS
   - Implement the fix
   - Build and test: cd build && cmake .. && make -j$(nproc) && ctest
   - Commit with "Fixes #$ARGUMENTS" in the message
   - Offer to create a PR
