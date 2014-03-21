/*
 * Copyright 2011-2014, EDF. This software was developed with the collaboration of INRIA (Bastien Pietropaoli)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef DEF_CONFIG
#define DEF_CONFIG

/**
 * @file config.h
 * @brief UTILITY: A file to config the compilation.
 */

/**
 * @def WINDOWS
 * Uncomment it to compile under Windows.
 */
/*#define WINDOWS*/
/**
 * @def UNIX
 * Uncomment it to compile under Unix systems.
 */
#define UNIX


/**
 * @def DEBUG
 * Uncomment it before compiling to get debug messages (memory failure, etc).
 */
#define DEBUG
/**
 * @def CHECK_MODELS
 * Uncomment it before compiling to check the validity of models in the 
 * applicative modules. Requires some more computation during the loading
 * phase.
 */
#define CHECK_MODELS
/**
 * @def CHECK_VALUES
 * Uncomment it before compiling to check the validity of belief values.
 */
#define CHECK_VALUES
/**
 * @def CHECK_SUM
 * Uncomment it before compiling to check the sum of mass functions.
 */
#define CHECK_SUM
/**
 * @def CHECK_COMPATIBILITY
 * Uncomment it before compiling to check the compatibility of fused mass functions.
 */
#define CHECK_COMPATIBILITY



/**
 * @def MAX_STR_LEN
 * The maximum length of the strings.
 */
#define MAX_STR_LEN 1024


#ifdef DEBUG
	#ifdef __GNUC__
	/* we check if we are using gcc. As using __func__ may not work with others
	 * compilers (__func__ was introduced in C99). */
	#define DEBUG_MALLOC_MESSAGE(var) \
	fprintf(stderr,"debug - %s:%d malloc failed for " #var " in %s.\n",\
				__FILE__, __LINE__, __func__)

	#else /* __GNUC__ */
		#define DEBUG_MALLOC_MESSAGE(var) \
		fprintf(stderr,"debug - %s:%d malloc failed for " #var ".\n", __FILE__, __LINE__)
	#endif

	#define DEBUG_CHECK_MALLOC(var) if(NULL == (var)) { DEBUG_MALLOC_MESSAGE(var); }

	#define DEBUG_CHECK_MALLOC_OR_RETURN(var, toReturn) if(NULL == (var)) { \
		DEBUG_MALLOC_MESSAGE(var); \
		return toReturn;\
	}
#else /* DEBUG */

	#define DEBUG_CHECK_MALLOC(var)
	#define DEBUG_CHECK_MALLOC_OR_RETURN(var, toReturn)

#endif /* DEBUG */





/**
 * @mainpage Belief Functions Implementation
 * @section intro_sec Introduction
 * This little library implements the basics to manipulate belief functions.
 * The module BeliefsFromSensors enables the creation of belief functions from sensor measures.
 * To run tests and a bench for speed of execution, please use the Tests module.
 *
 * If some errors occur anywhere when running the tests, please check the format of the
 * files in data/beliefs/. If you're stuck anywhere, feel free to send me an email.
 * Different combination rules are implemented to enable the use in the case
 * of different theories. The source code is documented AND commented (most of the time) to 
 * facilitate its reading. It is also under the Apache License v2 (http://www.apache.org/licenses/LICENSE-2.0), 
 * thus, if any error/weird behaviour/bad coding is detected, please feel free to correct the source code and to
 * notify me at one of the given email addresses given below. @n
 *
 * If something you need is missing and you do not want/cannot contribute to the development, send me
 * an email with your request and I will see what I can do for you! This library is meant to be as useful
 * as possible! Anyway, please read the doc and specifically the description of modules, it should explain
 * what it does.
 * 
 * This implementation of the basics of the belief functions theory has been created in order to test the applicability
 * of this theory in specific cases. Thus, every part has not been developped yet! Anyway, a huge part of the work consisted in
 * the optimization of the computations. Even if that's not perfert at all, a real effort has been done.
 *
 * What it does:
 * @li Sets: creation and manipulation of sets (required for belief functions) and generation of powersets.
 * @li Belief functions: manipulation, fusion/combination, characterization and decision making from belief functions. 
 *
 * The building of belief functions has been implemented in three cases:
 * @li Using raw sensor measures --> BeliefsFromSensors module
 * @li Transforming belief functions --> BeliefsFromBeliefs module
 * @li Random generation of belief functions --> BeliefsFromRandomness module
 *
 * @section compil_sec Compiling
 * The Project needs cmake to be built.
 * 
 * Before building, you can change the config of the library in order to add or
 * remove some model checking for instance (but the current configuration should
 * work fine most of the time).  
 *
 * I advise to build it out of source directory as follow:
 *
 * @code
 *	#create an out of source build dir
 *	mkdir build && cd build
 *	#to enable debug use -DCMAKE_BUILD_TYPE=Debug
 *	cmake -DCMAKE_BUILD_TYPE=Release ..
 *	make
 * @endcode
 *	
 * This will build the libraries (static and shared) in build/lib/Release.
 * 
 *
 * The given main in Tests.c launches a ton of tests to check if everything is okay. 
 * If you built as told before, you can launch the test program with "make test".
 * 
 * If this is the first time you use this library, please have a look at the documentation or refer to the 
 * raw code given in Tests.c to see how the functions are used and called (a tutorial page is coming!).
 *
 * @section ref_philo Philosophy of the implementation
 * It seems important to me to give more precision on how to use this little library and how it has been implemented.
 *
 * The main idea in this library is that I don't want to lose any information at any time. Thus, all the functions that could modify mass 
 * functions create new ones instead! You have to free every new mass function that has been created. To create copies of mass function,
 * there is a function called BF_copyBeliefFunction() which creates a hard copy of the given mass function. That is why most functions work
 * with structures instead of pointers on structures.
 *
 * @section ref_sec Main References
 * @li A. Appriou - Formulation et traitement de l'incertain en analyse multi-senseurs -
 * Quatorzieme Colloque GRETSI, Juan les Pins, 951-953 - 1993
 * @li A. Appriou - Multisensor Signal Processing in the Framework of the Theory of Evidence -
 * Application of Mathematical Signal Processing Techniques to Mission Systems, pages (5-1) (5 - 31),
 * Research and Technology Organization - 1999
 * @li A. Appriou - Approche generique de la gestion de l'incertain dans les processus de
 * fusion multisenseur - Traitement du Signal 22, 307-319 - 2005
 * @li L.-Z. Chen, W.-K. Shi, Y. Deng, Z.-F. Zhu - A new fusion approach based on distance of evidences -
 * Journal of Zhejiang University Science 61(5), pg 476-482 - 2005
 * @li D. Dubois & H. Prade - Representation and combination of uncertainty with belief functions
 * and possibility measures - Computational intelligence 4, 244-264 - 1988
 * @li R. Haenni and N. Lehmann - Implementing belief function computations - International Journal of
 * Intelligent Systems volume 18, pg 31-49 - 2003
 * @li A. Martin - Modelisation et gestion du conflit dans la theorie des fonctions de croyance -
 * These d'Habilitation a Diriger des Recherches de l'Universite Occidentale - 2009
 * @li C.K. Murphy - Combining belief functions when evidence conflicts - Decision support systems 29
 * 1-9 - 1999
 * @li B. Pietropaoli, M. Dominici, F. Weis - Multi-sensor data fusion within the belief functions
 * framework - In S. Balandin, Y. Koucheryavy, H. Hu (eds.) NEW2AN, Lecture Notes in Computer
 * Science, vol. 6869, pp. 123-134. Springer - 2011
 * @li B. Pietropaoli, M. Dominici, F. Weis - Belief Inference with Timed Evidence : Methodology and
 * Application using Sensors in a Smart Home - Proceedings of Belief 2012, Compiegne, France, 9-11 May - 2012
 * @li V. Ricquebourg, M. Delafosse, L. Delahoche, B. Marhic, A. Jolly-Desodt, D. Menga, - Fault
 * Detection by Combining Redundant Sensors: a Conflict ApproachWithin the TBM Framework.
 * In COGIS 2007, COGnitive systems with Interactive Sensors. Stanford University - 2007
 * @li G. Shafer - A mathematical theory of evidence - Princeton University Press, Princeton, NJ, 1976
 * @li Ph. Smets and R. Kruse - The Transferable Belief Model for Belief Representation -
 * Smets Ph. and Motro A. eds. Uncertainty Management in information systems: from needs
 * to solutions. UMIS Workshop, Puerto Andraix.Volume 2. pg 91-111 - 1993
 * @li Ph. Smets - The transferable belief model for quantified belief representation -
 * In D. M. Gabbay & P. Smets (Eds.), Handbook of defeasible reasoning
 * and uncertainty management - 1998
 * @li Ph. Smets - Theories of Uncertainty - Handbook of fuzzy computing, Section B.1.1.2 - 1995
 * @li P. Vannoorenberghe - Un etat de l'art sur les fonctions de croyance appliquees
 * au traitement de l'information - 2002
 * @li R.R. Yager - On the Dempster-Shafer framework and new combination rules - Information Sciences 41,
 * 93-138 - 1987
 * @li ... and more I'm sure I forgot... (Anyway, you should find it in the doc of concerned functions if required...)
 * In any case, this theory is HUGE and it is hard to cite everything you can find on it! To find more resources, 
 * you may refer to the Belief Functions and Applications Society (http://www.bfasociety.org/).
 *
 * @section contact_sec Contact
 * Bastien Pietropaoli @n
 *
 * @b Email: @n
 * bastien.pietropaoli@inria.fr @n
 * bastien.pietropaoli@gmail.com @n
 *
 * Copyright 2011-2014, EDF. This software was developed with the collaboration of INRIA (Bastien Pietropaoli)
 */

/** 
 * @page version_sec Version notes
 * @section v02_subsec V0.2
 * New operations are now available:
 * @li discounting() : This function corresponds to the classical discounting operation. The lost belief
 * is transferred on the complete universe.
 * @li *Combination() and full*Combination() : These functions implements the different combination rules
 * proposed in the belief functions framework
 * Two functions have been modified: combination() and fullCombination(). They now take an argument
 * concerning the type of combination rule to use.
 *
 * @section v03_subsec V0.3
 * Name of operations have been corrected.
 * The implementation of sets has been optimized a lot (exec time divided by 2 and a lot of memory saved).
 * The implementation is totally mem-check free (checked with Valgrind) !
 *
 * @section v03_1_subsec V0.3.1
 * An emulation of namespace has been added in modules for function names.
 * Minor bugs have been corrected.
 *
 * @section v03_2_subsec V0.3.2
 * The pure BFT has been separated from the application with sensors. Thus, the belief building from sensor
 * measures has been put in a new specific file called BeliefsFromSensors(.h and .c).
 *
 * @section v04_subsec V0.4
 * New functionalities:
 * @li Options: It is now possible to add options to the belief model adding a file called 'options' with the
 * files containing the points for sets of belief functions. The file should contain one option per line composed
 * of a name of option and a float parameter. Only variation and tempo are now available.
 * @li Decision support functions: those functions implemented in BeliefFunctions module enable to find
 * a max or a min of a belief function for mass, belief and plausibility.
 * The implementation is totally mem-check free (checked with Valgrind)!
 *
 * @section v04_1_subsec V0.4.1
 * Correction of the function used to measure time in the tempo option. (Use of clock_gettime(CLOCK_REALTIME,...) now) @n
 * Adding Sets_elementToBitString() in Sets module. @n
 * Adding the DEBUG, CHECK_SUM and CHECK_VALUES defines in config.h to customize the compilation and optimize a little more the execution time. @n
 * The implementation is totally mem-check free (checked with Valgrind)!
 *
 * @section v04_2_subsec V0.4.2
 * Adding Sets_elementFromNumber() and Sets_numberFromElement() in Sets module. @n
 * Correction of a bug in the function Sets_elementToBitString(). @n
 * Addition of a function Sets_setToBitString(). @n
 * Separation of the temporization in a new function BFS_temporization() instead of doing it directly in BFS_getProjection(). @n
 * Code of the function BFS_getProjection() cleaned up. @n
 * In BFS_getProjection(), if sensorMeasure == WEIRD_NUMBER, then the projection equals the vacuous belief function. The temporization
 * is done anyway. It's really usefull if you replace a lost measure by this WEIRD_NUMBER because you'll get the vacuous or a trace
 * of the last belief using the temporization. @n
 * The BeliefFunctions module has been cleaned of all the "elementSize" by putting this variable inside the BF_BeliefFunction structure. @n
 * Diverse things have been renamed to be more explicit and/or more convenient for the extension of the library. @n
 * The implementation is totally mem-check free (checked with Valgrind)! @n
 *
 * @section v05_subsec V0.5
 * Addition of a new applicative module BeliefsFromBeliefs. @n
 * Minor changes in diverse modules (that's the spirit of uncertain modification :)). @n
 * Addition of a new function to clean the BF_BeliefFunction of non-focal elements. @n
 * Addition of a new way of doing temporization using Dubois & Prade's combination rule instead of a discrimination based on specificity. @n
 * In the module BeliefsFromSensors, the options are now managed using flags, thus it should be easier to add and manage new options! @n
 * The implementation is totally mem-check free (checked with Valgrind)! @n
 *
 * @section v051_subsec V0.5.1
 * Addition of a new function to normalize the BF_BeliefFunctions. @n
 * Addition of a new applicative module BeliefsFromRandomness (in progress...) @n
 * Some things have been renamed to be more explicit. @n
 * No more warnings! (compiling well with -Wall -Wextra -Werror -pedantic) @n
 * In BeliefsFromSensors.h, a new define has been added to enable the use of different clocks for the temporization. It is useful when working on diverse OS implementing different clocks. @n
 * Some combination rules have been reimplemented to increase performance and this is much better ! (I got rid of the dirty code of the beginning of the project...) @n
 * The implementation is totally mem-check free (checked with Valgrind)! @n
 *
 * @section v06_subsec V0.6
 * Many things have changed:
 * @li Combination rules have been moved to a new module called BeliefCombinations.c/h
 * @li Decision making support functions have been moved to a new module called BeliefDecisions.c/h
 * @li New and more generic decision support functions are provided.
 * @li Few things have been renamed properly
 * @li unit_tests have been added
 * @li sources have been sorted properly in directories
 * @li You now have to specify a directory to load belief models for sensors
 * @li The debug code has been cleaned (Thx Aurélien!)
 * @li ... there are certainly things I forgot...
 *
 * @section Version_contact Contact
 * Bastien Pietropaoli @n
 *
 * @b Email: @n
 * bastien.pietropaoli@inria.fr @n
 * bastien.pietropaoli@gmail.com @n
 *
 * Copyright 2011-2014, EDF. This software was developed with the collaboration of INRIA (Bastien Pietropaoli)
 */
 
/**
 * @page TODO TODO Page
 * @section TODO_Intro Introduction
 * If you feel in the mood to enjoy some C code, on this page, you can find a list a things to do to improve this library.
 *
 * @section Todo_list Todo list
 * @li Implement new combination rules (PCR5/6, Florea, etc...)
 * @li Implement new applicative modules
 * @li Implement other theories (multi-frame functions, continuous belief function, DSmT, etc...)
 * @li Add some tools to build belief functions (randomly, from certain files, from statistics, etc...)
 * @li Write a tutorial page explaining the basics of the theory and how to start using the library.
 *
 * @section Todo_contact Contact
 * Bastien Pietropaoli @n
 *
 * @b Email: @n
 * bastien.pietropaoli@inria.fr @n
 * bastien.pietropaoli@gmail.com @n
 *
 * Copyright 2011-2014, EDF. This software was developed with the collaboration of INRIA (Bastien Pietropaoli)
 */
 
/**
 * @page origin_page Origin of the name
 * I lost...
 *
 * http://en.wikipedia.org/wiki/The_Game_(mind_game)
 *
 * @section Origin_contact Contact
 * Bastien Pietropaoli @n
 *
 * @b Email: @n
 * bastien.pietropaoli@inria.fr @n
 * bastien.pietropaoli@gmail.com @n
 *
 * Copyright 2011-2014, EDF. This software was developed with the collaboration of INRIA (Bastien Pietropaoli)
 */
 
/**
 * @page FAQ_page FAQ
 * As sometimes, we got a question, the frequently asked question section may contain the answer we are looking for.
 *
 * @section FAQ_character The names of my states are badly cut or contain ^M characters
 * This is due to the end of line character difference between Windows and Linux/MacOS. It counts for two characters on certain systems and for only one on others.
 * To correct this, you can try to compile with another system variable (WINDOWS or LINUX).
 *
 * For Linux users, it is also possible to get a package called "tofrodos" which enables to convert files from windows to linux format and vice versa.
 *
 * @section FAQ_contact Contact
 * Bastien Pietropaoli @n
 *
 * @b Email: @n
 * bastien.pietropaoli@inria.fr @n
 * bastien.pietropaoli@gmail.com @n
 *
 * Copyright 2011-2014, EDF. This software was developed with the collaboration of INRIA (Bastien Pietropaoli)
 */
 
/**
 * @page Contributors_page Contributors
 * Here is a list of contributors:
 * @li Bastien Pietropaoli
 * @li Aurélien Richez
 *
 * @section FAQ_contact Contact
 * Bastien Pietropaoli @n
 *
 * @b Email: @n
 * bastien.pietropaoli@inria.fr @n
 * bastien.pietropaoli@gmail.com @n
 *
 * Copyright 2011-2014, EDF. This software was developed with the collaboration of INRIA (Bastien Pietropaoli)
 */
 
 
/**
 * @page Tuto_page Quick start and tutorial
 * This page is still under construction.
 *
 * @section ref_hard Starting the hard way
 * I know some people like to code the hard way. If you want to build everything by yourself, this is an example of how it is possible to 
 * build a mass function from scratch. It also presents how to combine mass functions. (Note that if you keep the -pedantic option in the makefile,
 * you should write the comments using /(*) comments instead of // comments.)
 * @code
 * BF_BeliefFunction m, m2, m3;
 * char* str = NULL;
 * char bits[2], bits2[2];
 *
 * //A two bits element:
 * bits[0] = 1;
 * bits[1] = 1;
 * 
 * //Another two bits element: 
 * bits2[0] = 1;
 * bits2[1] = 0;
 * 
 * //Building a mass function the hard way: 
 * m.elementSize = 2;
 * m.nbFocals = 2;
 * m.focals = malloc(sizeof(BF_FocalElement) * 2);
 * m.focals[0].element = Sets_createElementFromBits(bits, 2);
 * m.focals[0].beliefValue = 0.3;
 * m.focals[1].element = Sets_createElementFromBits(bits2, 2);
 * m.focals[1].beliefValue = 0.7;
 * 
 * //Printing the mass function: 
 * str = BF_beliefFunctionToBitString(m);
 * printf("m = \n%s\n", str);
 * free(str);
 * 
 * //Combination of mass functions: 
 * m2 = BF_DempsterCombination(m, m);
 * m3 = BF_DempsterCombination(m2, m);
 * 
 * //Printing the result of combination: 
 * str = BF_beliefFunctionToBitString(m2);
 * printf("m2 = \n%s\n", str);
 * free(str);
 * 
 * str = BF_beliefFunctionToBitString(m3);
 * printf("m3 = \n%s\n", str);
 * free(str);
 * 
 * //Freeing mass functions: 
 * BF_freeBeliefFunction(&m);
 * BF_freeBeliefFunction(&m2);
 * BF_freeBeliefFunction(&m3);
 * @endcode
 * 
 * @section Tuto_contact Contact
 * Bastien Pietropaoli @n
 *
 * @b Email: @n
 * bastien.pietropaoli@inria.fr @n
 * bastien.pietropaoli@gmail.com @n
 *
 * Copyright 2011-2014, EDF. This software was developed with the collaboration of INRIA (Bastien Pietropaoli)
 */

#endif



