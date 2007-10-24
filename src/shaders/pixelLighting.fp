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

uniform sampler2D textureObjet;
uniform sampler3D textureSP;
uniform vec3 centreSP[NBSOURCES];
uniform float fluctuationIntensite[NBSOURCES];
uniform bool isTextured;

void main()
{
/*   if(dot(IN.normale,IN.PT3D.xyz) > 0.15) */
/*     discard; */
  // Couleur de la flamme (en dur pour l'instant)
  vec4 CouleurFlamme = vec4(R,G,B,1.0);
  
  // application de la rotation du SP selon l'inclinaison de la flamme
  /* IN.PT3D = mul(TextureSPMatrix, IN.PT3D); */
  
  vec4 colorTex;
  // calcul de l'intensite d'eclairage stockee dans la texture du solide photometrique
  if(isTextured)
    // couleur de la texture de l'objet codee dans le fichier OBJ
    colorTex = texture2D(textureObjet, gl_TexCoord[0].xy);
  else
    colorTex = gl_Color;
  
  vec3 point,texcoordsSP,direction;
  float result, r, theta, phi, ColorSP, attenuation, cosSN;
  /* Les normales sont normalisées dans le programme grâce à glEnable (GL_NORMALIZE) */
  /*   vec3 normal1 = normalize(normal); */
  float color=0.0;
  float rtex=0.0;
  float c;
  for (int i = 0; i < NBSOURCES; i++) {
    // Calcul de la direction du point3D vers le centre du SP
    direction = centreSP[i] - point3D.xyz;
    r = length(direction.xyz);
    
/*     // passage des coord. cartesiennes (x,y,z) en coord. spheriques (r,theta,phi) */
/*     // attention y est suppose etre la hauteur et z la profondeur */
    theta = acos(direction.y / r);
    //theta=acos(-direction.y );
    
    direction = normalize(direction);
    // - cos de l'angle entre la direction de la source et la normale au point
    cosSN = dot(direction,normal);
    // attenuation de l'intensite en fonction de l'angle d'eclairage, de la distance et de la taille
    attenuation = 4.0/r;
    ColorSP = theta * cosSN * attenuation * fluctuationIntensite[i];
    
    color += ColorSP;
  }
  color = color / float(NBSOURCES);
  // melange de la couleur de l'objet, de sa texture et de l'intensite
  gl_FragColor = colorTex * vec4(color,color,color,1.0) * CouleurFlamme;
}
