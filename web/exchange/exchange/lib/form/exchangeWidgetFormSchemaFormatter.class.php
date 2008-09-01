<?php

class exchangeWidgetFormSchemaFormatter extends sfWidgetFormSchemaFormatter
{
  protected
    $rowFormat       = "<tr>\n  <th>%label%</th>\n  <td>%field%%help%%error%%hidden_fields%</td>\n</tr>\n",
    $errorRowFormat  = "<tr><td colspan=\"2\">\n%errors%</td></tr>\n",
    $helpFormat      = '<br />%help%',
    $errorListFormatInARow     = "  <ul class=\"form_error\">\n%errors%  </ul>\n",
    $decoratorFormat = "<table>\n  %content%</table>";
}
