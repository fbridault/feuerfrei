// -*-C-*- // utilisation du mode C dans emacs
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect text;
uniform float gamma;

void main()
{  
  gl_FragColor = pow(texture2DRect(text,gl_TexCoord[0].st), vec4(gamma,gamma,gamma,gamma));
}
