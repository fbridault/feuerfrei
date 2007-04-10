// -*-C-*- // utilisation du mode C dans emacs
#extension GL_ARB_texture_rectangle : enable

uniform float scale;
uniform sampler2DRect text;

void main()
{
  vec2 texcoord = vec2(gl_FragCoord.x*scale,gl_FragCoord.y*scale);
  gl_FragColor=texture2DRect(text, texcoord);
}
