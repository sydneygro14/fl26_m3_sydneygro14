#include "aiws/gui/main_window.hpp"

namespace aiws::gui {

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    // TODO: Construct the M3 application window, compose the GUI components,
    // establish layouts/presentation, connect events, and establish initial state.
}

void MainWindow::set_exit_callback(ExitCallback callback) noexcept {
    // TODO
    (void)callback;  // To suppress unused-parameter warning.
}

void MainWindow::add_document() {
    // TODO
}

void MainWindow::build_corpus() {
    // TODO
}

void MainWindow::document_selected(int row) {
    // TODO
    (void)row;  // To suppress unused-parameter warning.
}

void MainWindow::search_requested(const QString& query, int k) {
    // TODO
    (void)query;  // To suppress unused-parameter warning.
    (void)k;  // To suppress unused-parameter warning.
}

void MainWindow::query_changed(const QString& query) {
    // TODO
    (void)query;  // To suppress unused-parameter warning.
}

void MainWindow::build_context_requested(int token_budget) {
    // TODO
    (void)token_budget;  // To suppress unused-parameter warning.
}

void MainWindow::source_document_requested(const QString& document_id) {
    // TODO
    (void)document_id;  // To suppress unused-parameter warning.
}

void MainWindow::term_inspection_requested(const QString& term,
                                           const QString& chunk_id) {
    // TODO
    (void)term;  // To suppress unused-parameter warning.
    (void)chunk_id;  // To suppress unused-parameter warning.
}

void MainWindow::reset_application() {
    // TODO
}

void MainWindow::exit_application() {
    // TODO
}

void MainWindow::update_action_states() {
    // TODO
}

void MainWindow::invalidate_corpus() {
    // TODO
}

void MainWindow::clear_processing_outputs() {
    // TODO
}

}  // namespace aiws::gui
