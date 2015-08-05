#####################################
# THeory of Evidence in a lanGuage  #
# Adapted for Many Embedded systems #
#         (THE GAME)                #
# --------------------------------- #
# Copyright 2011-2014, EDF. This    #
# software was developed with the   #
# collaboration of INRIA            #
# (Bastien Pietropaoli)             #
# --------------------------------- #
# Authors:                          #
# Bastien Pietropaoli               #
# Bastien.Pietropaoli@inria.fr      #
# Bastien.Pietropaoli@gmail.com     #
#                                   #
# Aurélien Richez                   #
# Aurelien.Richer@inria.fr          #
#                                   #
# Last update: 20/03/2014           #
#####################################

This little library implements the basics of the belief functions theory. There are also a few application modules in order to build belief functions from sensor measures, from other belief functions and from randomness. To run tests and a bench for speed of execution, please use the Tests module.

Keywords:BFT, Theory of Evidence, Dempster-Shafer's theory.

Compiling
=========

The Project needs cmake to be built.

Before building, you can change the config of the library in order to add or
remove some model checking for instance (but the current configuration should
work fine most of the time). 

I advise to build it out of source directory as follow :

	#create an out of source build dir
	mkdir build && cd build
	#to enable debug use -DCMAKE_BUILD_TYPE=Debug
	cmake -DCMAKE_BUILD_TYPE=Release ..
	#And then build the library
	make
	
This will build the libraries (static and shared) in build/lib/Release.

The given main in Tests.c launches a ton of tests to check if everything is okay. 
If you built as told before, you can launch the test program with `make test`.
The tests can be build only if you have the package "check".

Building the documentation
==========================

Building the documentation requires doxygen and cmake. In your build directory,
just call `make doc` after doing the steps described in the compiling part, 
this will build your documentation in generatedDoc/html.
There are images in the directory doc, just copy the directory "images" to the
"generatedDoc" directory.
To get to the homepage of the doc, open index.html.

Please, check the documentation for the way to use this little library, 
get the references and the version notes.

The config.h file is used to customize what you need. Look for it!

