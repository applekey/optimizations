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
fivetwelveKInput = '/usr/bin/time -f "%e real" ./gol 10000 inputs/512.pbm outputs/512_par.pbm';
oneTwentyEight = '/usr/bin/time -f "%e real" ./gol 10000 inputs/128.pbm outputs/128_par.pbm';

weird = '/usr/bin/time -f "%e real" ./gol 10000 inputs/weird.pbm outputs/weird_par.pbm';
thirtyThree = '/usr/bin/time -f "%e real" ./gol 10000 inputs/33.pbm outputs/33_par.pbm';
seven = '/usr/bin/time -f "%e real" ./gol 10000 inputs/777.pbm outputs/777_par.pbm';

a1 = 'diff output/1k_par.pbm output/1k_verify_out.pbm'
a2 = 'diff output/512_par.pbm output/512_base.pbm'
a3 = 'diff output/128_par.pbm output/128_base.pbm'

a4 = 'diff output/weird_par.pbm output/weird_base.pbm'
a5 = 'diff output/33_par.pbm output/33_base.pbm'
a6 = 'diff output/777_par.pbm output/777_base.pbm'

def runChecks():
        printf 'running programs'
        command(makeClean)
        command(make)
        command(oneKInput)
        command(fivetwelveKInput)
        command(oneTwentyEight)

        command(weird)
        command(thirtyThree)
        command(seven)

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

        output = command(a4)
        if  output:
                print 'error 4'

        output = command(a5)
        if  output:
                print 'error 5'

        output = command(a6)
        if  output:
                print 'error 6'
     
runChecks();
runDiff();