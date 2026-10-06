#include "aiws/gui/search_result_model.hpp"

#include <QString>

#include <utility>

namespace aiws::gui {

namespace {

// column titles in display order
constexpr const char* kHeaders[] = {"Rank", "Document", "Chunk", "Score", "Text"};
constexpr int kColumns = static_cast<int>(sizeof(kHeaders) / sizeof(kHeaders[0]));

}  // namespace

SearchResultModel::SearchResultModel(QObject* parent)
    : QAbstractTableModel(parent) {}

int SearchResultModel::rowCount(const QModelIndex& parent) const {
    // flat table so a valid parent index has no child rows
    return parent.isValid() ? 0 : static_cast<int>(results_.size());
}

int SearchResultModel::columnCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : kColumns;
}

QVariant SearchResultModel::data(const QModelIndex& index, int role) const {
    // only display text is supplied and every other role gets an empty value
    const SearchResult* result =
        index.isValid() && role == Qt::DisplayRole ? result_at(index.row()) : nullptr;
    if (!result) return {};

    // one value per column using the type qt displays best
    switch (index.column()) {
        case 0: return index.row() + 1;
        case 1: return QString::fromStdString(result->document_id);
        case 2: return static_cast<qulonglong>(result->chunk_sequence);
        case 3: return result->score;
        case 4: return QString::fromStdString(result->text);
        default: return {};
    }
}

QVariant SearchResultModel::headerData(int section,
                                       Qt::Orientation orientation,
                                       int role) const {
    // only horizontal column titles are supplied
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal ||
        section < 0 || section >= kColumns) {
        return {};
    }
    return QString::fromLatin1(kHeaders[section]);
}

void SearchResultModel::set_results(std::vector<SearchResult> results) {
    // reset notifications make the view drop old rows and redraw
    beginResetModel();
    results_ = std::move(results);
    endResetModel();
}

void SearchResultModel::clear() {
    beginResetModel();
    results_.clear();
    endResetModel();
}

const SearchResult* SearchResultModel::result_at(int row) const noexcept {
    // an out of range row is a normal answer and maps to no result
    if (row < 0 || static_cast<std::size_t>(row) >= results_.size()) return nullptr;
    return &results_[static_cast<std::size_t>(row)];
}

}  // namespace aiws::gui