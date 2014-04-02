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



#ifndef XMLBELIEFFROMSENSORS_H_
#define XMLBELIEFFROMSENSORS_H_

#include "BeliefsFromSensors.h"

/**
 * Load a belief structure from an xml file.
 * @see BFS_freeBeliefStructure()
 *
 * @param path path to the xml file.
 * @return a new BFS_BeliefStructure, must be freed after use.
 */
BFS_BeliefStructure BFS_loadBeliefStructureFromXml(char *path);

#endif /* XMLBELIEFFROMSENSORS_H_ */
