#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <cmath>    
#include <cctype>   
#include <sstream>  
#include <limits>   
#include <cstdlib>  

const int MIN_DEGREE = 0;
const int MAX_DEGREE = 9;
const double EPSILON = 1e-9;

struct Monomial {
    double coeff;
    int px, py, pz; 

    
};

class Polynomial {
private:
    std::vector<Monomial> terms; 
    


public:
    Polynomial() {}

    
};




