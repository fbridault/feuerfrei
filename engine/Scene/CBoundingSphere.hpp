#ifndef CBOUNDINGSPHERE_H
#define CBOUNDINGSPHERE_H

#include "CCamera.hpp"

#include "../Utility/GraphicsFn.hpp"

class CBoundingSphere
{
public:
	CBoundingSphere() : radius(0.0f) {};
	/** Centre de la sphère englobante. */
	CPoint centre;

	/** Rayon de la sphère englobante. */
	float radius;

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
			plan=view.getFrustum(i);
			if ( plan[0] * centre.x + plan[1] * centre.y + plan[2] * centre.z + plan[3] <= -radius )
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
			plan=view.getFrustum(i);
			d = plan[0] * centre.x + plan[1] * centre.y + plan[2] * centre.z + plan[3];
			if ( d <= -radius )
				return 0.0f;
		}
		return d + radius;
	}

	/** Calcule la couverture en pixel de la sphère.
	 * On projette le centre et un point à la périphérie sur l'écran, puis on calcule
	 * l'aire du disque obtenu.
	 */
	float getPixelCoverage(const CCamera &view) const
	{
		CPoint centerSC, periSC;

		view.getSphereCoordinates(centre, radius, centerSC, periSC);
		/* PI.R² */
		return ( M_PI* centerSC.squaredDistanceFrom(periSC));
	}

	void draw(void) const
	{
		glEnable(GL_BLEND);
		glPushMatrix();
		glTranslatef(centre.x, centre.y, centre.z);
		glColor4f(1.0f,0.0f,0.0f,0.3f);
		UGraphicsFn::SolidSphere(radius, 30, 30);
		glPopMatrix();
		glDisable(GL_BLEND);
	}
};

#endif // CBOUNDINGSPHERE_H
