# custom-clang-analyzer-sugestions

## Authors

* [**Nikola Nesic**](https://github.com/neske99)


## Problem description
The goal of this project is to extend the llvm static analyzer with 2 new implementations of my choice.
One of the analyzers detects direct diamond problem occurence and suggests it can be fixed.
The Second one reports a private field of a class that is not initialized in at least one constructor and points it out, suggesting we add a default field to it.

## Solution description

