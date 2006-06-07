package Etk::FillPolicy;
use strict;
require Etk;
use constant
{
    None => 1 << 0,
    HFill => 1 << 1,
    VFill => 1 << 2,
    HExpand => 1 << 3,
    VExpand => 1 << 4
};

1;

  
