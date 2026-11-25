#pragma once

#include "collector/CollectionResult.hpp"
#include "config/Config.hpp"
#include "parser/ParseResult.hpp"
#include "semantic/SemanticResult.hpp"

namespace vnlc {
    class Session {
    private:
        const Config config;

        std::optional<CollectionResult> collectionResult;
        std::optional<ParseResult> parseResult;
        std::optional<SemanticResult> semanticAnalysisResult;

    public:
        Session(Config&& config);

        void run();
    };
} // namespace vnlc
