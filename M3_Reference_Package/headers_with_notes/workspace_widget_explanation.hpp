// ======================================================================
// REFERENCE ONLY (version Oct. 6, 2026)
// Read this file to understand the provided interface. Do NOT add it to
// your build, and do NOT replace the provided header with it. Your project
// must keep using the original header; the required API must stay unchanged.
// ======================================================================

// QT NOTE: Big picture. WorkspaceWidget shows the list of document titles, the
// selected document's title and text, the corpus status, and the Add Document and
// Build Corpus buttons. It only DISPLAYS and reports: MainWindow reads files and
// runs the backend, then tells this widget what to show.
// For the basics, see search_result_model_explanation.hpp,
// context_widget_explanation.hpp and main_window_explanation.hpp.
// Comments labeled "QT NOTE" are explanations only; the code is unchanged.
#pragma once

#include <QWidget>

#include "aiws/gui/resettable.hpp"

// QT NOTE: Forward declarations (see context_widget_explanation.hpp). QListWidget =
// a simple list of text items (one per document title); QLabel = read-only text;
// QPlainTextEdit = multi-line text box (read-only here); QPushButton = a button.
class QLabel;
class QListWidget;
class QPlainTextEdit;
class QPushButton;
class QString;

namespace aiws::gui {

/**
 * @brief Presents the workspace and selected-document portions of the M3 GUI.
 *
 * The widget displays document titles, the currently selected document, corpus
 * status, and the actions used to add documents and build the corpus. Backend
 * workspace and processing operations are coordinated by MainWindow.
 */
// QT NOTE: A QWidget and a Resettable (QWidget first).
class WorkspaceWidget final : public QWidget, public Resettable {
    Q_OBJECT

public:
    /**
     * @brief Constructs the workspace and selected-document GUI component.
     * @param parent Optional Qt parent widget.
     */
    // QT NOTE: Creates the list, buttons, labels and text area, arranges them with layouts,
    // and connects the child widgets' signals to this widget's own signals (see the
    // notes at "signals:").
    explicit WorkspaceWidget(QWidget* parent = nullptr);

    /**
     * @brief Adds a document title to the workspace list.
     * @param title Title to display for the document.
     */
    // QT NOTE: Adds a row to the visible list, for example document_list_->addItem(title).
    // Same name as MainWindow::add_document(), but a different class and a different
    // job: that slot picks the file and updates the backend; this function only
    // updates the display.
    void add_document(const QString& title);

    /**
     * @brief Selects the requested document row in the workspace list.
     * @param row Zero-based document row to select.
     *
     * This operation changes GUI selection only; it does not perform backend
     * processing.
     */
    // QT NOTE: Highlights a row, for example document_list_->setCurrentRow(row).
    // GUI selection only; no backend work. Be careful that changing the selection can
    // emit a signal again, so avoid writing a loop where one change triggers another.
    void select_document(int row);

    /**
     * @brief Displays the selected document's title and complete text.
     * @param title Document title to display.
     * @param text Document text to display.
     */
    // QT NOTE: Fills the title label and the read-only text area (setText / setPlainText).
    void show_selected_document(const QString& title, const QString& text);

    /**
     * @brief Clears the selected-document display.
     */
    // QT NOTE: Empties the title and text display.
    void clear_selected_document();

    /**
     * @brief Updates the displayed corpus status.
     * @param text Corpus-status text to display.
     */
    // QT NOTE: Shows a status message such as the chunk count (QLabel::setText).
    void set_corpus_status(const QString& text);

    /**
     * @brief Sets whether the Build Corpus action is available.
     * @param enabled true to enable the action; false to disable it.
     */
    // QT NOTE: MainWindow decides when Build Corpus is valid (for example the workspace is
    // not empty); this just enables or disables the button.
    void set_build_corpus_enabled(bool enabled);

    /**
     * @brief Restores the workspace component to its initial application state.
     *
     * The document list and selected-document display are cleared, the corpus
     * status returns to its initial value, and Build Corpus is unavailable.
     */
    // QT NOTE: Implements Resettable::reset(): clear the document list and the selected
    // document display, put the corpus status back to its initial text, and disable Build Corpus.
    void reset() override;

// QT NOTE: This class has signals but no private slots, because each signal can be
// fed straight from a child widget (a signal can connect to another signal):
//     connect(add_document_button_, &QPushButton::clicked,
//             this, &WorkspaceWidget::add_document_requested);
//     connect(build_corpus_button_, &QPushButton::clicked,
//             this, &WorkspaceWidget::build_corpus_requested);
//     connect(document_list_, &QListWidget::currentRowChanged,
//             this, &WorkspaceWidget::document_selected);
// (A receiver may ignore extra arguments, such as clicked's bool.) MainWindow
// connects these three signals to its own slots.
signals:
    /** @brief Emitted when the user requests that a document be added. */
    void add_document_requested();

    /** @brief Emitted when the user requests that the corpus be built. */
    void build_corpus_requested();

    /**
     * @brief Emitted when the current document selection changes.
     * @param row Zero-based selected row, or a negative value when no row is selected.
     */
    // QT NOTE: -1 (any negative number) means "nothing selected"; check before using it.
    void document_selected(int row);

private:
    // QT NOTE: Pointers to child widgets: created with new and given a parent or layout,
    // so Qt deletes them; {nullptr} = not created yet; trailing underscore = private.
    QListWidget* document_list_{nullptr};
    QPushButton* add_document_button_{nullptr};
    QPushButton* build_corpus_button_{nullptr};
    QLabel* corpus_status_label_{nullptr};
    QLabel* document_title_label_{nullptr};
    QPlainTextEdit* document_text_{nullptr};
};

}  // namespace aiws::gui

// QT NOTE: Common beginner mistakes:
// - doing file reading or backend work in this widget (MainWindow does it)
// - selecting a row and accidentally triggering another selection signal in a loop
// - forgetting to clear the selected-document display or status text in reset()
// - leaving Build Corpus enabled when the workspace is empty
// - using a lambda in connect()
