#include "aiws/gui/search_widget.hpp"

#include "aiws/gui/search_result_model.hpp"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QString>
#include <QStringList>
#include <QTableView>
#include <QVBoxLayout>

#include <utility>

namespace aiws::gui {

namespace {

// result count limits and the startup and reset value for k
constexpr int kMinResults = 1;
constexpr int kMaxResults = 100;
constexpr int kDefaultResults = 3;

// result for the selected table row or null when nothing is selected
const SearchResult* selected_result(const QTableView* view, const SearchResultModel* model) {
    const QModelIndexList rows = view->selectionModel()->selectedRows();
    return rows.isEmpty() ? nullptr : model->result_at(rows.first().row());
}

}  // namespace

SearchWidget::SearchWidget(QWidget* parent) : QWidget(parent) {
    // child widgets owned by qt and the model is parented to this widget
    query_edit_ = new QLineEdit;
    result_count_spin_ = new QSpinBox;
    search_button_ = new QPushButton("Search");
    results_view_ = new QTableView;
    result_model_ = new SearchResultModel(this);
    selected_result_text_ = new QPlainTextEdit;
    view_source_button_ = new QPushButton("View Source");

    query_edit_->setPlaceholderText("Search the current corpus");
    result_count_spin_->setRange(kMinResults, kMaxResults);
    result_count_spin_->setValue(kDefaultResults);
    search_button_->setEnabled(false);
    view_source_button_->setEnabled(false);
    selected_result_text_->setReadOnly(true);
    selected_result_text_->setPlaceholderText("Select a result to inspect its details.");

    // table draws whatever the model holds and rows are single and read only
    results_view_->setModel(result_model_);
    results_view_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    results_view_->setSelectionBehavior(QAbstractItemView::SelectRows);
    results_view_->setSelectionMode(QAbstractItemView::SingleSelection);
    results_view_->setWordWrap(false);
    results_view_->verticalHeader()->hide();
    results_view_->horizontalHeader()->setStretchLastSection(true);
    const int widths[] = {50, 220, 60, 90};
    for (int column = 0; column < 4; ++column) results_view_->setColumnWidth(column, widths[column]);

    // query row then the table then the selected result heading and details
    auto* query_row = new QHBoxLayout;
    query_row->addWidget(new QLabel("<b>Query:</b>"));
    query_row->addWidget(query_edit_, 1);
    query_row->addWidget(new QLabel("Results:"));
    query_row->addWidget(result_count_spin_);
    query_row->addWidget(search_button_);

    auto* selected_row = new QHBoxLayout;
    selected_row->addWidget(new QLabel("<b>Selected Result:</b>"));
    selected_row->addStretch();
    selected_row->addWidget(view_source_button_);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(query_row);
    layout->addWidget(results_view_, 2);
    layout->addLayout(selected_row);
    layout->addWidget(selected_result_text_, 1);

    // selection model exists only after setmodel so connecting happens last
    connect(search_button_, &QPushButton::clicked, this, &SearchWidget::request_search);
    connect(query_edit_, &QLineEdit::textChanged, this, &SearchWidget::query_changed);
    connect(results_view_->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &SearchWidget::show_selected_result);
    connect(view_source_button_, &QPushButton::clicked, this, &SearchWidget::request_source_document);
}

QString SearchWidget::query() const {
    return query_edit_->text();
}

int SearchWidget::result_count() const {
    return result_count_spin_->value();
}

bool SearchWidget::has_query() const {
    return !query().trimmed().isEmpty();
}

void SearchWidget::set_search_enabled(bool enabled) {
    search_button_->setEnabled(enabled);
}

void SearchWidget::set_results(std::vector<SearchResult> results) {
    // dropping the selection first makes show_selected_result clear the details
    // disable view source and report that no chunk is selected
    results_view_->clearSelection();
    result_model_->set_results(std::move(results));
}

void SearchWidget::clear_results() {
    // same order here because a model reset alone clears the selection silently
    results_view_->clearSelection();
    result_model_->clear();
}

void SearchWidget::reset() {
    query_edit_->clear();
    clear_results();
    result_count_spin_->setValue(kDefaultResults);
    search_button_->setEnabled(false);
}

void SearchWidget::request_search() {
    // only reports the request and leaves searching to the main window
    emit search_requested(query(), result_count());
}

void SearchWidget::show_selected_result() {
    const SearchResult* result = selected_result(results_view_, result_model_);

    // no selected row clears the details and reports no selected chunk
    if (!result) {
        selected_result_text_->clear();
        view_source_button_->setEnabled(false);
        emit selected_chunk_changed(QString());
        return;
    }

    // metadata lines then a blank line then the full chunk text
    const QStringList lines{
        "Document: " + QString::fromStdString(result->document_id),
        "Chunk: " + QString::number(result->chunk_sequence) + " (" +
            QString::fromStdString(result->chunk_id) + ")",
        "Score: " + QString::number(result->score, 'f', 12),
        "Matched terms: " + QString::number(result->matched_terms),
        QString(),
        QString::fromStdString(result->text)};
    selected_result_text_->setPlainText(lines.join(QLatin1Char('\n')));
    view_source_button_->setEnabled(true);
    emit selected_chunk_changed(QString::fromStdString(result->chunk_id));
}

void SearchWidget::request_source_document() {
    // view source is only enabled for a valid row but the check stays cheap
    const SearchResult* result = selected_result(results_view_, result_model_);
    if (result) emit source_document_requested(QString::fromStdString(result->document_id));
}

}  // namespace aiws::gui