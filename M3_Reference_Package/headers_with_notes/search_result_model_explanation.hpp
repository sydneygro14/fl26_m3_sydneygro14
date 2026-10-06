// ======================================================================
// REFERENCE ONLY (version Oct. 6, 2026)
// Read this file to understand the provided interface. Do NOT add it to
// your build, and do NOT replace the provided header with it. Your project
// must keep using the original header; the required API must stay unchanged.
// ======================================================================

// QT NOTE: Big picture.
// Qt separates WHERE DATA LIVES (the model) from HOW IT
// IS SHOWN (the view). QTableView only draws. It asks this model
// questions such as "how many rows?" and "what text goes in this cell?".
// This class is the model: it owns the search results and answers.
// Each "QT NOTE" group of comments is explanation only; the code and
// the required API are unchanged. Do NOT change the signatures below.

#pragma once

// QT NOTE: QAbstractTableModel is Qt's ready-made base class for flat tables.
// It handles the hard parts so you only implement a few functions.
// (QAbstractListModel = one column; QAbstractItemModel = trees.)
#include <QAbstractTableModel>

#include <vector>

#include "aiws/processing_types.hpp"

// QT NOTE: A namespace is a name prefix that avoids clashes with other code.
namespace aiws::gui {

/**
 * @brief Qt table model that presents SearchResult objects in the results view.
 *
 * The model exposes one row per search result and the M3 result columns used by
 * the QTableView. It also provides access to the SearchResult represented by a
 * selected row.
 */
// QT NOTE: "final" means no class can inherit from this one.
// Q_OBJECT turns on Qt features (signals/slots, etc.). It needs Qt's
// moc tool, so CMake must have set(CMAKE_AUTOMOC ON).
class SearchResultModel final : public QAbstractTableModel {
    Q_OBJECT

public:
    /**
     * @brief Constructs an empty search-result model.
     * @param parent Optional Qt parent object.
     */
    // QT NOTE: Qt ownership: if you pass a parent (for example "this" from a
    // widget), Qt deletes the model when the parent is destroyed, so
    // you do not call delete yourself. "explicit" blocks accidental
    // automatic conversions from a QObject*.
    explicit SearchResultModel(QObject* parent = nullptr);

    /**
     * @brief Returns the number of search-result rows in the model.
     * @param parent Parent model index. The M3 model is a flat table.
     * @return Number of top-level result rows, or 0 for a valid parent index.
     */
    // QT NOTE: The view asks: "how many rows should I draw?"
    // "override" = replaces a function from the Qt base class; the
    // compiler errors if the signature does not match exactly.
    // "const" at the end = this function only reads, never modifies.
    // "parent" exists because Qt models can also be trees. A default
    // QModelIndex() is an INVALID index and means "the top level".
    // Typical body:
    //    if (parent.isValid()) return 0;   // table cells have no children
    //    return static_cast<int>(results_.size());
    // Default arguments appear only here in the header, not in the .cpp.
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief Returns the number of columns in the M3 search-results table.
     * @param parent Parent model index. The M3 model is a flat table.
     * @return Number of top-level table columns, or 0 for a valid parent index.
     */
    // QT NOTE: "How many columns?" Same pattern as rowCount(): return 0 for a
    // valid parent, otherwise a fixed number (one per result field).
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief Returns display data for a search-result table cell.
     * @param index Model index identifying the requested cell.
     * @param role Qt item-data role being requested.
     * @return Display value for supported indexes and roles; otherwise an empty QVariant.
     */
    // QT NOTE: "What goes in this cell?" The view calls this for every visible
    // cell, so keep it fast and do not copy big objects.
    // index.row() / index.column() say WHICH cell is requested.
    // role says WHAT KIND of data (text, color, alignment, ...). For
    // M3 only answer Qt::DisplayRole (the text to show).
    // QVariant is a container for ONE value of any common type (int,
    // QString, double, ...). That lets one function return different
    // types. Return {} (an empty QVariant) for "no value".
    // Convert std::string with QString::fromStdString(...), because Qt
    // widgets work with QString.
    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const override;

    /**
     * @brief Returns header data for the search-results table.
     * @param section Row or column header section.
     * @param orientation Header orientation.
     * @param role Qt item-data role being requested.
     * @return Header value for supported sections and roles; otherwise an empty QVariant.
     */
    // QT NOTE: "What is the title of this header cell?" For column titles,
    // orientation == Qt::Horizontal and section is the column number.
    // Return a QString such as "Score" for Qt::DisplayRole, and {}
    // for anything you do not handle.
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    /**
     * @brief Replaces all search results represented by the model.
     * @param results New ranked search results.
     */
    // QT NOTE: Takes the vector BY VALUE so callers can hand it over cheaply
    // with std::move. When the data changes, tell the view or it will
    // keep showing old rows:
    // beginResetModel();
    // results_ = std::move(results);
    // endResetModel();
    void set_results(std::vector<SearchResult> results);

    /**
     * @brief Removes all search results from the model.
     */
    // QT NOTE: Same idea as set_results() with an empty vector, again wrapped
    // in beginResetModel() / endResetModel().
    void clear();

    /**
     * @brief Returns the SearchResult represented by a model row.
     * @param row Zero-based result row.
     * @return Pointer to the corresponding result, or nullptr when row is invalid.
     */
    // QT NOTE: The bridge from a user's click to your backend object: the
    // selected row number gives you the SearchResult. A pointer is
    // returned because "nothing" (nullptr) is a valid answer for a bad
    // row; always check the row range first. "noexcept" promises this
    // function never throws. Do not keep the pointer after the results
    // change, because it may then dangle.
    const SearchResult* result_at(int row) const noexcept;

private:
    // QT NOTE: The model's own copy of the data. The trailing underscore is a
    // naming convention meaning "private data member".
    // Keep retrieval/search logic OUT of the model: it only adapts
    // data for display.
    std::vector<SearchResult> results_;
};

}  // namespace aiws::gui

// QT NOTE: Wiring it up (for example in a widget's constructor):
// auto* model = new SearchResultModel(this);
// auto* view  = new QTableView(this);
// view->setModel(model);        // the view now asks the model for data
// model->set_results(results);  // the table appears
// setModel() does not take ownership; the "this" parent keeps the
// model alive.
//
// QT NOTE: Common beginner mistakes:
// - changing results_ without beginResetModel()/endResetModel()
// - not returning {} for roles other than Qt::DisplayRole
// - returning the full row count when parent is valid
// - repeating default arguments (= Qt::DisplayRole) in the .cpp file
// - putting retrieval or search logic inside the model
