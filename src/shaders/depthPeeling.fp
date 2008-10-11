uniform sampler2D s_textureObjet;

uniform sampler2DRectShadow s_prevDepth;
uniform sampler2DRectShadow s_sceneDepth;

void main()
{
	// Test de profondeur sur la passe précédente
	float fdepth1 = shadow2DRect(s_prevDepth, gl_FragCoord).r;
	if(fdepth1 <= 0) discard;

	// Test de profondeur sur le reste de la scène
	float fdepth2 = shadow2DRect(s_sceneDepth, gl_FragCoord).r;
	if(fdepth2 <= 0) discard;

	// Texture de couleur "normale"
	gl_FragColor = texture2D(s_textureObjet, gl_TexCoord[0]);
}
