# stochsim
A fast stochastic simulator written in C++ allowing for propensity and fixed delay based reactions, as well as for species where each molecule has variable properties.

The Matlab interface can either be compiled using Visual C++, or via Matlab's mex command. If using Visual Studio, an environmental
variable with name "MATLAB_DIR" (all capitalized) has to be set, pointing to the main folder of Matlab (e.g. C:\Program Files\MATLAB\R2015a). The main
folder of Matlab can be recognized by containing a directory with name "extern".


This software is licensed under the GNU General Public License, Version 3 (see LICENSE).
It uses 3rd party components that are distributed under their own terms (see LICENSE-3RD-PARTY).