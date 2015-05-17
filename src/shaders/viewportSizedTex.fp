// -*-C-*- // utilisation du mode C dans emacs
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect text;

void main()
{
  gl_FragColor = texture2DRect(text, vec2(gl_FragCoord.x,gl_FragCoord.y));
}
