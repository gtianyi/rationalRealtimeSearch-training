CXX=g++
STD=c++11
debug_dir=../build_debug
release_dir=../build_release

all: clean distributionPractice collect

clean:
	rm -rf ${debug_dir}/*
	rm -rf ${release_dir}/*

distributionPractice:
	rm -rf ${release_dir}/distributionPractice
	rm -rf ${debug_dir}/distributionPractice-debug

	${CXX} -o3 -std=${STD} -static cpp/distributionPractice.cpp -o ${release_dir}/distributionPractice
	${CXX} -g -D DEBUG -std=${STD} -static cpp/distributionPractice.cpp -o ${debug_dir}/distributionPractice-debug

	chmod a+x ${release_dir}/distributionPractice
	chmod a+x ${debug_dir}/distributionPractice-debug

collect:
	rm -rf ${release_dir}/collect
	rm -rf ${debug_dir}/collect-debug

	${CXX} -o3 -std=${STD} -static cpp/collector/collect.cpp -o ${release_dir}/collect
	${CXX} -g -D DEBUG -std=${STD} -static cpp/collector/collect.cpp -o ${debug_dir}/collect-debug

	chmod a+x ${release_dir}/collect
	chmod a+x ${debug_dir}/collect-debug

tile-pdb-batch:
	rm -rf ${release_dir}/tile-pdb-batch
	rm -rf ${debug_dir}/tile-pdb-batch-debug

	${CXX} -o3 -std=${STD} -static cpp/optimalSolver/fastAStar-pdb-batch/*.cc -o ${release_dir}/tile-pdb-batch
	${CXX} -g -D DEBUG -std=${STD} -static cpp/optimalSolver/fastAStar-pdb-batch/*.cc -o ${debug_dir}/tile-pdb-batch-debug

	chmod a+x ${release_dir}/tile-pdb-heavy-batch-inverse
	chmod a+x ${debug_dir}/tile-pdb-batch-debug

tile-pdb:
	rm -rf ${release_dir}/tile-pdb
	rm -rf ${debug_dir}/tile-pdb-debug

	${CXX} -o3 -std=${STD} -static cpp/optimalSolver/fastAStar-pdb/*.cc -o ${release_dir}/tile-pdb
	${CXX} -g -D DEBUG -std=${STD} -static cpp/optimalSolver/fastAStar-pdb/*.cc -o ${debug_dir}/tile-pdb-debug

	chmod a+x ${release_dir}/tile-pdb
	chmod a+x ${debug_dir}/tile-pdb-debug

pdb-generator:
	rm -rf ${release_dir}/pdb-generator
	rm -rf ${debug_dir}/pdb-generator-debug

	${CXX} -o3 -std=${STD} cpp/optimalSolver/fastAStar-pdb/*.hpp cpp/optimalSolver/fastAStar-pdb/tiles.cc cpp/optimalSolver/fastAStar-pdb/fatal.cc cpp/optimalSolver/pdb/*.hpp cpp/optimalSolver/pdb/* -o ${release_dir}/pdb-generator

	${CXX} -g -std=${STD} cpp/optimalSolver/fastAStar-pdb/*.hpp cpp/optimalSolver/fastAStar-pdb/tiles.cc cpp/optimalSolver/fastAStar-pdb/fatal.cc cpp/optimalSolver/pdb/*.hpp cpp/optimalSolver/pdb/* -o ${debug_dir}/pdb-generator-debug

	chmod a+x ${release_dir}/pdb-generator
	chmod a+x ${debug_dir}/pdb-generator-debug

tile-uniform:
	rm -rf ${release_dir}/tile-uniform
	rm -rf ${debug_dir}/tile-uniform-debug

	${CXX} -o3 -std=${STD} cpp/optimalSolver/fastAStar/*.cc -o ${release_dir}/tile-uniform

	${CXX} -g -std=${STD} cpp/optimalSolver/fastAStar/*.cc -o ${debug_dir}/tile-uniform-debug

	chmod a+x ${release_dir}/tile-uniform
	chmod a+x ${debug_dir}/tile-uniform-debug

#g++ -o3 -std=c++11 -static ../cpp/distributionPractice.cpp -o ../../build_release/distributionPractice
#g++ -g -std=c++11 -static ../cpp/distributionPractice.cpp -o ../../build_debug/distributionPractice
#g++ -g -std=c++11 -static ../cpp/collect.cpp -o ../../build_release/collect
#g++ -o3 -std=c++11 -static ../cpp/collect-inverse.cpp -o ../../build_release/collect-inverse
#g++ -g -std=c++11 -static ../cpp/collect-post-search.cpp -o ../../build_release/collect-post-search
#g++ -g -std=c++11 -static ../cpp/fastAStar/*.cc -o ../../build_release/tiles-uniform
#g++ -o3 -std=c++11 -static ../cpp/fastAStar-pdb/*.cc -o ../../build_release/tiles-pdb
#g++ -o3 -std=c++11 -static ../cpp/fastAStar-inverse/*.cc ../cpp/fastAStar-inverse/*.hpp -o ../../build_release/tiles-inverse


#chmod a+x ../../build_release/collect
#chmod a+x ../../build_release/collect-inverse
#chmod a+x ../../build_release/collect-post-search
#chmod a+x ../../build_release/tiles-uniform
#chmod a+x ../../build_release/tiles-pdb
#chmod a+x ../../build_release/tiles-inverse
