g++ -g -std=c++11 -static ../cpp/generateTrees.cpp -o ../../build_release/generateTrees
g++ -g -std=c++11 -static ../cpp/lastIncrementalTests.cpp -o ../../build_release/lastIncremental
g++ -g -std=c++11 -static ../cpp/backupTestsDFS.cpp -o ../../build_release/backupTestsDFS
g++ -g -std=c++11 -static ../cpp/backupTestsAS.cpp -o ../../build_release/backupTestsAS
g++ -g -std=c++11 -static ../cpp/expansionTests.cpp -o ../../build_release/expansionTests

chmod a+x ../../build_release/generateTrees
chmod a+x ../../build_release/lastIncremental
chmod a+x ../../build_release/backupTestsDFS
chmod a+x ../../build_release/backupTestsAS
chmod a+x ../../build_release/expansionTests

