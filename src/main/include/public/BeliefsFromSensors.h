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


#ifndef DEF_BELIEFSFROMSENSORS
#define DEF_BELIEFSFROMSENSORS


#include <time.h>
#include <ctype.h>
#include <errno.h>

#include "ReadDirectory.h"
#include "ReadFile.h"
#include "BeliefCombinations.h"

/**
 * @section BFS_intro Introduction
 * This module enables the building of belief functions from raw sensor measures.
 * The method used is the one used in Ricquebourg et at. 2007 and Pietropaoli et al. 2011/2012.
 * For more details, please check those references. Anyway, I will try here to introduce the way it works.
 * The mass functions are built from raw sensor measures using predefined set of mass functions (cf figure).
 * 
 * @image html ../images/FunctionSet.jpg "Example of set of mass functions for a Phidget motion sensor connected to an USB Interface Kit and applied to the detection of presence."
 * @image latex ../images/FunctionSet.jpg "Example of set of mass functions for a Phidget motion sensor connected to an USB Interface Kit and applied to the detection of presence." width=8cm
 *
 * In the given example, whenever a raw sensor measure is received, the projection on this set gives 
 * the resulting mass function. For instance, with the given figure, if the motion
 * sensor returns a value of 450, then the resulting mass function would have two focal
 * sets: m({Yes}) = 0.7 and m({Yes u No}) = 0.3.
 *
 * The following options are developped until now:
 * @li Variation : considers the variation of the raw measure instead of the measure itself.
 * It takes as a parameter the number of previous measures to compare to.
 * @li Tempo-Specificity : includes temporization in the building of mass functions using a discrimination based on specificity. Please 
 * read Pietropaoli et al. 2012 for explanations. It takes the number of seconds before complete forgetness as parameter.
 * @li Tempo-Fusion : includes temporization in the building of mass functions using a fusion based on Dubois & Prade's combination rule.
 * It takes the number of seconds before complete forgetness as parameter.
 *
 * @section BFS_howto How to use this module
 * So, to build your sets of mass functions, you should do the following :
 * @li For each thing you want to compute, create a directory with the name you want in data/beliefsFromSensors/ (or the directory defined by BFS_PATH).
 * For example, if you want to compute a thing called "Presence" to determine if there is someone in a room or not, just call the directory "Presence".
 * @li In this directory, create a file called "values" (or with the name defined by BFS_VALUES_NAME) with on each line, the name of
 * one possible world (and no more line than necessary !)
 * Example:
 * @code
 * Yes
 * No
 * @endcode
 * This example creates two possible worlds : Yes and No (useful for a binary case...).
 * @li In this directory, for each sensor, build a directory with the name of that sensor.
 * @li In each sensor subdirectory, create, if necessary, a file called "options" (no extension...) with on each
 * line, the name of one of the options you want and the corresponding parameter. Example:
 * @code
 * 2 options                 //Number of options
 * Tempo-Specificity 3       //Type of tempo + nb of seconds
 * Variation 2               //Variation + nb of measures to consider
 * @endcode
 * This options file applies a temporization of 3 seconds and compares the given measure with the 2 previous one...
 * The case does not matter to write the name of the options.
 * @li In each sensor subdirectory, for each subset of possible worlds, create a file with on first line, the number
 * of considered world in the subset, then come the names of those worlds, one per line. Then, put the number of specific
 * points used to build the set of mass functions. Then, indicate the couples "sensorMeasure beliefValue" for the set of
 * mass functions, one per line. Example:
 * @code
 * 2 elements   //Number of worlds in the subset
 * Aka          //World 1
 * Elf          //World 2
 * 5 points     //Number of considered measure
 * 100 0        //For a measure of 100, get a null mass for this subset of worlds
 * 200 0.40     //etc...
 * 300 0.15
 * 400 0.40
 * 500 0
 * @endcode
 * This file corresponds to the set of mass function only for the subset {Aka u Elf}. Those files can have any name, that
 * is not important at all... However, the given sensor measures should be in an increasing order or it could produce weird
 * behaviors! Also, for any measure, the sum of masses over files should always be 1. Uncomment the constant CHECK_MODELS 
 * in config.h to make sure your models are okay.
 *
 * It is not actually necessary to write a file for each subset of possible worlds. A file per possible focal element is enough.
 *
 * WARNING: Check that your files are in the correct format for you OS (specifically for the endline character...), thus,
 * do not forget to format them correctly (or you could see ^M characters appearing in your possible worlds for example...).
 * To convert in a correct format, on Linux, it is possible to use the package 'tofrodos' with the two commands 'fromdos' and 'todos'.
 *
 * If you're not sure you understood this quick explanation, please check the example files (for instance those used
 * to run tests...).
 * 
 * Once everything has been created correctly, it is possible to load a belief structure using 
 * BFS_loadBeliefStructure(nameOfTheThingYouWantToCompute), the files will all be loaded automatically.
 * The functions BFS_getEvidences() et BFS_getProjection() are then used to build the mass functions from
 * given raw measures.
 *
 * @file BeliefsFromSensors.h
 * @author Bastien Pietropaoli (bastien.pietropaoli@inria.fr)
 * @brief APPLICATION: Gives structures and main
 *        functions to create belief functions
 *        from sensor measures.
 */

/**
 * @def BFS_VALUES_NAME
 * The name of the file containing the possible values of
 * a frame of discernment
 */
#define BFS_VALUES_NAME "values"

/**
 * @def NO_MEASURE
 * A weird number to get a vacuous mass function when doing a projection if used as sensor measure.
 * It's usefull to handle easily the loss of measure !
 */
#define NO_MEASURE -1048576

/**
 * @def CLOCK_ID
 * Defines the clock ID to use for the function clock_gettime(). By default, it is set to CLOCK_MONOTONIC.
 * If a dysfunction seems to occur when using the temporization, try to change it by CLOCK_REALTIME.
 * Their depends on your OS and its version.
 */
#define CLOCK_ID CLOCK_MONOTONIC

/*
  +--------------+
  | ENUMERATIONS |
  +--------------+
*/

/**
 * @enum BFS_OptionFlags
 * The flags corresponding to the different options that is possible
 * to apply to the building of mass functions. For more information
 * on the options, refer to the introduction of the module.
 */
enum BFS_OptionFlags{
	OP_NONE               = 0,
	OP_VARIATION          = 1 << 0,
	OP_TEMPO_SPECIFICITY  = 1 << 1,
	OP_TEMPO_FUSION       = 1 << 2
};
typedef enum BFS_OptionFlags BFS_OptionFlags;



/*
  +------------+
  | STRUCTURES |
  +------------+
*/


/* !!! Structures to define the model of belief !!! */


/**
 * Defines a point for belief functions
 * definition. A linear approximation
 * will be used to get the values
 * corresponding to the sensor measures.
 * @param sensorValue A value of the sensor
 * @param belief the mass corresponding to the value of the sensor
 * @struct BFS_Point
 */
struct BFS_Point{
    float sensorValue;
    float belief;
};
typedef struct BFS_Point BFS_Point;


/**
 * A set of belief functions for only one focal element.
 * @param focalElement The element associated to the function
 * @param points The points to define the function
 * @param nbPts The number of points to define the function
 * @struct BFS_PartOfBelief
 */
struct BFS_PartOfBelief{
    Sets_Element focalElement;
    BFS_Point *points;
    int nbPts;
};
typedef struct BFS_PartOfBelief BFS_PartOfBelief;


/**
 * An union used to store data in options.
 * @param time A time obtained via clock_gettime(CLOCK_REALTIME, &...);
 * @param measure A double to store a sensor measure
 * @param bf A pointer to a BF_BeliefFunction
 * @union BFS_UtilData
 */
union BFS_UtilData{
    double measure;
    BF_BeliefFunction bf;
    struct timespec time;
};
typedef union BFS_UtilData BFS_UtilData;


/**
 * A structure to store options, for instance, tempo(risation) or
 * variation to specify different behaviour or way to create and
 * manage the belief functions created from sensor measures.
 * For now, only "tempo" and "variation" are taken into account.
 * tempo : add a temporisation to the belief function. Thus, discounting
 * is applied depending on the time given in parameter (in s).
 * util storage: time of the previous measure + a pointer to the previous BF_BeliefFunction
 * variation : enable the belief function to be computed from the variation
 * of a sensor measure not directly from the measure at a given time. Give
 * a number of measures to compare to.
 * util storage: the "parameter" previous measures
 * @param type The type of option, there can be only one!
 * @param parameter A parameter to apply to the option
 * @param util A list of BFS_UtilData to store specific data for the option
 * @struct BFS_Option
 */
struct BFS_Option{
    BFS_OptionFlags type;
    float parameter;
    BFS_UtilData* util;
};
typedef struct BFS_Option BFS_Option;


/**
 * A set of functions (each one associated to
 * a specific value) associated to a sensor type.
 * @param sensorType The type of sensor
 * @param beliefOnElements The set of functions representing
 * the belief of the sensor on different focal elements
 * @param nbFocal The number of focal elements for the sensor
 * @param options A list of options to apply when building the
 * belief from sensor measures
 * @param nbOptions The number of options applied
 * @param optionFlags The flags to save the different option types
 * @struct BFS_SensorBeliefs
 */
struct BFS_SensorBeliefs{
    char* sensorType;
    BFS_PartOfBelief *beliefOnElements;
    int nbFocal;
    BFS_Option *options;
    int nbOptions;
    BFS_OptionFlags optionFlags;
};
typedef struct BFS_SensorBeliefs BFS_SensorBeliefs;


/**
 * The complete belief structure with all the beliefs
 * of all the sensors on a specific frame of discernment.
 * @param frameName The name of the frame of discernment (or the thing we want to believe on)
 * @param refList The Sets_ReferenceList corresponding to the real values of the frame of discernment
 * @param possibleValues The set of all possible values for the frame of discernment
 * @param powerset The set of all subsets of possible values
 * @param beliefs The model of belief to get the frame of discernment value
 * @param nbSensors The number of sensors in the structure
 * @struct BFS_BeliefStructure
 */
struct BFS_BeliefStructure{
    char* frameName;
    Sets_ReferenceList refList;
    Sets_Set possibleValues;
    Sets_Set powerset;
    BFS_SensorBeliefs* beliefs;
    int nbSensors;
};
typedef struct BFS_BeliefStructure BFS_BeliefStructure;



/*
  +-----------+
  | FUNCTIONS |
  +-----------+
*/


/* !!! Creation of beliefs !!! */


/**
 * @name Manually creating a model
 * @{
 */

/**
 * Creates an empty belief structure.
 * @param name the name of the structure
 * @param possibleValues the possible worlds for our frame of discernment.
 * @param size size of the array possibleValues
 * @return the new BFS_BeliefStrucure, must be freed after use.
 */
BFS_BeliefStructure BFS_createBeliefStructure(const char* name, const char* const * possibleValues,
		int size);

/**
 * Puts the sensor belief model inside the belief structure. The sensor belief
 * is not copied so you should not free it afterward (it will be freed with the
 * belief structure). Besides, you should not add the same sensor belief model
 * to several belief structures for the same reason.
 * @param beliefStructure belief structure which will be modified.
 * @param sensorBelief sensor belief which will be put in the structure.
 */
void BFS_putSensorBelief(BFS_BeliefStructure *beliefStructure,
		const BFS_SensorBeliefs sensorBelief);

/**
 * Create a new empty sensor belief with no focal element. This sensor belief
 * should be filled with BFS_addPointTosensorBelief() and BFS_addOption(). The
 * new BFS_SensorBeliefs is generally inserted in a BFS_BeliefStructure with
 * BFS_putSensorBelief(). In any other case, it should be freed with
 * BFS_freeSensorBeliefs().
 * @param sensorType name of the sensor.
 * @return the new BFS_SensorBeliefs
 */
BFS_SensorBeliefs BFS_createSensorBeliefs(const char* sensorType);

/**
 * Copy an existing sensor belief with a new name. The created BFS_SensorBeliefs
 * will be exactly the same with the given name. The memory will be reallocated
 * for the new belief. The new name should be different than the original name.
 * as for BFS_createSensorBeliefs(), the new BFS_SensorBeliefs is generally
 * inserted in a BFS_BeliefStructure with BFS_putSensorBelief(). In any other
 * case, it should be freed with BFS_freeSensorBeliefs().
 * @param toCopy BFS_SensorBeliefs which will be copied.
 * @param elementSize number of possible worlds for the frame of discernment.
 * @param newSensorName new name for the sensorType.
 * @return The new BFS_SensorBeliefs.
 */
BFS_SensorBeliefs BFS_copySensorBelief(const BFS_SensorBeliefs toCopy, int elementSize, const char* newSensorName);

/**
 * Add an option to a sensorBeliefs, such as tempo specificity, tempo fusion or
 * variation.
 * @param sensorBeliefs pointer to the sensor belief we want to modify.
 * @param flag option flag to add
 * @param param parameter for the option
 */
void BFS_addOption(BFS_SensorBeliefs *sensorBeliefs, BFS_OptionFlags flag, float param);

/**
 * Adds a point to the belief function set contained in the structure.
 * @param sensorBeliefs pointer to the sensor belief we want to modify.
 * @param elem element on which the belief value will apply
 * @param elemSize size of elem
 * @param sensorValue value for which the belief value will apply
 * @param mass resulting mass for the sensor value.
 */
void BFS_addPointTosensorBelief(BFS_SensorBeliefs *sensorBeliefs, Sets_Element elem, int elemSize,
		float sensorValue, float mass);

/**
 * @}
 */

/**
 * @name Loading a model
 * @{
 */

/**
 * Loads a belief structure from a directory. The directory must have
 * the frame of discernment name and should be in the given directory.
 * It may contain :
 * - values A file with the different possible values for the
 *        frame of discernment (the name of this file is @link BFS_VALUES_NAME @endlink (if unchanged "values"). 
 *        See the function @link loadSet() @endlink for more information about the format of the file.
 * - sensorType A bunch of subdirectories, each one with the name
 *        of a specific type of sensor
 * - beliefValues Each sensor type subdirectory may contain a collection
 *        of files, each one corresponding to the beliefs associated to a specific
 *        value. The filenames aren't important at all. See the documentation of this module itself for
 *        more information about the format of the file.
 * @param directory directory where the belief structure folder is.
 * @param frameName The name of the frame of discernment (Name of the directory to look for.)
 * @return The complete BFS_BeliefStructure containing all the beliefs that may be used
 *         to estimate the real world state. The BFS_BeliefStructure is empty if an error occurs.
 *         (The frameName attribute should be NULL in case of error.) Must be freed after use.
 */
BFS_BeliefStructure BFS_loadBeliefStructure(const char * directory, const char* frameName);

/**
 * Load sthe sensor beliefs from a directory. A bunch of files corresponding to the BFS_PartOfBelief
 * may be in the path (see @link loadPartOfBelief() @endlink ).
 * @param sensorType The type of sensor (name of the directory) whose beliefs are loaded
 * @param path The path of the directory containing the different part of belief
 * @param rl The Sets_ReferenceList containing the real values of the frame of discernment
 * @return The BFS_SensorBeliefs with the different BFS_PartOfBeliefs. Must be freed after use.
 */
BFS_SensorBeliefs BFS_loadSensorBeliefs(const char* sensorType, const char* path, const Sets_ReferenceList rl);

/**
 * Loads a part of belief corresponding to the function use to create the belief functions
 * associated to a set of possible values of the frame of discernment and a sensor type.
 * Format :
 * @li @c nbOfAtoms The number of atoms associated to the function
 * @li @c atomsNames The list of corresponding atoms, one per line
 * @li @c nbOfPoints The number of points to define the function
 * @li @c points The couples (sensorValue, belief) used to define the function, one per line.
 *        The points may be placed in order from the smallest sensor value to the biggest
 * @param fileName The name of the file to load
 * @param rl The Sets_ReferenceList containing the real values of the frame of discernment
 * @return A BFS_PartOfBelief to be freed after use.
 */
BFS_PartOfBelief BFS_loadPartOfBelief(const char* fileName, const Sets_ReferenceList rl);


/** @} */

/**
 * @name Creation of belief functions
 * @{
 */
 
/**
 * Generates the BF_BeliefFunctions from the BFS_BeliefStructure and
 * a set of sensor measures. If a sensor does not correspond to anything
 * given in the model (BFS_BeliefStructure), then a vacuous mass function is
 * returned for this sensor.
 * @param bs The belief structure to use
 * @param sensorTypes The types of sensors giving data (table of Strings)
 * @param sensorMeasures The set of measures given by sensors
 * @param nbMeasures The number of measures (and of elements in sensorTypes)
 * @return The list of BeliefFunctions associated to the sensors
 */
BF_BeliefFunction* BFS_getEvidence(const BFS_BeliefStructure bs,
		const char* const * const sensorTypes, const double* sensorMeasures, const int nbMeasures);

/**
 * Generates the BF_BeliefFunctions from the BFS_BeliefStructure and
 * a set of sensor measures. If a sensor does not correspond to anything
 * given in the model (BFS_BeliefStructure), then a vacuous mass function is
 * returned for this sensor. This function takes the elapsed time as a parameter
 * and does use use real time (and time stored in the belief structure) for the
 * temporizations.
 * @param bs The belief structure to use
 * @param sensorTypes The types of sensors giving data (table of Strings)
 * @param sensorMeasures The set of measures given by sensors
 * @param nbMeasures The number of measures (and of elements in sensorTypes)
 * @param elapsedTime The time since the last set of sensor measure
 * @return The list of BeliefFunctions associated to the sensors
 */
BF_BeliefFunction* BFS_getEvidenceElapsedTime(const BFS_BeliefStructure bs,
		const char* const * const sensorTypes, const double* sensorMeasures, const int nbMeasures,
		const float elapsedTime);

/**
 * Get the instant BF_BeliefFunction from a model of belief associated to a sensor.
 * Options are applied here. The timing used for the temporization options will
 * the real time.
 * @param sensorBelief The model of belief associated to the sensor
 * @param sensorMeasure The measure given by the sensor (if sensorMeasure == NO_MEASURE, projection = vacuous + tempo discrimination)
 * @param elementSize The number of digits in the representation of elements
 * @return The projection (the instant BF_BeliefFunction) associated to the sensor and its measure.
 */
BF_BeliefFunction BFS_getProjection(const BFS_SensorBeliefs sensorBelief, const double sensorMeasure,
		const int elementSize);

/**
 * Gets the BF_BeliefFunction from a model of belief associated to a sensor.
 * Options are applied. The difference between this function and
 * BFS_getProjection() is the elapsedTime parameter. This parameter allows to
 * manually specify in seconds how much time was elapsed between this
 * sensor measure and the previous sensor measure. The date contained by the
 * sensor belief will be ignored and the given elapsed time will be used to
 * compute temporization options.
 * @param sensorBelief The model of belief associated to the sensor
 * @param sensorMeasure The measure given by the sensor (if sensorMeasure == NO_MEASURE, projection = vacuous + tempo discrimination)
 * @param elementSize The number of digits in the representation of elements
 * @param elapsedTime time since the last sensor measure in seconds
 * @return The projection (the instant BF_BeliefFunction) associated to the sensor and its measure.
 */
BF_BeliefFunction BFS_getProjectionElapsedTime(const BFS_SensorBeliefs sensorBelief,
		const double sensorMeasure, const int elementSize, const float elapsedTime);

/**
 * Get the belief value associated to a specific possible value of the
 * frame of discernment.
 * @param pob The part of belief associated to a sensor and a specific possible value
 * @param sensorMeasure The measure given by the sensor
 * @param elementSize The number of digits in the representation of elements
 * @return A BF_BeliefPoint corresponding to the belief in the measure given. A linear
 *         approximation is done to get the belief value.
 */
BF_FocalElement BFS_getBeliefValue(const BFS_PartOfBelief pob, const double sensorMeasure, const int elementSize);


/** @} */

/**
 * @name Temporizations
 * @{
 */

/**
 * Applies the discrimination of temporization based on the specificity of the
 * discounted old belief function (cf Pietropaoli et al. - Belief Inference with
 * Timed Evidence - 2012). This function uses real time for discounting.
 * @param oldOne The old BF_BeliefFunction
 * @param newOne The last BF_BeliefFunction corresponding to the last received measure (or vacuous if not receive)
 * @param timeFactor The factor used to discount over time (linear discount over time for now)
 * @param oldTime The time at which the old BF_BeliefFunction has been built
 * @param op A pointer to the temporization option to store the needed data
 * @return The BF_BeliefFunction after the temporization has been applied (must be freed after use).
 */
BF_BeliefFunction BFS_temporization_specificity(const BF_BeliefFunction oldOne,
		const BF_BeliefFunction newOne, const float timeFactor, const struct timespec oldTime,
		BFS_Option* op);

/**
 * Applies the discrimination of temporization based on the specificity of the
 * discounted old belief function (cf Pietropaoli et al. - Belief Inference with
 * Timed Evidence - 2012). The difference between this function and
 * BFS_temporization_specificity() is that the elapsed time is given as a
 * parameter, instead of using real time.
 *
 * @param oldOne The old BF_BeliefFunction
 * @param newOne The last BF_BeliefFunction corresponding to the last received measure (or vacuous if not receive)
 * @param timeFactor The factor used to discount over time (linear discount over time for now)
 * @param op A pointer to the temporization option to store the needed data
 * @param elapsedTime time since the last measure in seconds
 * @return The BF_BeliefFunction after the temporization has been applied (must be freed after use).
 */
BF_BeliefFunction BFS_temporization_specificityElapsedTime(const BF_BeliefFunction oldOne,
		const BF_BeliefFunction newOne, const float timeFactor, BFS_Option* op,
		const float elapsedTime);

/**
 * Applies a temporization of belief based on a discounting of the old belief
 * plus a combination with the new belief using the Dubois & Prade's combination
 * rule (the combination occurs only if the given belief function correspond to
 * a new measure, otherwise, only discounting is applied).
 * @param oldOne The old BF_BeliefFunction
 * @param newOne The last BF_BeliefFunction corresponding to the last received measure (if newOne.focals == NULL, then it only applies discounting)
 * @param timeFactor The factor used to discount over time (linear discount over time for now)
 * @param oldTime The time at which the old BF_BeliefFunction has been built
 * @param op A pointer to the temporization option to store the needed data
 * @return The BF_BeliefFunction after the temporization has been applied (must be freed after use).
 */
BF_BeliefFunction BFS_temporization_fusion(const BF_BeliefFunction oldOne,
		const BF_BeliefFunction newOne, const float timeFactor, const struct timespec oldTime,
		BFS_Option* op);

/**
 * Applies a temporization of belief based on a discounting of the old belief
 * plus a combination with the new belief using the Dubois & Prade's combination
 * rule (the combination occurs only * if the given belief function correspond
 * to a new measure, otherwise, only discounting is applied). While
 * BFS_temporization_fusion() uses real time, this function takes as a parameter
 * the elapsed time instead of the old date. This allow to use an external
 * function to measure time or use the library in a simulation.
 * @param oldOne The old BF_BeliefFunction
 * @param newOne The last BF_BeliefFunction corresponding to the last received measure (if newOne.focals == NULL, then it only applies discounting)
 * @param timeFactor The factor used to discount over time (linear discount over time for now)
 * @param oldTime The time at which the old BF_BeliefFunction has been built
 * @param op A pointer to the temporization option to store the needed data
 * @param elapsedTime time since the last measure in seconds
 * @return The BF_BeliefFunction after the temporization has been applied (must be freed after use).
 */
BF_BeliefFunction BFS_temporization_fusionElapsedTime(const BF_BeliefFunction oldOne,
		const BF_BeliefFunction newOne, const float timeFactor, BFS_Option* op,
		const float elapsedTime);
/** @} */

/* !!! Deallocate memory given to beliefs !!! */

/**
 * @name Memory deallocation
 * @{
 */

/**
 * Frees the memory used for the BFS_BeliefStructure.
 * @param bs A pointer to the BFS_BeliefStructure to free
 */
void BFS_freeBeliefStructure(BFS_BeliefStructure* bs);

/**
 * Frees the memory used for the BFS_Option.
 * @param o A pointer to the BFS_Option to free
 */
void BFS_freeOption(BFS_Option* o);

/**
 * Frees the memory used for the BFS_SensorBeliefs.
 * @param sb A pointer to the BFS_SensorBeliefs to free
 */
void BFS_freeSensorBeliefs(BFS_SensorBeliefs* sb);

/**
 * Frees the memory used for the BFS_PartOfBelief.
 * @param pob A pointer to the BFS_PartOfBelief to free
 */
void BFS_freePartOfBelief(BFS_PartOfBelief* pob);

/** @} */

/* !!! Conversion into strings !!! */

/**
 * @name Conversion into string
 * @{
 */

/**
 * Converts a BFS_PartOfBelief into a string ready to print.
 * @param pob The BFS_PartOfBelief to convert
 * @param rl The Sets_ReferenceList containing the real values of the frame of discernment
 * @return A string ready to print representing the BFS_PartOfBelief.
 *         Must be freed after use.
 */
char* BFS_partOfBeliefToString(const BFS_PartOfBelief pob, const Sets_ReferenceList rl);

/**
 * Converts an BFS_Option into a string ready to print.
 * @param o The BFS_Option to convert
 * @return A string ready to print representing the BFS_PartOfBelief.
 *         Must be freed after use.
 */
char* BFS_optionToString(const BFS_Option o);

/**
 * Converts a BFS_SensorBeliefs into a string ready to print.
 * @param sb The BFS_SensorBeliefs to convert
 * @param rl The Sets_ReferenceList containing the real values of the frame of discernment
 * @return A string ready to print representing the BFS_SensorBeliefs.
 *         Must be freed after use.
 */
char* BFS_sensorBeliefsToString(const BFS_SensorBeliefs sb, const Sets_ReferenceList rl);

/**
 * Converts a BFS_BeliefStructure into a string ready to print.
 * @param bs The BFS_BeliefStructure to convert
 * @return A string ready to print representing the BFS_BeliefStructure.
 *         Must be freed after use.
 */
char* BFS_beliefStructureToString(const BFS_BeliefStructure bs);

/** @} */

#endif
