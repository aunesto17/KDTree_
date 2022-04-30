/*  
    - Curso : Estructuras de Datos Avanzadas - Laboratorio
    - Alumno: Alexander Baylon
    - KD Tree: soporte de dimensiones y templates, insert, search, delete
      NN, K-NN, Range Query
*/

#include <chrono>
#include <algorithm>
#include "KDTree.hpp"
#include "helper.h"

int main () {
    // N-dimension, Sample Size
    cout << "This program will use 100 dimension and 1000 points for testing." << endl << 
    "ElemType will be integers" << endl;

    RNG rng_;
    chrono::time_point<chrono::steady_clock> start, end;

    // gen points data
    vector<vector<double>> dataPoints;
    rng_.genData_double(dataPoints, 100, 1000);
    // gen key data
    vector<int> dataKeys;
    rng_.genData_int(dataKeys, 1000);
    // pre-sort data points by first dimension
    sort(dataPoints.begin(),dataPoints.end(),
        [](const vector<double> &a, const vector<double> &b){
            return a[0] < b[0];
        });
    
    // build KD Tree
    KDTree<100, int> kd;
    Point<100> median_point;
    for (size_t i = 0; i < 100; i++)
    {
        median_point[i] = dataPoints[500][i];
    }
    // insert median element of data as root
    kd.insert(median_point, dataKeys[500]);

    // insert the rest of the sorted data
    start = chrono::steady_clock::now();
    kd.build(dataPoints, dataKeys);
    end = chrono::steady_clock::now();
    double duration = chrono::duration<double> (end-start).count();
    cout << "build time(1000 points): " << duration << endl;

    // save 5 random points from input/generated
    vector<Point<100>> pointVec;
    Point<100> tempPoint;
    for (int i = 0; i < 5; i++)
    {
        int rnd = rng_.getRandom_int() % 1000;
        for (int j = 0; j < 100; j++)
        {
            tempPoint[j] = dataPoints[rnd][j];
        }
        pointVec.push_back(tempPoint);
    }

    // test nearest neighbor
    vector<int> nn_values;
    for (int i = 0; i < 5; i++)
    {
        nn_values.push_back(kd.nn_value(pointVec[i]));
    }
    cout << "nearest neighbors of saved points:" << endl;
    for (size_t val: nn_values) {
        cout << val << endl;
    }

    // test k-nearest neighbors
    // k = 5
    vector<int> knnValues;
    int k = 5;
    for (int i = 0; i < 5; i++)
    {
        knnValues = kd.knn_query(pointVec[i], k);
        cout << "knn for point[" << i << "]: ";
        for (int j = 0; j < k; j++)
        {
            cout << knnValues[j] << endl;
        }
    }

    // test range query
    vector<int> rangeQueryValues;
    Point<100> rangePoint;
    for (size_t i = 0; i < 100; i++)
    {
        rangePoint[i] = rng_.getRandom_double(100);
    }
    double queryRange = rng_.getRandom_double(2000);
    rangeQueryValues = kd.range_query(rangePoint, queryRange);
    cout << "value of nodes inside point range(" << queryRange << "):" << endl; 
    for (int i = 0; i < rangeQueryValues.size(); i++)
    {
        cout << rangeQueryValues[i] << endl;
    }
    
    return 0;
}
