#include "aiws/processing_core.hpp"

#include "aiws/chunker.hpp"
#include "aiws/context_builder.hpp"
#include "aiws/corpus_index.hpp"
#include "aiws/retrieval_engine.hpp"
#include "aiws/text_processor.hpp"

#include <stdexcept>
#include <unordered_set>
#include <utility>

namespace aiws {

struct ProcessingCore::Impl {
    // corpus state, same as m1, rebuilt on every successful rebuild() call
    std::vector<Chunk> chunks;
    CorpusIndex index;

    // owned polymorphic processing capabilities, one active implementation
    // per role, selected at construction time and used for the lifetime
    // of this processingcore
    std::unique_ptr<ChunkingStrategy> chunking;
    std::unique_ptr<RetrievalStrategy> retrieval;
    std::unique_ptr<ContextStrategy> context;

    // takes ownership of three already-validated strategy objects
    Impl(std::unique_ptr<ChunkingStrategy> chunking_strategy,
        std::unique_ptr<RetrievalStrategy> retrieval_strategy,
        std::unique_ptr<ContextStrategy> context_strategy)
        : chunking(std::move(chunking_strategy)),
          retrieval(std::move(retrieval_strategy)),
          context(std::move(context_strategy)) {}
};

ProcessingCore::ProcessingCore()
    : impl_(std::make_unique<Impl>(
          std::make_unique<Chunker>(ChunkingPolicy{kMaxChunkTokens, kChunkOverlap,
                                                    kParagraphPreferenceWindow}),
          std::make_unique<RetrievalEngine>(),
          std::make_unique<ContextBuilder>())) {}

ProcessingCore::ProcessingCore(std::unique_ptr<ChunkingStrategy> chunking,
                               std::unique_ptr<RetrievalStrategy> retrieval,
                               std::unique_ptr<ContextStrategy> context) {
    // a null strategy argument is an invalid configuration per spec, checked
    // before any ownership transfer happens
    if (!chunking || !retrieval || !context) {
        throw std::invalid_argument("strategy arguments must not be null");
    }
    impl_ = std::make_unique<Impl>(std::move(chunking), std::move(retrieval), std::move(context));
}

ProcessingCore::~ProcessingCore() = default;
ProcessingCore::ProcessingCore(ProcessingCore&&) noexcept = default;
ProcessingCore& ProcessingCore::operator=(ProcessingCore&&) noexcept = default;

std::string ProcessingCore::normalize(const std::string& text) {
    return TextProcessor::normalize(text);
}

void ProcessingCore::rebuild(const Workspace& workspace) {
    std::unordered_set<std::string> document_ids;
    std::vector<Chunk> next_chunks;
    for (std::size_t order = 0; order < workspace.documents().size(); ++order) {
        const auto& doc = workspace.documents()[order];
        if (!document_ids.insert(doc.id()).second) {
            throw std::invalid_argument("duplicate document id: " + doc.id());
        }
        auto produced = impl_->chunking->chunk(doc, order);
        next_chunks.insert(next_chunks.end(),
                           std::make_move_iterator(produced.begin()),
                           std::make_move_iterator(produced.end()));
    }
    CorpusIndex next_index(next_chunks);
    impl_->chunks = std::move(next_chunks);
    impl_->index = std::move(next_index);
}

const std::vector<Chunk>& ProcessingCore::chunks() const noexcept { return impl_->chunks; }
std::size_t ProcessingCore::chunk_count() const noexcept { return impl_->chunks.size(); }

std::size_t ProcessingCore::document_frequency(const std::string& term) const {
    const auto terms = TextProcessor::terms(term);
    if (terms.empty()) return 0;
    if (terms.size() != 1) throw std::invalid_argument("term must normalize to one token");
    return impl_->index.document_frequency(terms.front());
}

std::size_t ProcessingCore::term_frequency(const std::string& term,
                                           const std::string& chunk_id) const {
    const auto terms = TextProcessor::terms(term);
    if (terms.empty()) return 0;
    if (terms.size() != 1) throw std::invalid_argument("term must normalize to one token");
    return impl_->index.term_frequency(terms.front(), chunk_id);
}

std::vector<SearchResult> ProcessingCore::search(const std::string& query, int k) const {
    return impl_->retrieval->search(query, k, impl_->chunks, impl_->index);
}

std::vector<ContextItem> ProcessingCore::build_context(const std::string& query,
                                                       int k,
                                                       std::size_t token_budget) const {
    if (k < 0) throw std::invalid_argument("k must be non-negative");
    if (token_budget == 0) return {};
    return impl_->context->build(search(query, k), token_budget);
}

}  // namespace aiws