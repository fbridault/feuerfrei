#include "compResAvgSolver3D.hpp"

CompResAvgSolver3D::CompResAvgSolver3D (CTransform& a_rTransform, uint n_x, uint n_y, uint n_z, float dim,
					float timeStep, float buoyancy, float vorticityConfinement,
					float omegaDiff, float omegaProj, float epsilon, uint nbTimeSteps) :
  Solver3D (a_rTransform, n_x, n_y, n_z, dim, timeStep, buoyancy, vorticityConfinement),
  LogResAvgSolver3D (nbTimeSteps, omegaDiff, omegaProj, epsilon)
{
  m_file.open ("logs/0.1/residualsAverage.log", ios::out | ios::trunc);

  m_omegaDiff = 0.1;
  m_omegaProj = 0.1;
}

CompResAvgSolver3D::~CompResAvgSolver3D ()
{
}

void CompResAvgSolver3D::vel_step ()
{
  if(m_nbIter > m_nbMaxIter)
    if(m_omegaDiff < 1.9)
      {
	char file[100];
	m_omegaDiff += 0.1;
	m_omegaProj += 0.1;
	sprintf(file, "logs/%2.1f/residualsAverage.log", m_omegaDiff);
	cout << "Processing " << file << endl;
	m_file.open (file, ios::out | ios::trunc);
	if (!m_file.is_open ())
	  cerr << "Can't open file " << file << endl;

	m_nbIter = 0;
	m_perturbateCount = 0;
      }
    else
      return;

  add_source (m_u, m_uSrc);
  add_source (m_v, m_vSrc);
  add_source (m_w, m_wSrc);
  addVorticityConfinement(m_u,m_v,m_w);
  SWAP (m_uPrev, m_u);
  diffuse (1, m_u, m_uPrev, m_aVisc);
  SWAP (m_vPrev, m_v);
  diffuse (2, m_v, m_vPrev, m_aVisc);
  SWAP (m_wPrev, m_w);
  diffuse (3, m_w, m_wPrev, m_aVisc);

  m_index = NB_DIFF_LOGS;
  project (m_uPrev, m_vPrev);
  SWAP (m_uPrev, m_u);
  SWAP (m_vPrev, m_v);
  SWAP (m_wPrev, m_w);
  advect (1, m_u, m_uPrev, m_uPrev, m_vPrev, m_wPrev);
  advect (2, m_v, m_vPrev, m_uPrev, m_vPrev, m_wPrev);
  advect (3, m_w, m_wPrev, m_uPrev, m_vPrev, m_wPrev);

  m_index = NB_DIFF_LOGS+1;
  project (m_uPrev, m_vPrev);

  if(m_nbIter == m_nbMaxIter){
    for(uint i=0; i <= m_nbSteps; i++){
      m_file << i << " ";

      for(uint j=0; j < (NB_PROJ_LOGS+NB_DIFF_LOGS) ; j++)
	m_file << m_averages[j*(m_nbSteps+1) + i] << " ";

      m_file << endl;
    }
    cout << "Simulation over" << endl;
    m_file.close ();
  }
}

/* Pas de diffusion */
void CompResAvgSolver3D::diffuse (unsigned char b, float *const x, float *const x0, float a)
{
  m_index = b+2;
  GCSSOR(x,x0,a, (1.0f + 6.0f * a), m_omegaDiff,m_nbSteps);
}


void CompResAvgSolver3D::project (float *const p, float *const div)
{
  float h_x = 1.0f / m_nbVoxelsX, h_y = 1.0f / m_nbVoxelsY, h_z = 1.0f / m_nbVoxelsZ;
  uint i, j, k;

  for (i = 1; i <= m_nbVoxelsX; i++)
    for (j = 1; j <= m_nbVoxelsY; j++)
      for (k = 1; k <= m_nbVoxelsZ; k++){
		div[IX (i, j, k)] =
		  -0.5f * (h_x * (m_u[IX (i + 1, j, k)] - m_u[IX (i - 1, j, k)]) +
			   h_y * (m_v[IX (i, j + 1, k)] - m_v[IX (i, j - 1, k)]) +
			   h_z * (m_w[IX (i, j, k + 1)] - m_w[IX (i, j, k - 1)]));
		//p[IX (i, j, k)] = 0;
      }
//   set_bnd (0, div);
  fill_n(p, m_nbVoxels, 0.0f);
//   set_bnd (0, p);

  m_index +=2;
  GCSSOR(p,div,1, 6.0f, m_omegaProj,m_nbSteps);

  for (i = 1; i <= m_nbVoxelsX; i++)
    for (j = 1; j <= m_nbVoxelsY; j++)
      for (k = 1; k <= m_nbVoxelsZ; k++){
	m_u[IX (i, j, k)] -= 0.5f * (p[IX (i + 1, j, k)] - p[IX (i - 1, j, k)]) / h_x;
	m_v[IX (i, j, k)] -= 0.5f * (p[IX (i, j + 1, k)] - p[IX (i, j - 1, k)]) / h_y;
	m_w[IX (i, j, k)] -= 0.5f * (p[IX (i, j, k + 1)] - p[IX (i, j, k - 1)]) / h_z;
      }
  //set_bnd (1, u);
  //set_bnd (2, v);
  //set_bnd (3, w);
}
