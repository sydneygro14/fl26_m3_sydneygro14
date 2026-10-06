// ======================================================================
// REFERENCE ONLY (version Oct. 6, 2026)
// Read this file to understand the provided interface. Do NOT add it to
// your build, and do NOT replace the provided header with it. Your project
// must keep using the original header; the required API must stay unchanged.
// ======================================================================

// QT NOTE: Big picture. TermInspectorWidget lets the user type a term and shows two
// numbers: how many documents contain it (document frequency) and, if a search
// result is selected, how often it appears in that chunk. Like every component, it
// only displays and reports; MainWindow asks the backend.
// For the basics, see search_result_model_explanation.hpp,
// context_widget_explanation.hpp and main_window_explanation.hpp.
// Comments labeled "QT NOTE" are explanations only; the code is unchanged.
#pragma once

#include <QWidget>
// QT NOTE: QString is included in full here because it is stored BY VALUE below
// (selected_chunk_id_), and a by-value member needs the full class.
#include <QString>

// QT NOTE: Defines std::size_t, the unsigned type C++ uses for counts and sizes.
#include <cstddef>

#include "aiws/gui/resettable.hpp"

// QT NOTE: Forward declarations (see context_widget_explanation.hpp): only pointers to
// these widgets are stored, so the full headers go in the .cpp.
class QLabel;
class QLineEdit;
class QPushButton;

namespace aiws::gui {

/**
 * @brief Presents term-frequency inspection controls and statistics.
 *
 * The component collects a term from the user and displays document frequency
 * for the current corpus. When a search-result chunk is selected, it can also
 * display that term's frequency in the selected chunk. ProcessingCore queries
 * are coordinated by MainWindow.
 */
// QT NOTE: A QWidget and a Resettable (QWidget first).
class TermInspectorWidget final : public QWidget, public Resettable {
    Q_OBJECT

public:
    /** @brief Constructs the term-inspector GUI component. @param parent Optional Qt parent widget. */
    // QT NOTE: Creates the term box, Inspect button and two result labels, lays them out,
    // and connects the Inspect button's clicked() signal to request_inspection().
    explicit TermInspectorWidget(QWidget* parent = nullptr);

    /** @brief Sets whether the Inspect action is available. @param enabled true to enable it. */
    // QT NOTE: MainWindow turns Inspect on only when a term can be inspected (for example
    // the corpus is current). Typically inspect_button_->setEnabled(enabled).
    void set_inspection_enabled(bool enabled);

    /**
     * @brief Records the chunk associated with the current search-result selection.
     * @param chunk_id Selected chunk identifier, or an empty string when no result is selected.
     */
    // QT NOTE: Just REMEMBERS which chunk is selected (in selected_chunk_id_); nothing is
    // searched here. The id is sent along the next time the user presses Inspect.
    // Empty = no result selected.
    void set_selected_chunk(const QString& chunk_id);

    /**
     * @brief Displays term statistics returned by the processing backend.
     * @param document_frequency Number of corpus documents containing the term.
     * @param has_chunk_frequency Whether a selected chunk frequency is available.
     * @param chunk_frequency Frequency of the term in the selected chunk when available.
     */
    // QT NOTE: Displays the numbers the backend returned. To show a number in a QLabel,
    // convert it: label->setText(QString::number(value)). If has_chunk_frequency is
    // false, show a neutral placeholder instead of the chunk number.
    void set_statistics(std::size_t document_frequency,
                        bool has_chunk_frequency,
                        std::size_t chunk_frequency);

    /** @brief Clears all displayed term statistics. */
    // QT NOTE: Puts both labels back to their neutral display.
    void clear_statistics();

    /**
     * @brief Restores the component to its initial application state.
     *
     * The term and selected chunk are cleared, statistics return to their
     * neutral display, and Inspect is disabled.
     */
    // QT NOTE: Implements Resettable::reset(): clear the term and the selected chunk,
    // reset the statistics display, and disable Inspect.
    void reset() override;

// QT NOTE: Declared only (moc writes the code). Sent with
// emit term_inspection_requested(term, chunk_id); chunk_id may be empty.
signals:
    /**
     * @brief Emitted when the user requests term inspection.
     * @param term Term entered by the user.
     * @param chunk_id Current selected chunk identifier; may be empty.
     */
    void term_inspection_requested(const QString& term, const QString& chunk_id);

private slots:
    /** @brief Converts an Inspect button event into the term_inspection_requested signal. */
    // QT NOTE: The Inspect button's clicked() signal arrives here. It reads the term
    // (term_edit_->text()), and emits term_inspection_requested(term, selected_chunk_id_).
    void request_inspection();

private:
    // QT NOTE: The first four members are POINTERS to child widgets (created with new,
    // owned by Qt through the parent/layout, {nullptr} = not created yet).
    QLineEdit* term_edit_{nullptr};
    QPushButton* inspect_button_{nullptr};
    QLabel* document_frequency_value_{nullptr};
    QLabel* chunk_frequency_value_{nullptr};
    // QT NOTE: Different on purpose: QString is a VALUE type, like std::string. No new, no
    // delete, no pointer. A default-constructed QString is empty, so no {nullptr} is
    // needed (that is the "nothing selected" state).
    QString selected_chunk_id_;
};

}  // namespace aiws::gui

// QT NOTE: Common beginner mistakes:
// - calling the backend from this widget (MainWindow does that)
// - forgetting to clear selected_chunk_id_ in reset()
// - treating "no chunk selected" (empty id) as an error: it is a normal state
// - showing a chunk frequency when has_chunk_frequency is false
// - using a lambda in connect()
