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
