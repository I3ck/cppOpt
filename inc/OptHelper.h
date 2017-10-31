/*
    Copyright (c) 2017 Martin Buck
    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation the rights to
    use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
    and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
    DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
    OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef OPTHELPER_H
#define OPTHELPER_H

#include "OptTarget.h"

namespace cppOpt
{

using namespace std;

template <typename T>
class OptHelper
{
public:

//------------------------------------------------------------------------------

    //targetValue won't be used when maximizing or minimizing
    static bool result_better(OptCalculation<T> const& result, OptCalculation<T> const& other, OptTarget const& optTarget, T const& targetValue) ///@todo consider implementing this in OptCalculation
    {
        switch(optTarget)
        {
            case OptTarget::MINIMIZE:
                return result.result < other.result;

            case OptTarget::MAXIMIZE:
                return result.result > other.result;

            case OptTarget::APPROACH:
                return fabs(targetValue - result.result) < fabs(targetValue - other.result);

            case OptTarget::DIVERGE:
                return fabs(targetValue - result.result) > fabs(targetValue - other.result);

            default: //MINIMIZE
                return result.result < other.result;
        }
    }

    static T random_factor() ///@todo all related functions now defined twice, move to helper
    {
        return rand()/(T)(RAND_MAX);
    }


};

} // namespace cppOpt

#endif // OPTHELPER_H
