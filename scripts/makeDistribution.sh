g++ -g -std=c++11 -static ../cpp/distributionPractice.cpp -o ../../build_release/distributionPractice
#g++ -g -std=c++11 -static ../cpp/collect.cpp -o ../../build_release/collect
#g++ -g -std=c++11 -static ../cpp/fastAStar/*.cc -o ../../build_release/tiles
g++ -g -std=c++11 -static ../cpp/fastAStar-intrusive/*.cc -o ../../build_release/tiles-intrusive

chmod a+x ../../build_release/distributionPractice
#chmod a+x ../../build_release/collect
#chmod a+x ../../build_release/tiles
chmod a+x ../../build_release/tiles-intrusive
