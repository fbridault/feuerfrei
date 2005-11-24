#include "Hsolver.hpp"

Hsolver::Hsolver (CPoint& position, int n_x, int n_y, int n_z, double dim, double timeStep) : 
  GSsolver(position, n_x, n_y, n_z, dim, timeStep)
{
  m_uResidu = new double[m_nbVoxels];
  memset (m_uResidu, 0, m_nbVoxels * sizeof (double));
  m_uPrevResidu = new double[m_nbVoxels];
  memset (m_uPrevResidu, 0, m_nbVoxels * sizeof (double));
  m_vResidu = new double[m_nbVoxels];
  memset (m_vResidu, 0, m_nbVoxels * sizeof (double));
  m_vPrevResidu = new double[m_nbVoxels];
  memset (m_vPrevResidu, 0, m_nbVoxels * sizeof (double));
  m_wResidu = new double[m_nbVoxels];
  memset (m_wResidu, 0, m_nbVoxels * sizeof (double));
  m_wPrevResidu = new double[m_nbVoxels];
  memset (m_wPrevResidu, 0, m_nbVoxels * sizeof (double));
}

Hsolver::~Hsolver ()
{
  delete[]m_uResidu;
  delete[]m_vResidu;
  delete[]m_wResidu;
  delete[]m_uPrevResidu;
  delete[]m_vPrevResidu;
  delete[]m_wPrevResidu;
}

/* Pas de diffusion */
void Hsolver::diffuse (int b, double *const x, const double *const x0,
		      double *const residu, double *const residu0,
		      double diff_visc)
{
  double alpha;
  double num = 0.0;
  double den = 0.0;
  double diff;
  /*double num=0.0, ei=0.0, sei=0.0,  den=0.0, dtmp, etmp *//*,ntmp */ ;
  double a = m_dt * diff_visc * m_nbVoxelsX * m_nbVoxelsY * m_nbVoxelsZ;
  int i, j, k, l;

  for (l = 0; l < m_nbSteps; l++)
    {

      for (i = 1; i <= m_nbVoxelsX; i++)
	for (j = 1; j <= m_nbVoxelsY; j++)
	  for (k = 1; k <= m_nbVoxelsZ; k++)
	    {
	      x[IX (i, j, k)] =
		(x0[IX (i, j, k)] +
		 a * (x[IX (i - 1, j, k)] +
		      x[IX (i + 1, j, k)] +
		      x[IX (i, j - 1, k)] +
		      x[IX (i, j + 1, k)] +
		      x[IX (i, j, k - 1)] +
		      x[IX (i, j, k + 1)])) /
		(1.0 + 6.0 * a);
	      residu[IX (i, j, k)] =
		x0[IX (i, j, k)] -
		(x[IX (i, j, k)] -
		 a * (x[IX (i - 1, j, k)] +
		      x[IX (i + 1, j, k)] +
		      x[IX (i, j - 1, k)] +
		      x[IX (i, j + 1, k)] +
		      x[IX (i, j, k - 1)] +
		      x[IX (i, j, k + 1)] -
		      6 * x[IX (i, j, k)]));
	    }
      //set_bnd (b, x);

      /* calcul du terme alpha */
      for (i = 1; i <= m_nbVoxelsX; i++)
	for (j = 1; j <= m_nbVoxelsY; j++)
	  for (k = 1; k <= m_nbVoxelsZ; k++)
	    {
	      diff = residu0[IX (i, j, k)] -
		residu[IX (i, j, k)];
	      //cout << residu[IX(i,j,k)] << " " << residu0[IX(i,j,k)] << " " << diff << endl;
	      //      getchar();
	      num += (residu[IX (i, j, k)] * diff);
	      den += (diff * diff);
	    }	/* for alpha */

      alpha = den ? -num / den : 0.0;

      /* test alpha */
      /*if(alpha >-1.0 && alpha < 1.0) alpha = 0.0;
       */

      /*  printf("numerateur : %f %f %f  \ndenominateur : %f %f %f\n",
       * num_r, num_v, num_b, den_r, den_v, den_b); */
      //printf("alpha : %f\n", alpha);

      /* hybridation des radiosites et des residus */
      for (i = 1; i <= m_nbVoxelsX; i++)
	for (j = 1; j <= m_nbVoxelsY; j++)
	  for (k = 1; k <= m_nbVoxelsZ; k++)
	    {
	      x[IX (i, j, k)] = alpha * x[IX (i, j, k)] + (1 - alpha) * x0[IX (i, j, k)];
	      residu0[IX (i, j, k)] = alpha * residu[IX (i, j, k)] + (1 - alpha) * residu0[IX (i, j, k)];
	      // if(residu0[IX(i,j,k)])
	      //           cout << "residu0 " << residu0[IX(i,j,k)] << endl;
	    }
    }
}

void Hsolver::vel_step ()
{
  add_source (m_u, m_uSrc);
  add_source (m_v, m_vSrc);
  add_source (m_w, m_wSrc);
  SWAP (m_uPrev, m_u);
  diffuse (1, m_u, m_uPrev, m_uResidu, m_uPrevResidu, m_visc);
  SWAP (m_vPrev, m_v);
  diffuse (2, m_v, m_vPrev, m_vResidu, m_vPrevResidu, m_visc);
  SWAP (m_wPrev, m_w);
  diffuse (3, m_w, m_wPrev, m_wResidu, m_wPrevResidu, m_visc);
  project (m_uPrev, m_vPrev);
  SWAP (m_uPrev, m_u);
  SWAP (m_vPrev, m_v);
  SWAP (m_wPrev, m_w);
  advect (1, m_u, m_uPrev, m_uPrev, m_vPrev, m_wPrev);
  advect (2, m_v, m_vPrev, m_uPrev, m_vPrev, m_wPrev);
  advect (3, m_w, m_wPrev, m_uPrev, m_vPrev, m_wPrev);
  project (m_uPrev, m_vPrev);
}
