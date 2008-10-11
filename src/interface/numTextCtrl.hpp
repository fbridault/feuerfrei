#ifndef NUMTEXTCTRL_H
#define NUMTEXTCTRL_H

class NumTextCtrl;
class LongTextCtrl;
class DoubleTextCtrl;


#include "interface.hpp"
#include "../common.hpp"

/** Super classe pour les classes spécialisées LongTextCtrl et DoubleTextCtrl,
 * elle permet de factoriser les fonctions de retour d'erreur.<hr>
 * Globalement les classes héritées permettent de définir un comportement spécifique
 * lors de la récupération de la valeur du contrôle, notamment afin de vérifier que celle-ci
 * rentre dans un intervalle spécifié.
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

/** Contrôle texte numérique de type long */
class LongTextCtrl : public NumTextCtrl
{
public:
  LongTextCtrl(wxWindow* parent, wxWindowID id, long min, long max, const wxString& value = _(""),
	      const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
	      const wxValidator& validator = wxDefaultValidator, const wxString& name = wxTextCtrlNameStr);
  virtual ~LongTextCtrl(){};

  /* Renvoit la valeur du contrôle en prenant en compte les erreurs (type, intervalle)
   * et ouvre une boîte de notification de l'erreur le cas échéant. Dans ce
   * dernier cas, la fonction lève une exception de type long, permettant de simplifier
   * les tests d'erreur dans une fonction faisant appel à cette fonction de nombreuses fois.
   */
  virtual long GetSafelyValue();
  virtual void nonNumericErrorDialog(wxString& s);
  virtual void rangeErrorDialog(wxString& s);

private:
  long m_min, m_max;
};

/** Contrôle texte numérique de type float */
class DoubleTextCtrl : public NumTextCtrl
{
public:
  DoubleTextCtrl(wxWindow* parent, wxWindowID id, float min, float max, const wxString& value = _(""),
	      const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
	      const wxValidator& validator = wxDefaultValidator, const wxString& name = wxTextCtrlNameStr);
  virtual ~DoubleTextCtrl(){};

  /* Renvoit la valeur du contrôle en prenant en compte les erreurs (type, intervalle)
   * et ouvre une boîte de notification de l'erreur le cas échéant. Dans ce
   * dernier cas, la fonction lève une exception de type float, permettant de simplifier
   * les tests d'erreur dans une fonction faisant appel à cette fonction de nombreuses fois.
   */
  virtual float GetSafelyValue();
  virtual void nonNumericErrorDialog(wxString& s);
  virtual void rangeErrorDialog(wxString& s);

private:
  float m_min, m_max;
};

#endif
