#include "benchsolver.hpp"

#include <math.h>

extern int done;

BenchSolver::BenchSolver (int n_x, int n_y, int n_z, double dim, double pas_de_temps): 
  Solver (n_x, n_y, n_z, dim, pas_de_temps)
{
  u_save = new double[size];
  v_save = new double[size];
  w_save = new double[size];
  u_prev_save = new double[size];
  v_prev_save = new double[size];
  w_prev_save = new double[size];
  dens_save = new double[size];
  dens_prev_save = new double[size];
  dens_src_save = new double[size];
  u_src_save = new double[size];
  v_src_save = new double[size];
  w_src_save = new double[size];
  row_size = N_x * N_y * N_z;
  row_save = new double[row_size];
	
  u_save = (double *) memset (u_save, 0, size * sizeof (double));
  v_save = (double *) memset (v_save, 0, size * sizeof (double));
  w_save = (double *) memset (w_save, 0, size * sizeof (double));
  u_prev_save =
    (double *) memset (u_prev_save, 0, size * sizeof (double));
  v_prev_save =
    (double *) memset (v_prev_save, 0, size * sizeof (double));
  w_prev_save =
    (double *) memset (w_prev_save, 0, size * sizeof (double));
  dens_save = (double *) memset (dens_save, 0, size * sizeof (double));
  dens_prev_save =
    (double *) memset (dens_prev_save, 0, size * sizeof (double));
  dens_src_save =
    (double *) memset (dens_src_save, 0, size * sizeof (double));
  u_src_save =
    (double *) memset (u_src_save, 0, size * sizeof (double));
  v_src_save =
    (double *) memset (v_src_save, 0, size * sizeof (double));
  w_src_save =
    (double *) memset (w_src_save, 0, size * sizeof (double));

  u_ref = new double[size];
  v_ref = new double[size];
  w_ref = new double[size];
  p_ref = new double[size];
  dens_ref = new double[size];
  p_save = new double[size];

  ulog = new double[2 * ref_val];
  vlog = new double[2 * ref_val];
  wlog = new double[2 * ref_val];
  plog = new double[2 * ref_val];

  //file.open ("solver.log", ios::out | ios::trunc);
}

BenchSolver::~BenchSolver ()
{
  //file.close ();

  delete[]u_save;
  delete[]v_save;
  delete[]w_save;
  delete[]u_prev_save;
  delete[]v_prev_save;
  delete[]w_prev_save;
  delete[]dens_save;
  delete[]dens_prev_save;
  delete[]u_src_save;
  delete[]v_src_save;
  delete[]w_src_save;
  delete[]dens_src_save;
  delete[]p_save;
  delete[]row_save;
  
  delete[]u_ref;
  delete[]v_ref;
  delete[]w_ref;
  delete[]dens_ref;
  delete[]p_ref;

  delete[]ulog;
  delete[]vlog;
  delete[]wlog;
  delete[]plog;
}

/* Pas de diffusion */
void
BenchSolver::diffuse_log (int b, double *const x, const double *const x0,
			  const double *const xref, double *const xlog,
			  double a, double diff_visc)
{
  int i, j, k, l;

  for (l = 0; l < nb_step_gauss_seidel; l++)
    {
      for (i = 1; i <= N_x; i++)
	for (j = 1; j <= N_y; j++)
	  for (k = 1; k <= N_z; k++)
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

  for (l = 0; l < nb_step_gauss_seidel; l++)
    {

      for (i = 1; i <= N_x; i++)
	for (j = 1; j <= N_y; j++)
	  for (k = 1; k <= N_z; k++)
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

      /* calcul du terme alpha */
      for (i = 1; i <= N_x; i++)
	for (j = 1; j <= N_y; j++)
	  for (k = 1; k <= N_z; k++)
	    {
	      diffe = residu[IX (i, j, k)] -
		residu0[IX (i, j, k)];
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
      for (i = 1; i <= N_x; i++)
	for (j = 1; j <= N_y; j++)
	  for (k = 1; k <= N_z; k++)
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
  double h_x = 1.0 / N_x, h_y = 1.0 / N_y, h_z = 1.0 / N_z;
  int i, j, k, l;

  for (i = 1; i <= N_x; i++)
    for (j = 1; j <= N_y; j++)
      for (k = 1; k <= N_z; k++)
	{
	  div[IX (i, j, k)] =
	    -0.5 * (h_x *
		    (u[IX (i + 1, j, k)] -
		     u[IX (i - 1, j, k)]) +
		    h_y * (v[IX (i, j + 1, k)] -
			   v[IX (i, j - 1, k)]) +
		    h_z * (w[IX (i, j, k + 1)] -
			   w[IX (i, j, k - 1)]));
	  p[IX (i, j, k)] = 0;
	}

  set_bnd (0, div);
  set_bnd (0, p);

  save_state (p, p_save);
  for (l = 0; l < nb_step_gauss_seidel; l++)
    {
      for (i = 1; i <= N_x; i++)
	for (j = 1; j <= N_y; j++)
	  for (k = 1; k <= N_z; k++)
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
  set_previous_state (p, p_save);
  for (l = 0; l < nb_step_gauss_seidel; l++)
    {
      for (i = 1; i <= N_x; i++)
	for (j = 1; j <= N_y; j++)
	  for (k = 1; k <= N_z; k++)
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

  for (i = 1; i <= N_x; i++)
    for (j = 1; j <= N_y; j++)
      for (k = 1; k <= N_z; k++)
	{
	  u[IX (i, j, k)] -=
	    0.5 * (p[IX (i + 1, j, k)] -
		   p[IX (i - 1, j, k)]) / h_x;
	  v[IX (i, j, k)] -=
	    0.5 * (p[IX (i, j + 1, k)] -
		   p[IX (i, j - 1, k)]) / h_y;
	  w[IX (i, j, k)] -=
	    0.5 * (p[IX (i, j, k + 1)] -
		   p[IX (i, j, k - 1)]) / h_z;
	}
  set_bnd (1, u);
  set_bnd (2, v);
  set_bnd (3, w);
}

/* Pas de projection pour garantir la conservation de la masse */
/* Les tableaux p et div sont certes modifiés; néanmoins Stam passe u0 et v0 dans vel_step */
/* qui ne servent plus après l'appel de la projection */
void
BenchSolver::project_log_hybride (double *p, double *const div,
				  double *const xref, double *const xlog,
				  double *const residu, double *const residu0)
{
  double h_x = 1.0 / N_x, h_y = 1.0 / N_y, h_z = 1.0 / N_z;
  int i, j, k, l;
  double diffe;
  double alpha;
  double num = 0.0;
  double den = 0.0;

  for (i = 1; i <= N_x; i++)
    for (j = 1; j <= N_y; j++)
      for (k = 1; k <= N_z; k++)
	{
	  div[IX (i, j, k)] =
	    -0.5 * (h_x *
		    (u[IX (i + 1, j, k)] -
		     u[IX (i - 1, j, k)]) +
		    h_y * (v[IX (i, j + 1, k)] -
			   v[IX (i, j - 1, k)]) +
		    h_z * (w[IX (i, j, k + 1)] -
			   w[IX (i, j, k - 1)]));
	  p[IX (i, j, k)] = 0;
	  p_save[IX (i, j, k)] = 0;
	}

  set_bnd (0, div);
  set_bnd (0, p);

  /* Résolution initiale en guise de référence */
  save_state (p, p_save);
  for (l = 0; l < nb_step_gauss_seidel; l++)
    {
      for (i = 1; i <= N_x; i++)
	for (j = 1; j <= N_y; j++)
	  for (k = 1; k <= N_z; k++)
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
  set_previous_state (p, p_save);

  /* Initialisation des résidus */
  for (i = 1; i <= N_x; i++)
    for (j = 1; j <= N_y; j++)
      for (k = 1; k <= N_z; k++)
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
  for (l = 0; l < nb_step_gauss_seidel; l++)
    {
      SWAP (p_save, p);
      for (i = 1; i <= N_x; i++)
	for (j = 1; j <= N_y; j++)
	  for (k = 1; k <= N_z; k++)
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
      for (i = 1; i <= N_x; i++)
	for (j = 1; j <= N_y; j++)
	  for (k = 1; k <= N_z; k++)
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
      for (i = 1; i <= N_x; i++)
	for (j = 1; j <= N_y; j++)
	  for (k = 1; k <= N_z; k++)
	    {
	      p[IX (i, j, k)] =
		alpha * p[IX (i, j, k)] + 
		(1 - alpha) * p_save[IX (i, j, k)];

	      residu0[IX (i, j, k)] =
		alpha * residu[IX (i, j, k)] +
		(1 - alpha) * residu0[IX (i, j, k)];
	      // if(residu0[IX(i,j,k)])
	      //         cout << "residu0 " << residu0[IX(i,j,k)] << endl;
	    }
		
      compare (l, p, xlog, xref);
    }
  /* Fin hybridation */
	
  for (i = 1; i <= N_x; i++)
    for (j = 1; j <= N_y; j++)
      for (k = 1; k <= N_z; k++)
	{
	  u[IX (i, j, k)] -=
	    0.5 * (p[IX (i + 1, j, k)] -
		   p[IX (i - 1, j, k)]) / h_x;
	  v[IX (i, j, k)] -=
	    0.5 * (p[IX (i, j + 1, k)] -
		   p[IX (i, j - 1, k)]) / h_y;
	  w[IX (i, j, k)] -=
	    0.5 * (p[IX (i, j, k + 1)] -
		   p[IX (i, j, k - 1)]) / h_z;
	}
  set_bnd (1, u);
  set_bnd (2, v);
  set_bnd (3, w);
}


void
BenchSolver::project_save (double *const p, double *const div, int num)
{
  double h_x = 1.0 / N_x, h_y = 1.0 / N_y, h_z = 1.0 / N_z;
  int i, j, k, l;

  for (i = 1; i <= N_x; i++)
    for (j = 1; j <= N_y; j++)
      for (k = 1; k <= N_z; k++)
	{
	  div[IX (i, j, k)] =
	    -0.5 * (h_x *
		    (u[IX (i + 1, j, k)] -
		     u[IX (i - 1, j, k)]) +
		    h_y * (v[IX (i, j + 1, k)] -
			   v[IX (i, j - 1, k)]) +
		    h_z * (w[IX (i, j, k + 1)] -
			   w[IX (i, j, k - 1)]));
	  p[IX (i, j, k)] = 0;
	}

  set_bnd (0, div);
  set_bnd (0, p);

  for (l = 0; l < nb_step_gauss_seidel; l++)
    {
      for (i = 1; i <= N_x; i++)
	for (j = 1; j <= N_y; j++)
	  for (k = 1; k <= N_z; k++)
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
  
  for (i = 1; i <= N_x; i++)
    for (j = 1; j <= N_y; j++)
      for (k = 1; k <= N_z; k++)
	{
	  u[IX (i, j, k)] -=
	    0.5 * (p[IX (i + 1, j, k)] -
		   p[IX (i - 1, j, k)]) / h_x;
	  v[IX (i, j, k)] -=
	    0.5 * (p[IX (i, j + 1, k)] -
		   p[IX (i, j - 1, k)]) / h_y;
	  w[IX (i, j, k)] -=
	    0.5 * (p[IX (i, j, k + 1)] -
		   p[IX (i, j, k - 1)]) / h_z;
	}
  set_bnd (1, u);
  set_bnd (2, v);
  set_bnd (3, w);
}


void
BenchSolver::vel_step_compare_diffuse_normal ()
{
  add_source (u, u_src);
  add_source (v, v_src);
  add_source (w, w_src);
  /* On effectue une résolution complète pour servir de valeur de référence */
  /* Puis on relance une méthode de diffusion en comparant les valeurs      */
  /* calculées successivement avec la valeur de référence                   */
  SWAP (u_prev, u);
  save_state (u, u_save);
  diffuse (1, u, u_prev, a_visc, visc);
  save_ref (u, u_ref);
  set_previous_state (u, u_save);
  diffuse_log (1, u, u_prev, u_ref, ulog, a_visc, visc);
	
  SWAP (v_prev, v);
  save_state (v, v_save);
  diffuse (2, v, v_prev, a_visc, visc);
  save_ref (v, v_ref);
  set_previous_state (v, v_save);
  diffuse_log (2, v, v_prev, v_ref, vlog, a_visc, visc);
	
  SWAP (w_prev, w);
  save_state (w, w_save);
  diffuse (3, w, w_prev, a_visc, visc);
  save_ref (w, w_ref);
  set_previous_state (w, w_save);
  diffuse_log (3, w, w_prev, w_ref, wlog, a_visc, visc);
  writeLog ();
  /**************************************************************************/
  project (u_prev, v_prev);
  SWAP (u_prev, u);
  SWAP (v_prev, v);
  SWAP (w_prev, w);
  advect (1, u, u_prev, u_prev, v_prev, w_prev);
  advect (2, v, v_prev, u_prev, v_prev, w_prev);
  advect (3, w, w_prev, u_prev, v_prev, w_prev);
  project (u_prev, v_prev);
}

void
BenchSolver::vel_step_compare_project_normal ()
{
  add_source (u, u_src);
  add_source (v, v_src);
  add_source (w, w_src);
  SWAP (u_prev, u);
  diffuse (1, u, u_prev, a_visc, visc);
  SWAP (v_prev, v);
  diffuse (2, v, v_prev, a_visc, visc);
  SWAP (w_prev, w);
  diffuse (3, w, w_prev, a_visc, visc);
  project_log (u_prev, v_prev, p_ref, ulog);
  SWAP (u_prev, u);
  SWAP (v_prev, v);
  SWAP (w_prev, w);
  advect (1, u, u_prev, u_prev, v_prev, w_prev);
  advect (2, v, v_prev, u_prev, v_prev, w_prev);
  advect (3, w, w_prev, u_prev, v_prev, w_prev);
  project_log (u_prev, v_prev, p_ref, vlog);
  writeLog ();
}

void
BenchSolver::vel_step_compare_project_hybride ()
{
  add_source (u, u_src);
  add_source (v, v_src);
  add_source (w, w_src);
  SWAP (u_prev, u);
  diffuse (1, u, u_prev, a_visc, visc);
  SWAP (v_prev, v);
  diffuse (2, v, v_prev, a_visc, visc);
  SWAP (w_prev, w);
  diffuse (3, w, w_prev, a_visc, visc);

  project_log_hybride (u_prev, v_prev, p_ref, ulog, residu_u,
		       residu_u_prev);
  SWAP (u_prev, u);
  SWAP (v_prev, v);
  SWAP (w_prev, w);
  advect (1, u, u_prev, u_prev, v_prev, w_prev);
  advect (2, v, v_prev, u_prev, v_prev, w_prev);
  advect (3, w, w_prev, u_prev, v_prev, w_prev);

  project_log_hybride (u_prev, v_prev, p_ref, vlog, residu_v,
		       residu_v_prev);
  writeLog ();
}

void
BenchSolver::vel_step_compare_diffuse_hybride ()
{
  add_source (u, u_src);
  add_source (v, v_src);
  add_source (w, w_src);
  /* On effectue une résolution complète pour servir de valeur de référence */
  /* Puis on relance une méthode de diffusion en comparant les valeurs      */
  /* calculées successivement avec la valeur de référence                   */
  SWAP (u_prev, u);
  save_state (u, u_save);
  diffuse (1, u, u_prev, a_visc, visc);
  save_ref (u, u_ref);
  set_previous_state (u, u_save);
  diffuse_hybride_log (1, u, u_prev, u_ref, residu_u, residu_u_prev,
		       ulog, a_visc, visc);
  SWAP (v_prev, v);
  save_state (v, v_save);
  diffuse (2, v, v_prev, a_visc, visc);
  save_ref (v, v_ref);
  set_previous_state (v, v_save);
  diffuse_hybride_log (2, v, v_prev, v_ref, residu_v, residu_v_prev,
		       vlog, a_visc, visc);
  SWAP (w_prev, w);
  save_state (w, w_save);
  diffuse (3, w, w_prev, a_visc, visc);
  save_ref (w, w_ref);
  set_previous_state (w, w_save);
  diffuse_hybride_log (3, w, w_prev, w_ref, residu_w, residu_w_prev,
		       wlog, a_visc, visc);
  writeLog ();
  /**************************************************************************/
  project (u_prev, v_prev);
  SWAP (u_prev, u);
  SWAP (v_prev, v);
  SWAP (w_prev, w);
  advect (1, u, u_prev, u_prev, v_prev, w_prev);
  advect (2, v, v_prev, u_prev, v_prev, w_prev);
  advect (3, w, w_prev, u_prev, v_prev, w_prev);
  project (u_prev, v_prev);
}

void
BenchSolver::iterate (bool brintage, int nb_step_GS)
{
  nb_step_gauss_seidel = nb_step_GS;

  for (int i = 0; i < nb_flammes; i++)
    flammes[i]->add_forces (brintage);

  //vel_step();
  //vel_step_compare_diffuse_normal();
  //vel_step_compare_diffuse_hybride();
  vel_step_compare_project_normal();
  //vel_step_compare_project_hybride ();
  //  dens_step();

  set_bnd (0, u);
  set_bnd (0, v);
  set_bnd (0, w);
}

void
BenchSolver::vel_step_save ()
{
  char buf[50];
  
  add_source (u, u_src);
  add_source (v, v_src);
  add_source (w, w_src);

  SWAP (u_prev, u);
  diffuse (1, u, u_prev, a_visc, visc);
  SWAP (v_prev, v);
  if(! (nb_iter % 10)){
    sprintf(buf,"avt_diffusion_%dx%dx%d_i%d",N_x,N_y,N_z,nb_iter);
    file.open (buf, ios::out | ios::trunc);
    save(v);
    file.close();
  }
  diffuse (2, v, v_prev, a_visc, visc);
  if(! (nb_iter % 10)){
    sprintf(buf,"apr_diffusion_%dx%dx%d_i%d",N_x,N_y,N_z,nb_iter);
    file.open (buf, ios::out | ios::trunc);
    save(v);
    file.close();
  }
  SWAP (w_prev, w);
  diffuse (3, w, w_prev, a_visc, visc);

  if(! (nb_iter % 10)){
    sprintf(buf,"avt_project_avt_advect%dx%dx%d_i%d",N_x,N_y,N_z,nb_iter);
    file.open (buf, ios::out | ios::trunc);
    save(v);
    file.close();
  }
  project (u_prev, v_prev);
  if(! (nb_iter % 10)){
    sprintf(buf,"apr_project_avt_advect%dx%dx%d_i%d",N_x,N_y,N_z,nb_iter);
    file.open (buf, ios::out | ios::trunc);
    save(v);
    file.close();
  }
  
  SWAP (u_prev, u);
  SWAP (v_prev, v);
  SWAP (w_prev, w);
  advect (1, u, u_prev, u_prev, v_prev, w_prev);
  advect (2, v, v_prev, u_prev, v_prev, w_prev);
  advect (3, w, w_prev, u_prev, v_prev, w_prev);
  
  if(! (nb_iter % 10)){
    sprintf(buf,"avt_project_apr_advect%dx%dx%d_i%d",N_x,N_y,N_z,nb_iter);
    file.open (buf, ios::out | ios::trunc);
    save(v);
    file.close();
  }
  project (u_prev, v_prev);
  if(! (nb_iter % 10)){
    sprintf(buf,"apr_project_apr_advect%dx%dx%d_i%d",N_x,N_y,N_z,nb_iter);
    file.open (buf, ios::out | ios::trunc);
    save(v);
    file.close();
  }
}

void
BenchSolver::iterate_save (bool brintage)
{  
  for (int i = 0; i < nb_flammes; i++)
    flammes[i]->add_forces (brintage);
	
  vel_step_save();
  
  //  dens_step();

  set_bnd (0, u);
  set_bnd (0, v);
  set_bnd (0, w);
}

void
BenchSolver::iterate_hybride (bool brintage, int nb_step_GS)
{
  nb_step_gauss_seidel = nb_step_GS;

  for (int i = 0; i < nb_flammes; i++)
    flammes[i]->add_forces (brintage);

  vel_step_compare_project_hybride ();
  //  dens_step();

  set_bnd (0, u);
  set_bnd (0, v);
  set_bnd (0, w);
}

void
BenchSolver::iterate (bool brintage)
{
  /* Temporaire : ajout de forces périodiques */
  if ((nb_iter % nb_iter_brintage) == 5)
    {
      cleanSources ();
      brintage = false;
    }
  else 
    if ((nb_iter % nb_iter_brintage) < 5)
      brintage = true;
    else
      brintage = false;

  printf ("Itération %d\r", nb_iter);
  fflush (stdout);

  /*if (file.bad ())
    {
      cout << "Attention, erreur dans l'écriture du log du solveur"
	   << endl;
      return;
    }*/

  iterate_save(brintage);
  /* Pour comparaison dans diffuse strictement */
  //iterate (brintage, ref_val);
  //iterate_hybride( brintage, ref_val);

  /* Comparaison simple */
  //   save_state();

  //   iterate( brintage, ref_val);
  //   save_ref();

  //   for(int i=1; i < ref_val; i++){
  //     set_previous_state();

  //     iterate(brintage, i);
  //     compare();
  //   }

  /* Comparaison GS et GS+hybridation */
  //   save_state();

  //   for(int i=1; i < ref_val; i++){
  //     iterate( brintage, i);
  //     save_ref();

  //     set_previous_state();

  //     iterate_hybride(brintage, i);
  //     compare();

  //     set_previous_state();
  //   }

  //   iterate( brintage, ref_val);

  //file << endl;
  nb_iter++;
      
  // if (nb_iter == 150)
//     done = 1;

  return;
}

void
BenchSolver::save_state ()
{
  u_save = (double *) memcpy (u_save, u, size * sizeof (double));
  v_save = (double *) memcpy (v_save, v, size * sizeof (double));
  w_save = (double *) memcpy (w_save, w, size * sizeof (double));
  u_prev_save =
    (double *) memcpy (u_prev_save, u_prev,
		       size * sizeof (double));
  v_prev_save =
    (double *) memcpy (v_prev_save, v_prev,
		       size * sizeof (double));
  w_prev_save =
    (double *) memcpy (w_prev_save, w_prev,
		       size * sizeof (double));
  dens_save =
    (double *) memcpy (dens_save, dens, size * sizeof (double));
  dens_prev_save =
    (double *) memcpy (dens_prev_save, dens_prev,
		       size * sizeof (double));
  dens_src_save =
    (double *) memcpy (dens_src_save, dens_src,
		       size * sizeof (double));
  u_src_save =
    (double *) memcpy (u_src_save, u_src, size * sizeof (double));
  v_src_save =
    (double *) memcpy (v_src_save, v_src, size * sizeof (double));
  w_src_save =
    (double *) memcpy (w_src_save, w_src, size * sizeof (double));
}

void
BenchSolver::save_state (const double *const x, double *x_save)
{
  x_save = (double *) memcpy (x_save, x, size * sizeof (double));
}

void
BenchSolver::save_ref (const double *const x, double *x_ref)
{
  x_ref = (double *) memcpy (x_ref, x, size * sizeof (double));
}

void
BenchSolver::set_previous_state (double *x, const double *const x_save)
{
  x = (double *) memcpy (x, x_save, size * sizeof (double));
}

void
BenchSolver::set_previous_state ()
{
  u = (double *) memcpy (u, u_save, size * sizeof (double));
  v = (double *) memcpy (v, v_save, size * sizeof (double));
  w = (double *) memcpy (w, w_save, size * sizeof (double));
  u_prev = (double *) memcpy (u_prev, u_prev_save,
			      size * sizeof (double));
  v_prev = (double *) memcpy (v_prev, v_prev_save,
			      size * sizeof (double));
  w_prev = (double *) memcpy (w_prev, w_prev_save,
			      size * sizeof (double));
  dens = (double *) memcpy (dens, dens_save, size * sizeof (double));
  dens_prev =
    (double *) memcpy (dens_prev, dens_prev_save,
		       size * sizeof (double));
  dens_src =
    (double *) memcpy (dens_src, dens_src_save,
		       size * sizeof (double));
  u_src = (double *) memcpy (u_src, u_src_save, size * sizeof (double));
  v_src = (double *) memcpy (v_src, v_src_save, size * sizeof (double));
  w_src = (double *) memcpy (w_src, w_src_save, size * sizeof (double));
}

void
BenchSolver::compare (int n, const double *const x, double *const xlog,
		      const double *const xref)
{
  double xerr = 0;
  double xerrtmp;
  double xerrmax = 0;

  for (int i = 1; i <= N_x; i++)
    for (int j = 1; j <= N_y; j++)
      for (int k = 1; k <= N_z; k++)
	{
	  xerrtmp =
	    x[IX (i, j, k)] - xref[IX (i, j, k)];

	  xerrtmp *= xerrtmp;

	  xerr += xerrtmp;

	  if (xerrtmp > xerrmax)
	    xerrmax = xerrtmp;
	}

  xlog[2 * n] = sqrt (xerr) / (N_x * N_y * N_z);
  xlog[2 * n + 1] = xerrmax;
}

void
BenchSolver::compare ()
{
  double uerr, verr, werr;
  double uerrtmp, verrtmp, werrtmp;
  double uerrmax = 0, verrmax = 0, werrmax = 0;
  uerr = verr = werr = 0;

  for (int i = 1; i <= N_x; i++)
    for (int j = 1; j <= N_y; j++)
      for (int k = 1; k <= N_z; k++)
	{
	  uerrtmp =
	    u[IX (i, j, k)] - u_ref[IX (i, j, k)];
	  verrtmp =
	    v[IX (i, j, k)] - v_ref[IX (i, j, k)];
	  werrtmp =
	    w[IX (i, j, k)] - w_ref[IX (i, j, k)];

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

  file << nb_iter << " " << nb_step_gauss_seidel << " ";
  file << sqrt (uerr) << " " << sqrt (verr) << " " << sqrt (werr) <<
    " ";
  file << uerrmax << " " << verrmax << " " << werrmax << endl;
}

const void BenchSolver::save (const double *const x)
{
  double a = dt * visc * row_size;

  for (int i = 1; i <= N_x; i++)
    for (int j = 1; j <= N_y; j++){
      for (int k = 1; k <= N_z; k++)
	{
	  file << x[IX(i,j,k)] << " ";
	  
	}
      file << endl;
    }
}

// const void BenchSolver::save (const double *const x)
// {
//   double a = dt * visc * row_size;

//   for (int i = 1; i <= N_x; i++)
//     for (int j = 1; j <= N_y; j++)
//       for (int k = 1; k <= N_z; k++)
// 	{
// 	  row_save = (double *) memset (row_save, 0, row_size * sizeof (double));
// 	  if(j>2) row_save[IX2 (i-1, j-2, k-1)] = a * x[IX (i, j-1, k)];
// 	  if(i>2) row_save[IX2 (i-2, j-1, k-1)] = a * x[IX (i-1, j, k)];
// 	  if(i<N_x) row_save[IX2 (i, j-1, k-1)] = a * x[IX (i+1, j, k)];
// 	  if(j<N_y) row_save[IX2 (i-1, j, k-1)] = a * x[IX (i, j+1, k)];
// 	  if(k>2) row_save[IX2 (i-1, j-1, k-2)] = a * x[IX (i, j, k-1)];
// 	  if(k<N_z) row_save[IX2 (i-1, j-1, k)] = a * x[IX (i, j, k+1)];
// 	  row_save[IX2 (i-1, j-1, k-1)] = ( 1+6.0 * a) * x[IX (i, j, k)];
	  
// 	  for(int l = 0; l<= row_size; l++)
// 	    file << row_save[l] << " ";
// 	  file << endl;
// 	}  
// }

void
BenchSolver::writeLog ()
{
  for (int i = 0; i < nb_step_gauss_seidel; i++)
    {
      file << nb_iter << " " << i << " ";
      file << ulog[2 * i] << " " << vlog[2 * i] << " " << wlog[2 *
							       i] <<
	" ";
      file << ulog[2 * i + 1] << " " << vlog[2 * i +
					     1] << " " << wlog[2 *
							       i +
							       1] <<
	endl;
    }
}
