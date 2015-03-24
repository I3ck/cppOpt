#structure
##optimization base-class  
this class has to contain all the logic except the algorithm parameter(s)
###optimization sub-classes which implement the algorithms
these have to implement the algorithms and will later be used by the user
##calculator base-class
this has to be derived from by the library-user to define the problem itself

