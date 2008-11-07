#version 120

/************************************************************************************/
/******* Paramètres omni-spots communs pour forward et deferred shading *************/
/************************************************************************************/

uniform vec3  u_intensity;
uniform bool  u_shadowsEnabled;
uniform float u_mapSize;

#ifdef OMNI
//varying vec3  v_vFilter[6];
varying vec3 v_worldVecToLight;
uniform samplerCube u_shadowMap;


/** Eclairage pour une source omnidirectionnelle */
vec4 omniLighting(in vec3 vecToLight, in vec3 worldVecToLight, in vec3 normal, in vec3 intensity, in samplerCube shadowMap)
{
  float   r, attenuation, cosSN;
  vec4    color;
  vec3    direction;
  float   squaredDistance, depth, sdepth;//, fShadow=0.0;
  vec4    shadowSample;

  // attenuation de l'intensite en fonction de l'angle d'eclairage, de la distance et de la taille
  r = length(vecToLight);
  direction = normalize(vecToLight);

  // - cos de l'angle entre la direction de la source et la normale au point
  cosSN = dot(direction,normal);

  if (cosSN > 0)
  {
    attenuation = clamp(1.0/(r*r),0.0,1.0);
    color = vec4(intensity.xyz * cosSN * attenuation,1.0);

    if (u_shadowsEnabled)
    {
      squaredDistance = r;//dot(vecToLight[0],vecToLight[0]);
      depth = squaredDistance*.9;

      //for (int i=0; i<6; i++)
      //  vecToLight[i+1] = vecToLight[0]+vFilter[i]*mapSize;

//      for (int i = 0; i < 4; i++)
//      {
//        shadowSample = textureCube(shadowMap, -vecToLight[i]);
//        sdepth = shadowSample.r * 255.0 + shadowSample.g;
//        fShadow += (depth - sdepth < 0.0f) ? 0.25f : 0.0f;
//      }
//      color *= fShadow;

      shadowSample = textureCube(shadowMap, worldVecToLight);
      sdepth = shadowSample.r * 255.0 + shadowSample.g;
      color *= ((depth - sdepth < 0.0) ? 1.0 : 0.0);

//      color *= ((depth - dot(shadowSample,vunpack) < 0.0) ? 1.0 : 0.0);
    }
  }

  return color;
}

#endif

#ifdef SPOT
uniform vec3  u_spotDirection;  // Normalisé au préalable dans la classe source
uniform vec2  u_texmapscale;
uniform float u_spotCutoff;
uniform sampler2DShadow u_shadowMap;

const float decay=20.0;

/** PCF pour une shadow map 2D */
float offset_lookup(in sampler2DShadow map, in vec4 loc, in vec2 offset, in vec2 texmapscale)
{
  return shadow2DProj(map, vec4(loc.xy + offset * texmapscale * loc.w, loc.z, loc.w)).r;
}

/** Eclairage pour une source ponctuelle */
vec4 spotLighting(in vec3 vecToLight, in vec3 normal, in vec3 intensity, in vec4 shadowTexCoord, in sampler2DShadow shadowMap)
{
  float   r, attenuation, cosSN;
  vec4    color;
  vec3    direction;
  float   sum=0.0;
  float   x,y;

  r = length(vecToLight);
  direction = normalize(vecToLight);

  // - cos de l'angle entre la direction de la source et la normale au point
  cosSN = dot(direction,normal);

  if (cosSN > 0)
  {
    float spotEffect = dot(u_spotDirection, -direction);
    if (spotEffect > u_spotCutoff)
    {
      attenuation = clamp(pow(spotEffect,decay)/r,0.0,1.0);
      color = vec4(intensity.xyz,0.0) * cosSN * attenuation;

//    vec2 offset;
//
//    if (shadowsEnabled){
//      offset.x = float(fract(gl_FragCoord.x * 0.5) > 0.25); // mod
//      offset.y = float(fract(gl_FragCoord.y * 0.5) > 0.25); // mod
//      offset.y += offset.x; // y ^= x
//      if (offset.y > 1.1)
//        offset.y = 0;
//
//      color *= (offset_lookup(shadowMap, shadowTexCoord, offset + vec2(-1.5,0.5),texmapscale) +
//                   offset_lookup(shadowMap, shadowTexCoord, offset + vec2(0.5,0.5),texmapscale) +
//                   offset_lookup(shadowMap, shadowTexCoord, offset + vec2(-1.5,-1.5),texmapscale) +
//                   offset_lookup(shadowMap, shadowTexCoord, offset + vec2(0.5,-1.5),texmapscale)) * .25;
//    }
      if (u_shadowsEnabled)
      {
        for (y = -0.5; y <= 0.5; y+=1.0)
          for (x = -0.5; x <= 0.5; x+=1.0)
            sum += offset_lookup(shadowMap,shadowTexCoord,vec2(x,y),u_texmapscale);
        color *= (sum * 0.25);
      }
//      if (u_shadowsEnabled)
//        color *= shadow2DProj(shadowMap, shadowTexCoord).r;
    }
  }
  return color;
}

#endif


#ifndef DEFERRED_SHADING
varying vec3 v_normal;
varying vec3 v_vecToLight;

#ifdef OMNI

void main()
{
  gl_FragColor = omniLighting(v_vecToLight, v_worldVecToLight, v_normal, u_intensity, u_shadowMap);
}

#endif //OMNI


#ifdef SPOT
varying vec4 v_shadowTexCoord;

void main()
{
  gl_FragColor = spotLighting(v_vecToLight, v_normal, u_intensity, v_shadowTexCoord, u_shadowMap);
}

#endif //SPOT


#endif //DEFERRED_SHADING
