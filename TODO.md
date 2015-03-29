#structure
##optimization base-class  
this class has to contain all the logic except the algorithm parameter(s)
###optimization sub-classes which implement the algorithms
these have to implement the algorithms and will later be used by the user
##calculator base-class
this has to be derived from by the library-user to define the problem itself
#features
##multi-threading
###create three different queues
QueueTodo (holding not-calculated OptValues)
QueueCalculated (holding calculated OptValues)
QueueFinished (holding calculated and read OptValues)
the algorithms fill the todo queue
workers pop from the todo queue, calculate them and put them onto the calculated queue
the algorithms pop from the calculated queue, read the result and move them onto the finished queue
the finished queue is later used for information output
use atomic or mutex locks where needed
##saving data
add ways to store data (maybe make this user-implementable aswell or provide sub-classes)
##results
maybe make it possible to work with a set of results instead of a result
#other
rename to either cppOpt or heurOpt or similar
define a namespace
there needs to be a flag to check whether all optimisers are done
rename Calculator to Solver
properly seed the random numbers
optimise for speed
allow logging and other output solutions
have a static variable which counts the number of running threads. that way its easy to figure out when all are done
add a method to easily split OptBoundaries or vectors of them (split direction, times) returning a vector of vector of Boundaries
maybe add logging to the calculator? rename it then to something else
