#include "HAPI.h"
#include "HApp.h"
#include "HDual.h"

void solve_fromArrays_dense(int *probStatus, int *basisStatus,
                            const int XnumCol, const int XnumRow,
                            const int XobjSense, const int XobjOffset,
                            const double *XcolCost, const double *XcolLower, const double *XcolUpper,
                            const double *XrowLower, const double *XrowUpper,
                            const double *XAmatrix,
                            double *colPrimalValues, double *colDualValues,
                            double *rowPrimalValues, double *rowDualValues,
                            int *basicVariables)
{
  double *XAvalue;
  int *XAstart;
  int *XAindex;
  int XnumNz = 0;
  for (int c_n = 0; c_n < XnumCol; c_n++)
  {
    for (int r_n = 0; r_n < XnumRow; r_n++)
    {
      double r_v = XAmatrix[r_n + c_n * XnumRow];
      if (r_v != 0)
        XnumNz++;
    }
  }
  XAvalue = (double *)malloc(sizeof(double) * XnumNz);
  XAindex = (int *)malloc(sizeof(int) * XnumNz);
  XAstart = (int *)malloc(sizeof(int) * (XnumCol + 1));
  XAstart[0] = 0;
  for (int c_n = 0; c_n < XnumCol; c_n++)
  {
    int el_n = XAstart[c_n];
    for (int r_n = 0; r_n < XnumRow; r_n++)
    {
      double r_v = XAmatrix[r_n + c_n * XnumRow];
      if (r_v != 0)
      {
        XAindex[el_n] = r_n;
        XAvalue[el_n] = r_v;
        el_n++;
      }
    }
    XAstart[c_n + 1] = el_n;
  }
  solve_fromArrays(probStatus, basisStatus,
                   XnumCol, XnumRow, XnumNz,
                   XobjSense, XobjOffset,
                   XcolCost, XcolLower, XcolUpper,
                   XrowLower, XrowUpper,
                   XAstart, XAindex, XAvalue,
                   colPrimalValues, colDualValues,
                   rowPrimalValues, rowDualValues,
                   basicVariables);
}

void solve_fromArrays(int *probStatus, int *basisStatus,
                      const int XnumCol, const int XnumRow, const int XnumNz,
                      const int XobjSense, const int XobjOffset,
                      const double *XcolCost, const double *XcolLower, const double *XcolUpper,
                      const double *XrowLower, const double *XrowUpper,
                      const int *XAstart, const int *XAindex, const double *XAvalue,
                      double *colPrimalValues, double *colDualValues,
                      double *rowPrimalValues, double *rowDualValues,
                      int *basicVariables)
{
  HModel model;
  model.load_fromArrays(XnumCol, XobjSense, XcolCost, XcolLower, XcolUpper,
                        XnumRow, XrowLower, XrowUpper,
                        XnumNz, XAstart, XAindex, XAvalue);
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
  model.util_reportSolverOutcome("Solve plain API");

  //  model.util_reportModel();
  //  model.util_reportModelSolution();

  //  printf("model.problemStatus = %d\n", model.problemStatus);
  (*probStatus) = model.problemStatus;
  (*basisStatus) = LcBasisStatus;
  // Remove any current model
  model.clearModel();
  //  printf("solve_fromArrays: probStatus = %d\n", (*probStatus));
  return;
}
