#ifndef CBOUNDINGSPHERE_H
#define CBOUNDINGSPHERE_H

#include "CCamera.hpp"

#include "../Utility/GraphicsFn.hpp"

class CBoundingSphere
{
public:
	CBoundingSphere() :
		m_fRadius(0.0f)
	{};

	/** Calcule la visibilité de la sphère par rapport au point de vue courant.
	 * @param view Référence sur la caméra
	 */
	bool isVisible(const CCamera &view) const
	{
		uint i;
		const float *plan;

		// Centre dans le frustrum ?
		//   for( p = 0; p < 6; p++ )
		//     if( frustum[p][0] * x + frustum[p][1] * y + frustum[p][2] * z + frustum[p][3] <= 0 ){
		//       m_visibility = false;
		//       return;
		//     }

		// Sphère dans le frustrum ?
		for ( i = 0; i < 6; i++ )
		{
			plan=view.GetFrustum(i);
			if ( plan[0] * m_oCentre.x + plan[1] * m_oCentre.y + plan[2] * m_oCentre.z + plan[3] <= -m_fRadius )
				return false;
		}
		return true;
	}

	/** Calcule la visibilité de la sphère par rapport au point de vue courant.
	 * Variation permettant de récupérer en même temps la distance par rapport à la caméra.
	 * @param view Référence sur la caméra.
	 * @return 0 si non visible, distance à la caméra sinon.
	 */
	float visibleDistance(const CCamera &view) const
	{
		uint i;
		const float *plan;
		float d;

		// Sphère dans le frustrum ?
		for ( i = 0; i < 6; i++ )
		{
			plan=view.GetFrustum(i);
			d = plan[0] * m_oCentre.x + plan[1] * m_oCentre.y + plan[2] * m_oCentre.z + plan[3];
			if ( d <= -m_fRadius )
				return 0.0f;
		}
		return d + m_fRadius;
	}

	/** Calcule la couverture en pixel de la sphère.
	 * On projette le centre et un point à la périphérie sur l'écran, puis on calcule
	 * l'aire du disque obtenu.
	 */
	float GetPixelCoverage(const CCamera &view) const
	{
		CPoint centerSC, periSC;

		view.GetSphereCoordinates(m_oCentre, m_fRadius, centerSC, periSC);
		/* PI.R² */
		return ( M_PI* centerSC.squaredDistanceFrom(periSC));
	}

	void draw(void) const
	{
		glEnable(GL_BLEND);
		glPushMatrix();
		glTranslatef(m_oCentre.x, m_oCentre.y, m_oCentre.z);
		glColor4f(1.0f,0.0f,0.0f,0.3f);
		UGraphicsFn::SolidSphere(m_fRadius, 30, 30);
		glPopMatrix();
		glDisable(GL_BLEND);
	}

	CPoint const& GetCentre() const
	{
		return m_oCentre;
	}
	CPoint& GrabCentre()
	{
		return m_oCentre;
	}
	void SetCentre(CPoint const& a_rCentre)
	{
		m_oCentre = a_rCentre;
	}
	float GetRadius() const
	{
		return m_fRadius;
	}
	float& GrabRadius()
	{
		return m_fRadius;
	}
	void SetRadius(float a_fRadius)
	{
		m_fRadius = a_fRadius;
	}

private:
	/** Centre de la sphère englobante. */
	CPoint m_oCentre;

	/** Rayon de la sphère englobante. */
	float m_fRadius;
};

#endif // CBOUNDINGSPHERE_H
