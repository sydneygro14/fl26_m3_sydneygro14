#include "aiws/gui/workspace_widget.hpp"

namespace aiws::gui {

WorkspaceWidget::WorkspaceWidget(QWidget* parent) : QWidget(parent) {
    // TODO: Construct and lay out the Workspace and Selected Document areas,
    // connect local widget events, and establish initial presentation state.
}

void WorkspaceWidget::add_document(const QString& title) {
    // TODO
    (void)title;  // To suppress unused-parameter warning.
}

void WorkspaceWidget::select_document(int row) {
    // TODO
    (void)row;  // To suppress unused-parameter warning.
}

void WorkspaceWidget::show_selected_document(const QString& title,
                                              const QString& text) {
    // TODO
    (void)title;  // To suppress unused-parameter warning.
    (void)text;  // To suppress unused-parameter warning.
}

void WorkspaceWidget::clear_selected_document() {
    // TODO
}

void WorkspaceWidget::set_corpus_status(const QString& text) {
    // TODO
    (void)text;  // To suppress unused-parameter warning.
}

void WorkspaceWidget::set_build_corpus_enabled(bool enabled) {
    // TODO
    (void)enabled;  // To suppress unused-parameter warning.
}

void WorkspaceWidget::reset() {
    // TODO
}

}  // namespace aiws::gui
