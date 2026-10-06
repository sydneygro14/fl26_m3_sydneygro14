#include "aiws/gui/search_result_model.hpp"

namespace aiws::gui {

SearchResultModel::SearchResultModel(QObject* parent)
    : QAbstractTableModel(parent) {
    // TODO
}

int SearchResultModel::rowCount(const QModelIndex& parent) const {
    // TODO
    (void)parent;  // To suppress unused-parameter warning.
    return 0;
}

int SearchResultModel::columnCount(const QModelIndex& parent) const {
    // TODO
    (void)parent;  // To suppress unused-parameter warning.
    return 0;
}

QVariant SearchResultModel::data(const QModelIndex& index, int role) const {
    // TODO
    (void)index;  // To suppress unused-parameter warning.
    (void)role;  // To suppress unused-parameter warning.
    return {};
}

QVariant SearchResultModel::headerData(int section,
                                       Qt::Orientation orientation,
                                       int role) const {
    // TODO
    (void)section;  // To suppress unused-parameter warning.
    (void)orientation;  // To suppress unused-parameter warning.
    (void)role;  // To suppress unused-parameter warning.
    return {};
}

void SearchResultModel::set_results(std::vector<SearchResult> results) {
    // TODO
    (void)results;  // To suppress unused-parameter warning.
}

void SearchResultModel::clear() {
    // TODO
}

const SearchResult* SearchResultModel::result_at(int row) const noexcept {
    // TODO
    (void)row;  // To suppress unused-parameter warning.
    return nullptr;
}

}  // namespace aiws::gui
