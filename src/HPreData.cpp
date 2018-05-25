/*
 * HPreData.cpp
 *
 *  Created on: 8 Jan 2017
 *      Author: ivet
 */

#include "HPreData.h"

HPreData::HPreData() {

}


double HPreData::getRowValue(int i) {
	double sum=0;
	for (int k = ARstart[i]; k < ARstart[i+1]; k++)
	 if (flagRow[ARindex[k]])
		 sum+=ARvalue[k]*valuePrimal[ARindex[k]];
	return sum;
}


void HPreData::printSolution() {
	char buff [10];
    cout<<endl<<"Col value: ";
    for (int i=0;i<numColOriginal;i++) {
    	sprintf(buff, "%2.2f ", valuePrimal[i]);
    	cout<<setw(5)<<buff;
    	if ((i%30) == 0)
    		cout<<std::flush;
    	}

    cout<<endl<<endl;
}

double HPreData::getaij(int i, int j) {
	int k=ARstart[i];
    while (j != ARindex[k] && k<=ARstart[i+1])
            k++;
    if (k==ARstart[i+1]) {
    	//cout<<"Error: No such element in A: row "<<i<<", column "<<j<<endl;
    	//exit(0);
    	}
    return ARvalue[k];
}

bool HPreData::isZeroA(int i, int j) {
	int k=ARstart[i];
    while (j != ARindex[k] && k<ARstart[i+1])
            k++;
    if (k==ARstart[i+1]) {
    	return true;
    	}
    return false;
}


void HPreData::makeARCopy() {
	// Make a AR copy
    int i,k;
    vector<int> iwork(numRow, 0);
    ARstart.resize(numRow + 1, 0);
    int AcountX = Aindex.size();
    ARindex.resize(AcountX);
    ARvalue.resize(AcountX);
    for (int k = 0; k < AcountX; k++)
        iwork.at(Aindex.at(k))++;
    for (i = 1; i <= numRow; i++)
        ARstart.at(i) = ARstart.at(i - 1) + iwork.at(i - 1);
    for (i = 0; i < numRow; i++)
        iwork.at(i) = ARstart.at(i);
    for (int iCol = 0; iCol < numCol; iCol++) {
        for (k = Astart.at(iCol); k < Astart.at(iCol + 1); k++) {
            int iRow = Aindex.at(k);
            int iPut = iwork.at(iRow)++;
            ARindex.at(iPut) = iCol;
            ARvalue.at(iPut) = Avalue[k];
        }
    }
}

void HPreData::makeACopy() {
    // Make a A copy

    int i,k;
    vector<int> iwork(numColOriginal, 0);
    Astart.assign(numColOriginal + 1, 0);
    int AcountX = ARindex.size();
    Aindex.resize(AcountX);
    Avalue.resize(AcountX);
    for (int k = 0; k < AcountX; k++)
    	if (ARindex[k] < numColOriginal)
    		iwork[ARindex[k]]++;
    for (i = 1; i <= numColOriginal; i++)
        Astart[i] = Astart[i - 1] + iwork[i - 1];
    for (i = 0; i < numColOriginal; i++)
        iwork[i] = Astart[i];
    for (int iRow = 0; iRow < numRowOriginal; iRow++) {
        for (k = ARstart[iRow]; k < ARstart[iRow + 1]; k++) {
            int iColumn = ARindex[k];
            if (iColumn != numColOriginal) {
				int iPut = iwork[iColumn]++;
				Aindex[iPut] = iRow;
				Avalue[iPut] = ARvalue[k];
            }
        }
    }

    Aend.resize(numColOriginal + 1, 0);
    for (i = 0; i < numColOriginal; i++)
        Aend[i] = Astart[i + 1];
}

void HPreData::writeNewFormat(string fileName) {

}
