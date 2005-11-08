#ifndef OBJREADER_H
#define OBJREADER_H

class COBJReader;

#include "object.hpp"
#include "scene.hpp"

#include <list>
#include <string>
#include <iostream>

//using namespace std;

/** 
 * Classe pour la lecture d'un fichier OBJ et la cr&eacute;ation d'une sc&egrave;ne 3D correspondante.
 * Cette classe lit &eacute;galement les fichiers MTL qui comportent les mat&eacute;riaux. 
 * Les informations de cam&eacute;ra, et les sources de lumi&egrave;res sont contenues dans un autre fichier.
 *
 * @author	Christophe Cassagnab&egrave;re modifi� par Flavien Bridault
 * @version	%I%, %G%
 * @since	1.0
 */
class COBJReader  
{
public:
  /**
   * Constructeur par d&eacute;faut. 
   * Lit un fichier OBJ pass&eacute; en param&egrave;tres et l'importe dans la sc&egrave;ne.
   * Si object est non nul, on ne charge qu'un seul objet. 
   * Si false est � nul, l'objet ne sera pas compris dans la display list de la sc�ne. En 
   * revanche, il appartiendra bien � la sc�ne et sera supprim� par elle.
   *
   * @param filename nom du fichier OBJ &agrave; importer.
   * @param sceneToImportInto pointeur vers la sc&egrave;ne dans laquelle importer le fichier OBJ.
   * @param object objet dans lequel importer le fichier
   * @param detached permet de sp�cifier si l'objet doit appartenir � la sc�ne ou non
   */
  COBJReader(const char* filename, CScene *sceneToImportInto, CObject* object=NULL, bool detached=false);
  /**
   * Destructeur par d&eacute;faut.
   */
  ~COBJReader();
private:
  /**
   * Lit un fichier MTL pass&eacute; en param&egrave;tres et importe les mat&eacute;riaux dans la sc&egrave;ne.
   */
  void importMaterial(const char* filename, CScene* scene);
};

#endif
