#include "benchsolver.hpp"

#include <math.h>

extern int done;

BenchSolver::BenchSolver (int n_x, int n_y, int n_z, double dim, double pas_de_temps): 
  Solver (n_x, n_y, n_z, dim, pas_de_temps)
{
  m_uSave = new double[m_nbVoxels];
  m_vSave = new double[m_nbVoxels];
  m_wSave = new double[m_nbVoxels];
  m_uPrevSave = new double[m_nbVoxels];
  m_vPrevSave = new double[m_nbVoxels];
  m_wPrevSave = new double[m_nbVoxels];
  m_densSave = new double[m_nbVoxels];
  m_densPrevSave = new double[m_nbVoxels];
  m_densSrcSave = new double[m_nbVoxels];
  m_uSrcSave = new double[m_nbVoxels];
  m_vSrcSave = new double[m_nbVoxels];
  m_wSrcSave = new double[m_nbVoxels];
  m_rowSize = m_nbVoxelsX * m_nbVoxelsY * m_nbVoxelsZ;
  m_rowSave = new double[m_rowSize];
	
  m_uSave = (double *) memset (m_uSave, 0, m_nbVoxels * sizeof (double));
  m_vSave = (double *) memset (m_vSave, 0, m_nbVoxels * sizeof (double));
  m_wSave = (double *) memset (m_wSave, 0, m_nbVoxels * sizeof (double));
  m_uPrevSave = (double *) memset (m_uPrevSave, 0, m_nbVoxels * sizeof (double));
  m_vPrevSave = (double *) memset (m_vPrevSave, 0, m_nbVoxels * sizeof (double));
  m_wPrevSave = (double *) memset (m_wPrevSave, 0, m_nbVoxels * sizeof (double));
  m_densSave = (double *) memset (m_densSave, 0, m_nbVoxels * sizeof (double));
  m_densPrevSave = (double *) memset (m_densPrevSave, 0, m_nbVoxels * sizeof (double));
  m_densSrcSave = (double *) memset (m_densSrcSave, 0, m_nbVoxels * sizeof (double));
  m_uSrcSave = (double *) memset (m_uSrcSave, 0, m_nbVoxels * sizeof (double));
  m_vSrcSave = (double *) memset (m_vSrcSave, 0, m_nbVoxels * sizeof (double));
  m_wSrcSave = (double *) memset (m_wSrcSave, 0, m_nbVoxels * sizeof (double));

  m_uRef = new double[m_nbVoxels];
  m_vRef = new double[m_nbVoxels];
  m_wRef = new double[m_nbVoxels];
  m_pRef = new double[m_nbVoxels];
  m_densRef = new double[m_nbVoxels];
  m_pSave = new double[m_nbVoxels];

  m_uLog = new double[2 * m_refVal];
  m_vLog = new double[2 * m_refVal];
  m_wLog = new double[2 * m_refVal];
  m_pLog = new double[2 * m_refVal];

  //m_file.open ("solver.log", ios::out | ios::trunc);
}

BenchSolver::~BenchSolver ()
{
  //m_file.close ();

  delete[]m_uSave;
  delete[]m_vSave;
  delete[]m_wSave;
  delete[]m_uPrevSave;
  delete[]m_vPrevSave;
  delete[]m_wPrevSave;
  delete[]m_densSave;
  delete[]m_densPrevSave;
  delete[]m_uSrcSave;
  delete[]m_vSrcSave;
  delete[]m_wSrcSave;
  delete[]m_densSrcSave;
  delete[]m_pSave;
  delete[]m_rowSave;
  
  delete[]m_uRef;
  delete[]m_vRef;
  delete[]m_wRef;
  delete[]m_densRef;
  delete[]m_pRef;

  delete[]m_uLog;
  delete[]m_vLog;
  delete[]m_wLog;
  delete[]m_pLog;
}

/* Pas de diffusion */
void
BenchSolver::diffuse_log (int b, double *const x, const double *const x0,
			  const double *const xref, double *const xlog,
			  double a, double diff_visc)
{
  int i, j, k, l;

  for (l = 0; l < m_nbStepsGS; l++)
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
	    }
      set_bnd (b, x);
      compare (l, x, xlog, xref);
    }
}

/* Pas de diffusion */
void
BenchSolver::diffuse_hybride_log (int b, double *const x, const double *const x0,
				  const double *const xref, double *const residu, 
				  double *const residu0, double *const xlog, 
				  double a, double diff_visc)
{
  double alpha;
  double num = 0.0;
  double den = 0.0;
  double diffe;
  int i, j, k, l;

  for (l = 0; l < m_nbStepsGS; l++)
    {

      for (i = 1; i <= m_nbVoxelsX; i++)
	for (j = 1; j <= m_nbVoxelsY; j++)
	  for (k = 1; k <= m_nbVoxelsZ; k++)
	    {
	      x[IX (i, j, k)] = (x0[IX (i, j, k)] +
		 a * (x[IX (i - 1, j, k)] + x[IX (i + 1, j, k)] +
		      x[IX (i, j - 1, k)] + x[IX (i, j + 1, k)] +
		      x[IX (i, j, k - 1)] + x[IX (i, j, k + 1)])) / (1.0 + 6.0 * a);
	      residu[IX (i, j, k)] = x0[IX (i, j, k)] -
		(x[IX (i, j, k)] - a * (x[IX (i - 1, j, k)] + x[IX (i + 1, j, k)] +
					x[IX (i, j - 1, k)] + x[IX (i, j + 1, k)] +
					x[IX (i, j, k - 1)] + x[IX (i, j, k + 1)] - 6 * x[IX (i, j, k)]));
	    }

      /* calcul du terme alpha */
      for (i = 1; i <= m_nbVoxelsX; i++)
	for (j = 1; j <= m_nbVoxelsY; j++)
	  for (k = 1; k <= m_nbVoxelsZ; k++)
	    {
	      diffe = residu[IX (i, j, k)] - residu0[IX (i, j, k)];
	      //cout << residu[IX(i,j,k)] << " " << residu0[IX(i,j,k)] << " " << diff << endl;
	      //      getchar();
	      num += (residu0[IX (i, j, k)] * diffe);
	      den += (diffe * diffe);
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
      set_bnd (b, x);
      compare (l, x, xlog, xref);
    }
}

/* Pas de projection pour garantir la conservation de la masse */
/* Les tableaux p et div sont certes modifiés; néanmoins Stam passe u0 et v0 dans vel_step */
/* qui ne servent plus après l'appel de la projection */
void
BenchSolver::project_log (double *const p, double *const div,
			  double *const xref, double *const xlog)
{
  double h_x = 1.0 / m_nbVoxelsX, h_y = 1.0 / m_nbVoxelsY, h_z = 1.0 / m_nbVoxelsZ;
  int i, j, k, l;

  for (i = 1; i <= m_nbVoxelsX; i++)
    for (j = 1; j <= m_nbVoxelsY; j++)
      for (k = 1; k <= m_nbVoxelsZ; k++)
	{
	  div[IX (i, j, k)] =
	    -0.5 * (h_x *
		    (m_u[IX (i + 1, j, k)] -
		     m_u[IX (i - 1, j, k)]) +
		    h_y * (m_v[IX (i, j + 1, k)] -
			   m_v[IX (i, j - 1, k)]) +
		    h_z * (m_w[IX (i, j, k + 1)] -
			   m_w[IX (i, j, k - 1)]));
	  p[IX (i, j, k)] = 0;
	}

  set_bnd (0, div);
  set_bnd (0, p);

  save_state (p, m_pSave);
  for (l = 0; l < m_nbStepsGS; l++)
    {
      for (i = 1; i <= m_nbVoxelsX; i++)
	for (j = 1; j <= m_nbVoxelsY; j++)
	  for (k = 1; k <= m_nbVoxelsZ; k++)
	    p[IX (i, j, k)] =
	      (div[IX (i, j, k)] +
	       p[IX (i - 1, j, k)] +
	       p[IX (i + 1, j, k)] +
	       p[IX (i, j - 1, k)] +
	       p[IX (i, j + 1, k)] +
	       p[IX (i, j, k - 1)] +
	       p[IX (i, j, k + 1)]) / 6.0;
      set_bnd (0, p);
    }
  save_ref (p, xref);
  set_previous_state (p, m_pSave);
  for (l = 0; l < m_nbStepsGS; l++)
    {
      for (i = 1; i <= m_nbVoxelsX; i++)
	for (j = 1; j <= m_nbVoxelsY; j++)
	  for (k = 1; k <= m_nbVoxelsZ; k++)
	    p[IX (i, j, k)] =
	      (div[IX (i, j, k)] +
	       p[IX (i - 1, j, k)] +
	       p[IX (i + 1, j, k)] +
	       p[IX (i, j - 1, k)] +
	       p[IX (i, j + 1, k)] +
	       p[IX (i, j, k - 1)] +
	       p[IX (i, j, k + 1)]) / 6.0;
      set_bnd (0, p);
      compare (l, p, xlog, xref);
    }

  for (i = 1; i <= m_nbVoxelsX; i++)
    for (j = 1; j <= m_nbVoxelsY; j++)
      for (k = 1; k <= m_nbVoxelsZ; k++)
	{
	  m_u[IX (i, j, k)] -=
	    0.5 * (p[IX (i + 1, j, k)] -
		   p[IX (i - 1, j, k)]) / h_x;
	  m_v[IX (i, j, k)] -=
	    0.5 * (p[IX (i, j + 1, k)] -
		   p[IX (i, j - 1, k)]) / h_y;
	  m_w[IX (i, j, k)] -=
	    0.5 * (p[IX (i, j, k + 1)] -
		   p[IX (i, j, k - 1)]) / h_z;
	}
  set_bnd (1, m_u);
  set_bnd (2, m_v);
  set_bnd (3, m_w);
}

/* Pas de projection pour garantir la conservation de la masse */
/* Les tableaux p et div sont certes modifiés; néanmoins Stam passe u0 et v0 dans vel_step */
/* qui ne servent plus après l'appel de la projection */
void
BenchSolver::project_log_hybride (double *p, double *const div,
				  double *const xref, double *const xlog,
				  double *const residu, double *const residu0)
{
  double h_x = 1.0 / m_nbVoxelsX, h_y = 1.0 / m_nbVoxelsY, h_z = 1.0 / m_nbVoxelsZ;
  int i, j, k, l;
  double diffe;
  double alpha;
  double num = 0.0;
  double den = 0.0;

  for (i = 1; i <= m_nbVoxelsX; i++)
    for (j = 1; j <= m_nbVoxelsY; j++)
      for (k = 1; k <= m_nbVoxelsZ; k++)
	{
	  div[IX (i, j, k)] =
	    -0.5 * (h_x *
		    (m_u[IX (i + 1, j, k)] -
		     m_u[IX (i - 1, j, k)]) +
		    h_y * (m_v[IX (i, j + 1, k)] -
			   m_v[IX (i, j - 1, k)]) +
		    h_z * (m_w[IX (i, j, k + 1)] -
			   m_w[IX (i, j, k - 1)]));
	  p[IX (i, j, k)] = 0;
	  m_pSave[IX (i, j, k)] = 0;
	}

  set_bnd (0, div);
  set_bnd (0, p);

  /* Résolution initiale en guise de référence */
  save_state (p, m_pSave);
  for (l = 0; l < m_nbStepsGS; l++)
    {
      for (i = 1; i <= m_nbVoxelsX; i++)
	for (j = 1; j <= m_nbVoxelsY; j++)
	  for (k = 1; k <= m_nbVoxelsZ; k++)
	    p[IX (i, j, k)] =
	      (div[IX (i, j, k)] +
	       p[IX (i - 1, j, k)] +
	       p[IX (i + 1, j, k)] +
	       p[IX (i, j - 1, k)] +
	       p[IX (i, j + 1, k)] +
	       p[IX (i, j, k - 1)] +
	       p[IX (i, j, k + 1)]) / 6.0;
      set_bnd (0, p);
    }
  save_ref (p, xref);
  set_previous_state (p, m_pSave);

  /* Initialisation des résidus */
  for (i = 1; i <= m_nbVoxelsX; i++)
    for (j = 1; j <= m_nbVoxelsY; j++)
      for (k = 1; k <= m_nbVoxelsZ; k++)
	{
	  residu0[IX (i, j, k)] =
	    div[IX (i, j, k)] +
	    (p[IX (i - 1, j, k)] +
	     p[IX (i + 1, j, k)] +
	     p[IX (i, j - 1, k)] +
	     p[IX (i, j + 1, k)] +
	     p[IX (i, j, k - 1)] +
	     p[IX (i, j, k + 1)] -
	     6 * p[IX (i, j, k)]);
	}
			
  /* Hybridation */
  for (l = 0; l < m_nbStepsGS; l++)
    {
      SWAP (m_pSave, p);
      for (i = 1; i <= m_nbVoxelsX; i++)
	for (j = 1; j <= m_nbVoxelsY; j++)
	  for (k = 1; k <= m_nbVoxelsZ; k++)
	    {
	      p[IX (i, j, k)] =
		(div[IX (i, j, k)] +
		 p[IX (i - 1, j, k)] +
		 p[IX (i + 1, j, k)] +
		 p[IX (i, j - 1, k)] +
		 p[IX (i, j + 1, k)] +
		 p[IX (i, j, k - 1)] +
		 p[IX (i, j, k + 1)]) / 6.0;
	      residu[IX (i, j, k)] =
		div[IX (i, j, k)] +
		(p[IX (i - 1, j, k)] +
		 p[IX (i + 1, j, k)] +
		 p[IX (i, j - 1, k)] +
		 p[IX (i, j + 1, k)] +
		 p[IX (i, j, k - 1)] +
		 p[IX (i, j, k + 1)] -
		 6 * p[IX (i, j, k)]);
	    }
      set_bnd (0, p);
      /* calcul du terme alpha */
      for (i = 1; i <= m_nbVoxelsX; i++)
	for (j = 1; j <= m_nbVoxelsY; j++)
	  for (k = 1; k <= m_nbVoxelsZ; k++)
	    {
	      diffe = residu[IX (i, j, k)] -
		residu0[IX (i, j, k)];
	      //cout << residu[IX(i,j,k)] << " " << residu0[IX(i,j,k)] << " " << diffe << endl;
	      //       getchar();
	      num += (residu0[IX (i, j, k)] * diffe);
	      den += (diffe * diffe);
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
	      p[IX (i, j, k)] =
		alpha * p[IX (i, j, k)] + 
		(1 - alpha) * m_pSave[IX (i, j, k)];

	      residu0[IX (i, j, k)] =
		alpha * residu[IX (i, j, k)] +
		(1 - alpha) * residu0[IX (i, j, k)];
	      // if(residu0[IX(i,j,k)])
	      //         cout << "residu0 " << residu0[IX(i,j,k)] << endl;
	    }
		
      compare (l, p, xlog, xref);
    }
  /* Fin hybridation */
	
  for (i = 1; i <= m_nbVoxelsX; i++)
    for (j = 1; j <= m_nbVoxelsY; j++)
      for (k = 1; k <= m_nbVoxelsZ; k++)
	{
	  m_u[IX (i, j, k)] -=
	    0.5 * (p[IX (i + 1, j, k)] -
		   p[IX (i - 1, j, k)]) / h_x;
	  m_v[IX (i, j, k)] -=
	    0.5 * (p[IX (i, j + 1, k)] -
		   p[IX (i, j - 1, k)]) / h_y;
	  m_w[IX (i, j, k)] -=
	    0.5 * (p[IX (i, j, k + 1)] -
		   p[IX (i, j, k - 1)]) / h_z;
	}
  set_bnd (1, m_u);
  set_bnd (2, m_v);
  set_bnd (3, m_w);
}


void
BenchSolver::project_save (double *const p, double *const div, int num)
{
  double h_x = 1.0 / m_nbVoxelsX, h_y = 1.0 / m_nbVoxelsY, h_z = 1.0 / m_nbVoxelsZ;
  int i, j, k, l;

  for (i = 1; i <= m_nbVoxelsX; i++)
    for (j = 1; j <= m_nbVoxelsY; j++)
      for (k = 1; k <= m_nbVoxelsZ; k++)
	{
	  div[IX (i, j, k)] =
	    -0.5 * (h_x *
		    (m_u[IX (i + 1, j, k)] -
		     m_u[IX (i - 1, j, k)]) +
		    h_y * (m_v[IX (i, j + 1, k)] -
			   m_v[IX (i, j - 1, k)]) +
		    h_z * (m_w[IX (i, j, k + 1)] -
			   m_w[IX (i, j, k - 1)]));
	  p[IX (i, j, k)] = 0;
	}

  set_bnd (0, div);
  set_bnd (0, p);

  for (l = 0; l < m_nbStepsGS; l++)
    {
      for (i = 1; i <= m_nbVoxelsX; i++)
	for (j = 1; j <= m_nbVoxelsY; j++)
	  for (k = 1; k <= m_nbVoxelsZ; k++)
	    p[IX (i, j, k)] =
	      (div[IX (i, j, k)] +
	       p[IX (i - 1, j, k)] +
	       p[IX (i + 1, j, k)] +
	       p[IX (i, j - 1, k)] +
	       p[IX (i, j + 1, k)] +
	       p[IX (i, j, k - 1)] +
	       p[IX (i, j, k + 1)]) / 6.0;
      set_bnd (0, p);
    }
  
  for (i = 1; i <= m_nbVoxelsX; i++)
    for (j = 1; j <= m_nbVoxelsY; j++)
      for (k = 1; k <= m_nbVoxelsZ; k++)
	{
	  m_u[IX (i, j, k)] -=
	    0.5 * (p[IX (i + 1, j, k)] -
		   p[IX (i - 1, j, k)]) / h_x;
	  m_v[IX (i, j, k)] -=
	    0.5 * (p[IX (i, j + 1, k)] -
		   p[IX (i, j - 1, k)]) / h_y;
	  m_w[IX (i, j, k)] -=
	    0.5 * (p[IX (i, j, k + 1)] -
		   p[IX (i, j, k - 1)]) / h_z;
	}
  set_bnd (1, m_u);
  set_bnd (2, m_v);
  set_bnd (3, m_w);
}


void
BenchSolver::vel_step_compare_diffuse_normal ()
{
  add_source (m_u, m_uSrc);
  add_source (m_v, m_vSrc);
  add_source (m_w, m_wSrc);
  /* On effectue une résolution complète pour servir de valeur de référence */
  /* Puis on relance une méthode de diffusion en comparant les valeurs      */
  /* calculées successivement avec la valeur de référence                   */
  SWAP (m_uPrev, m_u);
  save_state (m_u, m_uSave);
  diffuse (1, m_u, m_uPrev, m_aVisc, m_visc);
  save_ref (m_u, m_uRef);
  set_previous_state (m_u, m_uSave);
  diffuse_log (1, m_u, m_uPrev, m_uRef, m_uLog, m_aVisc, m_visc);
	
  SWAP (m_vPrev, m_v);
  save_state (m_v, m_vSave);
  diffuse (2, m_v, m_vPrev, m_aVisc, m_visc);
  save_ref (m_v, m_vRef);
  set_previous_state (m_v, m_vSave);
  diffuse_log (2, m_v, m_vPrev, m_vRef, m_vLog, m_aVisc, m_visc);
	
  SWAP (m_wPrev, m_w);
  save_state (m_w, m_wSave);
  diffuse (3, m_w, m_wPrev, m_aVisc, m_visc);
  save_ref (m_w, m_wRef);
  set_previous_state (m_w, m_wSave);
  diffuse_log (3, m_w, m_wPrev, m_wRef, m_wLog, m_aVisc, m_visc);
  writeLog ();
  /**************************************************************************/
  project (m_uPrev, m_vPrev);
  SWAP (m_uPrev, m_u);
  SWAP (m_vPrev, m_v);
  SWAP (m_wPrev, m_w);
  advect (1, m_u, m_uPrev, m_uPrev, m_vPrev, m_wPrev);
  advect (2, m_v, m_vPrev, m_uPrev, m_vPrev, m_wPrev);
  advect (3, m_w, m_wPrev, m_uPrev, m_vPrev, m_wPrev);
  project (m_uPrev, m_vPrev);
}

void
BenchSolver::vel_step_compare_project_normal ()
{
  add_source (m_u, m_uSrc);
  add_source (m_v, m_vSrc);
  add_source (m_w, m_wSrc);
  SWAP (m_uPrev, m_u);
  diffuse (1, m_u, m_uPrev, m_aVisc, m_visc);
  SWAP (m_vPrev, m_v);
  diffuse (2, m_v, m_vPrev, m_aVisc, m_visc);
  SWAP (m_wPrev, m_w);
  diffuse (3, m_w, m_wPrev, m_aVisc, m_visc);
  project_log (m_uPrev, m_vPrev, m_pRef, m_uLog);
  SWAP (m_uPrev, m_u);
  SWAP (m_vPrev, m_v);
  SWAP (m_wPrev, m_w);
  advect (1, m_u, m_uPrev, m_uPrev, m_vPrev, m_wPrev);
  advect (2, m_v, m_vPrev, m_uPrev, m_vPrev, m_wPrev);
  advect (3, m_w, m_wPrev, m_uPrev, m_vPrev, m_wPrev);
  project_log (m_uPrev, m_vPrev, m_pRef, m_vLog);
  writeLog ();
}

void
BenchSolver::vel_step_compare_project_hybride ()
{
  add_source (m_u, m_uSrc);
  add_source (m_v, m_vSrc);
  add_source (m_w, m_wSrc);
  SWAP (m_uPrev, m_u);
  diffuse (1, m_u, m_uPrev, m_aVisc, m_visc);
  SWAP (m_vPrev, m_v);
  diffuse (2, m_v, m_vPrev, m_aVisc, m_visc);
  SWAP (m_wPrev, m_w);
  diffuse (3, m_w, m_wPrev, m_aVisc, m_visc);

  project_log_hybride (m_uPrev, m_vPrev, m_pRef, m_uLog, m_uResidu, m_uPrevResidu);
  SWAP (m_uPrev, m_u);
  SWAP (m_vPrev, m_v);
  SWAP (m_wPrev, m_w);
  advect (1, m_u, m_uPrev, m_uPrev, m_vPrev, m_wPrev);
  advect (2, m_v, m_vPrev, m_uPrev, m_vPrev, m_wPrev);
  advect (3, m_w, m_wPrev, m_uPrev, m_vPrev, m_wPrev);

  project_log_hybride (m_uPrev, m_vPrev, m_pRef, m_vLog, m_vResidu, m_vPrevResidu);
  writeLog ();
}

void
BenchSolver::vel_step_compare_diffuse_hybride ()
{
  add_source (m_u, m_uSrc);
  add_source (m_v, m_vSrc);
  add_source (m_w, m_wSrc);
  /* On effectue une résolution complète pour servir de valeur de référence */
  /* Puis on relance une méthode de diffusion en comparant les valeurs      */
  /* calculées successivement avec la valeur de référence                   */
  SWAP (m_uPrev, m_u);
  save_state (m_u, m_uSave);
  diffuse (1, m_u, m_uPrev, m_aVisc, m_visc);
  save_ref (m_u, m_uRef);
  set_previous_state (m_u, m_uSave);
  diffuse_hybride_log (1, m_u, m_uPrev, m_uRef, m_uResidu, m_uPrevResidu, m_uLog, m_aVisc, m_visc);
  SWAP (m_vPrev, m_v);
  save_state (m_v, m_vSave);
  diffuse (2, m_v, m_vPrev, m_aVisc, m_visc);
  save_ref (m_v, m_vRef);
  set_previous_state (m_v, m_vSave);
  diffuse_hybride_log (2, m_v, m_vPrev, m_vRef, m_vResidu, m_vPrevResidu, m_vLog, m_aVisc, m_visc);
  SWAP (m_wPrev, m_w);
  save_state (m_w, m_wSave);
  diffuse (3, m_w, m_wPrev, m_aVisc, m_visc);
  save_ref (m_w, m_wRef);
  set_previous_state (m_w, m_wSave);
  diffuse_hybride_log (3, m_w, m_wPrev, m_wRef, m_wResidu, m_wPrevResidu, m_wLog, m_aVisc, m_visc);
  writeLog ();
  /**************************************************************************/
  project (m_uPrev, m_vPrev);
  SWAP (m_uPrev, m_u);
  SWAP (m_vPrev, m_v);
  SWAP (m_wPrev, m_w);
  advect (1, m_u, m_uPrev, m_uPrev, m_vPrev, m_wPrev);
  advect (2, m_v, m_vPrev, m_uPrev, m_vPrev, m_wPrev);
  advect (3, m_w, m_wPrev, m_uPrev, m_vPrev, m_wPrev);
  project (m_uPrev, m_vPrev);
}

void
BenchSolver::iterate (bool flickering, int nb_step_GS)
{
  m_nbStepsGS = nb_step_GS;

  for (int i = 0; i < m_nbFlames; i++)
    m_flames[i]->add_forces (flickering);

  //vel_step();
  //vel_step_compare_diffuse_normal();
  //vel_step_compare_diffuse_hybride();
  vel_step_compare_project_normal();
  //vel_step_compare_project_hybride ();
  //  dens_step();

  set_bnd (0, m_u);
  set_bnd (0, m_v);
  set_bnd (0, m_w);
}

void
BenchSolver::vel_step_save ()
{
  char buf[50];
  
  add_source (m_u, m_uSrc);
  add_source (m_v, m_vSrc);
  add_source (m_w, m_wSrc);

  SWAP (m_uPrev, m_u);
  diffuse (1, m_u, m_uPrev, m_aVisc, m_visc);
  SWAP (m_vPrev, m_v);
  if(! (m_nbIter % 10)){
    sprintf(buf,"avt_diffusion_%dx%dx%d_i%d",m_nbVoxelsX,m_nbVoxelsY,m_nbVoxelsZ,m_nbIter);
    m_file.open (buf, ios::out | ios::trunc);
    save(m_v);
    m_file.close();
  }
  diffuse (2, m_v, m_vPrev, m_aVisc, m_visc);
  if(! (m_nbIter % 10)){
    sprintf(buf,"apr_diffusion_%dx%dx%d_i%d",m_nbVoxelsX,m_nbVoxelsY,m_nbVoxelsZ,m_nbIter);
    m_file.open (buf, ios::out | ios::trunc);
    save(m_v);
    m_file.close();
  }
  SWAP (m_wPrev, m_w);
  diffuse (3, m_w, m_wPrev, m_aVisc, m_visc);

  if(! (m_nbIter % 10)){
    sprintf(buf,"avt_project_avt_advect%dx%dx%d_i%d",m_nbVoxelsX,m_nbVoxelsY,m_nbVoxelsZ,m_nbIter);
    m_file.open (buf, ios::out | ios::trunc);
    save(m_v);
    m_file.close();
  }
  project (m_uPrev, m_vPrev);
  if(! (m_nbIter % 10)){
    sprintf(buf,"apr_project_avt_advect%dx%dx%d_i%d",m_nbVoxelsX,m_nbVoxelsY,m_nbVoxelsZ,m_nbIter);
    m_file.open (buf, ios::out | ios::trunc);
    save(m_v);
    m_file.close();
  }
  
  SWAP (m_uPrev, m_u);
  SWAP (m_vPrev, m_v);
  SWAP (m_wPrev, m_w);
  advect (1, m_u, m_uPrev, m_uPrev, m_vPrev, m_wPrev);
  advect (2, m_v, m_vPrev, m_uPrev, m_vPrev, m_wPrev);
  advect (3, m_w, m_wPrev, m_uPrev, m_vPrev, m_wPrev);
  
  if(! (m_nbIter % 10)){
    sprintf(buf,"avt_project_apr_advect%dx%dx%d_i%d",m_nbVoxelsX,m_nbVoxelsY,m_nbVoxelsZ,m_nbIter);
    m_file.open (buf, ios::out | ios::trunc);
    save(m_v);
    m_file.close();
  }
  project (m_uPrev, m_vPrev);
  if(! (m_nbIter % 10)){
    sprintf(buf,"apr_project_apr_advect%dx%dx%d_i%d",m_nbVoxelsX,m_nbVoxelsY,m_nbVoxelsZ,m_nbIter);
    m_file.open (buf, ios::out | ios::trunc);
    save(m_v);
    m_file.close();
  }
}

void
BenchSolver::iterate_save (bool flickering)
{  
  for (int i = 0; i < m_nbFlames; i++)
    m_flames[i]->add_forces (flickering);
	
  vel_step_save();
  
  //  dens_step();

  set_bnd (0, m_u);
  set_bnd (0, m_v);
  set_bnd (0, m_w);
}

void
BenchSolver::iterate_hybride (bool flickering, int nb_step_GS)
{
  m_nbStepsGS = nb_step_GS;

  for (int i = 0; i < m_nbFlames; i++)
    m_flames[i]->add_forces (flickering);

  vel_step_compare_project_hybride ();
  //  dens_step();

  set_bnd (0, m_u);
  set_bnd (0, m_v);
  set_bnd (0, m_w);
}

void
BenchSolver::iterate (bool flickering)
{
  /* Temporaire : ajout de forces périodiques */
  if ((m_nbIter % m_nbIterFlickering) == 5)
    {
      cleanSources ();
      flickering = false;
    }
  else 
    if ((m_nbIter % m_nbIterFlickering) < 5)
      flickering = true;
    else
      flickering = false;

  printf ("Itération %d\r", m_nbIter);
  fflush (stdout);

  /*if (m_file.bad ())
    {
      cout << "Attention, erreur dans l'écriture du log du solveur"
	   << endl;
      return;
    }*/

  iterate_save(flickering);
  /* Pour comparaison dans diffuse strictement */
  //iterate (flickering, m_refVal);
  //iterate_hybride( flickering, m_refVal);

  /* Comparaison simple */
  //   save_state();

  //   iterate( flickering, m_refVal);
  //   save_ref();

  //   for(int i=1; i < m_refVal; i++){
  //     set_previous_state();

  //     iterate(flickering, i);
  //     compare();
  //   }

  /* Comparaison GS et GS+hybridation */
  //   save_state();

  //   for(int i=1; i < m_refVal; i++){
  //     iterate( flickering, i);
  //     save_ref();

  //     set_previous_state();

  //     iterate_hybride(flickering, i);
  //     compare();

  //     set_previous_state();
  //   }

  //   iterate( flickering, m_refVal);

  //m_file << endl;
  m_nbIter++;
      
  // if (m_nbIter == 150)
//     done = 1;

  return;
}

void
BenchSolver::save_state ()
{
  m_uSave = (double *) memcpy (m_uSave, m_u, m_nbVoxels * sizeof (double));
  m_vSave = (double *) memcpy (m_vSave, m_v, m_nbVoxels * sizeof (double));
  m_wSave = (double *) memcpy (m_wSave, m_w, m_nbVoxels * sizeof (double));
  m_uPrevSave = (double *) memcpy (m_uPrevSave, m_uPrev, m_nbVoxels * sizeof (double));
  m_vPrevSave = (double *) memcpy (m_vPrevSave, m_vPrev, m_nbVoxels * sizeof (double));
  m_wPrevSave = (double *) memcpy (m_wPrevSave, m_wPrev, m_nbVoxels * sizeof (double));
  m_densSave = (double *) memcpy (m_densSave, m_dens, m_nbVoxels * sizeof (double));
  m_densPrevSave = (double *) memcpy (m_densPrevSave, m_densPrev, m_nbVoxels * sizeof (double));
  m_densSrcSave = (double *) memcpy (m_densSrcSave, m_densSrc, m_nbVoxels * sizeof (double));
  m_uSrcSave = (double *) memcpy (m_uSrcSave, m_uSrc, m_nbVoxels * sizeof (double));
  m_vSrcSave = (double *) memcpy (m_vSrcSave, m_vSrc, m_nbVoxels * sizeof (double));
  m_wSrcSave = (double *) memcpy (m_wSrcSave, m_wSrc, m_nbVoxels * sizeof (double));
}

void
BenchSolver::save_state (const double *const x, double *x_save)
{
  x_save = (double *) memcpy (x_save, x, m_nbVoxels * sizeof (double));
}

void
BenchSolver::save_ref (const double *const x, double *x_ref)
{
  x_ref = (double *) memcpy (x_ref, x, m_nbVoxels * sizeof (double));
}

void
BenchSolver::set_previous_state (double *x, const double *const x_save)
{
  x = (double *) memcpy (x, x_save, m_nbVoxels * sizeof (double));
}

void
BenchSolver::set_previous_state ()
{
  m_u = (double *) memcpy (m_u, m_uSave, m_nbVoxels * sizeof (double));
  m_v = (double *) memcpy (m_v, m_vSave, m_nbVoxels * sizeof (double));
  m_w = (double *) memcpy (m_w, m_wSave, m_nbVoxels * sizeof (double));
  m_uPrev = (double *) memcpy (m_uPrev, m_uPrevSave, m_nbVoxels * sizeof (double));
  m_vPrev = (double *) memcpy (m_vPrev, m_vPrevSave, m_nbVoxels * sizeof (double));
  m_wPrev = (double *) memcpy (m_wPrev, m_wPrevSave, m_nbVoxels * sizeof (double));
  m_dens = (double *) memcpy (m_dens, m_densSave, m_nbVoxels * sizeof (double));
  m_densPrev =
    (double *) memcpy (m_densPrev, m_densPrevSave,
		       m_nbVoxels * sizeof (double));
  m_densSrc =
    (double *) memcpy (m_densSrc, m_densSrcSave,
		       m_nbVoxels * sizeof (double));
  m_uSrc = (double *) memcpy (m_uSrc, m_uSrcSave, m_nbVoxels * sizeof (double));
  m_vSrc = (double *) memcpy (m_vSrc, m_vSrcSave, m_nbVoxels * sizeof (double));
  m_wSrc = (double *) memcpy (m_wSrc, m_wSrcSave, m_nbVoxels * sizeof (double));
}

void
BenchSolver::compare (int n, const double *const x, double *const xlog,
		      const double *const xref)
{
  double xerr = 0;
  double xerrtmp;
  double xerrmax = 0;

  for (int i = 1; i <= m_nbVoxelsX; i++)
    for (int j = 1; j <= m_nbVoxelsY; j++)
      for (int k = 1; k <= m_nbVoxelsZ; k++)
	{
	  xerrtmp =
	    x[IX (i, j, k)] - xref[IX (i, j, k)];

	  xerrtmp *= xerrtmp;

	  xerr += xerrtmp;

	  if (xerrtmp > xerrmax)
	    xerrmax = xerrtmp;
	}

  xlog[2 * n] = sqrt (xerr) / (m_nbVoxelsX * m_nbVoxelsY * m_nbVoxelsZ);
  xlog[2 * n + 1] = xerrmax;
}

void
BenchSolver::compare ()
{
  double uerr, verr, werr;
  double uerrtmp, verrtmp, werrtmp;
  double uerrmax = 0, verrmax = 0, werrmax = 0;
  uerr = verr = werr = 0;

  for (int i = 1; i <= m_nbVoxelsX; i++)
    for (int j = 1; j <= m_nbVoxelsY; j++)
      for (int k = 1; k <= m_nbVoxelsZ; k++)
	{
	  uerrtmp = m_u[IX (i, j, k)] - m_uRef[IX (i, j, k)];
	  verrtmp = m_v[IX (i, j, k)] - m_vRef[IX (i, j, k)];
	  werrtmp = m_w[IX (i, j, k)] - m_wRef[IX (i, j, k)];

	  uerrtmp *= uerrtmp;
	  verrtmp *= verrtmp;
	  werrtmp *= werrtmp;

	  uerr += uerrtmp;
	  verr += verrtmp;
	  werr += werrtmp;

	  if (uerrtmp > uerrmax)
	    uerrmax = uerrtmp;
	  if (verrtmp > verrmax)
	    verrmax = verrtmp;
	  if (werrtmp > werrmax)
	    werrmax = werrtmp;
	}

  m_file << m_nbIter << " " << m_nbStepsGS << " ";
  m_file << sqrt (uerr) << " " << sqrt (verr) << " " << sqrt (werr) <<
    " ";
  m_file << uerrmax << " " << verrmax << " " << werrmax << endl;
}

const void BenchSolver::save (const double *const x)
{
  for (int i = 1; i <= m_nbVoxelsX; i++)
    for (int j = 1; j <= m_nbVoxelsY; j++){
      for (int k = 1; k <= m_nbVoxelsZ; k++)
	{
	  m_file << x[IX(i,j,k)] << " ";
	  
	}
      m_file << endl;
    }
}

// const void BenchSolver::save (const double *const x)
// {
//   double a = dt * m_visc * m_rowSize;

//   for (int i = 1; i <= m_nbVoxelsX; i++)
//     for (int j = 1; j <= m_nbVoxelsY; j++)
//       for (int k = 1; k <= m_nbVoxelsZ; k++)
// 	{
// 	  m_rowSave = (double *) memset (m_rowSave, 0, m_rowSize * sizeof (double));
// 	  if(j>2) m_rowSave[IX2 (i-1, j-2, k-1)] = a * x[IX (i, j-1, k)];
// 	  if(i>2) m_rowSave[IX2 (i-2, j-1, k-1)] = a * x[IX (i-1, j, k)];
// 	  if(i<m_nbVoxelsX) m_rowSave[IX2 (i, j-1, k-1)] = a * x[IX (i+1, j, k)];
// 	  if(j<m_nbVoxelsY) m_rowSave[IX2 (i-1, j, k-1)] = a * x[IX (i, j+1, k)];
// 	  if(k>2) m_rowSave[IX2 (i-1, j-1, k-2)] = a * x[IX (i, j, k-1)];
// 	  if(k<m_nbVoxelsZ) m_rowSave[IX2 (i-1, j-1, k)] = a * x[IX (i, j, k+1)];
// 	  m_rowSave[IX2 (i-1, j-1, k-1)] = ( 1+6.0 * a) * x[IX (i, j, k)];
	  
// 	  for(int l = 0; l<= m_rowSize; l++)
// 	    m_file << m_rowSave[l] << " ";
// 	  m_file << endl;
// 	}  
// }

void
BenchSolver::writeLog ()
{
  for (int i = 0; i < m_nbStepsGS; i++)
    {
      m_file << m_nbIter << " " << i << " ";
      m_file << m_uLog[2 * i] << " " << m_vLog[2 * i] << " " << m_wLog[2 *
							       i] <<
	" ";
      m_file << m_uLog[2 * i + 1] << " " << m_vLog[2 * i +
					     1] << " " << m_wLog[2 *
							       i +
							       1] <<
	endl;
    }
}
