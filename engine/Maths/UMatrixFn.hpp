#ifndef UMATRIXFN_H
#define UMATRIXFN_H

#include "../Common.hpp"

class UMatrixFn
{
private:
  UMatrixFn() {};
public:
  // On suppose que la matrice est orthogonale et on prend donc la transposée...
  static void MatrixInvRotate(GLfloat mtxin[4][4], GLfloat mtxout[4][4]);
  static void VectorRotate (const GLfloat vin[3], const GLfloat mtx[4][4], GLfloat vout[3]);
  static void InvertMatrix(GLfloat m[4][4]);
};

#endif // UMATRIXFN_H
