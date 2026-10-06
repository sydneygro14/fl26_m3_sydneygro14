#include "aiws/gui/context_widget.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSlider>
#include <QString>
#include <QStringList>
#include <QVBoxLayout>

namespace aiws::gui {

namespace {

// token budget slider limits and the startup and reset value
constexpr int kMinBudget = 50;
constexpr int kMaxBudget = 1000;
constexpr int kBudgetStep = 50;
constexpr int kDefaultBudget = 300;

}  // namespace

ContextWidget::ContextWidget(QWidget* parent) : QWidget(parent) {
    // child widgets owned by qt through the layouts below
    context_budget_slider_ = new QSlider(Qt::Horizontal);
    budget_value_label_ = new QLabel(QString::number(kDefaultBudget));
    build_context_button_ = new QPushButton("Build Context");
    context_text_ = new QPlainTextEdit;

    // slider moves in steps of 50 between 50 and 1000
    context_budget_slider_->setRange(kMinBudget, kMaxBudget);
    context_budget_slider_->setSingleStep(kBudgetStep);
    context_budget_slider_->setPageStep(kBudgetStep);
    context_budget_slider_->setValue(kDefaultBudget);
    build_context_button_->setEnabled(false);
    context_text_->setReadOnly(true);
    context_text_->setPlaceholderText("Built context will appear here.");

    // top row holds the label slider value and action then the output below
    auto* budget_row = new QHBoxLayout;
    budget_row->addWidget(new QLabel("Token budget:"));
    budget_row->addWidget(context_budget_slider_, 1);
    budget_row->addWidget(budget_value_label_);
    budget_row->addWidget(build_context_button_);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("<b>Context:</b>"));
    layout->addLayout(budget_row);
    layout->addWidget(context_text_, 1);

    // connected after the initial value is set so startup stays silent
    connect(build_context_button_, &QPushButton::clicked, this, &ContextWidget::request_context);
    connect(context_budget_slider_, &QSlider::valueChanged, this, &ContextWidget::budget_changed);
}

int ContextWidget::token_budget() const {
    return context_budget_slider_->value();
}

void ContextWidget::set_build_context_enabled(bool enabled) {
    build_context_button_->setEnabled(enabled);
}

void ContextWidget::set_context(const std::vector<ContextItem>& items) {
    // one block per item with the backend values shown as returned
    QStringList blocks;
    for (const ContextItem& item : items) {
        const QStringList lines{
            "Document: " + QString::fromStdString(item.document_id),
            "Chunk: " + QString::number(item.chunk_sequence),
            "Tokens: " + QString::number(item.token_count),
            "Score: " + QString::number(item.score, 'f', 12),
            "Truncated: " + QString::fromLatin1(item.truncated ? "true" : "false"),
            "Text: " + QString::fromStdString(item.text)};
        blocks << lines.join(QLatin1Char('\n'));
    }
    context_text_->setPlainText(blocks.join(QLatin1String("\n\n")));
}

void ContextWidget::clear_context() {
    context_text_->clear();
}

void ContextWidget::reset() {
    // value changes fire budget_changed which refreshes the visible number
    context_budget_slider_->setValue(kDefaultBudget);
    clear_context();
    build_context_button_->setEnabled(false);
}

void ContextWidget::request_context() {
    // only reports the request and leaves the backend work to the main window
    emit build_context_requested(token_budget());
}

void ContextWidget::budget_changed(int value) {
    // dragging gives every integer so values snap to the nearest multiple of 50
    const int snapped = (value + kBudgetStep / 2) / kBudgetStep * kBudgetStep;
    if (snapped != value) {
        context_budget_slider_->setValue(snapped);
        return;
    }
    budget_value_label_->setText(QString::number(value));
}

}  // namespace aiws::gui