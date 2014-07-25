This directory contains source code and documents concerning Melkor.

 Melkor is a Go playing program that implements GTP v2.0 (Go Text Protocol).
See the GnuGo website for details on GTP. This program uses game tree search
in conjunction with neural network methods. The game tree search is actually
the MTD(f) algorithm in an iterative deepening framework. It uses Enhanced 
Transposition Cutoffs (ETC) and BMF (Best Move First). The neural network (NN)
is used to order moves in the tree, prune them if required and to determine
when to pass.

*******************
** Documentation **
*******************

Docs
----

Contains the project specification and a sub directory for UML design documents.

********************
** Source modules **
********************

Melkor
-------

Melkor is the Go playing engine which can be run standalone with a text menu 
interface or through the Go Text Protocol (GTP). 

Goal
----

A set of classes defining abstract goals.

MelkorDLL
---------

A DLL abstraction layer to simplify construction of a GUI.

NeuralNetwork
-------------

Contains neural network specific code to allow use of various types of neural
network including BPNs and temporal difference based nets.

Patterns
--------

Contains code to allow pattern matching. Including Eye Space Pattern Matching (ESPM)
which is used by the Life and Death goal for evaluation and move suggestion.

Sgf
---

Contains code to allow reading of SGF version 4 files.

Util
----

Contains various pieces of source code that are shared by many of the 
project programs.


*************************************
** Neural Network related programs **
*************************************

 In the subdirectory "NNPrograms" are several projects to aid the development
and training of neural networks.

CreateNet
---------

CreateNet is a program to create a specified BPN neural network, specifying number
of layers, neurons per layer and other variables.

NetInfo
-------

NetInfo is a program to print out various pieces of information about a BPN
neural network.

TestNet
-------

TestNet is a program that performs a selection of performance tests on a 
specified neural network.

Trainnet
--------

Trainnet is a program used to train neural networks on sgf games to assign
some sort of quality score to a suggested move.

*******
* GUI *
*******

 In the subdirectory "gui" will be found programs to provide a graphical user interface
to Melkor.

MelkorNETGUI
------------

 A GUI using the .NET framework and MelkorDLL.
 
********
* Test *
********

 In the subdirectory "test" will be found programs and scripts to allow performance and
regression testing for Melkor.