#include <iostream>   //  TODO
#include <vector>
#include <cstddef>
#include "SCIPY_INTF.h"

/* Modified function from "HApp.cpp"
   - controlled verbosity
*/

double presolve_mod(HModel &mod, double &time, bool verbose)
{
  HPresolve *pre = new HPresolve();
  mod.copy_fromHModelToHPresolve(pre);
  int status = pre->presolve();
  if (status == HPresolve::Unset)
  {
    //pre->reportTimes();
    mod.load_fromPresolve(pre);

    HDual solver;
    mod.scaleModel();
    solver.solve(&mod);
    pre->setProblemStatus(mod.getPrStatus());
    mod.util_getPrimalDualValues(pre->colValue, pre->colDual, pre->rowValue, pre->rowDual);
    mod.util_getBasicIndexNonbasicFlag(pre->basicIndex, pre->nonbasicFlag);
    pre->postsolve();
    mod.load_fromPostsolve(pre);
    solver.solve(&mod);
    if(verbose)
      mod.util_reportSolverOutcome("Postsolve");
    time = mod.totalTime;
  }
  else if (status == HPresolve::Empty)
  {
    pre->postsolve();
    mod.load_fromPostsolve(pre);
    HDual solver;

    solver.solve(&mod);
    if(verbose)
      mod.util_reportSolverOutcome("Postsolve");
    time = mod.totalTime;
  }
  else
  {
    if(verbose)
    {
      std::cout << "Presolve detected problem status: ";
      if (status == HPresolve::Infeasible)
        std::cout << "Infeasible" << std::endl;
      else if (status == HPresolve::Unbounded)
        std::cout << "Unbounded" << std::endl;
      else
        std::cout << "Unknown, status=" << status << std::endl;
    }
    return 0;
  }

  return mod.util_getObjectiveValue();
  delete pre;
}

/* Modified function from "HAPI.cpp"
  - preSolve parameter
   - controlled verbosity
*/
void solve_fromArrays_mod(int *probStatus, int *basisStatus,
                          const int XnumCol, const int XnumRow, const int XnumNz,
                          const int XobjSense, const int XobjOffset,
                          const double *XcolCost, const double *XcolLower, const double *XcolUpper,
                          const double *XrowLower, const double *XrowUpper,
                          const int *XAstart, const int *XAindex, const double *XAvalue,
                          double *colPrimalValues, double *colDualValues,
                          double *rowPrimalValues, double *rowDualValues,
                          int *basicVariables,
                          const bool preSolve, const bool verbose)
{
  HModel model;

  if(verbose)
    model.intOption[INTOPT_PRINT_FLAG] = 1;

  model.load_fromArrays(XnumCol, XobjSense, XcolCost, XcolLower, XcolUpper,
                        XnumRow, XrowLower, XrowUpper,
                        XnumNz, XAstart, XAindex, XAvalue);

  if(preSolve)
  {
    double time1;
    presolve_mod(model, time1, verbose);
    return;
  }
  else
  {
    model.scaleModel();

    int LcBasisStatus = (*basisStatus);
    //printf("solve_fromArrays: LcBasisStatus = %d\n", LcBasisStatus);fflush(stdout);
    if (LcBasisStatus)
    {
      //    printf("Basis status is %d\n", LcBasisStatus);
      model.replaceWithNewBasis(basicVariables);
      //    printf("Number of basic logicals is %d\n", model.numBasicLogicals);
    }

    HDual solver;
    solver.solve(&model);

    vector<double> XcolPrimalValues;
    vector<double> XcolDualValues;
    vector<double> XrowPrimalValues;
    vector<double> XrowDualValues;
    vector<double> XbasicVariables;

    model.util_getPrimalDualValues(XcolPrimalValues, XcolDualValues, XrowPrimalValues, XrowDualValues);

    memcpy(colPrimalValues, &(XcolPrimalValues[0]), sizeof(double) * model.numCol);
    memcpy(rowPrimalValues, &(XrowPrimalValues[0]), sizeof(double) * model.numRow);
    memcpy(colDualValues, &(XcolDualValues[0]), sizeof(double) * model.numCol);
    memcpy(rowDualValues, &(XrowDualValues[0]), sizeof(double) * model.numRow);
    memcpy(basicVariables, &(model.basicIndex[0]), sizeof(int) * model.numRow);
    LcBasisStatus = HiGHS_basisStatus_yes;
    if(verbose)
      model.util_reportSolverOutcome("Solve plain API");

    (*probStatus) = model.problemStatus;
    (*basisStatus) = LcBasisStatus;
    // Remove any current model
    model.clearModel();
    //  printf("solve_fromArrays: probStatus = %d\n", (*probStatus));
    return;
  }
}

// Function to call from within scipy

/* Basically the same arguments as solve_fromArrays, but:
     - no portable NaN-check before C++11
       -> extra-args needed to mark NaN's
     - preSolve param
     - verbose param

*/

void solveLP(int *probStatus, int *basisStatus,
             const int XnumCol, const int XnumRow, const int XnumNz,
             const int XobjSense, const int XobjOffset,
             const double *XcolCost, double *XcolLower, double *XcolUpper,
             double *XrowLower, double *XrowUpper,
             const int *XAstart, const int *XAindex, const double *XAvalue,
             double *colPrimalValues, double *colDualValues,
             double *rowPrimalValues, double *rowDualValues,
             int *basicVariables,
             // extra-args
             const std::vector<int> XcolLowerInf,
             const std::vector<int> XcolUpperInf,
             const std::vector<int> XrowLowerInf,
             const std::vector<int> XrowUpperInf,
             const bool preSolve, const bool verbose
            )
{

  // // DEBUG
  // for(int i=0; i<XnumCol; ++i)
  //   std::cout << XcolLower[i] << ",";
  // std::cout << std::endl;
  //
  // for(int i=0; i<XnumCol; ++i)
  //   std::cout << XcolUpper[i] << ",";
  // std::cout << std::endl;
  //
  // for(int i=0; i<XnumRow; ++i)
  //   std::cout << XrowLower[i] << ",";
  // std::cout << std::endl;
  //
  // for(int i=0; i<XnumRow; ++i)
  //   std::cout << XrowUpper[i] << ",";
  // std::cout << std::endl;

  // Replace non-finite values with project-specific constants
  for(std::size_t i=0; i<XcolLowerInf.size(); ++i)
    XcolLower[XcolLowerInf[i]] = -HSOL_CONST_INF;
  for(std::size_t i=0; i<XcolUpperInf.size(); ++i)
    XcolUpper[XcolUpperInf[i]] = HSOL_CONST_INF;
  for(std::size_t i=0; i<XrowLowerInf.size(); ++i)
    XrowLower[XrowLowerInf[i]] = -HSOL_CONST_INF;
  for(std::size_t i=0; i<XrowUpperInf.size(); ++i)
    XrowUpper[XrowUpperInf[i]] = HSOL_CONST_INF;

  // std::cout << "AFTER" << std::endl;
  //
  // for(int i=0; i<XnumCol; ++i)
  //   std::cout << XcolLower[i] << ",";
  // std::cout << std::endl;
  //
  // for(int i=0; i<XnumCol; ++i)
  //   std::cout << XcolUpper[i] << ",";
  // std::cout << std::endl;
  //
  // for(int i=0; i<XnumRow; ++i)
  //   std::cout << XrowLower[i] << ",";
  // std::cout << std::endl;
  //
  // for(int i=0; i<XnumRow; ++i)
  //   std::cout << XrowUpper[i] << ",";
  // std::cout << std::endl;

  solve_fromArrays_mod(probStatus, basisStatus,
                       XnumCol, XnumRow, XnumNz,
                       XobjSense,  XobjOffset,
                       XcolCost, XcolLower, XcolUpper,
                       XrowLower, XrowUpper,
                       XAstart, XAindex, XAvalue,
                       colPrimalValues, colDualValues,
                       rowPrimalValues, rowDualValues,
                       basicVariables,
                       preSolve, verbose);
}
