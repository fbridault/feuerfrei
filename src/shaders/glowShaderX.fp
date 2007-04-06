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
  float pos2Dx=gl_FragCoord.x*scale;
  
  texcoord = vec2(pos2Dx+offsets[0],gl_FragCoord.y*scale);
  color = texture2DRect(text, texcoord)*weights[0];
  
  texcoord.x = pos2Dx+offsets[1];
  color += texture2DRect(text, texcoord)*weights[1];

  texcoord.x = pos2Dx+offsets[2];
  color += texture2DRect(text, texcoord)*weights[2];

  texcoord.x = pos2Dx+offsets[3];
  color += texture2DRect(text, texcoord)*weights[3];

  texcoord.x = pos2Dx+offsets[4];
  color += texture2DRect(text, texcoord)*weights[4];

  texcoord.x = pos2Dx+offsets[5];
  color += texture2DRect(text, texcoord)*weights[5];

  texcoord.x = pos2Dx+offsets[6];
  color += texture2DRect(text, texcoord)*weights[6];

  texcoord.x = pos2Dx+offsets[7];
  color += texture2DRect(text, texcoord)*weights[7];
  
  texcoord.x = pos2Dx+offsets[8];
  color += texture2DRect(text, texcoord)*weights[8];

  texcoord.x = pos2Dx+offsets[9];
  color += texture2DRect(text, texcoord)*weights[9];

  texcoord.x = pos2Dx+offsets[10];
  color += texture2DRect(text, texcoord)*weights[10];

  texcoord.x = pos2Dx+offsets[11];
  color += texture2DRect(text, texcoord)*weights[11];

  texcoord.x = pos2Dx+offsets[12];
  color += texture2DRect(text, texcoord)*weights[12];

  texcoord.x = pos2Dx+offsets[13];
  color += texture2DRect(text, texcoord)*weights[13];

  texcoord.x = pos2Dx+offsets[14];
  color += texture2DRect(text, texcoord)*weights[14];

/*   texcoord.x = pos2Dx+offsets[15]; */
/*   color += texture2DRect(text, texcoord)*weights[15]; */
  
  gl_FragColor = color*divide;
}
