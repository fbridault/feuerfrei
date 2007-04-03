// -*-C-*- // utilisation du mode C dans emacs
#extension GL_ARB_texture_rectangle : enable

uniform float weights[8];
uniform float divide;
uniform samplerRect text;

void main()
{
  gl_FragColor = (texture2DRect(text, gl_TexCoord[0].xy)*weights[0] + texture2DRect(text, gl_TexCoord[1].xy)*weights[1] + texture2DRect(text, gl_TexCoord[2].xy)*weights[2] + texture2DRect(text, gl_TexCoord[3].xy)*weights[3] + texture2DRect(text, gl_TexCoord[4].xy)*weights[4] + texture2DRect(text, gl_TexCoord[5].xy)*weights[5] + texture2DRect(text, gl_TexCoord[6].xy)*weights[6] + texture2DRect(text, gl_TexCoord[7].xy)*weights[7])/divide;
}
