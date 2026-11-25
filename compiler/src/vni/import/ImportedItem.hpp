#pragma once

#include <string>
#include <string_view>

namespace vnlc {
    class ImportedPackage;

    class ImportedItem {
    private:
        std::string name;

    protected:
        ImportedItem(std::string_view name);

    public:
        [[nodiscard]] std::string_view getName() const;
        [[nodiscard]] const ImportedItem* getChildByName(std::string_view name) const;

        virtual ~ImportedItem() = default;
    };
} // namespace vnlc
