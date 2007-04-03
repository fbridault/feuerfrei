// -*-C-*- // utilisation du mode C dans emacs
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect text;
uniform float gamma;

void main()
{  
  vec4 color;
  color=pow(texture2DRect(text,gl_TexCoord[0].st), gamma);
  gl_FragColor = color;
}
