# stochsim
A fast and versatile stochastic simulator. The simulator is written in C++, but also offers a convenient Matlab interface.
Different to many other stochastic simulators, stochsim provides functionality for
- both propensity and fixed time delay reactions (reactions firing a pre-defined time after a molecule was created).
- custom reaction rate/propensity definitions to implement non-mass action kinetic reactions (e.g. logarithmic growth).
- assigning-if required-a unique identity to each molecule of a species/state, for example to count the number of how often a given molecule participated in
a reaction as a catalyst.
- probabilistic outcomes of reactions, e.g. A->B with probability 0.1, and A->C otherwise. This is especially useful when combined with fixed time delay reactions, or with molecules having each a unique identity (probabilities can depend on this identity).

The stochsim simulator can be accessed in three different ways:
- Directly adding the C++ source and header files, respectively the compiled static libraries and the header files, to a C++ project, and configuring and running the simulation directly via C++.
- Via a Matlab (mex) interface, mapping the C++ class structure directly to a (simplified) Matlab class structure which provides nearly the same capabilities as the C++ interface.
- By calling a provided standalone executable and providing the simulation configuration as an argument.

A simulation/model can be configured in two different ways:
- Directly via the native or Matlab interface by calling the respective functions.
- Via a cmdl model file, which is essentially a simple mini-language allowing to describe a stochsim model in an intuitive way (see below). 

Stochsim is organized into five sub-projects:
- stochsim: The main project. Provides the implementation of the simulator as well as the C++ interfaces to configure and run a simulation".
- cmdstochsim: A small wrapper in form of an executable which can be called via the command line to configure and run a simulation.
- matstochsim: A wrapper to provide a Matlab interface for stochsim.
- expression: A simple parser for mathematical expressions, providing the functionality to e.g. define custom rates/propensities for reactions not following mass action kinetics.
- cmdlparser: A parser for cmdl model files, allowing to define a complete stochsim model via an intuitive mini-language.

Additionally, the repository also includes the project "lemon". Lemon is a LALR(1) parser generator completely independent from stochsim.
It is, nevertheless, included in the repository since the parsers for the projects "expression" and "cmdlparser" are generated using
this parser generator. It is normally not required to include lemon when one wants to use stochsim. However, if one wants to change
the syntax of the two parsers, one has to regenerate the parsers using lemon, and we conveniently included lemon into the stochsim
repository to simplify this task. Both the source code to lemon itself and the code that lemon generates are in the public domain.
The source code of lemon was not modified by the authors of stochsim.
Please visit http://www.hwaci.com/sw/lemon/ for more information.

Stochsim can be easily compiled either directly via Visual C++ (the authors used Microsoft Visual Studio Community 2017, v15.2 (26430.16)), or by calling
MSBuild with the solution as an argument. Compilation should also work with other compilers supporting C++11 and/or on other OSs (stochsim only uses the C++ standard libraries), even though this might require the generation of a makefile compatible with the respective setup.
The Matlab interface depends on proprietary components from MathWorks which are included in Matlab distributions.
In order for the compiler to find these components, an environmental variable with name "MATLAB_DIR" (all capitalized) has to be set, pointing to the main folder of Matlab (e.g. C:\Program Files\MATLAB\R2015a). The main
folder of Matlab can be recognized by containing a directory with name "extern". Compilation was tested with Matlab R2015a.

The Matlab interface can also be compiled directly from Matlab. Specifically, this can be done by calling the script "install.m". Note, that
for this script to correctly operate, the folder structure has to match the one which is automatically generated in the "deploy" folder when compiling
stochsim via the traditional way (which is also the same folder structure as in the releases).
For this to work, mex has to be configured to use a C++ compiler supporting C++11. In Matlab, the compiler can be chosen using the "mex -setup" command.
The authors tested compilation in Matlab R2015a using msvc2017 as the mex compiler, but compilation should also work with other C++11 compilers and/or on other OSs
it should also work with other compilers and/or on other OSs (stochsim only uses the C++ standard libraries).; this compiler is part of Microsoft Visual Studio Community 2017. Only relatively new versions of Matlab seem to automatically support this compiler.
See https://www.mathworks.com/support/bugreports/1487958 on how support can be added to R2017a and R2016b. Support can also be added to R2015a by changing 
LINKEXPORTVER="/EXPORT:mexFunction /EXPORT:mexfilerequiredapiversion" to LINKEXPORTVER="/EXPORT:mexFunction" in both XMLs. It seems that the additional export
"mexfilerequiredapiversion" was added after R2015a, but is not really necessary.

The CMDL mini language was originally developed for Dizzy, a Java based stochastic simulator with different goals than stochsim. Currently,
stochsim supports nearly all syntax originally defined in the Dizzy manual which can be found at http://magnet.systemsbiology.net/software/Dizzy/ .
Notable exceptions are currently lacking support for loops and macros, and a different implementation of "boundary species" ("$A") than in Dizzy, which
was replaced by the more intuitive concept of "modifiers" (=catalysts) in stochsim (that is, species can be boundary species in one reaction, but normal species in other). Additionally, stochsim supports C++/Java type conditional statements ("A<5 ? 7+B : sin(pi)"), 
which can also be used in reaction specifications to e.g. describe stochastic outcomes of reactions ("A->B+[rand()<0.1 ? C : 2D], 1;"), so called
"Choices" in stochsim. When adding a boundary species both on the left and the right hand side of a reaction, with the stochiometry of the RHS smaller or equal to the one on the RHS, a species becomes a
"transformee". The concentration of a transformee neither increases nor decreases when a reaction fires. Instead, it is counted how often a species was a transformee during the course of its existence, which can
be either exported to a csv file or used in Choices (automatically defined variable "numModified").
Since for most CMDL models the differences between the CMDL implementations of stochsim and Dizzy are not relevant, we expect that most
existing CMDL models from Dizzy can be immediately, without changes, be used in stochsim. We are currently working on a separate documentation for
CMDL in stochsim, until then we refer to the very nice part of the Dizzy documentation concerning CMDL which can be found at http://magnet.systemsbiology.net/software/Dizzy/ .

This software is licensed under the GNU General Public License, Version 3 (see LICENSE). It uses 3rd party components that are distributed under their own terms (see LICENSE-3RD-PARTY).