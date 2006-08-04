package Etk::Constants;

require Exporter;
our @ISA = qw/Exporter/;

our %EXPORT_TAGS = (
   combobox	=> [qw/ColumnTypeLabel ColumnTypeImage ColumnTypeOther/],
   fillpolicy	=> [qw/FillNone HFill VFill HExpand VExpand/],
   messagedialog=> [qw/None Warning Question Error
    ButtonsNone ButtonsOk ButtonsClose ButtonsCancel ButtonsYesNo ButtonsOkCancel/],
   progressbar	=> [qw/LeftToRight RightToLeft/],
   scrolledview	=> [qw/PolicyShow PolicyHide PolicyAuto/],
   toplevelwidget=>[qw/PointerDefault PointerMove PointerHDoubleArrow 
    PointerVDoubleArrow PointerResize PointerResizeTL PointerResizeT 
    PointerResizeTR PointerResizeR PointerResizeBR PointerResizeB 
    PointerResizeBL PointerResizeL PointerTextEdit PointerDndDrop/],
   tree		=> [qw/ModeList ModeTree FromFile FromEdje/],
   textblock	=> [qw/GravityLeft GravityRight/]

   );

my @all = ();
push @all, map { @{$EXPORT_TAGS{$_}} } keys %EXPORT_TAGS;

$EXPORT_TAGS{all} = [@all];

our @EXPORT_OK = @all;

# Combobox Constants
use constant
{
    ColumnTypeLabel => 0,
    ColumnTypeImage => 1,
    ColumnTypeOther => 2
};


# Etk::FillPolicy
use constant
{
    FillNone => 1 << 0,
    HFill => 1 << 1,
    VFill => 1 << 2,
    HExpand => 1 << 3,
    VExpand => 1 << 4
};

# Message Dialog
use constant
{
      None => 0,
      Warning => 1,
      Question => 2,
      Error => 3,
      
      ButtonsNone => 0,
      ButtonsOk => 1,
      ButtonsClose => 2,
      ButtonsCancel => 3,
      ButtonsYesNo => 4,
      ButtonsOkCancel => 5
};

# progressbar
use constant 
{
    LeftToRight => 0,
    RightToLeft => 1
};


# scrolled view
use constant {
	PolicyShow	=> 0,
	PolicyHide	=> 1,
	PolicyAuto	=> 2
};

# toplevel widget
use constant {

   PointerDefault	=> 0,
   PointerMove		=> 1,
   PointerHDoubleArrow	=> 2,
   PointerVDoubleArrow	=> 3,
   PointerResize	=> 4,
   PointerResizeTL	=> 5,
   PointerResizeT	=> 6,
   PointerResizeTR	=> 7,
   PointerResizeR	=> 8,
   PointerResizeBR	=> 9,
   PointerResizeB	=> 10,
   PointerResizeBL	=> 11,
   PointerResizeL	=> 12,
   PointerTextEdit	=> 13,
   PointerDndDrop	=> 14
};

# tree
use constant
{
   ModeList => 0,
   ModeTree => 1,
};


# tree model icon text
use constant
{
    FromFile => 0,
    FromEdje => 1
};

# textblock gravity
use constant
{
    GravityLeft => 0,
    GravityRight => 1
};


package Etk::Colorpicker::Mode;

use constant  {

	H	=> 0,
	S	=> 1,
	V	=> 2,
	R	=> 3,
	G	=> 4,
	B	=> 5

};

