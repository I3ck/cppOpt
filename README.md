#cppOpt
A header-only C++ library which allows the numerical optimisation of any given problem, function, program or you-name-it  
without knowing the "function" of the problem simply by smartly testing certain values  
- you only have to include a single header file `cppOpt.h`
- using heuristic algorithms http://en.wikipedia.org/wiki/Heuristic  
- using Catch as testing framework https://github.com/philsquared/Catch

##version 0.2.4

##optimise anything!
optimise any given problem by defining your very own Solver class:
```cpp
template <typename T>
class MySolver : public OptSolverBase<T>
{
public:
    void calculate(OptCalculation<T> &optCalculation) const
    {
        optCalculation.result = YOUR_CODE_COMES_HERE;
    }
};
```
for optimising x^2 you could write:

```cpp
void calculate(OptCalculation<T> &optCalculation) const
{
    //defined x^2 as function to be optimised
    optCalculation.result = pow(optCalculation.get_parameter("X"),2);
}
```

you can even run other programs:

```cpp
void calculate(OptCalculation<T> &optCalculation) const
{
    std::string parameter1 = optCalculation.get_parameter("X").to_string();
    std::string parameter2 = optCalculation.get_parameter("Y").to_string();

    system(( "someExecuteable -p1 " + parameter1 + " -p2 " + parameter2).c_str() );
    optCalculation.result = parse_generated_output_file();
}
```

##define your parameters and boundaries
```cpp
OptBoundaries<T> optBoundaries;
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

##templated
use any of these:
```cpp
<float>  
<double>  
<long double>
```

##choose your algorithm
```cpp
OptSimulatedAnnealing<double> opt1(optBoundaries,
                                   maxCalculations,
                                   &mySolver,
                                   optTarget,
                                   targetValue,
                                   coolingFactor,
                                   startChance);

OptThresholdAccepting<double> opt2(optBoundaries,
                                   maxCalculations,
                                   &mySolver,
                                   optTarget,
                                   targetValue,
                                   coolingFactor,
                                   threshold,
                                   thresholdFactor);

OptGreatDeluge<double> opt3(optBoundaries,
                            maxCalculations,
                            &mySolver,
                            optTarget,
                            targetValue,
                            coolingFactor,
                            waterLevel,
                            rain);

OptEvolutionary<double> opt4(optBoundaries,
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
OptBase<double>::run_optimisations(NUMBER_OF_WORKER_THREADS);
```

##logging / outputting results
you can enable thread-safe logging as simple as:
```cpp
OptBase<double>::enable_logging("logfile.log", optBoundaries);
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
OptCalculation<double> best1 = opt1.get_best_calculation();
OptCalculation<double> best2 = opt2.get_best_calculation();
OptCalculation<double> bestAll = OptBase<double>::get_best_calculation(optTarget, targetValue);
```

##examples
###optimising x*x [-5.0:+5.0] with simulated annealing
note that the optimiser doesn't "know" that the function actually is x*x
####minimizing:
![alt tag](https://raw.githubusercontent.com/I3ck/cppOptImages/master/images/animations/xSquare/minimize.gif)  
####maximizing:
![alt tag](https://raw.githubusercontent.com/I3ck/cppOptImages/master/images/animations/xSquare/maximize.gif)  
####approaching 3.0:
![alt tag](https://raw.githubusercontent.com/I3ck/cppOptImages/master/images/animations/xSquare/approach_3.gif)  
####diverging from 3.0:
![alt tag](https://raw.githubusercontent.com/I3ck/cppOptImages/master/images/animations/xSquare/diverge_3.gif)  
check out https://github.com/I3ck/cppOptImages for more images  
or `examples/` for code examples

###optimising the rastrigrin function with simulated annealing
http://en.wikipedia.org/wiki/Rastrigin_function
####one-dimensional
![alt tag](https://raw.githubusercontent.com/I3ck/cppOptImages/master/images/animations/rastrigrin/minimize_sa_1d.gif)  
####two-dimensional
![alt tag](https://raw.githubusercontent.com/I3ck/cppOptImages/master/images/animations/rastrigrin/minimize_sa_2d.gif)  
#usage

##configuration
open up `inc/config.h` to enable/disable DEBUG output  
```cpp
//#define DEBUG
```

##building the examples / tests
`cmake .`  
`make`


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
