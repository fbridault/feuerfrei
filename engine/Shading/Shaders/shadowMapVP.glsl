#ifdef SHADOW_MAP_2D

varying vec4 v_shadowTexCoord;

void main()
{
  gl_Position = ftransform();

  // shadow texture coordinates generation
  v_shadowTexCoord = gl_TextureMatrix[3] * gl_ModelViewMatrix * gl_Vertex;
  //v_shadowTexCoord = gl_TextureMatrix[3] * gl_Vertex;
} // main end

#endif

#ifdef SHADOW_MAP_CUBE

varying vec3 v_vecToLight;
uniform vec3 u_lightCentre;

void main()
{
  gl_Position = ftransform();

  v_vecToLight = u_lightCentre - gl_Vertex.xyz;
} // main end

#endif
