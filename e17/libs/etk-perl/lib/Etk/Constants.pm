package Etk::Constants;

=head1 NAME

Etk::Constants - Constants to use with Etk

=head1 SYNOPSIS

   use Etk::Constants qw/ButtonsOk ButtonsClose/;
   # 
   use Etk::Constants qw/:messagedialog/;
   # 
   use Etk::Constants qw/:all/;

=head1 DESCRIPTION

This module contains constants to use while building Etk apps.
The constants have numerical values which can be used instead.

=head1 EXPORT

None by default.

=head1 EXPORTABLE

   box combobox fillpolicy messagedialog progressbar scrolledview
   toplevelwidget tree textblock table colorpicker mouse toolbar
   button shadow image entry popupwindow window swallow slider all
 
=cut

require Exporter;
our @ISA = qw/Exporter/;

our %EXPORT_TAGS = (
   box		=> [qw/BoxStart BoxEnd BoxNone BoxExpand BoxFill BoxExpandFill/],
   combobox	=> [qw/ColumnTypeLabel ColumnTypeImage ColumnTypeOther EntryImage EntryLabel EntryOther
    EntryNone EntryExpand EntryFill EntryExpandFill/],
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
   textblock	=> [qw/GravityLeft GravityRight/],
   table	=> [qw/TableNone TableHFill TableVFill TableHExpand TableVExpand
    TableFill TableExpand TableExpandFill/],
   colorpicker	=> [qw/ModeH ModeS ModeV ModeR ModeG ModeB/],
   mouse	=> [qw/ModifierNone ModifierCtrl ModifierAlt ModifierShift ModifierWin
    LockNone LockNum LockCaps LockScroll
    MouseNone MouseDoubleClick MouseTripleClick/],
   toolbar	=> [qw/ToolbarIcons ToolbarText ToolbarBothVert ToolbarBothHoriz 
    ToolbarDefault ToolbarHoriz ToolbarVert/],
   button	=> [qw/ButtonIcon ButtonText ButtonBothVert ButtonBothHoriz/],
   shadow	=> [qw/ShadowNone ShadowInside ShadowOutside ShadowNoEdge ShadowLeft ShadowAll
    ShadowRight ShadowTop ShadowBottom ShadowLeftRight ShadowTopBottom/],
   image	=> [qw/ImageFile ImageEdje ImageStock ImageEvasObject ImageData/],
   entry	=> [qw/ImagePrimary ImageSecondary/],
   popupwindow	=> [qw/BelowRight BelowLeft AboveRight AboveLeft/],
   window	=> [qw/Normal Above Below/],
   swallow	=> [qw/None IncompatibleParent NotRealized NoPart/],
   slider	=> [qw/Continuous Discontinuous Delayed/],
   );

my @all = ();
push @all, map { @{$EXPORT_TAGS{$_}} } keys %EXPORT_TAGS;

$EXPORT_TAGS{all} = [@all];

our @EXPORT_OK = @all;

# Box constants
use constant
{
   BoxStart	=> 0,
   BoxEnd	=> 1,

   BoxNone	=> 0,
   BoxExpand	=> 1 << 0,
   BoxFill	=> 1 << 1,
   BoxExpandFill=> (1 << 0) | (1 << 1)

};

# Combobox Constants
use constant
{
    ColumnTypeLabel => 0,
    ColumnTypeImage => 1,
    ColumnTypeOther => 2,
    EntryLabel	=> 0,
    EntryImage	=> 1,
    EntryOther	=> 2,
    EntryNone	=> 0,
    EntryExpand	=> 1 << 0,
    EntryFill	=> 1 << 1,
    EntryExpandFill	=> (1 << 0) | (1 << 1)
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

# colorpicker mode
use constant  {

	ModeH	=> 0,
	ModeS	=> 1,
	ModeV	=> 2,
	ModeR	=> 3,
	ModeG	=> 4,
	ModeB	=> 5

};

# Table Constants
use constant {
	TableNone	=> 0,
	TableHFill	=> 1 << 0,
	TableVFill	=> 1 << 1,
	TableHExpand	=> 1 << 2,
	TableVExpand	=> 1 << 3,
	TableFill	=> (1<<0) | (1<<1),
	TableExapnd	=> (1<<2) | (1<<3),
	TableExpandFill	=> (1<<0) | (1<<1) | (1<<2) | (1<<3)

};

# events
use constant {
   ModifierNone => 0,
   ModifierCtrl => 1 << 0, 
   ModifierAlt => 1 << 1, 
   ModifierShift => 1 << 2, 
   ModifierWin => 1 << 3, 
};

use constant {
   LockNone => 0,
   LockNum => 1 << 0, 
   LockCaps => 1 << 1, 
   LockScroll => 1 << 2, 
};

use constant {
   MouseNone => 0,
   MouseDoubleClick => 1 << 0, 
   MouseTripleClick => 1 << 1, 
};


# toolbar
use constant {
   ToolbarIcons => 0,
   ToolbarText  => 1,
   ToolbarBothVert => 2,
   ToolbarBothHoriz => 3,
   ToolbarDefault => 4,

   ToolbarHoriz => 0,
   ToolbarVert =>  1,

};

#button
use constant {
   ButtonIcon => 0,
   ButtonText => 1,
   ButtonBothVert => 2,
   ButtonBothHoriz => 3
};


#shadow
use constant {
   ShadowNone => 0,
   ShadowInside => 1,
   ShadowOutside => 2,

   ShadowNoEdge => 0,
   ShadowLeft => 1 << 0,
   ShadowRight => 1 << 1,
   ShadowTop => 1 << 2,
   ShadowBottom => 1 << 3   
};
use constant {
   ShadowLeftRight => ShadowLeft | ShadowRight,
   ShadowTopBottom => ShadowTop | ShadowBottom
};
use constant {
   ShadowAll => ShadowLeftRight | ShadowTopBottom
};

# image source
use constant {
   ImageFile=>0,
   ImageEdje=>1,
   ImageStock=>2,
   ImageEvasObject=>3,
   ImageData=>4
};

# entry
use constant {
   ImagePrimary=>0,
   ImageSecondary=>1
};

# popup window
use constant {
   BelowRight=>0,
   BelowLeft=>1,
   AboveRight=>2,
   AboveLeft=>3
};

# window stacking
use constant {
   Normal=>0,
   Above=>1,
   Below=>2
};

# swallow errors
use constant {
   IncompatibleParent=>1,
   NotRealized=>2,
   NoPart=>3
};

# slider
use constant {
   Continuous=>0,
   Discontinuous=>1,
   Delayed=>2
};

