import subprocess
debug = 0

def command(input):
	#import os
	#os.system(input)
	if debug == 1:
		print input

	p = subprocess.Popen(input, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
	lines = p.stdout.readlines();
	retval = p.wait()
	return lines
	
	#retval = p.wait()
## JUST RUN THE MAKE FILE
def makeConfiguration(commandArg):
	command(makeClean)
	return command(commandArg)

## RUN MAKE FILE AND THEN RUN
def makeConfigurationRun(commandArg):
	command(makeClean)
	lines = command(commandArg)
	lines += command(run)
	return lines
def pMakeBuilder(number):
	arg = time + 'make OPT_FLAGS=\"-O3\" -j '+str(number)+ ' -silent'
	return arg

def buildAndGetSize(commandArg):
	command(makeClean)
	command(commandArg)
	#find the size of vpr
	return command('find . -name \"vpr\" -ls | awk \'{total += $7} END {print total}\'')

def makeConfigurationTimeRun(commandArg):
	makeConfigurationRun(commandArg)
	output = command(run)
	return output[-3:]

def moveGMONResult(name):
	commandArg = 'mv gmon.out gmon/'+name+'.out'
	command(commandArg)

# define some functions
time ='/usr/bin/time -p '
makeClean = 'make clean -silent'
makeProg = time+'make -silent'
makeGprof = time+'make OPT_FLAGS=\"-g -pg\" -silent'
makeGcov = time+'make OPT_FLAGS=\"-g -fprofile-arcs -ftest-coverage\" -silent'
makeG = time+'make OPT_FLAGS=\"-g\" -silent'
makeO2 = time+'make OPT_FLAGS=\"-O2\" -silent'
makeO3 = time+'make OPT_FLAGS=\"-O3\" -silent'
makeOs =time+'make OPT_FLAGS=\"-Os\" -silent'
run = time+'vpr iir1.map4.latren.net k4-n10.xml place.out route.out -nodisp -place_only -seed 0'
#gprof
makeGGProf = 'make OPT_FLAGS=\"-g -pg\" -silent'
makeO2GProf = 'make OPT_FLAGS=\"-O2 -pg\" -silent'
makeO3GProf = 'make OPT_FLAGS=\"-O3 -pg\" -silent'

############### HOMEWORK1 METHODS########################
def run_three_point_three():
	print 'section 3.3'
	print 'gprof'
	print makeConfiguration(makeGprof)  #gprof
	print 'gcof'
	print makeConfiguration(makeGcov)  #gcof
	print '-g'
	print makeConfiguration(makeG)  #-g
	print 'o2'
	print makeConfiguration(makeO2)  #-02
	print '03'
	print makeConfiguration(makeO3)  # -03
	print '0s'
	print makeConfiguration(makeOs)  # -0s

def run_three_point_four():
	print 'section 3.4'
	print '1'
	print makeConfiguration(pMakeBuilder(1))
	print '2'
	print makeConfiguration(pMakeBuilder(2))
	print '4'
	print makeConfiguration(pMakeBuilder(4))
	print '8'
	print makeConfiguration(pMakeBuilder(8))

def run_three_point_five():
	print 'section 3.5'
	print 'gprof'
	print buildAndGetSize(makeGprof)  #gprof
	print 'gcof'
	print buildAndGetSize(makeGcov)  #gcof
	print '-g'
	print buildAndGetSize(makeG)  #-g
	print 'o2'
	print buildAndGetSize(makeO2)  #-02
	print '03'
	print buildAndGetSize(makeO3)  # -03
	print '0s'
	print buildAndGetSize(makeOs)  # -0s
	
def run_three_point_six():
	print 'section 3.6'
	print 'gprof'
	print makeConfigurationTimeRun(makeGprof)  #gprof
	print 'gcof'
	print makeConfigurationTimeRun(makeGcov)  #gcof
	print '-g'
	print makeConfigurationTimeRun(makeG)  #-g
	print 'o2'
	print makeConfigurationTimeRun(makeO2)  #-02
	print '03'
	print makeConfigurationTimeRun(makeO3)  # -03
	print '0s'
	print makeConfigurationTimeRun(makeOs)  # -0s

def run_three_point_seven():
	#create a result director
	command('mkdir -p gmon')
	print 'g Gprof'
	makeConfigurationRun(makeGGProf)
	moveGMONResult('gGprof')
	
	print 'O2 Gprof'
	makeConfigurationRun(makeO2GProf)
	moveGMONResult('o2Gprof')
	print 'O3 Gprof'
	makeConfigurationRun(makeO3GProf)
	moveGMONResult('o3Gprof')
	#information as to how to use gprof
	#http://web.eecs.umich.edu/~sugih/pointers/gprof_quick.html

####################################HOMEWORK SCRIPT BEGINS#################################################
# compile application
run_three_point_six()
# for x in range(0,5):
# 	print '*************************'
# 	run_three_point_four()

#run_three_point_five()
# run_three_point_four()
# run_three_point_six()
#run_three_point_seven()


