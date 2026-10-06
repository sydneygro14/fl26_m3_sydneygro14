# AI Workspace — M3 Starter

This package contains the required M2 headers, the fixed M3 GUI headers, the M3
implementation skeletons, CMake configuration, and public sample data.

## Before building

M3 is cumulative. Copy your completed M2 implementation `.cpp` files into
`src/` using these filenames:

- `document.cpp`
- `workspace.cpp`
- `text_processor.cpp`
- `chunker.cpp`
- `corpus_index.cpp`
- `retrieval_engine.cpp`
- `context_builder.cpp`
- `processing_core.cpp`

Those M2 implementation files are intentionally not included in this starter.

## M3 work

Implement the `// TODO` portions of the seven supplied M3 source files:

- `main.cpp`
- `main_window.cpp`
- `workspace_widget.cpp`
- `search_widget.cpp`
- `context_widget.cpp`
- `term_inspector_widget.cpp`
- `search_result_model.cpp`

The required headers under `include/aiws/gui/` define the M3 API contract and
must not be modified. You may add separate helper source/header files if needed,
provided the required API remains unchanged and any added files are included in
your build.

Follow the M3 specification for required behavior, initial state, GUI layout,
window size/resizing, widget presentation, signals/slots, callback behavior, and
backend integration.
