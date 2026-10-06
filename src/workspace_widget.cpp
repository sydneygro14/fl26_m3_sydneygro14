#include "aiws/gui/workspace_widget.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSizePolicy>
#include <QString>
#include <QVBoxLayout>

namespace aiws::gui {

namespace {

// startup text for the corpus status and the selected document heading
const QString kCorpusNotBuilt = QStringLiteral("Corpus: Not built");
const QString kNoDocument = QStringLiteral("Selected Document -");

}  // namespace

WorkspaceWidget::WorkspaceWidget(QWidget* parent) : QWidget(parent) {
    // child widgets owned by qt through the layouts below
    document_list_ = new QListWidget;
    add_document_button_ = new QPushButton("Add Document");
    build_corpus_button_ = new QPushButton("Build Corpus");
    corpus_status_label_ = new QLabel(kCorpusNotBuilt);
    document_title_label_ = new QLabel(kNoDocument);
    document_text_ = new QPlainTextEdit;

    // selected document is display only and building waits for a document
    document_text_->setReadOnly(true);
    document_text_->setPlaceholderText("Select a document to view its contents.");
    build_corpus_button_->setEnabled(false);
    // plain bold heading that is clipped instead of widening the window
    document_title_label_->setTextFormat(Qt::PlainText);
    document_title_label_->setStyleSheet("font-weight: bold;");
    document_title_label_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

    // left column has the heading with both actions then the list and status
    auto* actions = new QHBoxLayout;
    actions->addWidget(new QLabel("<b>Workspace</b>"));
    actions->addWidget(add_document_button_);
    actions->addWidget(build_corpus_button_);
    actions->addStretch();
    auto* left = new QVBoxLayout;
    left->addLayout(actions);
    left->addWidget(document_list_, 1);
    left->addWidget(corpus_status_label_);

    // right column has the selected document heading and its full text
    auto* right = new QVBoxLayout;
    right->addWidget(document_title_label_);
    right->addWidget(document_text_, 1);

    auto* layout = new QHBoxLayout(this);
    layout->addLayout(left, 1);
    layout->addLayout(right, 1);

    // child events pass straight through as component signals
    connect(add_document_button_, &QPushButton::clicked, this, &WorkspaceWidget::add_document_requested);
    connect(build_corpus_button_, &QPushButton::clicked, this, &WorkspaceWidget::build_corpus_requested);
    connect(document_list_, &QListWidget::currentRowChanged, this, &WorkspaceWidget::document_selected);
}

void WorkspaceWidget::add_document(const QString& title) {
    document_list_->addItem(title);
}

void WorkspaceWidget::select_document(int row) {
    document_list_->setCurrentRow(row);
}

void WorkspaceWidget::show_selected_document(const QString& title, const QString& text) {
    document_title_label_->setText(kNoDocument + QLatin1Char(' ') + title);
    document_text_->setPlainText(text);
}

void WorkspaceWidget::clear_selected_document() {
    document_title_label_->setText(kNoDocument);
    document_text_->clear();
}

void WorkspaceWidget::set_corpus_status(const QString& text) {
    corpus_status_label_->setText(text);
}

void WorkspaceWidget::set_build_corpus_enabled(bool enabled) {
    build_corpus_button_->setEnabled(enabled);
}

void WorkspaceWidget::reset() {
    // clearing the list can report a negative row which clears the display too
    document_list_->clear();
    clear_selected_document();
    set_corpus_status(kCorpusNotBuilt);
    build_corpus_button_->setEnabled(false);
}

}  // namespace aiws::gui