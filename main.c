/*
 * Copyright 2011-2013, EDF. This software was developed with the collaboration of INRIA (Bastien Pietropaoli)
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


#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>
#include "BeliefFunctions.h"
#include "BeliefsFromSensors.h"
#include "BeliefsFromBeliefs.h"
#include "BeliefsFromRandomness.h"
#include "Sets.h"
#include "Tests.h"

int main(){
	int nbIterations = 10000;
	int write = 1;
	int okay = 0;
	
	okay = Tests_runTests(nbIterations, write);
	
	return okay;
}


