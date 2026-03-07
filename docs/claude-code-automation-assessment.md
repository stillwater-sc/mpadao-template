# Claude Code Automation Assessment for MPADAO

> Assessment date: 2026-03-06
> Repository: stillwater-sc/mpadao-template

## Table of Contents

- [How It Actually Works](#how-it-actually-works)
- [Current Configuration Audit](#current-configuration-audit)
- [Recommendations](#recommendations)
  - [1. GitHub Integration: gh CLI](#1-github-integration-gh-cli)
  - [2. Hooks: Lifecycle Automation](#2-hooks-lifecycle-automation)
  - [3. Project Skills](#3-project-skills)
  - [4. Specialized Subagents](#4-specialized-subagents)
  - [5. CLAUDE.md Improvements](#5-claudemd-improvements)
  - [6. GitHub Actions Integration](#6-github-actions-integration)
- [Note on MCP Servers](#note-on-mcp-servers)
- [Implementation Roadmap](#implementation-roadmap)
- [Reference: Configuration File Locations](#reference-configuration-file-locations)

---

## How It Actually Works

### Demystifying "Markdown Intelligence"

Claude Code is not magic — it's a loop: **read context, reason, act, observe, repeat**. The markdown files (`.claude/`, `CLAUDE.md`, skills, memory) are **persistent context injection** — they shape what Claude sees at the start of every turn. Better context = better autonomous decisions.

| Layer | What It Does | Analogy |
|-------|-------------|---------|
| `CLAUDE.md` | Project rules, always loaded | Senior engineer's onboarding doc |
| `MEMORY.md` | Accumulated session learnings | Personal notebook across sessions |
| Skills (`.claude/skills/`) | Reusable multi-step workflows | Runbooks / SOPs |
| Subagents (`.claude/agents/`) | Specialist workers with restricted tools | Team members with defined roles |
| Hooks (`settings.json`) | Automatic triggers on events | Git hooks / CI triggers |
| MCP servers (`.mcp.json`) | External tool integrations | API clients / plugins |

### The Automation Stack

```
                    ┌──────────────────────────────────┐
                    │     GitHub Actions (claude.yml)  │  ← @claude in issues/PRs
                    └───────────────┬──────────────────┘
                                    │
     ┌─────────────┬────────────────▼───────────┬──────────────┐
     │   Skills    │       Claude Code Core     │   Subagents  │
     │ (workflows) │    (reasoning + tools)     │ (specialists)│
     └──────┬──────┴────┬───────────┬───────────┴──────┬───────┘
            │           │           │                  │
     ┌──────▼──────┐  ┌─▼────┐  ┌──▼───────┐   ┌──────▼───────┐
     │    Hooks    │  │  gh  │  │ CLAUDE.md │   │    Memory    │
     │ (triggers)  │  │ CLI  │  │  (rules)  │   │  (learnings) │
     └─────────────┘  └──────┘  └──────────┘   └──────────────┘
```

Each layer adds capability:
- **CLAUDE.md** tells Claude *what the project is* and *how to work in it*
- **Skills** give Claude *reusable recipes* for common tasks
- **Subagents** let Claude *delegate* to specialists with restricted tools
- **Hooks** make things happen *automatically* at lifecycle events (no human trigger needed)
- **`gh` CLI** gives Claude *native access* to GitHub (issues, PRs, CI, code search)
- **GitHub Actions** let Claude work *asynchronously* on issues and PRs
- **MCP servers** (optional) integrate services without CLI equivalents (Notion, Figma, etc.)

---

## Current Configuration Audit

### What Exists

| Category | Status | Details |
|----------|--------|---------|
| Project `CLAUDE.md` | Good | Build commands, architecture, conventions, CI, Docker |
| Global skills | 4 commands | `/cpp-review`, `/cpp-optimize`, `/cpp-modernize`, `/wrapup` |
| Global skill | 1 skill | `cpp-model-invariant` (5-phase invariant-first code generation) |
| Project subagent | 1 agent | `code-docs-writer` (Haiku model, Doxygen specialist) |
| Hooks | **None** | No lifecycle automation configured |
| GitHub CLI (`gh`) | Installed | Authenticated, used for `gh api` calls to remote repos |
| MCP servers | **None** | Not needed — `gh` CLI is the recommended GitHub integration |
| Memory | **Empty** | Directory exists at `~/.claude/projects/.../memory/` but no files |
| GitHub templates | **None** | No issue or PR templates in `.github/` |
| CI workflow | Basic | 3-platform matrix, no Boost install, no artifact caching |

### What's in `settings.local.json`

Currently contains 11 pre-approved `gh api` bash commands for reading the `stillwater-sc/mtl5` repository remotely. These were added manually during the MTL4-to-MTL5 migration. Consider broadening these to wildcard patterns (e.g., `Bash(gh api repos/stillwater-sc/*)`) to reduce friction for future cross-repo queries.

---

## Recommendations

### 1. GitHub Integration: gh CLI

**Impact: High | Effort: Already done (gh is installed and authenticated)**

Anthropic's official best practices recommend the `gh` CLI over the GitHub MCP server for interactive local development. Claude already knows `gh` natively — it was trained extensively on its usage and can invoke it fluently without in-context documentation.

#### Why `gh` CLI over GitHub MCP Server

| Factor | `gh` CLI | GitHub MCP Server |
|--------|----------|-------------------|
| Model knowledge | Native — trained on extensive `gh` usage | Requires loading tool descriptions into context |
| Context efficiency | Zero overhead — just a Bash call | Adds dozens of tool definitions, inflating context |
| Tool count | No increase | Balloons tool count, empirically degrades tool selection |
| Setup | Already authenticated | Requires Docker or PAT-authenticated HTTP transport |
| Maintenance | Follows system package updates | npm package deprecated (April 2025), requires Docker |

#### What `gh` Already Provides

Claude can already do all of this via Bash:
- **Issues**: `gh issue list`, `gh issue view`, `gh issue create`
- **PRs**: `gh pr list`, `gh pr view`, `gh pr create`, `gh pr checks`
- **CI status**: `gh run list`, `gh run view --log-failed`
- **Code review**: `gh api repos/{owner}/{repo}/pulls/{n}/comments`
- **Cross-repo queries**: `gh api repos/stillwater-sc/universal/contents/...`
- **Releases**: `gh release list`, `gh release create`

#### Recommended Permission Improvements

Broaden the `settings.local.json` permissions to reduce friction:

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

This replaces the 11 specific `gh api` entries with 4 wildcard patterns covering all common GitHub operations across the Stillwater organization.

#### When MCP *Does* Make Sense

MCP servers earn their place for services that **don't have a capable CLI equivalent**: Notion, Figma, Sentry, custom databases, or internal APIs. For GitHub specifically, the CLI is the better path.

---

### 2. Hooks: Lifecycle Automation

**Impact: High | Effort: 10 minutes**

Hooks are shell commands that fire automatically at specific lifecycle events. Unlike skills (which Claude chooses to use), hooks are **deterministic** — they always execute when their trigger condition is met.

#### Available Hook Events (Most Useful for MPADAO)

| Event | When It Fires | Use Case |
|-------|---------------|----------|
| `PreToolUse` | Before any tool call | Block edits to protected files |
| `PostToolUse` | After a tool succeeds | Format code, validate changes |
| `Stop` | Claude finishes responding | Verify build still works |
| `SessionStart` | Session begins | Load project state |
| `Notification` | Claude needs attention | Desktop notifications |

#### Recommended Hooks

**a) Protect submodule directories** — prevent accidentally editing Universal/MTL5/GoogleTest code:

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
    ]
  }
}
```

How it works: Every time Claude tries to `Edit` or `Write` a file, this hook checks if the path contains `/ext/`. If so, it exits with code 2 (block), and Claude sees the error message and adjusts its approach.

**b) Desktop notifications** — get notified when Claude finishes a long task:

```json
{
  "hooks": {
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

**c) Prompt-based build verification** — an LLM checks whether a build test is needed:

```json
{
  "hooks": {
    "Stop": [
      {
        "hooks": [
          {
            "type": "prompt",
            "prompt": "Were any C++ source files (.cpp, .hpp, CMakeLists.txt) modified in this conversation turn? If yes, respond {\"decision\": \"block\", \"reason\": \"Run build verification: cd build && cmake .. && make -j$(nproc) && ctest\"}. If only non-code files changed or no files changed, respond {\"decision\": \"allow\"}."
          }
        ]
      }
    ]
  }
}
```

#### Where Hooks Are Configured

| File | Scope | Shared? |
|------|-------|---------|
| `.claude/settings.json` | Project (team) | Yes, checked into git |
| `.claude/settings.local.json` | Project (personal) | No, gitignored |
| `~/.claude/settings.json` | Global (all projects) | No |

---

### 3. Project Skills

**Impact: High | Effort: 5-15 minutes each**

Skills are reusable workflows invoked with `/skill-name`. Each is a markdown file at `.claude/skills/<name>/SKILL.md`.

#### a) `/scaffold-app` — Generate new app from template

All MPADAO apps follow an identical CMakeLists.txt + main.cpp pattern. This skill eliminates the boilerplate:

```yaml
---
name: scaffold-app
description: Scaffold a new MPADAO app or benchmark with CMakeLists.txt boilerplate
argument-hint: "<category> <name> — e.g., 'high-precision fft' or 'benchmark memory'"
disable-model-invocation: true
---

Create a new MPADAO application or benchmark. Parse $ARGUMENTS to determine placement:

If first argument is "benchmark":
  - Create in benchmark/<name>/
  - Follow the CMakeLists.txt pattern from benchmark/accuracy/
  - Add add_subdirectory() to benchmark/CMakeLists.txt
  - Set folder to "Benchmarks/<Name>"

Otherwise:
  - Create in src/apps/<category>/<name>/
  - Follow the CMakeLists.txt pattern from src/apps/high-precision/digits_of_pi/
  - Add add_subdirectory() to src/CMakeLists.txt
  - Set folder to "Applications/<Category>"

For all targets:
1. Create the directory
2. Create CMakeLists.txt with:
   - cmake_minimum_required(VERSION 3.22)
   - Appropriate app_name, include paths, folder property
   - install(TARGETS ...) line
3. Create main.cpp with:
   - Standard copyright header (Stillwater Supercomputing, Inc., MIT license)
   - try/catch main() pattern matching existing apps
   - Appropriate Universal/MTL5 includes based on the app's purpose
4. Wire up the parent CMakeLists.txt
5. Build to verify: cd build && cmake .. && make <target> -j$(nproc)
```

#### b) `/build-test` — Build, test, report

```yaml
---
name: build-test
description: Build the MPADAO project and run tests, reporting results concisely
disable-model-invocation: true
allowed-tools: Bash
---

Build and test the MPADAO project:

1. Configure: cd build && cmake .. 2>&1 | tail -5
2. Build: make -j$(nproc) 2>&1
3. Test: ctest --output-on-failure 2>&1
4. Report:
   - Which targets built successfully
   - Any compiler warnings (summarize, don't dump full output)
   - Which tests passed/failed
   - If any test failed, show the relevant failure output
```

#### c) `/check-ci` — Check GitHub Actions CI status

```yaml
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
```

#### d) `/fix-issue` — Analyze and fix a GitHub issue

```yaml
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
```

#### e) `/bump-version` — Semantic version management

```yaml
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
```

---

### 4. Specialized Subagents

**Impact: Medium | Effort: 10 minutes each**

Subagents are specialist Claude instances with restricted tools, custom prompts, and their own memory. They run in isolated contexts and return results to the main conversation.

#### a) `build-doctor` — Diagnose build failures

```yaml
# .claude/agents/build-doctor.md
---
name: build-doctor
description: Diagnose C++ build failures, linker errors, and CMake configuration issues. Use proactively when builds fail.
tools: Read, Grep, Glob, Bash
model: sonnet
maxTurns: 15
memory: project
---

You are a C++20/CMake build failure specialist for the MPADAO project.

When invoked with a build error:

1. Parse the error output to identify: source file, line number, error type
2. Read the failing source file around the error line
3. Classify the error:
   - **Include error**: Check CMakeLists.txt for missing include_directories()
   - **Template error**: Trace the instantiation chain, check type constraints
   - **Linker error**: Check target_link_libraries(), symbol visibility
   - **CMake error**: Check variable definitions, option dependencies
4. Search for similar patterns in working code (Grep for the same includes/types)
5. Propose a minimal, targeted fix

Key project facts:
- Universal headers included via: include_directories(${MPADAO_ROOT_DIR}/ext/stillwater-sc/universal/include/sw)
- MTL5 headers included via: include_directories(${MPADAO_ROOT_DIR}/ext/stillwater-sc/mtl5/include)
- Algorithm headers: include_directories(${MPADAO_ROOT_DIR}/src/algorithms)
- C++20 enforced, namespaces: sw::universal, mpadao::, sw::hprblas
- Boost is optional (only for solvers/)
```

#### b) `benchmark-analyst` — Run and interpret benchmarks

```yaml
# .claude/agents/benchmark-analyst.md
---
name: benchmark-analyst
description: Run benchmarks and analyze numerical results for accuracy, precision, performance, and convergence
tools: Read, Bash, Grep, Glob
model: sonnet
maxTurns: 20
memory: project
---

You are a numerical computing benchmark analyst for the MPADAO project.

Available benchmark targets:
- accuracy_benchmark: Basel series error across number types
- precision_benchmark: ULP analysis and iterative precision loss
- tts: dot product time-to-solution performance
- convergence_benchmark: pi formula convergence rates and precision ceilings

When invoked:
1. Build all benchmark targets: cd build && cmake .. && make -j$(nproc) accuracy_benchmark precision_benchmark tts convergence_benchmark 2>&1
2. Run each benchmark and capture output
3. Analyze results:
   - Accuracy: error magnitudes, which types perform best
   - Precision: where ULP loss accumulates
   - Performance: time-to-solution scaling characteristics
   - Convergence: digits-per-term efficiency, precision ceiling patterns
4. Cross-reference: do accuracy results explain convergence limits?
5. Flag anomalies or regressions
6. Produce a clear summary with tables
```

#### c) `issue-researcher` — Investigate GitHub issues

```yaml
# .claude/agents/issue-researcher.md
---
name: issue-researcher
description: Research a GitHub issue by analyzing the codebase, identifying root cause, and proposing solutions
tools: Read, Grep, Glob, Bash
model: sonnet
maxTurns: 25
memory: project
---

You are a research analyst for the MPADAO project. Your job is to investigate
GitHub issues and produce actionable findings — you do NOT implement fixes.

When given an issue number or description:

1. Fetch the issue: gh issue view <number> --json title,body,comments,labels
2. Extract key information: symptoms, reproduction steps, affected components
3. Search the codebase:
   - Grep for mentioned files, functions, types, error messages
   - Read relevant source files
   - Check CMakeLists.txt for build configuration issues
   - Check CI workflow for environment-specific issues
4. Identify root cause (or top 2-3 candidates if ambiguous)
5. Assess impact: what else might be affected?
6. Propose solutions ranked by:
   - Correctness (does it fix the root cause?)
   - Risk (what could break?)
   - Effort (how many files change?)
7. Return a structured report with: Summary, Root Cause, Impact, Proposed Fix(es)

Do NOT modify any files. Research only.
```

---

### 5. CLAUDE.md Improvements

**Impact: Medium | Effort: 10 minutes**

Your current `CLAUDE.md` is solid. These additions would help Claude make better autonomous decisions:

#### Additions to append

```markdown
## Common Development Tasks

When adding a new application:
1. Create directory under src/apps/<category>/<name>/
2. Follow the CMakeLists.txt pattern from digits_of_pi (see scaffold-app skill)
3. Add add_subdirectory() to src/CMakeLists.txt in the appropriate section
4. Build and verify before committing

When adding a new benchmark:
1. Create directory under benchmark/<name>/
2. Follow the CMakeLists.txt pattern from benchmark/accuracy/
3. Add add_subdirectory() to benchmark/CMakeLists.txt

## Branch Strategy

- `dev` -> `integration` -> `main` (three-tier)
- Feature branches from `dev`
- PRs target `integration` or `main`
- CI triggers on push to dev/integration/main and PRs to integration/main

## Testing Expectations

- All new apps should produce verifiable output (reference values, assertions)
- Benchmarks should include hardcoded reference data for validation
- Run ctest before committing to verify no regressions

## Known Constraints

- ereal<19> arithmetic is very slow (~minutes for 300-digit computations).
  Use ereal<8> (127 digits) for interactive demos.
- CI does not install Boost — solvers/ is not exercised in CI.
- Version is managed centrally in root CMakeLists.txt
  (MPADAO_MAJOR/MINOR/PATCH) and propagated via configure_file.
```

#### Corrections

The current `CLAUDE.md` says CI targets "macOS 13" but the workflow actually targets `macos-15`. This should be updated.

---

### 6. GitHub Actions Integration

**Impact: High | Effort: 15 minutes**

The `anthropics/claude-code-action` GitHub Action enables Claude to respond to `@claude` mentions in issues and PRs. This is how you get "automatic issue analysis and fixing."

#### Workflow File

```yaml
# .github/workflows/claude.yml
name: Claude Code

on:
  issue_comment:
    types: [created]
  pull_request_review_comment:
    types: [created]
  issues:
    types: [opened, labeled]

jobs:
  claude:
    if: |
      (github.event_name == 'issue_comment' && contains(github.event.comment.body, '@claude')) ||
      (github.event_name == 'pull_request_review_comment' && contains(github.event.comment.body, '@claude')) ||
      (github.event_name == 'issues' && contains(github.event.issue.labels.*.name, 'claude'))
    runs-on: ubuntu-latest
    permissions:
      contents: write
      pull-requests: write
      issues: write
    steps:
      - uses: actions/checkout@v4
        with:
          submodules: recursive
          fetch-depth: 0

      - uses: anthropics/claude-code-action@v1
        with:
          anthropic_api_key: ${{ secrets.ANTHROPIC_API_KEY }}
```

#### What This Enables

- Comment `@claude analyze this issue` on any issue — Claude reads the issue, searches the codebase, and posts an analysis
- Comment `@claude fix this` on an issue — Claude creates a PR with the fix
- Comment `@claude` on a PR review comment — Claude implements the requested change and pushes
- Label an issue with `claude` — Claude automatically investigates it

#### Prerequisites

- Add `ANTHROPIC_API_KEY` to the repository's GitHub Actions secrets
- The action respects your `CLAUDE.md` rules, skills, and project configuration

---

## Implementation Roadmap

| Priority | What | Effort | Impact | File(s) to Create/Edit |
|----------|------|--------|--------|----------------------|
| **1** | Broaden `gh` CLI permissions | 2 min | High | `.claude/settings.local.json` |
| **2** | `/scaffold-app` skill | 10 min | High | `.claude/skills/scaffold-app/SKILL.md` |
| **3** | `/build-test` skill | 5 min | High | `.claude/skills/build-test/SKILL.md` |
| **4** | Submodule protection hook | 5 min | High | `.claude/settings.json` |
| **5** | `/check-ci` skill | 5 min | Medium | `.claude/skills/check-ci/SKILL.md` |
| **6** | `/fix-issue` skill | 10 min | Medium | `.claude/skills/fix-issue/SKILL.md` |
| **7** | `build-doctor` subagent | 10 min | Medium | `.claude/agents/build-doctor.md` |
| **8** | `benchmark-analyst` subagent | 10 min | Medium | `.claude/agents/benchmark-analyst.md` |
| **9** | `issue-researcher` subagent | 10 min | Medium | `.claude/agents/issue-researcher.md` |
| **10** | CLAUDE.md updates | 10 min | Medium | `CLAUDE.md` |
| **11** | `/bump-version` skill | 5 min | Low | `.claude/skills/bump-version/SKILL.md` |
| **12** | GitHub Actions `claude.yml` | 15 min | High | `.github/workflows/claude.yml` |
| **13** | Desktop notification hook | 2 min | Low | `.claude/settings.json` |

**Total estimated effort: ~100 minutes for all items.**

Items 1-4 give the biggest immediate payoff. Items 5-9 build the autonomous workflow. Items 10-13 complete the picture.

> **Note:** No MCP server is needed for GitHub integration. The `gh` CLI is Anthropic's
> recommended approach — Claude knows it natively, it adds zero context overhead, and
> it's already authenticated on this machine. See [Note on MCP Servers](#note-on-mcp-servers)
> for when MCP does make sense.

---

## Reference: Configuration File Locations

### Where Everything Lives

```
~/.claude/
├── settings.json              # Global settings (all projects)
├── CLAUDE.md                  # Global instructions (currently empty)
├── commands/                  # Global slash commands
│   ├── cpp-review.md
│   ├── cpp-optimize.md
│   ├── cpp-modernize.md
│   └── wrapup.md
├── skills/                    # Global skills
│   └── cpp-model-invariant/
│       └── SKILL.md
└── projects/<hash>/memory/    # Auto-memory (per project)
    └── MEMORY.md

<project-root>/
├── CLAUDE.md                  # Project instructions (checked in)
├── .mcp.json                  # MCP servers (only if needed for non-CLI services)
└── .claude/
    ├── settings.json          # Project settings, hooks (checked in)
    ├── settings.local.json    # Personal overrides (gitignored)
    ├── skills/                # Project skills (checked in)
    │   ├── scaffold-app/
    │   │   └── SKILL.md
    │   ├── build-test/
    │   │   └── SKILL.md
    │   └── ...
    ├── agents/                # Project subagents (checked in)
    │   ├── code-docs-writer.md
    │   ├── build-doctor.md
    │   └── ...
    └── agent-memory/          # Agent persistent memory
        └── <agent-name>/
            └── MEMORY.md
```

### Scope Precedence (highest to lowest)

1. Command-line flags
2. `.claude/settings.local.json` (personal, gitignored)
3. `.claude/settings.json` (team, checked in)
4. `~/.claude/settings.json` (user, all projects)
5. Managed policy (organization-wide)

### Key Distinctions

| Checked into Git? | File | Purpose |
|-------------------|------|---------|
| Yes | `CLAUDE.md` | Project rules for all developers |
| Yes | `.claude/settings.json` | Shared hooks and permissions |
| Yes | `.claude/skills/` | Team workflows |
| Yes | `.claude/agents/` | Team subagents |
| Yes | `.mcp.json` | Shared MCP servers (only for non-CLI services) |
| **No** | `.claude/settings.local.json` | Personal permissions, API keys |
| **No** | `~/.claude/` | Personal global config |
| **No** | Auto-memory | Machine-local learnings |

---

## Note on MCP Servers

MCP (Model Context Protocol) servers extend Claude Code with external tool integrations.
However, **not every external service needs an MCP server**.

### The `gh` CLI vs GitHub MCP Server

Anthropic's official best practices recommend the `gh` CLI over the GitHub MCP server
for interactive local development:

> CLI tools are the most context-efficient way to interact with external services.
> If you use GitHub, install the `gh` CLI — Claude knows how to use it for creating
> issues, opening pull requests, and reading comments.

**Key reasons to prefer `gh` CLI:**

1. **Claude already knows it** — trained extensively on `gh` usage, no in-context docs needed
2. **Context efficiency** — zero overhead vs dozens of MCP tool definitions loaded into context
3. **Tool count** — adding MCP inflates tool count, empirically degrading tool selection quality
4. **Maintenance** — the npm `@modelcontextprotocol/server-github` package was deprecated (April 2025);
   the official replacement requires Docker or PAT-authenticated HTTP transport

### When MCP Servers DO Make Sense

Use MCP for services that **don't have a capable CLI equivalent**:

| Service | Has Good CLI? | Use MCP? |
|---------|:------------:|:--------:|
| GitHub | Yes (`gh`) | No |
| Git | Yes (`git`) | No |
| Docker | Yes (`docker`) | No |
| Notion | No | **Yes** |
| Figma | No | **Yes** |
| Sentry | Partial | Maybe |
| Custom internal APIs | No | **Yes** |
| Databases (direct query) | Varies | **Yes** |

### When MCP Makes Sense for GitHub Specifically

- **Headless/CI environments** where `gh` auth isn't set up but an MCP endpoint with a PAT is
- **API surface beyond `gh`** — project boards, security advisories, fine-grained GraphQL queries
- **Orchestration layers** that need structured tool call results rather than parsed CLI output
