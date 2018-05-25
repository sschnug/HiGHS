#ifndef SCIPYINTF_HPP
#define SCIPYINTF_HPP

#include "HModel.h"
#include "HPresolve.h"
#include "HDual.h"
#include "HApp.h"

double presolve_mod(HModel &mod, double &time, bool verbose);

void solve_fromArrays_mod(int *probStatus, int *basisStatus,
                          const int XnumCol, const int XnumRow, const int XnumNz,
                          const int XobjSense, const int XobjOffset,
                          const double *XcolCost, const double *XcolLower, const double *XcolUpper,
                          const double *XrowLower, const double *XrowUpper,
                          const int *XAstart, const int *XAindex, const double *XAvalue,
                          double *colPrimalValues, double *colDualValues,
                          double *rowPrimalValues, double *rowDualValues,
                          int *basicVariables,
                          const bool preSolve, const bool verbose);

void solveLP(int *probStatus, int *basisStatus,
             const int XnumCol, const int XnumRow, const int XnumNz,
             const int XobjSense, const int XobjOffset,
             const double *XcolCost, double *XcolLower, double *XcolUpper,
             double *XrowLower, double *XrowUpper,
             const int *XAstart, const int *XAindex, const double *XAvalue,
             double *colPrimalValues, double *colDualValues,
             double *rowPrimalValues, double *rowDualValues,
             int *basicVariables,
             const std::vector<int> XcolLowerInf,
             const std::vector<int> XcolUpperInf,
             const std::vector<int> XrowLowerInf,
             const std::vector<int> XrowUpperInf,
             const bool preSolve, const bool verbose);

#endif
