import subprocess
debug = 1

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

make = 'make'
makeClean = 'make clean'

oneKInput = '/usr/bin/time -f "%e real" ./gol 10000 inputs/1k.pbm outputs/1k_par.pbm';
fivetwelveKInput = '/usr/bin/time -f "%e real" ./gol 10000 inputs/1k.pbm outputs/1k_par.pbm';
oneTwentyEight = '/usr/bin/time -f "%e real" ./gol 10000 inputs/1k.pbm outputs/1k_par.pbm';

a1 = 'diff output/1k_par.pbm output/1k_base.pbm'
a2 = 'diff output/1k_par.pbm output/512_base.pbm'
a3 = 'diff output/1k_par.pbm output/128_base.pbm'

def runChecks():
		printf 'running programs'
		command(makeClean)
	 	command(make)
		command(oneKInput)
		command(fivetwelveKInput)
		command(oneTwentyEight)

def runDiff():
        print 'diffing'
        output = command(a1)
        if  output:
                print 'error 1' 
        output = command(a2)
        if  output:
                print 'error 2' 
        output = command(a3)
        if  output:
                print 'error 3' 
     
runChecks();
runDiff();