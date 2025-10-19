# custom-clang-analyzer-sugestions

## Authors

* [**Nikola Nesic**](https://github.com/neske99)


## Problem description
The goal of this project is to extend the llvm static analyzer with 2 new implementations of my choice.
One of the analyzers detects direct diamond problem occurence and suggests it can be fixed.
The Second one reports a private field of a class that is not initialized in at least one constructor and points it out, suggesting we add a default field to it.

## Direct diamond problem solution description
First we go iterate through all the classes and fill in global mappings for all the classes so we can anlyze them after.
The first mapping is the class to its in level(Number of its base classes from which it inherits). The second mapping, we map the class to the list of classes that inherit from that class and the virtuality of that inheritance.

When we have those mappings we make a queue of classes with in levels zero like in the Kan Algorithm for topological sortig. After that we iterate through the queue and for each class check its grandchildrens inheritance to check if the diamond problem occurs in it direct form, using a dfs algorithm that goes to the depth of 2.After that we add the child to the end of the zero parents queue.


## Private field not initialized in constructor problem
For each class, for each private field(of that class), we try and find a constructor that doesen't initialize that field.


