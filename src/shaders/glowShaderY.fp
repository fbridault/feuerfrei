// -*-C-*- // utilisation du mode C dans emacs
#extension GL_ARB_texture_rectangle : enable

uniform float offsets[16];
uniform float weights[16];
uniform float divide;
uniform float scale;
uniform sampler2DRect text;

void main()
{
  vec2 texcoord;
  vec4 color;
  float pos2Dy=gl_FragCoord.y*scale;
  
  texcoord = vec2(gl_FragCoord.x*scale,pos2Dy+offsets[0]);
  color = texture2DRect(text, texcoord)*weights[0];
  
  texcoord.y = pos2Dy+offsets[1];
  color += texture2DRect(text, texcoord)*weights[1];

  texcoord.y = pos2Dy+offsets[2];
  color += texture2DRect(text, texcoord)*weights[2];

  texcoord.y = pos2Dy+offsets[3];
  color += texture2DRect(text, texcoord)*weights[3];

  texcoord.y = pos2Dy+offsets[4];
  color += texture2DRect(text, texcoord)*weights[4];

  texcoord.y = pos2Dy+offsets[5];
  color += texture2DRect(text, texcoord)*weights[5];

  texcoord.y = pos2Dy+offsets[6];
  color += texture2DRect(text, texcoord)*weights[6];

  texcoord.y = pos2Dy+offsets[7];
  color += texture2DRect(text, texcoord)*weights[7];
  
  texcoord.y = pos2Dy+offsets[8];
  color += texture2DRect(text, texcoord)*weights[8];

  texcoord.y = pos2Dy+offsets[9];
  color += texture2DRect(text, texcoord)*weights[9];

  texcoord.y = pos2Dy+offsets[10];
  color += texture2DRect(text, texcoord)*weights[10];

  texcoord.y = pos2Dy+offsets[11];
  color += texture2DRect(text, texcoord)*weights[11];

  texcoord.y = pos2Dy+offsets[12];
  color += texture2DRect(text, texcoord)*weights[12];

  texcoord.y = pos2Dy+offsets[13];
  color += texture2DRect(text, texcoord)*weights[13];

  texcoord.y = pos2Dy+offsets[14];
  color += texture2DRect(text, texcoord)*weights[14];
  
/*   texcoord.y = pos2Dy+offsets[15]; */
/*   color += texture2DRect(text, texcoord)*weights[15]; */

  gl_FragColor = color*divide;
}
