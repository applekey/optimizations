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
randtrackTm = 'randtrack_tm 4 50 >tm.out'
randtrackll = 'randtrack_list_lock 4 50 >ll.out'
randtrackel = 'randtrack_element_lock 4 50 >el.out'
randtrackre = 'randtrack_reduction 4 50 >re.out'

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
	print 'running checks\n'

	print 'compiling'
	command(makeClean)
	command(make)
	command(randtrack)
	command(randtrackGlobal)
	command(randtrackTm)
	command(randtrackll)
	command(randtrackel)
	command(randtrackre)

	print 'sorting'
	command(c1)
	command(c2)
	command(c3)
	command(c4)
	command(c5)
	command(c6)

	print 'diffing'

	print command(a1)
	print command(a2)
	print command(a3)
	print command(a4)
	print command(a5)
	print command(a6)

runChecks();