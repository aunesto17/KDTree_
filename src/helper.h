#ifndef HELPER_H_
#define HELPER_H_

#include <iostream>
#include <random>

using namespace std;

class RNG
{
public:
    RNG() : gen(random_device()()) {} // Seeds the mt19937.
    ~RNG(){};

    double getRandom_double(double l){  
        uniform_real_distribution<double> dis(0, l);
        double random = dis(gen);
        return random;
    }

    int getRandom_int(){  
        return i_dis(gen);
    }

    void genData_double(vector<vector<double>> & data, size_t N, size_t S)
    {
        for (size_t i = 0; i < S; ++i) {
            vector<double> tmp;
            for (int j = 0; j < N; ++j)
            {
                tmp.push_back(getRandom_double(100));
            }
            data.push_back(tmp);
        }
    }

    void genData_int(vector<int> & data, size_t S)
    {
        for (size_t i = 0; i < S; ++i) {
            data.push_back(getRandom_int());
        }
    }
private:
    mt19937 gen;
    uniform_int_distribution<> i_dis;
};

#endif