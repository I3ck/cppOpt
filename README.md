#cppOpt
A C++ library which allows the numerical optimisation of any given problem, function, program or you-name-it  
without knowing the "function" of the problem simply by smartly testing certain values  
- using heuristic algorithms http://en.wikipedia.org/wiki/Heuristic  
- using Catch as testing framework https://github.com/philsquared/Catch

##version 0.1.4

##optimise anything!
optimise any given problem by defining your very own Solver class:
```cpp
class MySolver : public OptSolverBase
{
public:
    void calculate(OptCalculation &optCalculation) const
    {
        optCalculation.result = YOUR_CODE_COMES_HERE;
    }
};
```
for optimising x^2 you could write:

```cpp
void calculate(OptCalculation &optCalculation) const
{
    //defined x^2 as function to be optimised
    optCalculation.result = pow(optCalculation.get_parameter("X"),2);
}
```

you can even run other programs:

```cpp
void calculate(OptCalculation &optCalculation) const
{
    std::string parameter1 = optCalculation.get_parameter("X").to_string();
    std::string parameter2 = optCalculation.get_parameter("Y").to_string();

    system(( "someExecuteable -p1 " + parameter1 + " -p2 " + parameter2).c_str() );
    optCalculation.result = parse_generated_output_file();
}
```

##define your parameters and boundaries
```cpp
OptBoundaries optBoundaries;
optBoundaries.add_boundary(-5.0, 5.0, "X");
optBoundaries.add_boundary(-50.0, 55.0, "Y");
optBoundaries.add_boundary(33.0, 3.0, "potatoe");
```

##minimize, maximize, approach or diverge
```cpp
OptTarget optTarget = MINIMIZE;
OptTarget optTarget = MAXIMIZE;
OptTarget optTarget = APPROACH;
OptTarget optTarget = DIVERGE;
```

##choose your algorithm
```cpp
OptSimulatedAnnealing opt1(optBoundaries,
                           maxCalculations,
                           &mySolver,
                           optTarget,
                           targetValue,
                           coolingFactor,
                           startChance);

OptThresholdAccepting opt2(optBoundaries,
                           maxCalculations,
                           &mySolver,
                           optTarget,
                           targetValue,
                           coolingFactor,
                           threshold,
                           thresholdFactor);

OptGreatDeluge opt3(optBoundaries,
                    maxCalculations,
                    &mySolver,
                    optTarget,
                    targetValue,
                    coolingFactor,
                    waterLevel,
                    rain);

OptEvolutionary opt4(optBoundaries,
                     maxCalculations,
                     &mySolver,
                     optTarget,
                     targetValue,
                     coolingFactor,
                     nIndividualsStart,
                     nIndividualsSelection,
                     nIndividualsOffspring,
                     mutation);
```

##multithreading
you can spawn any number of objects with different algorithms, boundaries, configurations or goals

##go!
```cpp
OptBase::run_optimisations(NUMBER_OF_WORKER_THREADS);
```

##logging / outputting results
you can enable thread-safe logging as simple as:
```cpp
OptBase::enable_logging("logfile.log", optBoundaries);
```
```
X RESULT
1.083865 1.174763
3.901791 15.223976
3.862659 14.920135
...
```
you can also define custom delimiters and line endings (e.g. for csv):
```cpp
OptBase::enable_logging("logfile.log", optBoundaries, ";", "\n");
```
you can retrieve both the best values of all your optimisers or specific ones:
```cpp
OptCalculation best1 = opt1.get_best_calculation();
OptCalculation best2 = opt2.get_best_calculation();
OptCalculation bestAll = OptBase::get_best_calculation(optTarget, targetValue);
```

##examples
###optimising x*x [-5.0:+5.0] with simulated annealing
note that the optimiser doesn't "know" that the function actually is x*x
####minimizing:
![alt tag](https://raw.githubusercontent.com/I3ck/cppOptImages/master/images/animations/xSquare/minimize.gif)  
####maximizing:
![alt tag](https://raw.githubusercontent.com/I3ck/cppOptImages/master/images/animations/xSquare/maximize.gif)  
###approaching 3.0:
![alt tag](https://raw.githubusercontent.com/I3ck/cppOptImages/master/images/animations/xSquare/approach_3.gif)  
###diverging from 3.0:
![alt tag](https://raw.githubusercontent.com/I3ck/cppOptImages/master/images/animations/xSquare/diverge_3.gif)  
check out https://github.com/I3ck/cppOptImages for more images  
or `examples/` for code examples

#usage

##configuration
open up `inc/config.h` to enable/disable DEBUG output  
or to define the used floating point precision
```cpp
//#define OPT_T float
#define OPT_T double
//#define OPT_T long double
```

##building
`cmake .`  
`make`

##linking
link your program with the library created in `lib/`  
(or feel free to directly link the source files and not use `make` at all)

##including
directly include `cppOpt.h` which includes all required headers or choose the header files you want to use  

##testing
after using `cmake` and `make` check `bin/` for the `test_xyz` executeables  
since the tests check whether the algorithms reach the wanted values, it's possible that the tests might fail in some cases (due to the random nature of the algorithms). Just restart the test in such a case.  
Only if the test fails often there's an actual bug

#algorithms explained / referenced

##simulated annealing
http://en.wikipedia.org/wiki/Simulated_annealing

##threshold accepting
http://comisef.wikidot.com/concept:thresholdaccepting

##great deluge
http://en.wikipedia.org/wiki/Great_Deluge_algorithm

##evolutionary
https://en.wikipedia.org/wiki/Evolutionary_algorithm
