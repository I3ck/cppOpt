- add ways to store data (maybe make this user-implementable aswell or provide sub-classes)
- optimise for speed  
- add single opt object logging  
- make it easy to use external plotting tools  
- add some example plots to the README (maybe use matplotlib for this)  
- evolutionary algorithm  
- more examples  
- rain / coolingFactor etc. can be defined const  
- rename referenceValue to center or vice versa  

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
