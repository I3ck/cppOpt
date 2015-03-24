#structure
##optimization base-class  
this class has to contain all the logic except the algorithm parameter(s)
###optimization sub-classes which implement the algorithms
these have to implement the algorithms and will later be used by the user
##calculator base-class
this has to be derived from by the library-user to define the problem itself
#features
##multi-threading
use atomic or mutex locks where needed
##saving data
add ways to store data (maybe make this user-implementable aswell or provide sub-classes)
##resultS
maybe make it possible to work with a set of results instead of a result
#other
rename to either cppOpt or heurOpt or similar

