#extension GL_ARB_texture_rectangle : enable

#ifdef GBUFFER_GEN

/************************************************************************************/
/************************** Empaquetage données G-Buffer ****************************/
/************************************************************************************/
#extension GL_ARB_draw_buffers : enable

/********************************************************************************/
varying vec4 v_normal;
varying vec4 v_position;

uniform bool      u_textureEnabled;
uniform sampler2D u_textureObjet;

void main()
{
  // Composante diffuse dans la première texture
  if (u_textureEnabled)
    // couleur de la texture de l'objet codee dans le fichier OBJ
    gl_FragData[0] = texture2D(u_textureObjet, gl_TexCoord[0].xy);
  else
    gl_FragData[0] = gl_Color;
  gl_FragData[1] = v_normal; // * 0.5 + 0.5; //Normalement déjà normalisé
  gl_FragData[2] = v_position;
  //gl_FragData[2] = vec4(gl_FragDepth.xxxx);
}

#else

/************************************************************************************/
/************************** Dépaquetage données G-Buffer ****************************/
/************************************************************************************/

vec3 unpackNormal(in sampler2DRect tex)
{
  return (texture2DRect( tex, gl_FragCoord.xy).xyz);//*2.0-1.0;
}

vec4 unpackPosition(in sampler2DRect tex)
{
  return  texture2DRect( tex, gl_FragCoord.xy);
}

vec4 unpackColor(in sampler2DRect tex)
{
  return  texture2DRect( tex, gl_FragCoord.xy);
}

vec4 unpackPositionFromDepth(in sampler2DRect tex)
{
  float zdepth = texture2DRect(tex, gl_FragCoord.xy).x;
  return  gl_ModelViewProjectionMatrixInverse * vec4(gl_FragCoord.xy, zdepth, 0);
}

/************************************************************************************/
/********************************** Eclairage ambiant *******************************/
/************************************************************************************/

#ifdef AMBIENT
uniform sampler2DRect u_colorsTex;

void main()
{
  gl_FragColor = texture2DRect(u_colorsTex, gl_FragCoord.xy);
}
#else

/************************************************************************************/
/**************************** Paramètres communs omni-spots *************************/
/************************************************************************************/

uniform sampler2DRect u_normalsTex;
uniform sampler2DRect u_positionsTex;
uniform vec3 u_lightCentre;

#endif

/************************************************************************************/
/********************************** Eclairage omni **********************************/
/************************************************************************************/
#ifdef OMNI

uniform vec3 u_worldLightCentre;

void main()
{
  vec4 position = unpackPosition(u_positionsTex);
  gl_FragColor = omniLighting(u_lightCentre - position.xyz, -u_worldLightCentre + (gl_TextureMatrix[3]*position).xyz,
                              unpackNormal(u_normalsTex), u_intensity, u_shadowMap);
}

#endif


/************************************************************************************/
/********************************** Eclairage spot **********************************/
/************************************************************************************/
#ifdef SPOT

void main()
{
  vec4 position = unpackPosition(u_positionsTex);
  vec3 vecToLight = u_lightCentre - position.xyz;
  vec4 shadowTexCoord = gl_TextureMatrix[3] * position;
  //vec4 shadowTexCoord = gl_TextureMatrix[3] * gl_ModelViewMatrix * position;
  gl_FragColor = spotLighting(vecToLight, unpackNormal(u_normalsTex), u_intensity, shadowTexCoord, u_shadowMap);
}
#endif

#endif
