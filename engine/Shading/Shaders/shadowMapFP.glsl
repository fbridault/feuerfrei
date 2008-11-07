
#ifdef SHADOW_MAP_2D

varying vec4 v_shadowTexCoord;
uniform sampler2D u_shadowMap;

#ifdef DEPTH_MAP
void main()
{
  gl_FragColor=texture2DProj(u_shadowMap, v_shadowTexCoord);
}
#endif

#ifdef DISTANCE_MAP
void main()
{
  gl_FragColor=vec4(1,1,1,1)*v_shadowTexCoord.z/v_shadowTexCoord.w;
}
#endif

#endif

#ifdef SHADOW_MAP_CUBE

varying vec3 v_vecToLight;

void main()
{
  // On écrit la distance au carré
  //vec4 vpack=vec4(1.0, 256.0, 65536.0, 16777216.0);
  //gl_FragColor = vpack * dot(vecToLight,vecToLight);
  float depth= length(v_vecToLight);//dot(vecToLight,vecToLight);
  gl_FragColor = vec4(floor(depth)/255.0, fract(depth), fract(depth), fract(depth));

  //gl_FragColor = vec4(depth, depth, depth, depth);
}

#endif
