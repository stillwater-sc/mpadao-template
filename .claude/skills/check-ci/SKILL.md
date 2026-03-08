---
name: check-ci
description: Check CI status for current branch, a PR, or recent runs
argument-hint: "[PR number, branch name, or empty for recent runs]"
disable-model-invocation: true
allowed-tools: Bash
---

Check CI status for the MPADAO project:

1. If $ARGUMENTS is a number, check PR: gh pr checks $ARGUMENTS
2. If $ARGUMENTS is a branch name: gh run list --branch $ARGUMENTS --limit 5
3. If no arguments: gh run list --limit 5

For any failed runs:
4. Get the run ID and fetch the log: gh run view <id> --log-failed | tail -100
5. Analyze the failure and identify root cause
6. Summarize: what passed, what failed, the failure reason, and suggested fix
