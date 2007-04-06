// -*-C-*- // utilisation du mode C dans emacs
#extension GL_ARB_texture_rectangle : enable

uniform float offsets[8];
uniform float weights[8];
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

  gl_FragColor = color*divide;
}
