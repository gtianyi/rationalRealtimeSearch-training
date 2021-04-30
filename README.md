# ABOUT 
This is a codebase for collecting non-parametric heuristic data error model. We run suboptimal search algorithm such as weighted A* and Lsslrta* to collect observed states. Then use optimal solver such as A*, IDA*(CR) (sometime with stronger heuristc) to compute the optimal cost-to-go value. In short, we generate belief distribution of h* values for h.

# PAPER
[1] Maximilian Fickert, Tianyi Gu, Leonhard Staut, Wheeler Ruml, Joerg Hoffmann, and Marek Petrik, Beliefs We Can Believe In: Replacing Assumptions with Data in Real-Time Search. Proceedings of the Thirty-fourth AAAI Conference on Artificial Intelligence (AAAI-20), 2020.

[[pdf]](http://cs.unh.edu/~tg1034/publication/DDNancy.pdf) [[slides]](http://cs.unh.edu/~tg1034/slides/DDNancy_slides.pdf) [[poster]](http://cs.unh.edu/~tg1034/slides/DDNancy_poster.pdf)

[2] Maximilian Fickert, Tianyi Gu, Leonhard Staut, Sai Lekyang, Wheeler Ruml, Joerg Hoffmann, and Marek Petrik, Real-time Planning as Data-driven Decision-making. Proceedings of the ICAPS Workshop on Bridging the Gap Between AI Planning and Reinforcement Learning (PRL-20), 2020.

[[pdf]](http://cs.unh.edu/~tg1034/publication/DDNancy-PRL.pdf) [[talk]](https://youtu.be/4f1ual5R4s0) [[slides]](http://cs.unh.edu/~tg1034/slides/prl_nancy_slides.pdf) [[poster]](http://cs.unh.edu/~tg1034/slides/prl_nancy_poster.pdf)

# BUILD. 
```console
mkdir ../realtime-nancy
cd ../realtime-nancy
mv ../nancybackup .
mkdir build_release
mkdir build_debug
mkdir plots
mkdir results
mkdir worlds

make
```
# PIPELINE TO GENERATE DATA. 
1. generate domain instances and put them into worlds folder
    * tiles instances can be found [here](https://github.com/ajx256/thesis-real-time-search/tree/master/worlds)  
    * feel free to email me if you need other domain's instance or generator  
2. run weighted A* or lsslrtA* on 500 random to gather all visited states
    * single run:
      ```console
      cd ../build_release
      ./distributionPractice -o out.txt < ../world/slidingTile/1-4x4.st
      ```
    * run batch of experiments:
      ```console
      cd script/testHarnesses/
      ./singleThread-suboptimalSolver.sh -h
      ```
3. check all observed states:
    ```console
    cd ../result/SlidingTilePuzzle/distributionTest
    ls -la
    ```
4. for each h value, collect 200 unique states according to their frequency counter
    ```console
    cd ../build_release
    ./collect -h
    ```

	* check all collected problem
      ```console
      cd ../result/SlidingTilePuzzle/sampleProblem
      ls -la 

5. use optimal solver to solve all the collected instances. 
    * For uniform puzzle, use idastar with manhattan heuristic 
    * For heavy and inverse puzzle, use ida star with pattern database heuristic 
    * For pancake and racetrack, use wastar with weight 1

    on all ai machines:
    ```console
    cd script/testHarnesses
    ```
    heavy tile or inverse tile:
    ```console
    ./multiThread-pdb.sh 2 1 67199 idastar heavy lsslrtastar
    ```
    uniform tile, pancake, or racetrack:
    ```console
    ./multThread-optimalSolver.sh -h
    ```
    check all solutions
    ```console
    cd ../result/SlidingPuzzle/sampleData
    ```
    6. compute h-h* distribution, and their sample sets
    ```console
    cd scripts/distributionGenerator/
    ./distGenerator.py -h
    ```
    This should generate a json file that contains the data that represent the histogram.
7. (optional) generate the post-search samples by doing one step lookahead on the samples and nancy backing-up their distribution and merging them. 
    ```console
    cd ../build_release
    ./collect-post-search heavy lsslrtastar
    ```
8. (optional) compute post-search h-h* distribution based on samples merged by previous step
    ```console
    cd script/plotting
    python distribtuion.py uniform postd
    ```
    This should generate another json file that contains the data that represent the histogram.

# PROBLEM INSTANCES
All the instances can be found [here](https://github.com/gtianyi/searchDomainInstanceFiles)

