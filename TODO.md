- add ways to store data (maybe make this user-implementable aswell or provide sub-classes)
- optimise for speed  
- make it easy to use external plotting tools  
- more examples  
- great deluge isn't used in an example (nor the evolutionary)  
- allow algorithms to leave boundaries  
- taboo areas  
- carefully check the evolutionary behaviour  
- evolutionary mutations won't check for borders (only relevant if the algorithms should have to stay within their borders)
- properly call reserve on all vectors where their size is known beforehand

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
