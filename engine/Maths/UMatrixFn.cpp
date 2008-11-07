#include "UMatrixFn.hpp"

#include <string.h>

// On suppose que la matrice est orthogonale et on prend donc la transposée...
void UMatrixFn::MatrixInvRotate(GLfloat mtxin[4][4], GLfloat mtxout[4][4])
{
  for (int i=0 ; i<3; i++) {
    for (int j=0; j<3; j++) {
      mtxout[j][i] = mtxin[i][j];
    }
  }
}

void UMatrixFn::VectorRotate (const GLfloat vin[3], const GLfloat mtx[4][4], GLfloat vout[3])
{
  vout[0] = vin[0]*mtx[0][0] + vin[1]*mtx[1][0] + vin[2]*mtx[2][0];
  vout[1] = vin[0]*mtx[0][1] + vin[1]*mtx[1][1] + vin[2]*mtx[2][1];
  vout[2] = vin[0]*mtx[0][2] + vin[1]*mtx[1][2] + vin[2]*mtx[2][2];
}

void UMatrixFn::InvertMatrix(GLfloat m[4][4])
{
  GLfloat mtx[4][4];
//
//  for(int i=0; i<4; i++){
//    for(int j=0; j<4; j++)
//      cerr << m[j][i] << " ";
//    cerr << endl;
//  }

  mempcpy(mtx,m,16*sizeof(GLfloat));

  MatrixInvRotate(mtx, m);
  GLfloat vTmp[3], vTmp2[3];

  vTmp[0] = -mtx[3][0];
  vTmp[1] = -mtx[3][1];
  vTmp[2] = -mtx[3][2];

  VectorRotate(vTmp, m, vTmp2);
  m[3][0] = vTmp2[0];
  m[3][1] = vTmp2[1];
  m[3][2] = vTmp2[2];

  m[0][3] = m[1][3] = m[2][3] = 0.0f;
  m[3][3] = 1.0f;
//
//  for(int i=0; i<4; i++){
//    for(int j=0; j<4; j++)
//      cerr << m[j][i] << " ";
//    cerr << endl;
//  }
}
