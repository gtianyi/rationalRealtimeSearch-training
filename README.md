# INIT Scripts. 
## RUN inside nancybackup dir
mkdir ../realtime-nancy

cd ../realtime-nancy

mv ../nancybackup .

mkdir build_release

mkdir build_debug

mkdir plots

mkdir results

mkdir worlds

# TO BUILD

cd scripts/

./make.sh

# TO GET TOY PROBLEMS

I would suggest clone Andrew's git repo to your local and move what you need to your relevent local dir

https://github.com/ajx256/thesis-real-time-search/tree/master/worlds

# Single TEST

cd ../build_release

./expansionTests slidingTilePuzzle 10 testResults.json < ../worlds/slidingTile/1-4x4.st

# TO RUN TESTS FOR AAAI SUBMISSION
## Better to run on UNH AI Cluster Machines

cd scripts/testHarnesses/

./expansionTestHarness.sh 1 100 <# of processes> SlidingPuzzle 4 10 30 100 300 1000


