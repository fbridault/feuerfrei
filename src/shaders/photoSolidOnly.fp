// -*-C-*- // utilisation du mode C dans emacs
#version 120

#define PI 3.14159265358979323846
#define DEUX_PI 6.283185307179586
#define R 1.0
#define G 0.5
#define B 0

/********************************************************************************/
varying vec3 normal;
varying vec4 point3D;

uniform sampler3D textureSP;
uniform vec3 centreSP[NBSOURCES];
uniform float fluctuationIntensite[NBSOURCES];
uniform vec2 angles[NBSOURCES];
uniform float incr;

void main()
{
  // calcul de l'intensite d'eclairage stockee dans la texture du solide photometrique
  
  // application de la rotation du SP selon l'inclinaison de la flamme
/*   IN.PT3D = mul(TextureSPMatrix, IN.PT3D); */
  
  vec3 direction,texcoordsSP;
  float theta, phi, r, attenuation, colorSP;
  float rtex=0.0;
  float color=0.0;
  for (int i = 0; i < NBSOURCES; i++) {
    // translation inverse du centre du SP
    direction =  centreSP[i] - point3D.xyz;
    r = length(direction.xyz);
    direction = normalize(direction);
    // passage des coord. cartesiennes (x,y,z) en coord. spheriques (r,theta,phi)
    // attention y est suppose etre la hauteur et z la profondeur
    theta = acos(direction.y / r);
    phi = atan(direction.z,direction.x);
    
    // Les coordonnees de textures doivent etre utilisees en fonction du sens de codage de la texture
    // On divise par la taille max de la texture en 2D (xy) pour avoir entre 0 et 1
    texcoordsSP = vec3(phi * angles[i].x, theta * angles[i].y, rtex);
    
    // recuperation de l'intensite du fragment
    colorSP = texture3D(textureSP, texcoordsSP).r * fluctuationIntensite[i];
    
    // attenuation de l'intensite en fonction de la distance
/*     attenuation = 3.0/r; */
/*     if(attenuation < 1.0) */
/*       colorSP *= attenuation; */
    // attenuation de l'intensite en fonction de la flamme
    color += colorSP;
    rtex += float(incr);
  }
  color = color / float(NBSOURCES);
  
  gl_FragColor = vec4(color,color,color,1.0);
}
