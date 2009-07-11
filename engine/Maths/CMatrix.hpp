#ifndef MATRIX_H
#define MATRIX_H

#include "../Common.hpp"
#include "CPoint.hpp"
#include <string.h>

 static const GLfloat _s_afMatrixIdentity[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

class CMatrix
{
public:

	CMatrix()
	{
		memcpy(m_afVal, _s_afMatrixIdentity, 16 * sizeof(GLfloat));
	}

	CMatrix(CPoint const& a_rPosition, CPoint const& a_rScale)
	{
		memcpy(m_afVal, _s_afMatrixIdentity, 16 * sizeof(GLfloat));
		m_afVal[0] = a_rScale.x;
		m_afVal[5] = a_rScale.y;
		m_afVal[10] = a_rScale.z;

		m_afVal[12] = a_rPosition.x;
		m_afVal[13] = a_rPosition.y;
		m_afVal[14] = a_rPosition.z;
	}

	CMatrix(CPoint const& a_rPosition)
	{
		memcpy(m_afVal, _s_afMatrixIdentity, 16 * sizeof(GLfloat));
		m_afVal[12] = a_rPosition.x;
		m_afVal[13] = a_rPosition.y;
		m_afVal[14] = a_rPosition.z;
	}


	CMatrix(GLfloat const a_afValues[16])
	{
		memcpy(m_afVal, a_afValues, 16 * sizeof(GLfloat));
	}

	CMatrix operator*(CMatrix const& a_rMat)
	{
		CMatrix oMatrix;

		// For each row
		ForEachUInt(i, 4)
		{
			// For each column
			ForEachUInt(j, 4)
			{
				oMatrix.m_afVal[i+j*4] = 	a_rMat.m_afVal[i] 		* 	m_afVal[j*4 + 0] +
											a_rMat.m_afVal[i+4] 	* 	m_afVal[j*4 + 1] +
											a_rMat.m_afVal[i+8] 	* 	m_afVal[j*4 + 2] +
											a_rMat.m_afVal[i+12] 	* 	m_afVal[j*4 + 3];
			}
		}
		return oMatrix;

//		// First column
//		m_afVal[0]  = m_afVal[]*a_rMat.m_afVal[0] + m_afVal[]*a_rMat.m_afVal[1] + m_afVal[]*a_rMat.m_afVal[2] + m_afVal[]*a_rMat.m_afVal[3];
//		m_afVal[1]  = m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[];
//		m_afVal[2]  = m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[];
//		m_afVal[3]  = m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[];
//
//		// Second column
//		m_afVal[4]  = m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[];
//		m_afVal[5]  = m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[];
//		m_afVal[6]  = m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[];
//		m_afVal[7]  = m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[];
//
//		// Third column
//		m_afVal[8]  = m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[];
//		m_afVal[9]  = m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[];
//		m_afVal[10] = m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[];
//		m_afVal[11] = m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[];
//
//		// Last column
//		m_afVal[12] = m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[];
//		m_afVal[13] = m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[];
//		m_afVal[14] = m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[];
//		m_afVal[15] = m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[] + m_afVal[]*a_rMat.m_afVal[];

	}

	operator GLfloat const* () const
	{	return m_afVal;
	}

	CPoint GetTranslation() const
	{
		CPoint oPoint(m_afVal[12], m_afVal[13], m_afVal[14]);
		return oPoint;
	}

	void SetTranslation(CPoint const& a_rTranslationDir)
	{
		m_afVal[12] = a_rTranslationDir.x;
		m_afVal[13] = a_rTranslationDir.y;
		m_afVal[14] = a_rTranslationDir.z;
	}

	void Translate(CPoint const& a_rTranslationDir)
	{
		m_afVal[12] += a_rTranslationDir.x;
		m_afVal[13] += a_rTranslationDir.y;
		m_afVal[14] += a_rTranslationDir.z;
	}

	CPoint GetScale() const
	{
		CPoint oScale(m_afVal[0], m_afVal[5], m_afVal[10]);
		return oScale;
	}

private:

//---------------------------------------------------------------------------------------------------------------------
//  Attributes
//---------------------------------------------------------------------------------------------------------------------

	GLfloat m_afVal[16];
};

static const CMatrix s_oMatrixIdentity(_s_afMatrixIdentity);

#endif // MATRIX_H
