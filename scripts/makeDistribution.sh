#g++ -g -std=c++11 -static ../cpp/distributionPractice.cpp -o ../../build_release/distributionPractice
#g++ -g -std=c++11 -static ../cpp/collect.cpp -o ../../build_release/collect
#g++ -g -std=c++11 -static ../cpp/collect-inverse.cpp -o ../../build_release/collect-inverse
#g++ -g -std=c++11 -static ../cpp/fastAStar/*.cc -o ../../build_release/tiles-uniform
#g++ -g -std=c++11 -static ../cpp/fastAStar-intrusive/*.cc -o ../../build_release/tiles-heavy
g++ -g -std=c++11 -static ../cpp/fastAStar-inverse/*.cc ../cpp/fastAStar-inverse/*.hpp -o ../../build_release/tiles-inverse-pdbtest

#chmod a+x ../../build_release/distributionPractice
#chmod a+x ../../build_release/collect
#chmod a+x ../../build_release/collect-inverse
#chmod a+x ../../build_release/tiles-uniform
#chmod a+x ../../build_release/tiles-heavy
chmod a+x ../../build_release/tiles-inverse-pdbtest
