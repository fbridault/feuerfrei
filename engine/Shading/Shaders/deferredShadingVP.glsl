
#ifdef GBUFFER_GEN

varying vec4 v_normal;
varying vec4 v_position;

void main()
{
  gl_Position = ftransform();
  gl_FrontColor = gl_Color;
  gl_TexCoord[0].xy = gl_MultiTexCoord0.xy;
  v_position = gl_ModelViewMatrix * gl_Vertex;
  v_normal = vec4(normalize(gl_NormalMatrix * gl_Normal).xyz,0.0);
} // main end

#else

/** Vertex shader minimal pour le deferred shading, tous les calculs
 *   sont effectués au niveau des fragments.
 */
void main()
{
  gl_Position = ftransform();
}

#endif
