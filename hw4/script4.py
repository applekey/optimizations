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

makeClean = 'make clean'
make = 'make'


####################################HOMEWORK SCRIPT BEGINS#################################################

randtrack = 'randtrack 1 50 >base.out'
randtrackGlobal = 'randtrack_global_lock 4 50 >global.out'
randtrackTm = 'randtrack_tm 4 50 >randtrack_tm.out'
randtrackll = 'randtrack_list_lock 4 50 >randtrack_list_lock.out'
randtrackel = 'randtrack_element_lock 4 50 >randtrack_element_lock.out'
randtrackre = 'randtrack_reduction 3 50 >randtrack_reduction.out'

################################ compare ################################
c1 = 'sort -n base.out > base.outs'
c2 = 'sort -n global.out > global.outs'
c3 = 'sort -n randtrack_tm.out > randtrack_tm.outs'
c4 = 'sort -n randtrack_list_lock.out > randtrack_list_lock.outs'
c5 = 'sort -n randtrack_element_lock.out > randtrack_element_lock.outs'
c6 = 'sort -n randtrack_reduction.out > randtrack_reduction.outs'

a1 = 'diff global.outs base.outs' 
a2 = 'diff randtrack_tm.outs base.outs' 
a3 = 'diff randtrack_list_lock.outs base.outs' 
a4 = 'diff randtrack_element_lock.outs base.outs' 
a5 = 'diff randtrack_reduction.outs base.outs' 


def runChecks():
	print 'running checks'

	print 'compiling'
	command(makeClean)
	command(make)

	print 'running'
	command(randtrack)
	# print 'running'
	# command(randtrackGlobal)
	# print 'running'
	# command(randtrackTm)
	# print 'running'
	# command(randtrackll)
	# print 'running'
	# command(randtrackel)
	# print 'running'
	command(randtrackre)

	print 'sorting'
	# command(c1)
	# command(c2)
	# command(c3)
	# command(c4)
	command(c5)
	#command(c6)

def runDiff():
	print 'diffing'

	# output = command(a1)
	# if  output:
	# 	print 'error 1' 
	# output = command(a2)
	# if  output:
	# 	print 'error 2' 
	# output = command(a3)
	# if  output:
	# 	print 'error 3' 
	# output = command(a4)
	# if  output:
	# 	print 'error 4' 
	output = command(a5)
	if  output:
		print 'error 5' 

runChecks();
runDiff();