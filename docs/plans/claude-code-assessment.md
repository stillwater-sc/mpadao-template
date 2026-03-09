# Implement Claude Code Automation Assessment Recommendations

## Context

The assessment document (`docs/claude-code-automation-assessment.md`) identified 13 improvements to the MPADAO development workflow. This plan implements all of them. Currently, the project has one subagent (`code-docs-writer`), no skills, no hooks, and no `.gitignore` entries for personal Claude Code files. The `.claude/settings.local.json` has 11 narrow `gh api` permission entries from the MTL4-to-MTL5 migration.

## Implementation Steps

### Step 1: Update `.gitignore` for personal Claude Code files

**File:** `.gitignore`

Add entries so personal/machine-local files aren't committed while team-shared files are:
```
# Claude Code personal/machine-local files
.claude/settings.local.json
.claude/agent-memory/
.claude/plans/
```

Team-shared files that SHOULD be committed: `.claude/settings.json`, `.claude/skills/`, `.claude/agents/`

---

### Step 2: Broaden `gh` CLI permissions (Roadmap #1)

**File:** `.claude/settings.local.json`

Replace the 11 specific `gh api` entries with 4 wildcard patterns:
```json
{
  "permissions": {
    "allow": [
      "Bash(gh issue *)",
      "Bash(gh pr *)",
      "Bash(gh run *)",
      "Bash(gh api repos/stillwater-sc/*)"
    ]
  }
}
```

---

### Step 3: Create project skills (Roadmap #2, #3, #5, #6, #11)

Create `.claude/skills/` directory and 5 skill files. Content for each is fully specified in the assessment document sections 3a-3e.

**Files to create:**
- `.claude/skills/scaffold-app/SKILL.md`
- `.claude/skills/build-test/SKILL.md`
- `.claude/skills/check-ci/SKILL.md`
- `.claude/skills/fix-issue/SKILL.md`
- `.claude/skills/bump-version/SKILL.md`

---

### Step 4: Create team-shared hooks (Roadmap #4, #13)

**File:** `.claude/settings.json` (new — team-shared, checked into git)

Combine the submodule protection hook and desktop notification hook:
```json
{
  "hooks": {
    "PreToolUse": [
      {
        "matcher": "Edit|Write",
        "hooks": [
          {
            "type": "command",
            "command": "jq -r '.tool_input.file_path // empty' | grep -q '/ext/' && { echo 'BLOCKED: Do not modify submodule files in ext/' >&2; exit 2; } || exit 0"
          }
        ]
      }
    ],
    "Notification": [
      {
        "hooks": [
          {
            "type": "command",
            "command": "notify-send 'Claude Code' \"$(jq -r '.message // \"Task complete\"')\" 2>/dev/null || true"
          }
        ]
      }
    ]
  }
}
```

Note: The prompt-based build verification hook (assessment item 2c) is intentionally excluded — it adds latency to every response and may be more disruptive than helpful. Can be added later if desired.

---

### Step 5: Create specialized subagents (Roadmap #7, #8, #9)

**Files to create:**
- `.claude/agents/build-doctor.md`
- `.claude/agents/benchmark-analyst.md`
- `.claude/agents/issue-researcher.md`

Content for each is fully specified in the assessment document section 4a-4c. These sit alongside the existing `code-docs-writer.md`.

---

### Step 6: Update CLAUDE.md (Roadmap #10)

**File:** `CLAUDE.md`

Two changes:
1. **Fix macOS version**: Change "macOS 13" to "macOS 15" in the CI/CD section
2. **Append new sections** at the end (before any closing matter):
   - Common Development Tasks
   - Branch Strategy
   - Testing Expectations
   - Known Constraints

Content is specified in assessment section 5.

---

### Step 7: Add GitHub Actions Claude workflow (Roadmap #12)

**File:** `.github/workflows/claude.yml`

Create the `claude-code-action` workflow as specified in assessment section 6. This enables `@claude` mentions in issues and PRs.

**Prerequisite:** The user must add `ANTHROPIC_API_KEY` to the repository's GitHub Actions secrets. This step will be flagged in the output but not automated (it requires GitHub web UI or `gh secret set`).

---

## File Summary

| File | Action | Shared? |
|------|--------|---------|
| `.gitignore` | Edit | Yes |
| `.claude/settings.local.json` | Edit | No (personal) |
| `.claude/settings.json` | Create | Yes (team) |
| `.claude/skills/scaffold-app/SKILL.md` | Create | Yes |
| `.claude/skills/build-test/SKILL.md` | Create | Yes |
| `.claude/skills/check-ci/SKILL.md` | Create | Yes |
| `.claude/skills/fix-issue/SKILL.md` | Create | Yes |
| `.claude/skills/bump-version/SKILL.md` | Create | Yes |
| `.claude/agents/build-doctor.md` | Create | Yes |
| `.claude/agents/benchmark-analyst.md` | Create | Yes |
| `.claude/agents/issue-researcher.md` | Create | Yes |
| `CLAUDE.md` | Edit | Yes |
| `.github/workflows/claude.yml` | Create | Yes |

**Total: 13 files (1 edit-only, 10 new, 2 edit existing)**

## Verification

After implementation:
1. Run `ls .claude/skills/` — should show 5 skill directories
2. Run `ls .claude/agents/` — should show 4 agent files (3 new + code-docs-writer)
3. Start a new Claude Code session and verify:
   - `/scaffold-app`, `/build-test`, `/check-ci`, `/fix-issue`, `/bump-version` appear as available skills
   - Attempting to edit a file in `ext/` is blocked by the hook
4. Verify `.gitignore` excludes personal files: `git status` should not show `settings.local.json` or `agent-memory/`
5. Check the GitHub Actions workflow parses correctly: `gh workflow list` after push
6. Reminder: user must set `ANTHROPIC_API_KEY` secret in GitHub for the claude.yml workflow to function
