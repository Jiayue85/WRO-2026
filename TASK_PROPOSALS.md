# Codebase Issue Triage: Proposed Tasks

## 1) Typo Fix Task
**Title:** Correct spelling mistakes in user-facing identifiers and headings.

**Why:** Multiple visible typos reduce readability and can make APIs harder to discover.

**Evidence:**
- `Usefull links` filename and heading are misspelled (`Usefull` → `Useful`).
- `move_until_balck_line` function name is misspelled (`balck` → `black`).
- Inline comments include misspellings like `eigth`, `pritn`, and `ot`.

**Scope proposal:**
- Rename file `Usefull links` to `Useful links`.
- Rename `move_until_balck_line` and update all call sites.
- Correct obvious spelling errors in comments and headings.

**Acceptance criteria:**
- No remaining occurrences of `Usefull`/`balck` in tracked files.
- Build/compile behavior unchanged after renaming.

---

## 2) Bug Fix Task
**Title:** Fix invalid token in Arduino sketch causing compile failure.

**Why:** The `improved c ++ code` sketch contains a stray `#` line between function definitions, which is invalid C++/Arduino syntax and will break compilation.

**Evidence:**
- A standalone `#` appears between `turn_90_Left()` and `turn_90_Right()`.

**Scope proposal:**
- Remove the stray `#` token.
- Run Arduino/CLI compile check for the sketch (or equivalent syntax check).

**Acceptance criteria:**
- Sketch compiles without preprocessor/syntax errors.
- No behavior change other than successful compilation.

---

## 3) Documentation Discrepancy Task
**Title:** Align README architecture claims with current implementation.

**Why:** README claims encoder-based closed-loop DC motor control and PID, while code currently uses step/dir stepper control via `AccelStepper`, which is open-loop unless external feedback is integrated.

**Evidence:**
- README states: "DC motors with encoders", "closed-loop control", and "PID control".
- Main control code is stepper-driver based (`AccelStepper`, step/dir pins, microstepping mode).

**Scope proposal:**
- Update README to describe the current, verified control stack.
- If encoder/PID work exists but is not committed, move those claims to a roadmap section.

**Acceptance criteria:**
- README reflects actual checked-in code paths.
- Any future/planned features are clearly labeled as planned, not implemented.

---

## 4) Test Improvement Task
**Title:** Add hardware-abstraction unit tests for motion math and turn conversion.

**Why:** There are no automated tests in the repo; movement math and turning conversion are high-risk regression areas.

**Evidence:**
- `stepsForDistance(...)` and `span(...)` perform critical conversion calculations.
- Repository has no test directory or test execution instructions.

**Scope proposal:**
- Extract pure calculation helpers (`stepsForDistance`, degree-to-step conversion) into a small platform-agnostic module.
- Add host-side tests (e.g., ArduinoFake/Catch2 or GoogleTest) with representative edge cases:
  - zero distance
  - small fractional distance rounding behavior
  - 90° and 180° turn mapping
  - negative degree handling

**Acceptance criteria:**
- Tests run in CI/local without hardware.
- Failing tests catch off-by-one and sign-direction regressions.
