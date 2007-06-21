/*
** SSE4.hpp
** 
** Made by Michel Leblond
** Login   <michel@localhost.localdomain>
** 
** Started on  Mon Oct  2 18:17:56 2006 Michel Leblond
** Last update Mon Oct  2 18:17:56 2006 Michel Leblond
*/

#ifndef   	SSE4_HPP
# define   	SSE4_HPP

#include <xmmintrin.h>

typedef	union{
		__m128 m;
		float f[4];
} SSE4f;

// union SSE4d{
// 	__m128d m;
// 	double f[2];
// };

#endif 	    /* !SSE4_HPP */
