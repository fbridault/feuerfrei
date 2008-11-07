#version 120

varying vec3 v_normal;
varying vec3 v_vecToLight;
uniform vec3 u_lightCentre;

void setAttributesForPixelLighting()
{
  gl_Position       = ftransform();
  gl_FrontColor     = gl_Color;
  gl_TexCoord[0].xy = gl_MultiTexCoord0.xy;
  v_vecToLight      = u_lightCentre - (gl_ModelViewMatrix * gl_Vertex).xyz;
  v_normal          = normalize(gl_NormalMatrix*gl_Normal);
}

#ifdef OMNI

uniform vec3 u_worldLightCentre;
varying vec3 v_worldVecToLight;

void main()
{
  setAttributesForPixelLighting();
  v_worldVecToLight = -(u_worldLightCentre - gl_Vertex.xyz);
}

#endif

#ifdef SPOT
varying vec4 v_shadowTexCoord;

void main()
{
  setAttributesForPixelLighting();
  v_shadowTexCoord = gl_TextureMatrix[3] * gl_ModelViewMatrix * gl_Vertex;
}

#endif
