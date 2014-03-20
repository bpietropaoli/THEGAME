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


#ifndef DEF_READFILE
#define DEF_READFILE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

/**
 * @file ReadFile.h
 * @author Bastien Pietropaoli (bastien.pietropaoli@inria.fr)
 * @brief UTILITY: A module to ease file reading.
 */

 /**
  * @def NB_ENDLINE_CHARS
  * The number of characters considered at the end of a line.
  * (Different for Windows and Unix systems.)
  */
#ifdef WINDOWS
#define NB_ENDLINE_CHARS 2
#endif
#ifdef UNIX
#define NB_ENDLINE_CHARS 1
#endif

/**
 * Counts the number of lines in the file.
 * Actually, it counts the number of occurrence
 * of the character '\n'.
 * @param fileName The name of the file to study
 * @return The number of lines in the file
 */
int ReadFile_countLines(const char* fileName);

/**
 * Counts the number of characters per line of the file.
 * @param fileName The name of the file to study
 * @param nbLines The number of lines in the file
 * @return A pointer to a list of integer representing the number
 *         of characters per line. NULL if error.
 *         CAUTION: Don't forget to free list when it 's
 *         not needed anymore!
 */
int* ReadFile_charsPerLine(const char* fileName, const int nbLines);

/**
 * Reads the lines of a file.
 * @param fileName The name of the file to read
 * @param nbLines The number of lines in this file
 * @param charsPerLine The number of characters per line
 * @return A list a strings representing the different lines of the file.
 *         NULL if error.
 *         CAUTION: Don't forget to free everything when it's not need anymore!
 */
char** ReadFile_readLines(const char* fileName, const int nbLines, const int* charsPerLine);

#endif
