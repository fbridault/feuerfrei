#include "glowengine.hp"


CRayEngine::CRayEngine() : pbuffer("float=32 depth"),  _iWidth(TAILLE_PBFX), _iHeight(TAILLE_PBFY)
{
  // intitialisatio glut et CG
  init_glutnCg();

  // Activer le pbuffer
  pbuffer.Activate();
  
  // Charger les programmes Cg
        
  // Desactiver le pbuffer
  pbuffer.Deactivate();
}

CRayEngine::~CRayEngine()
{
  cgDestroyProgram(_vertexProgram);
  cgDestroyProgram(_fragmentProgram);
  glutDestroyWindow( win );
}

void CRayEngine::init_glutnCg() 
{
  // Initialiser le pbuffer maintenant que nous avons un contexte valide
  // a utiliser pour la creation du pbuffer
  pbuffer.Initialize(TAILLE_PBFX, TAILLE_PBFY, false, false);
  // Initialisations pour le contexte de rendu du pbuffer
  // Activer le pbuffer
  pbuffer.Activate();
  // Couleur pour l'effacement
  glClearColor( 0.0, 0.0, 0.0, 0.0 );
  // Parametres de visualisation
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glOrtho( 0.0, 1.0, 0.0, 1.0, -1.0, 1.0 );
  
  // Activer le zbuffer
  glEnable(GL_DEPTH_TEST);
  // Parametre de visualisation
  glMatrixMode(GL_PROJECTION);    
  glLoadIdentity();               
  gluOrtho2D(-1, 1, -1, 1);       
  glMatrixMode(GL_MODELVIEW);     
  glLoadIdentity();               
  
  pbuffer.Deactivate();
};


void CRayEngine::coder_rayons(s_node<CRay>* _raysArray, int size)//CBspcell *cell)
{
  mypbuffer.Activate();
  // Generation de 2 textures pour stocker les origines et les directions des rayons
  // Le format interne GL_FLOAT_RGB32_NV code 3 reels de 32bits
  glGenTextures(2, _iTexture);

  // Creation de la texture des origines
  // test rayons primaires : remplissage de la texture avec la meme origine (0,0,0)
  //   for (int i = 0; i < _iWidth*_iHeight*3; i++)
  //     imgdata[i] = 0.0;
	
  int pos=0;

  //	int imax = cell->_raysArray.size();
  //	imax = imax < (_iWidth*_iHeight)  ? imax :(_iWidth*_iHeight);
  for (int i = 0; i < size; i++)//imax; i++)
    {
      CPoint org (_raysArray->_data[i].getOrigin());//cell->_raysArray.select(i)->getOrigin());
      CVector dir (_raysArray->_data[i].getDirection());//cell->_raysArray.select(i)->getDirection());

      imgdata[pos] = (float)org.getX();
      imgdata2[pos++] = (float)dir.getX();
      imgdata[pos] = (float)org.getY();
      imgdata2[pos++] = (float)dir.getY();
      imgdata[pos] = (float)org.getZ();
      imgdata2[pos++] = (float)dir.getZ();

      //		cout << dir.getX() << " " << dir.getY() << " " << dir.getZ()<< "\n";
      //		cout << imgdata2[i*3] << " " << imgdata2[i*3+1] << " " << imgdata2[i*3+2]<< "\n"; 
    }

  glBindTexture(GL_TEXTURE_RECTANGLE_NV, _iTexture[0]);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_RGB32_NV, _iWidth, _iHeight, 0, GL_RGB, GL_FLOAT, imgdata);

  // Creation de la texture des directions
  // test rayons primaires : remplissage de la texture avec des directions primaires
  // utilisation d'une double boucle pour simplifier le test ...
  /*	int pos=0;
    for (int y = 0; y < _iHeight; y++)
    for (int x = 0; x < _iWidth; x++)
    {
    imgdata[pos++] = (float)x/_iWidth - 0.5;
    imgdata[pos++] = (float)y/_iHeight - 0.5;
    //imgdata[pos++] = 0.0;
    //imgdata[pos++] = 0.0;
    imgdata[pos++] = -1.0;
    }
  */
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, _iTexture[1]);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_RGB32_NV, _iWidth, _iHeight, 0, GL_RGB, GL_FLOAT, imgdata2);
  mypbuffer.Deactivate();
}

float * CRayEngine::calculer_intersections(CBspcell *cell)
{ 
  mypbuffer.Activate();
  // effacer le tampon d'image et le tampon de profondeur
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // Creation d'un viewport de la meme taille que les textures de rayons
  glViewport(0, 0, _iWidth, _iHeight);
        
  // Activer le vertex program de calcul d'intersections
  cgGLBindProgram(_vertexProgram);
  cgGLEnableProfile(cgvpProfile);
            
  // Activer le fragment program de calcul d'intersections
  cgGLBindProgram(_fragmentProgram);
  cgGLEnableProfile(cgfpProfile);
            
  // Lier les textures au fragment program
  cgGLSetTextureParameter(_paramTextureOrigines, _iTexture[0]); // origines
  cgGLEnableTextureParameter(_paramTextureOrigines);
  cgGLSetTextureParameter(_paramTextureDirections, _iTexture[1]); // directions
  cgGLEnableTextureParameter(_paramTextureDirections);

  glBegin(GL_QUADS);
  //	cout << glGetError();
  {
    int ind1=0;
	  
    //for (int A = 0; A < 100; A++) {
    cell->_geometricalEntitiesArray.begin();
    while (!cell->_geometricalEntitiesArray.finished()) {
      int tempMax = cell->_geometricalEntitiesArray.current_bloc_size();
      for (int i = 0; i<tempMax; i++){
	CFacet** geometricalEntitiesIterator = cell->_geometricalEntitiesArray.select(i);
	
	// mise en place des parametres du 1er triangle a passer aux 4 points du quadrilatere
	cgGLSetParameter1f(_paramIndexTriangle,(float)ind1++);
	cgGLSetParameter3f(_paramPointA, (float)(*geometricalEntitiesIterator)->vertex[0].getX(), (float)(*geometricalEntitiesIterator)->vertex[0].getY(), (float)(*geometricalEntitiesIterator)->vertex[0].getZ()); // precision coordonnees textures 32bits par composante OK
	cgGLSetParameter3f(_paramPointB, (float)(*geometricalEntitiesIterator)->vertex[1].getX(), (float)(*geometricalEntitiesIterator)->vertex[1].getY(), (float)(*geometricalEntitiesIterator)->vertex[1].getZ());
	cgGLSetParameter3f(_paramPointC, (float)(*geometricalEntitiesIterator)->vertex[2].getX(), (float)(*geometricalEntitiesIterator)->vertex[2].getY(), (float)(*geometricalEntitiesIterator)->vertex[2].getZ());
	cgGLSetParameter3f(_paramNormale,(float)(*geometricalEntitiesIterator)->geometricalNormal.getX(), (float)(*geometricalEntitiesIterator)->geometricalNormal.getY(), (float)(*geometricalEntitiesIterator)->geometricalNormal.getZ());
	// quadrilatere + coordonnees de texture permettant d'acceder aux rayons
	glTexCoord2f(0, 0); glVertex3f(-1.0f, -1.0f, -0.5f);
	glTexCoord2f(_iWidth, 0); glVertex3f( 1.0f, -1.0f, -0.5f);
	glTexCoord2f(_iWidth, _iHeight); glVertex3f( 1.0f,  1.0f, -0.5f);
	glTexCoord2f(0, _iHeight); glVertex3f(-1.0f,  1.0f, -0.5f);
      }
      cell->_geometricalEntitiesArray++;
    };
    //}

  }
  glEnd();
    
  // Desactiver le fragment program de calcul d'intersections
  // et les textures associees
  cgGLDisableTextureParameter(_paramTextureOrigines);
  cgGLDisableTextureParameter(_paramTextureDirections);
  cgGLDisableProfile(cgfpProfile);
  cgGLDisableProfile(cgvpProfile);

  // Lire les donnees a partir du pbuffer dans le tableau imgdata pour exploitation
  glReadPixels( 0, 0, TAILLE_PBFX, TAILLE_PBFY, GL_RGBA, GL_FLOAT, imgdataout );
  //	printf("rgba = %f %f %f %f\n",imgdataout[68*4],imgdataout[68*4+1],imgdataout[68*4+2], imgdataout[68*4+3]);
  
  // Ou alors :
  // Recuperation du frame buffer dans une texture pour reutilisation dans
  // un autre programme Cg ou pour affichage
  // glBindTexture(GL_TEXTURE_2D, _iTexture[0]);
  // glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, _iWidth, _iHeight);

  mypbuffer.Deactivate();
  return imgdataout;
}



void CRayEngine::display()
{
  // Utilisation du tableau imgdata pour afficher le resultat
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glRasterPos2i( 0, 0 );
  glDrawPixels( TAILLE_PBFX, TAILLE_PBFY, GL_RGB, GL_FLOAT, imgdata );

  // Ou alors :
  // Utilisation de la texture dans laquelle les resultats ont ete recuperes
  // glBindTexture(GL_TEXTURE_2D, _iTexture[0]);
  // glEnable(GL_TEXTURE_2D);

  // afficher un carre texture en plein ecran pour voir le resultat
  //     glBegin(GL_QUADS);
  //     {
  //       glTexCoord2f(0, 0); glVertex3f(-1, -1, -0.5f);
  //       glTexCoord2f(1, 0); glVertex3f( 1, -1, -0.5f);
  //       glTexCoord2f(1, 1); glVertex3f( 1,  1, -0.5f);
  //       glTexCoord2f(0, 1); glVertex3f(-1,  1, -0.5f);
  //     }
  //     glEnd();
}

