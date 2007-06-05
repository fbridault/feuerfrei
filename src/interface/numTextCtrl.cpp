#include "numTextCtrl.hpp"

LongTextCtrl::LongTextCtrl(wxWindow* parent, wxWindowID id, long min, long max, const wxString& value, 
	      const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name) : 
  NumTextCtrl(parent, id, value, pos, size, style, validator, name)
{
  m_min = min;
  m_max = max;
}

long LongTextCtrl::GetSafelyValue(void)
{
  wxString tmp; 
  long val;
    
  tmp = GetValue();
  if( tmp.ToLong(&val))
    if(val >= m_min && val <= m_max)
      return(val); 
    else
      rangeErrorDialog(tmp);
  else
    nonNumericErrorDialog(tmp);
    
  throw(tmp);
  return -1;  
}

void LongTextCtrl::nonNumericErrorDialog(wxString& s)
{
  wxMessageDialog errorDialog(this,_("\"") + s + _("\" is a not an integer value"),
						     _("Error"),wxOK|wxICON_ERROR);
  errorDialog.ShowModal();
}
  
void LongTextCtrl::rangeErrorDialog(wxString& s)
{
  wxString message;
  message << _("Bad value range : ") << s << _("\nValue must be between ") << m_min << _(" and ") << m_max;
  wxMessageDialog errorDialog(this,message, _("Error"),wxOK|wxICON_ERROR);
  errorDialog.ShowModal();
}

DoubleTextCtrl::DoubleTextCtrl(wxWindow* parent, wxWindowID id, float min, float max, const wxString& value, 
	      const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name)
: NumTextCtrl(parent, id, value, pos, size, style, validator, name)
{
  m_min = min;
  m_max = max;
}

float DoubleTextCtrl::GetSafelyValue(void)
{
  wxString tmp;
  float val;
    
  tmp = GetValue();
  
  if( tmp.ToDouble((double *)&val))
    if(val >= m_min && val <= m_max)
      return(val);
    else
      rangeErrorDialog(tmp);
  else
    nonNumericErrorDialog(tmp);
  
  throw(tmp);
  return -1;
}

void DoubleTextCtrl::nonNumericErrorDialog(wxString& s)
{
  wxMessageDialog errorDialog (this,_("\"") + s + _("Not a numeric value"), _("Error"),wxOK|wxICON_ERROR);
  errorDialog.ShowModal();
}
  
void DoubleTextCtrl::rangeErrorDialog(wxString& s)
{
  wxString message;
  message << _("Bad value range : ") << s << _("\nValue must be between ") << m_min << _(" and ") << m_max;
  wxMessageDialog errorDialog(this,message, _("Error"),wxOK|wxICON_ERROR);
  errorDialog.ShowModal();
}
