// ======================================================================
// REFERENCE ONLY (version Oct. 6, 2026)
// Read this file to understand the provided interface. Do NOT add it to
// your build, and do NOT replace the provided header with it. Your project
// must keep using the original header; the required API must stay unchanged.
// ======================================================================

// QT NOTE: Big picture. ContextWidget is a COMPONENT: it owns a slider, a button
// and a text display, and it shows things on screen. It does NOT build the context
// itself. When the user clicks Build Context, it emits a signal, and MainWindow
// (which owns the backend) does the real work and passes the results back.
// For override, const, Q_OBJECT, explicit, parent ownership and CMAKE_AUTOMOC,
// see the notes in search_result_model_explanation.hpp.
// Comments labeled "QT NOTE" are explanations only; the code is unchanged.
#pragma once

// QT NOTE: QWidget is the base class of everything visible in a Qt GUI. A widget
// can be placed in a layout and can contain other widgets.
#include <QWidget>

#include <vector>

// QT NOTE: The interface this class implements (see resettable.hpp).
#include "aiws/gui/resettable.hpp"
#include "aiws/processing_types.hpp"

// QT NOTE: Forward declarations. They tell the compiler "these classes exist" without
// including their full headers. This header only stores POINTERS to these widgets,
// which is enough, so the full headers (#include <QLabel>, ...) go in the .cpp
// file. This keeps builds fast and avoids circular includes.
class QLabel;
class QPlainTextEdit;
class QPushButton;
class QSlider;

namespace aiws::gui {

/**
 * @brief Presents controls and output for building query context.
 *
 * ContextWidget owns the token-budget control, Build Context action, and
 * read-only context display. It reports user requests to MainWindow; backend
 * context construction is not performed by this component.
 */
// QT NOTE: Two base classes: ContextWidget IS a QWidget (it can be shown and put in
// a layout) and IS a Resettable (it can be reset through the common interface).
// With multiple inheritance, list the QObject-derived class (QWidget) FIRST,
// because Qt's moc requires it.
class ContextWidget final : public QWidget, public Resettable {
    Q_OBJECT

public:
    /** @brief Constructs the context GUI component. @param parent Optional Qt parent widget. */
    // QT NOTE: The constructor creates the child widgets (slider, label, button, text
    // box), places them in a layout, and connects their signals to this class's slots.
    // Passing "this" or a layout as their parent means Qt deletes them for you.
    explicit ContextWidget(QWidget* parent = nullptr);

    /** @brief Returns the token budget currently selected by the user. */
    // QT NOTE: A "getter": returns the slider's current value. It only reads, hence const.
    int token_budget() const;

    /** @brief Sets whether Build Context is available. @param enabled true to enable it. */
    // QT NOTE: MainWindow calls this to turn the Build Context button on or off
    // (typically with setEnabled(enabled)). The widget does not decide when it is
    // valid; MainWindow does, because it knows the application state.
    void set_build_context_enabled(bool enabled);

    /**
     * @brief Replaces the context display with the supplied context items.
     * @param items Context items returned by ProcessingCore::build_context().
     */
    // QT NOTE: const& = pass by reference without copying, and promise not to change it.
    // The widget only DISPLAYS the items, typically by building one QString and
    // calling setPlainText() on the read-only text box.
    void set_context(const std::vector<ContextItem>& items);

    /** @brief Clears all currently displayed context output. */
    // QT NOTE: Empties the display, for example with context_text_->clear().
    void clear_context();

    /**
     * @brief Restores the component to its initial application state.
     *
     * The token budget returns to its initial value, displayed context is
     * cleared, and Build Context is disabled.
     */
    // QT NOTE: Implements Resettable::reset(). Per the M3 startup state: put the
    // slider back to its initial value (token budget 300), clear the displayed
    // context, and disable Build Context.
    void reset() override;

// QT NOTE: A SIGNAL is a message this object can announce. You only DECLARE it;
// Qt's moc generates the body, so you never write a definition. To send it:
//     emit build_context_requested(token_budget());
// Whoever connected a slot to it gets called. The widget does not know or care who.
signals:
    /**
     * @brief Emitted when the user requests context construction.
     * @param token_budget Current token budget selected by the user.
     */
    void build_context_requested(int token_budget);

private slots:
    /** @brief Converts a Build Context button event into the component request signal. */
    // QT NOTE: A SLOT is an ordinary member function that can be connected to a signal.
    // The M3 event path is: widget signal -> component slot -> component signal ->
    // MainWindow slot. Here the button's clicked() signal arrives at this slot, which
    // emits build_context_requested(token_budget()). Written in the .cpp with a
    // function pointer (no lambdas in M3):
    //     connect(build_context_button_, &QPushButton::clicked,
    //             this, &ContextWidget::request_context);
    void request_context();

    /** @brief Updates the visible budget value when the slider changes. */
    // QT NOTE: Connected to the slider's valueChanged(int) signal:
    //     connect(context_budget_slider_, &QSlider::valueChanged,
    //             this, &ContextWidget::budget_changed);
    // It only updates the visible number (budget_value_label_); it must not
    // trigger any backend work, because changing a budget just updates GUI state.
    void budget_changed(int value);

private:
    // QT NOTE: Pointers to the child widgets. They are created with new and given a
    // parent, so Qt owns and deletes them (no manual delete). {nullptr} starts each
    // pointer as null, which is safe. The trailing underscore marks private members.
    QSlider* context_budget_slider_{nullptr};
    QLabel* budget_value_label_{nullptr};
    QPushButton* build_context_button_{nullptr};
    QPlainTextEdit* context_text_{nullptr};
};

}  // namespace aiws::gui

// QT NOTE: Common beginner mistakes:
// - doing the backend work (building context) inside this widget
// - using a lambda in connect() (M3 requires named slots and function pointers)
// - forgetting to declare Q_OBJECT, so signals and slots do not work
// - not making reset() restore EVERY control (slider, label, text, button)
// - calling the signal like a normal function instead of using emit
