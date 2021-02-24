#include "MathFunctions.h"
#include <stdio.h>
#include <Table.h>
#include <iostream>

// a hack square root calculation using simple operations
double mysqrt(double x) {
    if (x <= 0) {
        return 0;
    }

    double result;
    double delta;
    result = x;

    // use the table to help find an initial value
    if (x >= 1 && x < 10) {
        std::cout << "Use the table to help find an initial value " << std::endl;
        result = sqrtTable[static_cast<int>(x)];
    }

    // do ten iterations
    for (int i = 0; i < 10; ++i) {
        if (result <= 0) {
            result = 0.1;
        }
        delta = x - (result * result);
        result = result + 0.5 * delta / result;
        fprintf(stdout, "Computing sqrt of %g to be %g\n", x, result);
    }
    return result;
}
