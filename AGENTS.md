# The Vanillang Project

## Overview
Vanillang is a DSL for Minecraft datapack development. It is cross-platform, and should be available on Windows, macOS and Linux.

- Refer to `spec` to get specification about Vanillang language.
- Refer to Minecraft Wiki to get information about Minecraft datapack and mcfunction.

## Commands
Be sure that your working directory is the root directory of the repository before running these commands.

- Use `cmake --preset default-ninja` to generate build directory.
- Use `cmake --build --preset debug` to get a debug build.
- Use `cmake --build --preset release` to get a release build.

## Commits
- Follow conventional commits when you need to write commit messages.
- Changes in each subproject should be committed separately.

## Explanation
- Always explain the reason why you make such changes according to `git diff`.