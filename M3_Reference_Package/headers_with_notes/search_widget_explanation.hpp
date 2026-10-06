// ======================================================================
// REFERENCE ONLY (version Oct. 6, 2026)
// Read this file to understand the provided interface. Do NOT add it to
// your build, and do NOT replace the provided header with it. Your project
// must keep using the original header; the required API must stay unchanged.
// ======================================================================

// QT NOTE: Big picture. SearchWidget is a COMPONENT: it owns the query box, the
// result-count control, the Search button, the results table, and the details
// area. It shows things and reports what the user did through signals; it does
// NOT search. MainWindow performs the search and hands the results back.
// Model/view in action: the table (QTableView) only draws; SearchResultModel holds
// the data. For override, const, Q_OBJECT, explicit, parent ownership and
// signals/slots basics, see search_result_model_explanation.hpp and
// context_widget_explanation.hpp.
// Comments labeled "QT NOTE" are explanations only; the code is unchanged.
#pragma once

#include <QWidget>

// QT NOTE: std::vector is standard C++. SearchResult comes from the backend header
// below; this GUI file only displays such objects.
#include <vector>

#include "aiws/gui/resettable.hpp"
#include "aiws/processing_types.hpp"

// QT NOTE: Forward declarations (see context_widget_explanation.hpp). What each Qt
// class is: QLineEdit = one-line text input; QPlainTextEdit = multi-line text box
// (can be read-only); QPushButton = a button; QSpinBox = a number input with up and
// down arrows (used for k); QTableView = a table that DISPLAYS data from a model;
// QString = Qt's text class.
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QSpinBox;
class QTableView;
class QString;

namespace aiws::gui {

// QT NOTE: Another forward declaration, for OUR model class. Only a pointer to it is
// stored here, so its full header is included in the .cpp.
class SearchResultModel;

/**
 * @brief Presents query input, ranked search results, and selected-result details.
 *
 * SearchWidget owns the query controls and results view. It uses
 * SearchResultModel to present SearchResult objects and reports search, query,
 * selection, and View Source events to the application coordinator.
 */
// QT NOTE: A QWidget and a Resettable, like the other components (QWidget listed first).
class SearchWidget final : public QWidget, public Resettable {
    Q_OBJECT

public:
    /** @brief Constructs the search GUI component. @param parent Optional Qt parent widget. */
    // QT NOTE: The constructor creates the child widgets, creates the model with
    // new SearchResultModel(this), attaches it to the table with
    // results_view_->setModel(result_model_), arranges everything in layouts, and makes
    // the connect() calls (function pointers, no lambdas).
    explicit SearchWidget(QWidget* parent = nullptr);

    /** @brief Returns the current query text exactly as entered in the query field. */
    // QT NOTE: Returns the text exactly as typed (no trimming), typically
    // query_edit_->text(). The backend uses std::string, so MainWindow converts with
    // toStdString() when it calls the backend.
    QString query() const;

    /** @brief Returns the currently requested maximum number of search results. */
    // QT NOTE: This is "k". Typically result_count_spin_->value().
    int result_count() const;

    /** @brief Returns true when the current query contains non-whitespace text. */
    // QT NOTE: True when the text is not empty after removing whitespace, for example:
    //     return !query().trimmed().isEmpty();
    // MainWindow uses it to decide whether Search should be enabled.
    bool has_query() const;

    /** @brief Sets whether the Search action is available. @param enabled true to enable it. */
    // QT NOTE: Enables or disables the Search button (typically setEnabled(enabled)).
    // MainWindow decides when; this widget just obeys.
    void set_search_enabled(bool enabled);

    /** @brief Replaces the model contents with new ranked results. */
    // QT NOTE: By value so MainWindow can pass it cheaply with std::move. This simply
    // forwards to the model: result_model_->set_results(std::move(results)). The model
    // performs beginResetModel()/endResetModel(), so the table redraws by itself.
    void set_results(std::vector<SearchResult> results);

    /**
     * @brief Clears search results and result-dependent presentation state.
     *
     * The table contents, current result selection, selected-result details,
     * and View Source availability are cleared.
     */
    // QT NOTE: Four things to clear: the model, the table selection, the details text,
    // and View Source (disabled). Leaving any of them behind shows stale output.
    void clear_results();

    /**
     * @brief Restores the component to its initial application state.
     *
     * Query text and results are cleared, the result-count control returns to
     * its initial value, and Search is disabled.
     */
    // QT NOTE: Implements Resettable::reset(): clear the query text and results, put the
    // result-count control back to its initial value (k = 3 at startup), and disable Search.
    void reset() override;

// QT NOTE: Signals can carry arguments. The receiving slot must accept compatible
// types: MainWindow::search_requested(const QString&, int) receives
// SearchWidget::search_requested(const QString&, int). Declared only; moc writes
// the code. Send with: emit search_requested(query(), result_count());
// Qt also allows connecting one signal straight to another, which is a neat way to
// forward an event, for example:
//     connect(query_edit_, &QLineEdit::textChanged,
//             this, &SearchWidget::query_changed);
signals:
    /** @brief Emitted when the user requests a search. */
    void search_requested(const QString& query, int k);

    /** @brief Emitted whenever the query text changes. */
    void query_changed(const QString& query);

    /**
     * @brief Emitted when View Source is requested for the selected result.
     * @param document_id Document identifier stored in the selected SearchResult.
     */
    void source_document_requested(const QString& document_id);

    /**
     * @brief Emitted when the selected search-result chunk changes.
     * @param chunk_id Selected chunk identifier, or an empty string when no result is selected.
     */
    // QT NOTE: Emitted when the selection changes. An empty string means "nothing
    // selected". MainWindow can pass it to the Term Inspector, which remembers which
    // chunk is selected.
    void selected_chunk_changed(const QString& chunk_id);

// QT NOTE: Slots that receive events from this widget's own children (button
// clicks, table selection).
private slots:
    /** @brief Converts a Search button event into the search_requested signal. */
    // QT NOTE: The Search button's clicked() signal arrives here. It reads the query and
    // k and emits search_requested(...). It does not search.
    void request_search();

    /** @brief Updates selected-result details after the table selection changes. */
    // QT NOTE: Connected to the table's selection signal, for example:
    //     connect(results_view_->selectionModel(), &QItemSelectionModel::selectionChanged,
    //             this, &SearchWidget::show_selected_result);
    // Call selectionModel() AFTER setModel(), because it is created then. Use
    // result_model_->result_at(row), check for nullptr, fill the details text, enable
    // View Source, and emit selected_chunk_changed(...). It must not search again.
    void show_selected_result();

    /** @brief Emits a source-document request for the currently selected result. */
    // QT NOTE: Emits source_document_requested(document_id) for the selected result's
    // document, so MainWindow can reveal it.
    void request_source_document();

private:
    // QT NOTE: Pointers to child widgets: created with new and given a parent or layout,
    // so Qt deletes them; {nullptr} is the safe starting value; the trailing
    // underscore marks private members. result_model_ is also created with "this" as
    // parent: the table does NOT take ownership of its model.
    QLineEdit* query_edit_{nullptr};
    QSpinBox* result_count_spin_{nullptr};
    QPushButton* search_button_{nullptr};
    QTableView* results_view_{nullptr};
    SearchResultModel* result_model_{nullptr};
    QPlainTextEdit* selected_result_text_{nullptr};
    QPushButton* view_source_button_{nullptr};
};

}  // namespace aiws::gui

// QT NOTE: Common beginner mistakes:
// - searching again when a result is selected or the query text changes
// - using QTableWidget instead of QTableView + SearchResultModel (M3 requires model/view)
// - calling selectionModel() before setModel() (it would be null)
// - using a lambda in connect()
// - not checking result_at(row) for nullptr
// - forgetting to clear the details text or disable View Source in clear_results()
