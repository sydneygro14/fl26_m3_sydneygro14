# Milestone 3 (M3) Reference Package (version Oct. 6, 2026)

## What this is
Seven copies of the provided M3 headers with extra explanations written as comments
(lines marked "QT NOTE"). They explain what each part of the provided interface means
and how the Qt pieces fit together. They do not contain any implementation.

## Rules
- **Reference only.** Read these files; do not add them to your project or your build.
- **Keep using the original headers.** The required headers and API contracts must stay
  unchanged. The annotated copies have the same code, plus comments.
- **You write the .cpp files yourself.** This package is for understanding, not copying.
- Nothing in this package is submitted or graded.

## Suggested reading order
1. `resettable_explanation.hpp`: what an interface is and why `reset()` is pure virtual
2. `search_result_model_explanation.hpp`: model/view, `QVariant`, `rowCount`, `data`
3. `context_widget_explanation.hpp`: a small widget; signals and slots; `connect()`
4. `workspace_widget_explanation.hpp`
5. `search_widget_explanation.hpp`
6. `term_inspector_widget_explanation.hpp`
7. `main_window_explanation.hpp`: coordination, state, and the exit callback

Later files point back to earlier ones for basics, so reading in this order helps.

## A caution
Code snippets inside the comments are small illustrations of an idea, not a complete
or required solution. Check them against the Milestone 3 specification, which is the
authority on required behavior.
