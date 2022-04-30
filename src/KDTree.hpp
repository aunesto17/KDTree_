// Copyright

#ifndef SRC_KDTREE_HPP_
#define SRC_KDTREE_HPP_

#include <cmath>
#include <iostream>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>
#include <map>
#include "Point.hpp"
#include "BoundedPQueue.h"


using namespace std;


template <size_t N, typename ElemType>
class KDTree {
public:
    // constructor
    KDTree();
    // destructor
    ~KDTree();

    void build(const vector<vector<double>> & data, const vector<ElemType> & indices);

    // returns dimension size of elements in tree
    size_t dimension() const;
    // returns current number of elements in tree
    size_t size() const;
    // return TRUE if point is "in" tree
    bool findNode(const Point<N> &pt) const;
    // find correct position of new node and inserts it in the tree
    void insert(const Point<N> &pt, const ElemType &value);

    ElemType nn_value(const Point<N> &key) const;
    vector<ElemType> range_query(const Point<N> &key, double range) const;
    vector<ElemType> knn_query(const Point<N> &key, size_t k) const;
    

private:
    size_t dimension_;
    // number of elements in tree
    size_t size_;
    // node
    struct Node {
        Point<N> key;
        ElemType val;
        size_t level;

        Node * leftNode;
        Node * rightNode;

        Node( Point<N> pt, ElemType value, size_t level, 
            Node * ln = nullptr, Node * rn = nullptr):
            key(pt), val(value), level(level), leftNode(ln), rightNode(rn){}
    };

    Node * root;

    // helper functions
    void deleteNode(Node * node);
    bool findPoint(const Point<N> &point) const;
    void range_query_helper(const Point<N> &key, BoundedPQueue<Node*> & knnPQ, Node * currentNode) const;
    void knn_value_helper(const Point<N> &key, BoundedPQueue<Node*> & knnPQ, Node * currentNode) const;
};

template <size_t N, typename ElemType>
KDTree<N, ElemType>::KDTree() {
    dimension_ = N;
    size_ = 0;
    root = nullptr;
}

template <size_t N, typename ElemType>
void KDTree<N, ElemType>::build(const vector<vector<double>> & data, const vector<ElemType> & indices) {
    Point<N> temp_point;
    for (size_t i = 0; i < 1000; i++)
    {
        for (size_t j = 0; j < N; j++)
        {
            temp_point[j] = data[i][j];
        }
        insert(temp_point,indices[i]);
    }
}

template <size_t N, typename ElemType>
KDTree<N, ElemType>::~KDTree() {
    deleteNode(root);
}

template <size_t N, typename ElemType>
size_t KDTree<N, ElemType>::dimension() const {
    return dimension_;
}

template <size_t N, typename ElemType>
size_t KDTree<N, ElemType>::size() const {
    return size_;
}

template <size_t N, typename ElemType>
void KDTree<N, ElemType>::insert(const Point<N> &pt, const ElemType &value) {
    Node * currentNode = root;
    Node * lastNode = nullptr;
    size_t level = 0;

    while (currentNode != nullptr)
    {
        level++;
        if (pt == currentNode->key)
        {
            currentNode->val = value;
            return;
        }
        size_t keyDimension = currentNode->level % N;
        if (pt[keyDimension] < currentNode->key[keyDimension])
        {
            lastNode = currentNode;
            currentNode = currentNode->leftNode;
        }
        else if (pt[keyDimension] >= currentNode->key[keyDimension])
        {
            lastNode = currentNode;
            currentNode = currentNode->rightNode;
        }
    }
    size_++;
    Node * nNode = new Node(pt, value, level);
    if (currentNode == root)
        root = nNode;
    else if (pt[lastNode->level % N] >= lastNode->key[lastNode->level % N])
        lastNode->rightNode = nNode;
    else
        lastNode->leftNode = nNode; 
}

template <size_t N, typename ElemType>
ElemType KDTree<N, ElemType>::nn_value(const Point<N> &key) const {
    BoundedPQueue<Node*> nearestPQ(1);
    knn_value_helper(key, nearestPQ, root);

    ElemType nn_val = nearestPQ.dequeueMin()->val;
    return nn_val;
}

template <size_t N, typename ElemType>
vector<ElemType> KDTree<N, ElemType>::range_query(const Point<N> &key, double range) const {
    vector<ElemType> values;
    Node * currentNode = root;
    while (currentNode != nullptr)
    {
        if (distance(currentNode->key, key) <= range){
            //cout << "dis: " << distance(currentNode->key, key) << endl;
            values.push_back(currentNode->val);
        }
        if (key[currentNode->level % N] >= currentNode->key[currentNode-> level % N])
            currentNode = currentNode->rightNode;
        else
            currentNode = currentNode->leftNode;
    }
    return values;
}

template <size_t N, typename ElemType>
vector<ElemType> KDTree<N, ElemType>::knn_query(const Point<N> &key, size_t k) const {
    BoundedPQueue<Node*> knearestPQ(k);
    knn_value_helper(key, knearestPQ, root);

    vector<ElemType> knnValues;
    while (!knearestPQ.empty())
        knnValues.push_back((knearestPQ.dequeueMin())->val);
    
    return knnValues;
}

template <size_t N, typename ElemType>
void KDTree<N, ElemType>::knn_value_helper(const Point<N> & key, BoundedPQueue<Node*> & knnPQ, Node * currentNode) const {
    if(currentNode == nullptr) return;
    knnPQ.enqueue(currentNode, distance(currentNode->key, key));
    size_t dim = currentNode->level % N;
    // turn left of the axis on current node
    if(key[dim] < currentNode->key[dim])
    {
        knn_value_helper(key, knnPQ, currentNode->leftNode);
        if(knnPQ.size() < knnPQ.maxSize() || fabs(currentNode->key[dim] < knnPQ.worst()))
            knn_value_helper(key, knnPQ, currentNode->rightNode);
    }
    else // turn right of the axis on current node
    {
        knn_value_helper(key, knnPQ, currentNode->rightNode);
        if(knnPQ.size() < knnPQ.maxSize() || fabs(currentNode->key[dim] < knnPQ.worst()))
            knn_value_helper(key, knnPQ, currentNode->leftNode);
    }
}

// helper funcs

template <size_t N, typename ElemType>
void KDTree<N, ElemType>::deleteNode(Node * node){
    if (node == nullptr) return;
    deleteNode(node->leftNode);
    deleteNode(node->rightNode);
    delete node;
}

template <size_t N, typename ElemType>
bool KDTree<N, ElemType>::findPoint(const Point<N> &point) const{
    Node * currentNode = root;
    while (currentNode != nullptr)
    {
        if (currentNode->key == point) return true;  
        if (point[currentNode->level % N] >= currentNode->key[currentNode-> level % N])
            currentNode = currentNode->rightNode;
        else
            currentNode = currentNode->leftNode;
    }
    return false;
}

#endif  // SRC_KDTREE_HPP_