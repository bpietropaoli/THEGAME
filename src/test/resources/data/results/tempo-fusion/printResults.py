#!/usr/bin/env python

##########################################
# printResults.py                        #
# -------------------------------------- #
# Author: Bastien Pietropaoli            #
# Date of creation: 26/05/2011           #
# -------------------------------------- #
# The main aim of this script is to draw #
# the results of a fusion process using  #
# the belief functions theory.           #
##########################################

#import matplotlib.pyplot as plt
#import matplotlib.lines
import sys
import os

from pylab import *

#----------
#Get arguments:
#----------

args = sys.argv

if len(args) == 1 :
	args.append('*')

if args[1] == "*" :
	args.remove("*")
	for d in os.listdir("./") :
		args.append(d)
	args.remove("images")
	args.remove("printAll.py")

CAToPrint = args[1:]

for arg in CAToPrint :
	#-----------
	#Load data:
	#-----------
	#Open files:
	path = "./"
	path2 = "./../tempo-fusion-temoin/"
	
	CAPath = path + arg
	CAPath2 = path2 + arg
	
	print "Processing " + CAPath + "..."
	
	try :
		f = open(CAPath, "r")
		f2 = open(CAPath2, "r")
	
		mass = {}
		mass2 = {}

		#Get the number of values:
		nbOfValues = eval(f.readlines()[-1].split(";")[0])
		f.seek(0,0)
		possibleValues = []

		#Get the possible values:
		for line in f :
			words = line.split(";")
			if words[1] not in possibleValues :
				possibleValues.append(words[1])

		f.seek(0,0)

		for value in possibleValues :
			mass[value] = [0]*nbOfValues
			mass2[value] = [0]*nbOfValues

		#Read files:
		for line in f :
			words = line.split(";")
			mass[words[1]][eval(words[0])-1] = eval(words[2])
	
		for line in f2 :
			words = line.split(";")
			mass2[words[1]][eval(words[0])-1] = eval(words[2])

		f.close()
		f2.close()

		#Create list of curves to draw:
		massCurves  =  []
		massCurves2 =  []

		for key in possibleValues :
			massCurves.append(mass[key])
			massCurves2.append(mass2[key])


		colors = ['b', 'g', 'r', 'c', 'm', 'y', 'k']
		dashStyle = ['-', '--', ':', '-.']
		index = 0

		#-----------
		#Create subplots:
		#-----------
		fig = plt.figure(figsize=(14,7), facecolor='w') 
		#Place figures:
		massAx = plt.subplot(211)
		plt.title(arg)
		plt.ylabel("Mass (no tempo)")
		
		#--------------------------------
		#Print only a part of the graph:
		#--------------------------------
		beginningIndex = 0
		nbOfValues = nbOfValues - beginningIndex

		for i in range(len(massCurves2)) :
			massAx.add_line(Line2D(list(range(nbOfValues)), massCurves2[i][beginningIndex:], color=colors[index%len(colors)], linewidth=2, linestyle=dashStyle[index%len(dashStyle)]))
			index += 1
		index = 0
		massAx.plot()
		plt.ylim(-0.1, 1.1)
		plt.xlim(0, nbOfValues + 5)

		massAx2 = plt.subplot(212)
		plt.ylabel("Mass (tempo)")
		for i in range(len(massCurves)) :
			massAx2.add_line(Line2D(list(range(nbOfValues)), massCurves[i][beginningIndex:], color=colors[index%len(colors)], linewidth=2, linestyle=dashStyle[index%len(dashStyle)]))
			index += 1
		index = 0
		massAx2.plot()
		plt.ylim(-0.1, 1.1)
		plt.xlim(0, nbOfValues + 5)
		plt.xlabel("Time")
		massAx2.legend(possibleValues, loc=7, bbox_to_anchor = (1.1, 1.1))

		#-----------
		#Save:
		#-----------
		fig.savefig("./images/" + arg + ".png", format="png", dpi=600)
		fig.savefig("./images/" + arg + ".pdf", format="pdf", dpi=600)
	except IOError :
		print "No such context attribute '" + arg + "' or a file does not exist..."



