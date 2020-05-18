Mie++ and RI_Map

Mie Scattering / Refractive Idex Correction Code
James Dorsey - University of Manchester - May 2010

CONTENTS

  This directory contains all source code and the makefile for the
  RI correction developed by James Dorsey at Manchester University
  during May 2010. The executables are for Windows and Linux, with
  the Windows versions distinguishable by the ".exe" extension.

WARNING

  You should clear all files out of the input and output
  directories before making a new refractive index map.
  If you don't, you'll get confused.

  The constants used to scale the output have been selected to give
  agreement between this software and Phil Rosenberg's windows GUI
  based Mie calculator. I don't agree with the way he's calculated
  scattering cross sections, but the difference between my preferred
  version and what's in this program is linear, so it makes no
  difference to the calculated diameter maps.

COMPILING

  To compile, make sure you have the following installed:

    GNU C++ compiler
    GNU Fortran 95 compilers
    GNU Make utilities

  The easiest way to do this on Windows is to install CYGWIN.

  Once your (UNIX) system is set up, type the following into a terminal:

    make
    make tidy

  For Windows executables you'll have to play about with the makefile
  unless you're building them on a well set up UNIX system, in which
  case just type:

    make windows
    make tidy

  For reduced executable size, enter make strip instead of just make.
  There's no great advantage to this, but the option's there. i.e.

  UNIX

    make strip
    make tidy

  Windows

    make winstrip
    make tidy

  That's it.

USING

  Type

    mie++

  to execute the Mie scattering code. You will see a helpful
  message about the parameters you need to enter to run the code. To
  see a full list of supported probe types, enter 

    mie++ which 0 0 0 0 0

  Running mie++ generates a scattering curve for particles with the
  properties specified on the command line. You'll need two of these
  as input for ri_map, along with a file containing the threshold
  diameters for the probe you're interested in. The input directory
  contains an example threshold file - CAS_Thresh.csv.

  Type

    ri_map

  to see another handy prompt about inputs and outputs.

  ri_map searches for water (control) scattering cross sections equal
  to the mapped scattering cross section within 10nm of each
  threshold diameter. It records the water equivalent diameter for
  each equivalent scattering cross section, providing the sizing
  thresholds for "mapped" particles in an instrument set up with
  threshold tables for "control" particles.

  Any problems with compiling or using either Mie++ or ri_map should
  be reported to James Dorsey.

  j.dorsey@manchester.ac.uk

UTILITIES

  For an example of how to run ri_map see the script named
  eyjafjallajokull in the scripts directory.

EXAMPLE

  Here's an example of a typical session:

james@goliath2:~/models/mie/mie++$ ./mie++

Usage: mie++ probe dmin dmax dstep nr nc [astep]

  probe - DMT probe type
  dmin  - Minimum particle diameter
  dmax  - Maximum particle diameter
  dstep - Particle diameter resolution
  nr    - Real component of refractive index
  nc    - Imaginary component of refractive index
  astep - Angular resolution to integrate over in degrees

i.e. There MUST be one text and five numeric arguments

james@goliath2:~/models/mie/mie++$ ./mie++ which 0 0 0 0 0

You MUST specify a probe type. Only the following are currently supported:
  BCP
  CAS
  CAS-B
  CAS-F
  CDP
  FSSP
  GRIMM (1.10x series)
  PCASP
  UHSAS
  WIBS

Note that probe type is case sensitive in this version of the software
CAS is equivalent to CAS-F (i.e. forward scatter) while CAS-B is backscatter

james@goliath2:~/models/mie/mie++$ ./mie++ GRIMM 0.6 0.6 0.01 1.33 0.0 0.1
GRIMM Diameter, Qscat (1.33 + 0i)
0.600000, 1.734527
james@goliath2:~/models/mie/mie++$./mie++ CAS-F 0.6 50.0 0.01 1.54 0.003 > AshScatteringCurve.csv
james@goliath2:~/models/mie/mie++$
