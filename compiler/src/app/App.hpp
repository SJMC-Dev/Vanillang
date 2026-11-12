#pragma once

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>

namespace vnlc {
    class App {
    private:
        CLI::App app;

        int argc;
        char** argv;

    public:
        App(int argc, char** argv);

        App() = delete;
        App(const App&) = delete;
        App(App&&) noexcept = delete;

        App& operator=(const App&) = delete;
        App& operator=(App&&) noexcept = delete;

        void run();
    };
} // namespace vnlc
