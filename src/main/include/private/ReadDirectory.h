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


#ifndef DEF_READDIRECTORY
#define DEF_READDIRECTORY
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "config.h"

/**
 * @file ReadDirectory.h
 * @author Bastien Pietropaoli (bastien.pietropaoli@inria.fr)
 * @brief UTILITY: A module to ease the manipulation of directories
 */

 /**
  * @def MAX_SIZE_PATH 1024
  * The maximum path size that can be managed by the module.
  */
 #define MAX_SIZE_PATH 1024

/**
 * @name Directory name manipulation
 * @{
 */

/**
 * Tells if the given path is a directory or not.
 * @param path The path to test
 * @return 1 if path is a directory, 0 if not
 */
int ReadDir_isDirectory(const char* path);

/**
 * Counts the number of subdirectories at the given path.
 * @param path The path to work on
 * @return The number of directories at path. -1 if error.
 */
int ReadDir_countDirectories(const char* path);

/**
 * Counts the number of characters in the names of subdirectories at the given path.
 * @param path The path to work on
 * @param nbDir The number of directories
 * @return A list of integers with the number of characters in the names of directories.
 * NULL if error. Must be freed after use.
 */
int* ReadDir_charsPerDirectory(const char* path, const int nbDir);

/**
 * Get the list of directories at the given path.
 * @param path The path to work on
 * @param nbDir The number of directories
 * @param charsPerDir The number of characters in the directories names
 * @return A list of strings corresponding to the names of directories.
 * NULL if error. Must be free after use.
 */
char** ReadDir_getDirectories(const char* path, const int nbDir, const int* charsPerDir);

/** @} */

/**
 * @name Filename manipulation
 * @{
 */

/**
 * Tells if the given path is a file or not.
 * @param path The path to test
 * @return 1 if path is a file, 0 if not
 */
int ReadDir_isFile(const char* path);

/**
 * Counts the number of files in at a given path.
 * @param path The path to work on
 * @return The number of files at path. -1 if error.
 */
int ReadDir_countFiles(const char* path);

/**
 * Counts the number of characters in the names of files at the given path.
 * @param path The path to work on
 * @param nbFiles The number of files at the given path
 * @return A list of integers with the number of characters in the filenames.
 * NULL if error. Must be freed after use.
 */
int* ReadDir_charsPerFilename(const char* path, const int nbFiles);

/**
 * Get the filenames at the given path.
 * @param path The path to work on
 * @param nbFiles The number of files at the given path
 * @param charsPerFilenam The number of characters per filename
 * @return A list of strings corresponding to the filenames at the given path.
 * NULL if error. Must be freed after use.
 */
char** ReadDir_getFilenames(const char* path, const int nbFiles, const int* charsPerFilenam);

/** @} */

#endif


