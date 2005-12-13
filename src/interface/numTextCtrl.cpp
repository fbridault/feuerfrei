#include "numTextCtrl.hpp"

long NumTextCtrl::getValueAsLong(void)
{
  wxString tmp; 
  long val;
    
  tmp = GetValue();
  if(tmp.IsNumber()){
    tmp.ToLong(&val);
    return(val);
  }else{
    throw(tmp);
    return -1;
  }
}

double NumTextCtrl::getValueAsDouble(void)
{
  wxString tmp;
  double val;
  unsigned int i=0;
    
  tmp = GetValue();
      
  while( i != tmp.Len()){
    if(!isdigit(tmp[i]) && tmp[i]!=','){
      throw(tmp);
      return -1;
    }
    i++;
  }
  tmp.ToDouble(&val);
  return(val);
}

void NumTextCtrl::nonNumericErrorDialog(wxString s)
{
  wxMessageDialog *errorDialog = new wxMessageDialog(this,_("\"") + s + _("\" is a non-numeric value"),
						     _("Error"),wxOK|wxICON_ERROR);
  errorDialog->ShowModal();
  errorDialog->Destroy();
}
  
void NumTextCtrl::rangeErrorDialog(wxString s)
{
  wxMessageDialog *errorDialog = new wxMessageDialog(this,_("Bad value range : ") + s, _("Error"),wxOK|wxICON_ERROR);
  errorDialog->ShowModal();
  errorDialog->Destroy();
}
