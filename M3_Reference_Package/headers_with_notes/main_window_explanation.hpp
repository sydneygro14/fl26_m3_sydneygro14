// ======================================================================
// REFERENCE ONLY (version Oct. 6, 2026)
// Read this file to understand the provided interface. Do NOT add it to
// your build, and do NOT replace the provided header with it. Your project
// must keep using the original header; the required API must stay unchanged.
// ======================================================================

// QT NOTE: Big picture. MainWindow is the COORDINATOR. It owns the backend objects
// (Workspace and ProcessingCore), creates the four component widgets, and is the
// only place where backend calls happen. The flow for every user action is:
// widget signal -> component slot -> component signal -> MainWindow slot
// -> backend -> component update.
// For override, const, Q_OBJECT, explicit, parent ownership, CMAKE_AUTOMOC and the
// signals/slots basics, see search_result_model_explanation.hpp and
// context_widget_explanation.hpp.
// Comments labeled "QT NOTE" are explanations only; the code is unchanged.
#pragma once

// QT NOTE: QMainWindow is a ready-made application window with room for a central
// widget, a menu bar, tool bars, and a status bar.
#include <QMainWindow>

// QT NOTE: Your M1/M2 backend. M3 keeps it and puts a GUI around it.
#include "aiws/processing_core.hpp"
#include "aiws/workspace.hpp"

// QT NOTE: Forward declarations: only pointers to these classes are stored below,
// so the full headers are included in the .cpp instead (see context_widget_explanation.hpp).
class QLabel;
class QPushButton;
class QString;
class QWidget;

namespace aiws::gui {

// QT NOTE: Forward declarations of the four GUI components in this project.
class ContextWidget;
class SearchWidget;
class TermInspectorWidget;
class WorkspaceWidget;

/**
 * @brief Main application window and coordinator for the M3 AI Workspace GUI.
 *
 * MainWindow owns the application Workspace and ProcessingCore, composes the
 * major GUI components, and coordinates application-wide state changes and
 * backend operations in response to component events.
 */
// QT NOTE: Inherits QMainWindow, so it is a top-level window. "final" = no one can
// inherit from it.
class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Function-pointer type used for the application Exit action.
     *
     * The callback receives the main window as a parent for any exit-related
     * interaction. In M3, the registered callback asks the user to confirm the
     * exit. Declining the confirmation leaves the running application and its
     * current state unchanged.
     */
    // QT NOTE: CALLBACK. This line creates a short name for "a pointer to a function that
    // takes a QWidget* and returns nothing". A callback is a function you hand to
    // someone else so THEY can call it later. Example of a matching function:
    //     void confirm_exit(QWidget* parent) {
    //         auto answer = QMessageBox::question(parent, "Exit", "Quit the application?");
    //         if (answer == QMessageBox::Yes) QCoreApplication::quit();
    //     }
    //     window.set_exit_callback(&confirm_exit);   // pass the function's address
    // Yes quits; No simply returns, leaving the application state unchanged.
    using ExitCallback = void (*)(QWidget* parent);

    /**
     * @brief Constructs the main AI Workspace application window.
     * @param parent Optional Qt parent widget.
     */
    // QT NOTE: The constructor builds the whole GUI: creates the four widgets and the
    // buttons, arranges them with layouts (never setFixedSize or fixed positions),
    // sets the initial 900 x 900 size, and makes every connect() call using function
    // pointers (no lambdas).
    explicit MainWindow(QWidget* parent = nullptr);

    /**
     * @brief Registers the callback invoked when the user requests Exit.
     * @param callback Function pointer to invoke for the Exit action. A null
     * callback means that an Exit request has no registered callback to invoke.
     */
    // QT NOTE: Stores the function pointer in exit_callback_. "noexcept" promises it never
    // throws. A null pointer means "no callback registered", so exit_application()
    // must check for null before calling it.
    void set_exit_callback(ExitCallback callback) noexcept;

// QT NOTE: Each slot below is a RECEIVER: a component emits a signal, and MainWindow's
// constructor connects it to one of these slots. They are private because only
// connect() calls them, never other code.
private slots:
    /** @brief Handles a request to select and add a document to the workspace. */
    // QT NOTE: Typically opens a file dialog (QFileDialog), adds the chosen document to
    // workspace_, and then marks the corpus as no longer current (invalidate_corpus()).
    void add_document();

    /** @brief Rebuilds the processing corpus from the current non-empty workspace. */
    // QT NOTE: Rebuilds the corpus from the workspace, sets corpus_current_ to true, and
    // shows the chunk count. Old processing output is cleared first.
    void build_corpus();

    /**
     * @brief Updates the selected-document display for the requested workspace row.
     * @param row Zero-based document row, or a negative value when no row is selected.
     */
    // QT NOTE: Qt uses -1 (any negative number) for "nothing selected", so check the row
    // before using it. Selecting a document only DISPLAYS it; it does not rebuild anything.
    void document_selected(int row);

    /**
     * @brief Performs a search of the current corpus and presents the returned results.
     * @param query Query text to search for.
     * @param k Maximum number of results requested.
     */
    // QT NOTE: const QString& = pass without copying. The backend uses std::string, so
    // convert with query.toStdString() before calling it. Only searches when the
    // corpus is current.
    void search_requested(const QString& query, int k);

    /**
     * @brief Responds to a query edit by updating application action availability.
     * @param query Updated query text.
     */
    // QT NOTE: Called while the user types. It only updates which buttons are enabled
    // (through update_action_states()). It must NEVER start a search by itself;
    // searching is an explicit user action.
    void query_changed(const QString& query);

    /**
     * @brief Builds and displays context for the current query and search settings.
     * @param token_budget Maximum token budget requested for the context.
     */
    // QT NOTE: A slot that happens to share its name with ContextWidget's signal. That
    // is fine: connect(context_widget_, &ContextWidget::build_context_requested,
    // this, &MainWindow::build_context_requested) pairs the signal of one class
    // with the slot of another.
    void build_context_requested(int token_budget);
    /**
     * @brief Selects and displays the workspace document identified by a search result.
     * @param document_id Identifier of the source document to reveal.
     */
    // QT NOTE: Finds the document with this id in the workspace and selects it, so the
    // user can see where a search result came from. Does not re-search.
    void source_document_requested(const QString& document_id);

    /**
     * @brief Queries and presents term statistics for the current corpus.
     * @param term Term to inspect.
     * @param chunk_id Selected result chunk identifier; may be empty, in which
     * case document frequency is still available but chunk frequency is not.
     */
    // QT NOTE: chunk_id may be an empty string. In that case only the document frequency
    // can be shown, not the chunk frequency (see the comment above).
    void term_inspection_requested(const QString& term, const QString& chunk_id);

    /**
     * @brief Restores the application and its major GUI components to initial state.
     */
    // QT NOTE: Restores the startup state. Because the four components implement
    // Resettable, this can call reset() on each one through the common interface,
    // then reset corpus_current_ and the status text.
    void reset_application();

    /**
     * @brief Invokes the registered Exit callback for the current exit request.
     */
    // QT NOTE: Calls exit_callback_ with "this" as the parent (if it is not null). The
    // decision to quit lives inside the callback, not here.
    void exit_application();

// QT NOTE: Everything below is private: helper functions and data that only MainWindow
// itself uses.
private:
    /**
     * @brief Updates availability of actions that depend on current application state.
     */
    // QT NOTE: The ONE place that decides which buttons and controls are enabled. Call it
    // after every state change (add, build, search, reset, query edit) so the GUI
    // never disagrees with corpus_current_ and the workspace contents.
    void update_action_states();

    /**
     * @brief Marks the processing corpus as no longer current for the workspace.
     *
     * Processing output that depends on the previous corpus is no longer made
     * available after invalidation.
     */
    // QT NOTE: Sets corpus_current_ to false and clears processing results that came
    // from the old corpus. Call it whenever the workspace changes (for example after
    // adding a document).
    void invalidate_corpus();

    /**
     * @brief Clears displayed search and context output that depends on processing state.
     */
    // QT NOTE: Clears the search results, the selection and details, and the context
    // display, so no stale output stays on screen.
    void clear_processing_outputs();

    // QT NOTE: The backend objects are stored BY VALUE: they live inside MainWindow and
    // are destroyed with it. The trailing underscore marks private data members.
    Workspace workspace_;
    ProcessingCore processing_core_;
    // QT NOTE: An explicit validity flag, the heart of M3's "state consistency". True only
    // after a successful Build Corpus; false at startup, after Add Document, and after
    // Reset. Search, Term Inspector and Context may run only while it is true.
    bool corpus_current_{false};
    // QT NOTE: Holds the registered function pointer. {nullptr} = nothing registered yet.
    ExitCallback exit_callback_{nullptr};

    // QT NOTE: Pointers to the child widgets. They are created with new and given a
    // parent or a layout, so Qt deletes them automatically; MainWindow never calls
    // delete on them. {nullptr} is the safe "not yet created" value.
    WorkspaceWidget* workspace_widget_{nullptr};
    SearchWidget* search_widget_{nullptr};
    TermInspectorWidget* term_inspector_widget_{nullptr};
    ContextWidget* context_widget_{nullptr};
    QLabel* status_label_{nullptr};
    QPushButton* reset_button_{nullptr};
    QPushButton* exit_button_{nullptr};
};

}  // namespace aiws::gui

// QT NOTE: Common beginner mistakes:
// - starting a search or processing when the query, k or budget changes
// - forgetting update_action_states() after a state change
// - forgetting to invalidate the corpus after adding a document
// - putting backend logic inside the widgets instead of MainWindow
// - using a lambda in connect() (M3 requires named slots)
// - calling delete on a child widget that Qt already owns
// - not checking exit_callback_ for null before calling it
