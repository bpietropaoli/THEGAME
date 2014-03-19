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
# Author:                           #
# Bastien Pietropaoli               #
#                                   #
# Bastien.Pietropaoli@inria.fr      #
# Bastien.Pietropaoli@gmail.com     #
#                                   #
# Last update: 24/02/2014           #
#####################################

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
	make
	
This will build the libraries (static and shared) in build/lib/Release.


The given main in Tests.c launches a ton of tests to check if everything is okay. 
If you built as told before, you can launch the test program with `make test`.


Building the documentation
==========================

Building the documentation requires doxygen.

To build the documentation :

	cd doc
	doxygen Doxyfile

This will build the html documentation under doc/html.

Please, check the documentation for the way to use this little library, 
get the references and the version notes.

The config.h file is used to customize what you need. Look for it!

