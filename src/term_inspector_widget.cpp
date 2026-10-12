#include "aiws/gui/term_inspector_widget.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace aiws::gui {

namespace {

// neutral display used before any statistic is available
const QString kNeutral = QStringLiteral("\u2014");

}  // namespace

TermInspectorWidget::TermInspectorWidget(QWidget* parent) : QWidget(parent) {
    // child widgets owned by qt through the layouts below
    term_edit_ = new QLineEdit;
    inspect_button_ = new QPushButton("Inspect");
    document_frequency_value_ = new QLabel(kNeutral);
    chunk_frequency_value_ = new QLabel(kNeutral);

    term_edit_->setPlaceholderText("Enter a term");
    inspect_button_->setEnabled(false);

    // term entry row then a row with both statistics
    auto* term_row = new QHBoxLayout;
    term_row->addWidget(new QLabel("Term:"));
    term_row->addWidget(term_edit_, 1);
    term_row->addWidget(inspect_button_);

    auto* stats_row = new QHBoxLayout;
    stats_row->addWidget(new QLabel("Document Frequency:"));
    stats_row->addWidget(document_frequency_value_);
    stats_row->addStretch();
    stats_row->addWidget(new QLabel("Selected Chunk TF:"));
    stats_row->addWidget(chunk_frequency_value_);
    stats_row->addStretch();

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("<b>Term Inspector:</b>"));
    layout->addLayout(term_row);
    layout->addLayout(stats_row);
    layout->addStretch();

    connect(inspect_button_, &QPushButton::clicked, this, &TermInspectorWidget::request_inspection);
}

void TermInspectorWidget::set_inspection_enabled(bool enabled) {
    inspect_button_->setEnabled(enabled);
}

void TermInspectorWidget::set_selected_chunk(const QString& chunk_id) {
    // only remembered for the next inspection and nothing is looked up here
    selected_chunk_id_ = chunk_id;
}

void TermInspectorWidget::set_statistics(std::size_t document_frequency,
                                         bool has_chunk_frequency,
                                         std::size_t chunk_frequency) {
    document_frequency_value_->setText(QString::number(document_frequency));
    // no selected chunk means the chunk frequency stays neutral
    chunk_frequency_value_->setText(has_chunk_frequency ? QString::number(chunk_frequency) : kNeutral);
}

void TermInspectorWidget::clear_statistics() {
    document_frequency_value_->setText(kNeutral);
    chunk_frequency_value_->setText(kNeutral);
}

void TermInspectorWidget::reset() {
    term_edit_->clear();
    selected_chunk_id_.clear();
    clear_statistics();
    inspect_button_->setEnabled(false);
}

void TermInspectorWidget::request_inspection() {
    // a blank term makes no inspection request
    const QString term = term_edit_->text();
    if (term.trimmed().isEmpty()) return;
    emit term_inspection_requested(term, selected_chunk_id_);
}

}  // namespace aiws::gui