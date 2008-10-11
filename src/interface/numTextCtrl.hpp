#ifndef NUMTEXTCTRL_H
#define NUMTEXTCTRL_H

class NumTextCtrl;
class LongTextCtrl;
class DoubleTextCtrl;


#include "interface.hpp"
#include "../common.hpp"

/** Super classe pour les classes sp�cialis�es LongTextCtrl et DoubleTextCtrl,
 * elle permet de factoriser les fonctions de retour d'erreur.<hr>
 * Globalement les classes h�rit�es permettent de d�finir un comportement sp�cifique
 * lors de la r�cup�ration de la valeur du contr�le, notamment afin de v�rifier que celle-ci
 * rentre dans un intervalle sp�cifi�.
 */
class NumTextCtrl : public wxTextCtrl
{
public:
  NumTextCtrl(wxWindow* parent, wxWindowID id, const wxString& value = _(""), const wxPoint& pos = wxDefaultPosition,
	      const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator,
	      const wxString& name = wxTextCtrlNameStr) :
    wxTextCtrl(parent, id, value, pos, size, style, validator, name)
  {};
  virtual ~NumTextCtrl(){};
protected:
  virtual void nonNumericErrorDialog(wxString& s) = 0;
  virtual void rangeErrorDialog(wxString& s) = 0;
};

/** Contr�le texte num�rique de type long */
class LongTextCtrl : public NumTextCtrl
{
public:
  LongTextCtrl(wxWindow* parent, wxWindowID id, long min, long max, const wxString& value = _(""),
	      const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
	      const wxValidator& validator = wxDefaultValidator, const wxString& name = wxTextCtrlNameStr);
  virtual ~LongTextCtrl(){};

  /* Renvoit la valeur du contr�le en prenant en compte les erreurs (type, intervalle)
   * et ouvre une bo�te de notification de l'erreur le cas �ch�ant. Dans ce
   * dernier cas, la fonction l�ve une exception de type long, permettant de simplifier
   * les tests d'erreur dans une fonction faisant appel � cette fonction de nombreuses fois.
   */
  virtual long GetSafelyValue();
  virtual void nonNumericErrorDialog(wxString& s);
  virtual void rangeErrorDialog(wxString& s);

private:
  long m_min, m_max;
};

/** Contr�le texte num�rique de type float */
class DoubleTextCtrl : public NumTextCtrl
{
public:
  DoubleTextCtrl(wxWindow* parent, wxWindowID id, float min, float max, const wxString& value = _(""),
	      const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
	      const wxValidator& validator = wxDefaultValidator, const wxString& name = wxTextCtrlNameStr);
  virtual ~DoubleTextCtrl(){};

  /* Renvoit la valeur du contr�le en prenant en compte les erreurs (type, intervalle)
   * et ouvre une bo�te de notification de l'erreur le cas �ch�ant. Dans ce
   * dernier cas, la fonction l�ve une exception de type float, permettant de simplifier
   * les tests d'erreur dans une fonction faisant appel � cette fonction de nombreuses fois.
   */
  virtual float GetSafelyValue();
  virtual void nonNumericErrorDialog(wxString& s);
  virtual void rangeErrorDialog(wxString& s);

private:
  float m_min, m_max;
};

#endif
