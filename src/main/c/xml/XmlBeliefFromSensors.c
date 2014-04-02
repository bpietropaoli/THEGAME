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

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <string.h>

#include "XmlBeliefFromSensors.h"
#include "BeliefFunctions.h"


/*
 * @param context xpath context for the document
 * @return The name of the frame of discernment.
 */
static char* extractFrameName(xmlXPathContext *context) {
	xmlXPathObject *xPathRequest = xmlXPathEvalExpression((xmlChar *)"/belief-from-sensors/frame",
				context);
	xmlNodePtr node = xPathRequest->nodesetval->nodeTab[0];
	char* frameName = (char*) xmlGetProp(node, (xmlChar*)"name");
	xmlXPathFreeObject(xPathRequest);
	return frameName;
}

/*
 *
 * @param context xpath context for the document
 * @return The number of worlds in the configuration.
 */
static int getWorldNb(xmlXPathContext *context) {
	xmlXPathObject *xPathRequest = xmlXPathEvalExpression(
			(xmlChar *)"/belief-from-sensors/frame/state", context);
	int worldNb;
	worldNb = xPathRequest->nodesetval->nodeNr;
	xmlXPathFreeObject(xPathRequest);
	return worldNb;
}

/*
 * Returns an array containing all the possible worlds in the configuration
 * in the same order as in the document. The string contained by the array
 * and the array itself must be freed by the user.
 * @param doc configuration doc
 * @param context xpath context used in the document
 * @return An array containing every world.
 */
static char** getWorldArray(xmlDoc *doc, xmlXPathContext *context) {
	xmlXPathObject *xPathRequest = xmlXPathEvalExpression((xmlChar *)"/belief-from-sensors/frame/state",
			context);
	int i = 0;
	int size = xPathRequest->nodesetval->nodeNr;
	char** valuesArray = malloc(sizeof(char*) * size);
	for (i = 0; i < size; ++i) {
		valuesArray[i] = (char*) xmlNodeListGetString(doc,
				xPathRequest->nodesetval->nodeTab[i]->children, 1);
	}

	xmlXPathFreeObject(xPathRequest);
	return valuesArray;
}

/*
 * Transforms a string to an optionFlag.
 * @param optionName sting representation of the option
 * @return option flag corresponding to the string.
 */
static BFS_OptionFlags toOptionFlag(xmlChar *optionName, xmlDoc* doc) {
	if(0 == xmlStrcmp((xmlChar*)"tempo-fusion",optionName)) {
		return OP_TEMPO_FUSION;
	}
	else if(0 == xmlStrcmp((xmlChar*)"tempo-specificity",optionName)) {
		return OP_TEMPO_SPECIFICITY;
	}
	else if(0 == xmlStrcmp((xmlChar*)"variation",optionName)) {
		return OP_VARIATION;
	} else {
		fprintf(stderr,
				"[THEGAME-xml]warning - while parsing %s : unknown option '%s'",
				doc->name, optionName);
	}
	return OP_NONE;
}

/*
 * Get the value of an option. If the value is not valid, this function returns
 * -1 and displays an error message.
 * @param doc parsed document to display in case of error
 * @param optionNode node pointing to the option tag
 * @return the float parameter of the option
 */
static float parseOptionValue(xmlDoc* doc, xmlNode* optionNode) {
	xmlChar* optionValueStr = xmlNodeListGetString(doc, optionNode->children, 1);
	float optionValue = 0.0f;
	sscanf((char*)optionValueStr, "%f", &optionValue);
	xmlFree(optionValueStr);

	if(optionValue <= 0.0f){
		fprintf(stderr,
				"[THEGAME-xml] warning - while parsing %s : wrong option value line %d",
				doc->name, optionNode->line);
		optionValue = -1.0f;
	}
	return optionValue;
}

/*
 * Parses a single option and adds it to the sensor belief.
 * @param sensorBelief sensor belief to add the option to.
 * @param optionNode node pointing to the option tag
 * @param doc document containing our configuration.
 */
static void parseOption(BFS_SensorBeliefs* sensorBelief, xmlNode* optionNode,
		xmlDoc* doc) {
	xmlChar *optionName = xmlGetProp(optionNode, (const xmlChar*) "name");
	BFS_OptionFlags optionFlag = toOptionFlag(optionName, doc);
	float optionValue = parseOptionValue(doc, optionNode);

	if (OP_NONE != optionFlag && -1.0f != optionValue) { /* no error in the document :*/
		BFS_addOption(sensorBelief, optionFlag, optionValue);
	}
	xmlFree(optionName);
}

/*
 * Parses all option for a sensor and adds them to the sensor beliefs.
 * @param sensorBelief pointer to the sensor belief we are currently working on
 * @param doc document containing the configuration.
 * @param sensorNode node pointing to the sensor tag.
 */
static void parseBeliefOptions(BFS_SensorBeliefs *sensorBelief, xmlDoc *doc, xmlNode* beliefNode) {

	xmlXPathContext *context = xmlXPathNewContext(doc);
	xmlXPathObject *xPathRequest;
	context->node = beliefNode;
	xPathRequest = xmlXPathEvalExpression((xmlChar *)"options", context);

	if(xPathRequest->nodesetval->nodeNr == 1) { /* we skip if there is no options */
		xmlNode *optionsNode, *currentNode;
		optionsNode = xPathRequest->nodesetval->nodeTab[0];

		for (currentNode = optionsNode->children; currentNode != NULL; currentNode = currentNode->next) {
			if(0 == xmlStrcmp(currentNode->name,(xmlChar*)"option")) {
				parseOption(sensorBelief, currentNode, doc);
			}
		}
	}

	xmlXPathFreeObject(xPathRequest);
	xmlXPathFreeContext(context);
}

/*
 * This functions taked a <mass> tag and parse the set attributes. It returns
 * an array of strings containing the world in the attribute. These world are
 * separated by spaces. It also takes a size parameter which is a pointer to
 * an int. It will be set to the size of the returned array.
 * @param massNode node from which the set is extracted
 * @param size pointer to the variable containing the size of the array
 * @return the array of strings
 */
static char** getWorldArrayForPoint(xmlNode *massNode, int *size) {
	xmlChar* setStr = xmlGetProp(massNode, (xmlChar*)"set");
	int setStrSize = strlen((char*)setStr);
	char** worlds;
	char* world;
	int i, worldNb = 1;

	for(i = 0; i < setStrSize; i++) {
		if(setStr[i] == ' ') worldNb++;
	}
	worlds = malloc(sizeof(char*) * worldNb);

	world = strtok((char*)setStr," ");
	i = 0;
	while (world != NULL) {
		worlds[i] = strdup(world);
		world = strtok (NULL, " ");
		i++;
	}

	xmlFree(setStr);
	*size = worldNb;
	return worlds;
}

/*
 * Gets the value for a point tag.
 * @param pointNode node pointing to the <point> tag
 * @param doc document containing the configuration
 * @return the sensor value for the point.
 */
static float getPointValue(xmlNode* pointNode, xmlDoc* doc) {
	xmlNode *currentNode;
	float value;
	for (currentNode = pointNode->children; NULL != currentNode;
			currentNode = currentNode->next) {
		if (xmlStrEqual((xmlChar*) "value", currentNode->name)) {
			xmlChar* valueStr = xmlNodeListGetString(doc, currentNode->children, 1);
			if (1 != sscanf((char*) valueStr, "%f", &value)) {
				fprintf(stderr,
						"[THEGAME-xml] error - error while parsing value line %d",
						currentNode->line);
			}

			xmlFree(valueStr);
			break;
		}
	}
	return value;
}

/*
 * Gets the mass value contained by a <mass> tag
 * @param doc the document containing the configuration
 * @param massNode node pointing to the mass tag
 * @return
 */
float getElemMass(xmlDoc *doc, xmlNode *massNode) {
	float mass = 0;
	xmlChar* massStr = xmlNodeListGetString(doc, massNode->children, 1);
	if(1 != sscanf((char*)massStr, "%f", &mass)) {
		fprintf(stderr, "[THEGAME-xml] error - error while parsing mass line %d",
				massNode->line);
	}
	xmlFree(massStr);
	return mass;
}

/*
 * Parses a <point> tag. A point tag contains a value and the masses attributed
 * to different elements from the powerset.
 * @param refList reference list for our current configuration
 * @param sensorBelief The sensor belief to add the point to
 * @param doc document containing the configuration
 * @param currentPointNode node pointing to the point tag to parse
 */
static void parsePoint(Sets_ReferenceList refList, BFS_SensorBeliefs *sensorBelief, xmlDoc *doc,
		xmlNode *currentPointNode) {
	xmlNode *currentNode;
	float value = 0;
	value = getPointValue(currentPointNode, doc);
	/* get the mass*/
	for (currentNode = currentPointNode->children; NULL != currentNode;
			currentNode = currentNode->next) {
		if(xmlStrEqual((xmlChar*)"mass", currentNode->name)) {
			int size;
			int i;
			char** values = getWorldArrayForPoint(currentNode, &size);
			Sets_Element elem = Sets_createElementFromStrings((const char* const * const)values,
					size, refList);
			float mass = getElemMass(doc,currentNode);
			BFS_addPointTosensorBelief(sensorBelief, elem, refList.card, value, mass);

			for (i = 0; i < size; ++i) {
				free(values[i]);
			}
			free(values);
			Sets_freeElement(&elem);
		}
	}

}

/*
 * Parses all belief points in a sensor-belief tag and add them to the
 * BFS_SensorBeliefs.
 * @param refList reference list for the current configuration.
 * @param sensorBelief BFS_SensorBeliefs into which the points will be added.
 * @param doc xml document containing the configuration.
 * @param beliefNode <sensor-belief> tag
 */
static void parseBeliefPoints(Sets_ReferenceList refList, BFS_SensorBeliefs *sensorBelief,
		xmlDoc *doc, xmlNode *beliefNode) {
	xmlXPathContext *context = xmlXPathNewContext(doc);
	xmlXPathObject *xPathRequest;
	int i;
	context->node = beliefNode;
	xPathRequest = xmlXPathEvalExpression((xmlChar *)"point", context);

	for (i = 0; i < xPathRequest->nodesetval->nodeNr; ++i) {
		xmlNode *currentPointNode = xPathRequest->nodesetval->nodeTab[i];
		parsePoint(refList, sensorBelief, doc, currentPointNode);
	}

	xmlXPathFreeObject(xPathRequest);
	xmlXPathFreeContext(context);
}

/*
 * Parses all belief set present in the document. These beliefs are named with the
 * name attribute from the tag <sensor-belief>. The goal is then to copy the sensors
 * beliefs with the real names of the sensors using them. The BFS_SensorBeliefs are
 * stored in an array and sensorBeliefs is modified to point to that array (which
 * must be freed after use).
 * @param refList the reference list for the current
 * @param sensorBeliefs this pointer will be modified to be the array of BFS_SensorBeliefs
 * @param doc document containing the configuration
 * @param context xpath context used in the document.
 * @return the size of the array sensorBeliefs
 */
static int loadSensorBeliefs(Sets_ReferenceList refList, BFS_SensorBeliefs **sensorBeliefs,
		xmlDoc *doc, xmlXPathContext *context) {
	xmlXPathObject *xPathRequest = xmlXPathEvalExpression(
			(xmlChar *)"/belief-from-sensors/sensor-beliefs/sensor-belief", context);
	int i;
	BFS_SensorBeliefs  sensorBelief;
	int beliefNb = xPathRequest->nodesetval->nodeNr;
	*sensorBeliefs = malloc(sizeof(BFS_SensorBeliefs) * (beliefNb));
	DEBUG_CHECK_MALLOC(sensorBeliefs);

	for (i = 0; i < beliefNb; ++i) {
		xmlNode *currentBeliefNode = xPathRequest->nodesetval->nodeTab[i];
		xmlChar *beliefSetName = xmlGetProp(currentBeliefNode, (const xmlChar*) "name");

		sensorBelief = BFS_createSensorBeliefs((char*)beliefSetName);
		parseBeliefOptions(&sensorBelief, doc, currentBeliefNode);
		parseBeliefPoints(refList, &sensorBelief, doc, currentBeliefNode);

		(*sensorBeliefs)[i] = sensorBelief;

		xmlFree(beliefSetName);
	}

	xmlXPathFreeObject(xPathRequest);

	return beliefNb;
}

/*
 * Creates a copy of the existing sensor beliefs for the given sensor.
 * This function takes as an argument sensorBeliefs which contains  the
 * BFS_SensorBeliefs present in the configuration file and find which one
 * is associated with the sensor.
 * @param sensorBeliefs array containing the available sensor beliefs in the configuration
 * @param beliefNb size sensorBeliefs
 * @param elemSize number of existing world in our configuration
 * @param sensorNode <sensor> tag.
 * @return
 */
static BFS_SensorBeliefs getSensorBelief(BFS_SensorBeliefs *sensorBeliefs, int beliefNb,
		int elemSize, xmlNode *sensorNode) {
	xmlChar* sensorName = xmlGetProp(sensorNode, (xmlChar*)"name");
	xmlChar* setName = xmlGetProp(sensorNode, (xmlChar*)"belief");
	BFS_SensorBeliefs sensorBelief;
	int i;

	for (i = 0; i < beliefNb; ++i) {
		if(xmlStrEqual(setName, (xmlChar*)sensorBeliefs[i].sensorType)) {
			sensorBelief = BFS_copySensorBelief(sensorBeliefs[i], elemSize, (char*)sensorName);
		}
	}

	xmlFree(sensorName);
	xmlFree(setName);
	return sensorBelief;
}

/*
 * Complete the given structure beliefStructure by adding every sensor belief
 * present in the configuration.
 * @param beliefStructure structure into which the BFS_SensorBeliefs will be added
 * @param doc xml document containing the configuration
 * @param context xpath context for the document.
 */
static void addBeliefstoStructure(BFS_BeliefStructure *beliefStructure,  xmlDoc *doc,
		xmlXPathContext *context) {
	BFS_SensorBeliefs  sensorBelief;
	xmlXPathObject *xPathRequest =
			xmlXPathEvalExpression((xmlChar *)"/belief-from-sensors/sensors/sensor", context);
	xmlNode *currentNode;
	int i = 0;
	char* sensorName;
	BFS_SensorBeliefs *sensorBeliefs = NULL;
	int beliefNb = loadSensorBeliefs(beliefStructure->refList, &sensorBeliefs, doc, context);

	for (i = 0; i < xPathRequest->nodesetval->nodeNr; ++i) {
		currentNode = xPathRequest->nodesetval->nodeTab[i];
		sensorName = (char*)xmlGetProp(currentNode, (xmlChar*) "name");
		sensorBelief = getSensorBelief(sensorBeliefs, beliefNb,	beliefStructure->refList.card,
				currentNode);
		BFS_putSensorBelief(beliefStructure, sensorBelief);
		free(sensorName);
	}

	xmlXPathFreeObject(xPathRequest);
	for (i = 0; i < beliefNb; ++i) {
		BFS_freeSensorBeliefs(&sensorBeliefs[i]);
	}
	free(sensorBeliefs);

}

BFS_BeliefStructure BFS_loadBeliefStructureFromXml(char *path) {
	BFS_BeliefStructure belief;
	xmlDoc *doc = xmlParseFile(path);
	xmlXPathContext *context = xmlXPathNewContext(doc);
	char* frameName = extractFrameName(context);
	char** worldArray = getWorldArray(doc, context);
	int worldNb = getWorldNb(context);
	int i;

	belief = BFS_createBeliefStructure(frameName,
			(const char * const *) worldArray, worldNb);
	addBeliefstoStructure(&belief, doc, context);

	/*
	 * free everything
	 */
	free(frameName);
	for (i = 0; i < worldNb; ++i) {
		free(worldArray[i]);
	}
	free(worldArray);
	xmlXPathFreeContext(context);
	xmlFreeDoc(doc);
	return belief;
}
