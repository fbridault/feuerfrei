#include "CPoint.hpp"

#include "CVector.hpp"

float CPoint::getTriangleArea(const CPoint &P1,const CPoint &P2,const CPoint &P3)
{
  CPoint Ph=P1;
  CVector V1, V2, VF;
  float nv1c, ps, base, hauteur;
  V1=P2-P1;
  nv1c=V1.sqrNorm();
  if(nv1c<EPSILON) return 0.0;
  base = sqrt(nv1c);
  V2=P3-P1;
  ps=V1*V2;
  ps = ps/nv1c;
  Ph += V1*ps;
  VF=P3-Ph;
  hauteur=VF.norm();
  return (base*hauteur/2.0);
}


void CPoint::setRandomPointInTriangle(const CPoint &P1, const CPoint &P2, const CPoint &P3)
{
  float R1=rand()/(float)RAND_MAX;
  float R2=rand()/(float)RAND_MAX;
  R1=sqrt(R1);
  float a=1-R1;
  float b=(1-R2)*R1;
  float c=R2*R1;
  x=a*P1.x+b*P2.x+c*P3.x;
  y=a*P1.y+b*P2.y+c*P3.y;
  z=a*P1.z+b*P2.z+c*P3.z;
}
