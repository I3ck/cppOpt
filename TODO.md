- add ways to store data (maybe make this user-implementable aswell or provide sub-classes)
- optimise for speed  
- either optimise or optimize (both correct)  
- add single opt object logging  
- logging should allow user defined value dividers and newline characters (but default to " " and "\n")  
- make it easy to use external plotting tools
- add some example plots to the README
- prefix `opt` on all classnames?
- rename get_parameter() to get_value() ?

- add a method to split entire boundaries:
```cpp
    optBoundaries A, B, C;
    A:
        X => 0.0 10.0
        Y => 3.0 11.0

    A.split("X", B, C);

    A:
        X => 0.0 10.0
        Y => 3.0 11.0
    B:
        X => 0.0 5.0
        Y => 3.0 11.0
    C:
        X => 5.0 10.0
        Y => 3.0 11.0
```
    maybe even allow the usage of any number of split targets
    A.split("X", B, C, D, E, F, G); (or use a list of optBoundaries)
    dividing into any number of elements
