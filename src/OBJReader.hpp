/* OBJReader.h: interface for the COBJReader class.*/

#ifndef OBJREADER_H
#define OBJREADER_H

//#pragma warning(disable:4786)
//#pragma warning(disable:4243)

class COBJReader;

#include "object.hpp"
#include "scene.hpp"

extern void importOBJFile2Scene (const char *filename, CObject * sceneToImportInto);
extern void importOBJFile2Scene (const char *filename, CScene* sceneToImportInto);

#include <list>
#include <string>
#include <iostream>
using namespace std;

/** 
 * Classe pour la lecture d'un fichier OBJ et la cr&eacute;ation d'une sc&egrave;ne 3D correspondante.
 * Cette classe lit &eacute;galement les fichiers MTL qui comportent les mat&eacute;riaux. 
 * Les informations de cam&eacute;ra, et les sources de lumi&egrave;res sont contenues dans un autre fichier.
 *
 * @author	Christophe Cassagnab&egrave;re modifié par Flavien Bridault
 * @version	%I%, %G%
 * @since	1.0
 */
class COBJReader  
{
private:
  list<string> mlist;/**< Liste des identifiants de mat&eacute;riaux.*/
  list<string>::iterator miter;/**< It&eacute;rateur sur la liste de identifiants*/
  
  list<CMaterial*> matlist;/**< Liste des mat&eacute;riaux.*/
  list<CMaterial*>::iterator matiter;/**< It&eacute;rateur sur la liste des mat&eacute;riaux.*/

  /**
   * Lit un fichier MTL pass&eacute; en param&egrave;tres et importe les mat&eacute;riaux dans la sc&egrave;ne.
   */
  void importMaterial(const char* filename);

public:
  /**
   * Constructeur par d&eacute;faut. 
   * Lit un fichier OBJ pass&eacute; en param&egrave;tres et l'importe dans la sc&egrave;ne.
   *
   * @param filename nom du fichier OBJ &agrave; importer.
   * @param sceneToImportInto pointeur vers la sc&egrave;ne dans laquelle importer le fichier OBJ.
   */
  COBJReader(const char* filename, CScene *sceneToImportInto);
  /**
   * Constructeur par d&eacute;faut. 
   * Lit un fichier OBJ pass&eacute; en param&egrave;tres et l'importe dans la sc&egrave;ne.
   *
   * @param filename nom du fichier OBJ &agrave; importer.
   * @param sceneToImportInto pointeur vers la sc&egrave;ne dans laquelle importer le fichier OBJ.
   */
  COBJReader(const char* filename, CObject *objectToImportInto);
  /**
   * Destructeur par d&eacute;faut.
   */
  ~COBJReader(){mlist.clear();matlist.clear();};
};

#endif
