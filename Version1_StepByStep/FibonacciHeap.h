#ifndef FIBONACCI_HEAP_H
#define FIBONACCI_HEAP_H

#include <iostream>
#include <vector>
#include <cmath>
#include <memory>
#include <unordered_map>

// Patient structure to store in the heap
struct Patient {
    int id;
    std::string name;
    int age;
    std::string gender;
    std::string department;
    double priorityScore;  // From ML model (0-100)
    std::string symptoms;
    std::string arrivalTime;

    // Medical parameters for ML model
    int chestPain;
    int diabetes;
    int bloodPressure;
    int cholesterol;
    double bmi;
    int maxHeartRate;
    int hypertension;

    Patient() : id(0), age(0), priorityScore(0.0), chestPain(0), diabetes(0),
                bloodPressure(0), cholesterol(0), bmi(0.0), maxHeartRate(0), hypertension(0) {}

    void display() const {
        std::cout << "ID: " << id << " | " << name << " | Age: " << age
                  << " | Priority: " << priorityScore << " | Dept: " << department << std::endl;
    }
};

// Fibonacci Heap Node
template <typename T>
class FibNode {
public:
    T data;
    int degree;
    bool marked;
    FibNode<T>* parent;
    FibNode<T>* child;
    FibNode<T>* left;
    FibNode<T>* right;
    double key;  // Priority key

    FibNode(T value, double priority)
        : data(value), degree(0), marked(false), parent(nullptr),
          child(nullptr), key(priority) {
        left = right = this;
    }
};

// Fibonacci Heap (MAX heap for hospital triage - highest priority first)
template <typename T>
class FibonacciHeap {
private:
    FibNode<T>* maxNode;
    int nodeCount;
    std::unordered_map<int, FibNode<T>*> nodeMap;  // For quick patient lookup

    void link(FibNode<T>* y, FibNode<T>* x) {
        // Remove y from root list
        y->left->right = y->right;
        y->right->left = y->left;

        // Make y a child of x
        y->parent = x;
        if (x->child == nullptr) {
            x->child = y;
            y->right = y;
            y->left = y;
        } else {
            y->left = x->child;
            y->right = x->child->right;
            x->child->right->left = y;
            x->child->right = y;
        }
        x->degree++;
        y->marked = false;
    }

    void consolidate() {
        int maxDegree = static_cast<int>(log2(nodeCount)) + 1;
        std::vector<FibNode<T>*> degreeTable(maxDegree + 1, nullptr);

        // Create a list of all root nodes
        std::vector<FibNode<T>*> rootList;
        FibNode<T>* current = maxNode;
        if (current != nullptr) {
            do {
                rootList.push_back(current);
                current = current->right;
            } while (current != maxNode);
        }

        // Consolidate
        for (FibNode<T>* w : rootList) {
            FibNode<T>* x = w;
            int d = x->degree;

            while (degreeTable[d] != nullptr) {
                FibNode<T>* y = degreeTable[d];
                if (x->key < y->key) {
                    std::swap(x, y);
                }
                link(y, x);
                degreeTable[d] = nullptr;
                d++;
            }
            degreeTable[d] = x;
        }

        // Rebuild root list
        maxNode = nullptr;
        for (FibNode<T>* node : degreeTable) {
            if (node != nullptr) {
                if (maxNode == nullptr) {
                    maxNode = node;
                    node->left = node->right = node;
                } else {
                    node->left = maxNode;
                    node->right = maxNode->right;
                    maxNode->right->left = node;
                    maxNode->right = node;
                    if (node->key > maxNode->key) {
                        maxNode = node;
                    }
                }
            }
        }
    }

    void cut(FibNode<T>* x, FibNode<T>* y) {
        // Remove x from child list of y
        if (x->right == x) {
            y->child = nullptr;
        } else {
            x->left->right = x->right;
            x->right->left = x->left;
            if (y->child == x) {
                y->child = x->right;
            }
        }
        y->degree--;

        // Add x to root list
        x->left = maxNode;
        x->right = maxNode->right;
        maxNode->right->left = x;
        maxNode->right = x;

        x->parent = nullptr;
        x->marked = false;
    }

    void cascadingCut(FibNode<T>* y) {
        FibNode<T>* z = y->parent;
        if (z != nullptr) {
            if (!y->marked) {
                y->marked = true;
            } else {
                cut(y, z);
                cascadingCut(z);
            }
        }
    }

    void destroyHeap(FibNode<T>* node) {
        if (node != nullptr) {
            FibNode<T>* current = node;
            do {
                FibNode<T>* next = current->right;
                destroyHeap(current->child);
                delete current;
                current = next;
            } while (current != node);
        }
    }

public:
    FibonacciHeap() : maxNode(nullptr), nodeCount(0) {}

    ~FibonacciHeap() {
        destroyHeap(maxNode);
    }

    bool isEmpty() const {
        return maxNode == nullptr;
    }

    int size() const {
        return nodeCount;
    }

    // Insert a new node with given data and priority (O(1))
    void insert(T data, double priority, int patientId = -1) {
        FibNode<T>* newNode = new FibNode<T>(data, priority);

        if (maxNode == nullptr) {
            maxNode = newNode;
        } else {
            // Add to root list
            newNode->left = maxNode;
            newNode->right = maxNode->right;
            maxNode->right->left = newNode;
            maxNode->right = newNode;

            if (priority > maxNode->key) {
                maxNode = newNode;
            }
        }

        if (patientId != -1) {
            nodeMap[patientId] = newNode;
        }

        nodeCount++;
    }

    // Get maximum element (O(1))
    T getMax() const {
        if (maxNode == nullptr) {
            throw std::runtime_error("Heap is empty");
        }
        return maxNode->data;
    }

    double getMaxPriority() const {
        if (maxNode == nullptr) {
            throw std::runtime_error("Heap is empty");
        }
        return maxNode->key;
    }

    // Extract maximum element (O(log n) amortized)
    T extractMax() {
        if (maxNode == nullptr) {
            throw std::runtime_error("Heap is empty");
        }

        FibNode<T>* z = maxNode;
        T maxData = z->data;

        // Add all children to root list
        if (z->child != nullptr) {
            FibNode<T>* child = z->child;
            do {
                FibNode<T>* nextChild = child->right;
                child->left = maxNode;
                child->right = maxNode->right;
                maxNode->right->left = child;
                maxNode->right = child;
                child->parent = nullptr;
                child = nextChild;
            } while (child != z->child);
        }

        // Remove z from root list
        z->left->right = z->right;
        z->right->left = z->left;

        if (z == z->right) {
            maxNode = nullptr;
        } else {
            maxNode = z->right;
            consolidate();
        }

        nodeCount--;
        delete z;

        return maxData;
    }

    // Increase key (for MAX heap) - O(1) amortized
    void increaseKey(FibNode<T>* x, double newKey) {
        if (newKey < x->key) {
            throw std::runtime_error("New key is smaller than current key");
        }

        x->key = newKey;
        FibNode<T>* y = x->parent;

        if (y != nullptr && x->key > y->key) {
            cut(x, y);
            cascadingCut(y);
        }

        if (x->key > maxNode->key) {
            maxNode = x;
        }
    }

    // Get all elements (for display)
    void getAllElements(std::vector<T>& elements, std::vector<double>& priorities) const {
        if (maxNode == nullptr) return;

        std::vector<FibNode<T>*> nodes;
        collectNodes(maxNode, nodes);

        for (FibNode<T>* node : nodes) {
            elements.push_back(node->data);
            priorities.push_back(node->key);
        }
    }

    void collectNodes(FibNode<T>* node, std::vector<FibNode<T>*>& nodes) const {
        if (node == nullptr) return;

        FibNode<T>* current = node;
        do {
            nodes.push_back(current);
            collectNodes(current->child, nodes);
            current = current->right;
        } while (current != node);
    }

    // Display heap structure (for debugging)
    void display() const {
        if (maxNode == nullptr) {
            std::cout << "Heap is empty" << std::endl;
            return;
        }

        std::vector<T> elements;
        std::vector<double> priorities;
        getAllElements(elements, priorities);

        std::cout << "\n=== Fibonacci Heap Contents (" << nodeCount << " patients) ===" << std::endl;
        for (size_t i = 0; i < elements.size(); i++) {
            std::cout << "Priority: " << priorities[i] << " - ";
            elements[i].display();
        }
    }
};

#endif // FIBONACCI_HEAP_H
