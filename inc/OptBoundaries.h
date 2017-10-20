/*
    Copyright (c) 2015 Martin Buck
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

#ifndef OPTBOUNDARIES_H
#define OPTBOUNDARIES_H

#include <string>
#include <map>

#include "config.h"
#include "OptBoundary.h"

namespace cppOpt
{

using namespace std;

template <typename T>
class OptBoundaries
{
private:
    map<string, OptBoundary<T> >
        optBoundaries;

//------------------------------------------------------------------------------

public:
    void add_boundary(T min, T max, const string &name)
    {
        OptBoundary<T> temp(min, max, name);
        add_boundary(temp);
    }

//------------------------------------------------------------------------------

    void add_boundary(const OptBoundary<T> &optBoundary)
    {
        optBoundaries.insert(make_pair(optBoundary.name, optBoundary));
    }

//------------------------------------------------------------------------------

    string to_string() const
    {
        string out("");

        for(const auto &boundary : optBoundaries)
            out += boundary.first + " ";

        return out;
    }

//------------------------------------------------------------------------------

    size_t size() const
    {
        return optBoundaries.size();
    }

//------------------------------------------------------------------------------

    vector<OptBoundaries> split(const string &name, unsigned int times) const
    {
        vector<OptBoundaries> out;

        T
            range(0.0),
            newRange(0.0);

        bool found(false);

        if(times < 1)
            return out; //error case

        for(const auto &boundary : optBoundaries)
        {
            if(boundary.first == name)
            {
                range = boundary.second.range();
                found = true;
            }
        }

        if(!found)
            return out; //error case if name doesn't exist

        newRange = range / (T)times;

        //split
        for(unsigned int i = 0; i < times; ++i)
        {
            OptBoundaries newBoundaries;
            for(const auto &boundary : optBoundaries)
            {
                if(boundary.first == name)
                {
                    T
                        newMin,
                        newMax;

                    newMin = boundary.second.min + i * newRange;
                    newMax = boundary.second.min + (i+1) * newRange;

                    OptBoundary<T> newBoundary(newMin, newMax, name);

                    newBoundaries.add_boundary(newBoundary);
                }
                else
                    newBoundaries.add_boundary(boundary.second);
            }
            out.push_back(newBoundaries);
        }

        return out;

    }

//------------------------------------------------------------------------------

    typename map< string, OptBoundary<T> >::iterator begin()
    {
        return optBoundaries.begin();
    }

//------------------------------------------------------------------------------

    typename map< string, OptBoundary<T> >::iterator end()
    {
        return optBoundaries.end();
    }

//------------------------------------------------------------------------------

    const typename map< string, OptBoundary<T> >::const_iterator cbegin() const
    {
        return optBoundaries.cbegin();
    }

//------------------------------------------------------------------------------

    const typename map< string, OptBoundary<T> >::const_iterator cend() const
    {
        return optBoundaries.cend();
    }

//------------------------------------------------------------------------------

};

} // namespace cppOpt

#endif // OPTBOUNDARIES_H
