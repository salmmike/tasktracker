# Task tracker

A program for scheduling and keeping track of daily, weekly or periodic tasks.

## Structure
The lib/ directory contains a library for creating and managing tasks.

The src contains a UI program for interracting with the task manager through UI.

## Building

Built with CMake. Requires Qt6 libraries to be installed.

Set the CMAKE\_PREFIX\_PATH variable when running cmake, e.g.
`cmake -B build/ -S . -DCMAKE_PREFIX_PATH=/opt/Qt/6.2.4/gcc_64`

## Style

Use `clang-format -style="{BasedOnStyle: Mozilla, IndentWidth: 4}"`

