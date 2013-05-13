#!/usr/bin/env python

import sys
import os
import time

files = os.listdir("./")
files.remove("images")
files.remove("printResults.py")
files.remove("printAll.py")

for f in files :
	os.system("python printResults.py " + '"' + f + '"')
	#time.sleep(2)
