#ifndef NUMTEXTCTRL_H
#define NUMTEXTCTRL_H

class NumTextCtrl;

#include "interface.hpp"

class NumTextCtrl : public wxTextCtrl
{
public:
  NumTextCtrl(wxWindow* parent, wxWindowID id, const wxString& value = _(""), const wxPoint& pos = wxDefaultPosition, 
	      const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator,
	      const wxString& name = wxTextCtrlNameStr) : 
    wxTextCtrl(parent, id, value, pos, size, style, validator, name)
  {};
  virtual ~NumTextCtrl(){};
    
  long getValueAsLong(void);  
  double getValueAsDouble(void);

  void nonNumericErrorDialog(wxString s);  
  void rangeErrorDialog(wxString s);
};

#endif
