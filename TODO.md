#features
##saving data
add ways to store data (maybe make this user-implementable aswell or provide sub-classes)
#other
rename to either cppOpt or heurOpt or similar
define a namespace
rename Calculator to Solver
optimise for speed
allow logging and other output solutions
have a static variable which counts the number of running threads. that way its easy to figure out when all are done
add a method to easily split OptBoundaries or vectors of them (split direction, times) returning a vector of vector of Boundaries
make proper cmake which will create the library, and compile examples / tests
rename the main.cpp to something like example1 and add some more examples
