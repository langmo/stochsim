# stochsim
A fast ad versatile stochastic simulator. The simulator is written in C++, but also offers a convenient Matlab interface.
Different to other stochastic simulators, stochsim allows for both propensity and fixed time delay based reactions, and has the special capability to be able-if required-to assign
a unique identity to each molecule of a species/state, for example to count the number of how often a given molecule participated in
a reaction as a catalyst.

The stochsim simulator can be accessed in three different ways:
- Directly adding the source and include files, respectively the compiled static libraries and the include files, to a C++ project, and configuring and running the simulation directly via C++.
- Via a Matlab (mex) interface, mapping the C++ class structure directly to a (simplified) Matlab class structure which provides nearly the same capabilities as the C++ interface.
- By calling the stochsim executable and providing the simulation configuration as arguments.

A simulation/model can be configured in two different ways:
- Directly via the native or Matlab interface by calling the respective functions.
- Via a cmdl model file, which is essentially a very simple mini-language allowing to describe a stochsim model in an intuitive way (see below). 

Stochsim is licensed under the GNU General Public License, Version 3 (see LICENSE). It is organized into five main projects:
- stochsim: The main project. Provides the implementation of the simulator as well as the C++ interfaces.
- cmdstochsim: A small wrapper in form of an executable which can be called via the command line to configure and run a simulation.
- matstochsim: A wrapper to provide a Matlab interface for stochsim.
- expression: A simple parser for mathematical expressions, providing the functionality to e.g. define custom rates/propensities for reactions not following mass action kinetics.
- cmdlparser: A parser for cmdl model files, allowing to define a complete stochsim model via an intuitive mini-language.

Additionally, the repository also includes the project lemon. Lemon is a LALR(1) parser generator completely independent from stochsim.
It is, nevertheless, included in the repository since the parsers for the projects "expression" and "cmdlparser" are generated using
this parser generator. It is normally not required to include lemon when one wants to use stochsim. However, if one wants to change
the syntax of the two parsers, one has to regenerate the parsers using lemon, and we conveniently included lemon into the stochsim
repository to simplify this task. Both the source code to lemon itself and the code that lemon generates are in the public domain.
The source code of lemon was not modified by the authors of stochsim.
Please visit http://www.hwaci.com/sw/lemon/ for more information.

Stochsim can be easily compiled either directly via Visual C++ (the authors used Microsoft Visual Studio Community 2017, v15.2 (26430.16), or by calling
MSBuild with the solution as an argument. The Matlab interface depends on proprietary components from MathWorks which are included in Matlab distributions.
In order for the compiler to find these components, an environmental variable with name "MATLAB_DIR" (all capitalized) has to be set, pointing to the main folder of Matlab (e.g. C:\Program Files\MATLAB\R2015a). The main
folder of Matlab can be recognized by containing a directory with name "extern". Compilation was tested with Matlab R2015a.

The Matlab interface can also be compiled directly from Matlab. Specifically, this can be done by calling the script "install.m". Note, that
for this script to correctly operate, the folder structure has to match the one which is automatically generated in the "deploy" folder when compiling
stochsim via the traditional way (which is also the same folder structure as in the releases).

The CMDL mini language was originally developed for Dizzy, a Java based stochastic simulator with different goals than stochsim. Currently,
stochsim supports nearly all syntax originally defined in the Dizzy manual which can be found at http://magnet.systemsbiology.net/software/Dizzy/ .
Notable exceptions are currently lacking support for loops and macros, and a different implementation of "boundary species" in Dizzy, which
was replaced by the more intuitive concept of "modifiers" (=catalysts) in stochsim. Additionally, stochsim supports C++/Java type conditional statements ("A<5 ? 7+B : sin(pi)"), 
which can also be used in reaction specifications to e.g. describe stochastic outcomes of reactions ("A->B+[rand()<0.1 ? C : 2D], 1;"), so called
"Choices" in stochsim.
Since for most CMDL models the differences between the CMDL implementations of stochsim and Dizzy are not relevant, we expect that most
existing CMDL models from Dizzy can be immediately, without changes, be used in stochsim. We are currently working on a separate documentation for
CMDL in stochsim, until then we refer to the very nice part of the Dizzy documentation concerning CMDL which can be found at http://magnet.systemsbiology.net/software/Dizzy/ .

This software is licensed under the GNU General Public License, Version 3 (see LICENSE). It uses 3rd party components that are distributed under their own terms (see LICENSE-3RD-PARTY).