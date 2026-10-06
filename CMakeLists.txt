cmake_minimum_required(VERSION 3.6)
project(ai_workspace_m3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# M3 is cumulative. Copy your completed M2 implementation files into src/
# using the filenames below before configuring/building this project.
add_library(aiws_m2
    src/document.cpp
    src/workspace.cpp
    src/text_processor.cpp
    src/chunker.cpp
    src/corpus_index.cpp
    src/retrieval_engine.cpp
    src/context_builder.cpp
    src/processing_core.cpp
)
target_include_directories(aiws_m2 PUBLIC include)
target_compile_options(aiws_m2 PRIVATE -Wall -Wextra -Wpedantic)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
set(CMAKE_AUTOMOC ON)

add_executable(ai_workspace_m3
    src/main.cpp
    src/main_window.cpp
    src/workspace_widget.cpp
    src/search_widget.cpp
    src/context_widget.cpp
    src/term_inspector_widget.cpp
    src/search_result_model.cpp
    include/aiws/gui/main_window.hpp
    include/aiws/gui/resettable.hpp
    include/aiws/gui/workspace_widget.hpp
    include/aiws/gui/search_widget.hpp
    include/aiws/gui/context_widget.hpp
    include/aiws/gui/term_inspector_widget.hpp
    include/aiws/gui/search_result_model.hpp
)
target_include_directories(ai_workspace_m3 PRIVATE include)
target_link_libraries(ai_workspace_m3 PRIVATE aiws_m2 Qt6::Widgets)
target_compile_options(ai_workspace_m3 PRIVATE -Wall -Wextra -Wpedantic)
