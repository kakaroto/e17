--- Eye theme reference ---

Data fields:
	- author 		// Theme author
	- contact 		// Author's email
	- version 		// Theme version
	- date			// Theme release date

Groups:
	- main_window	// This group contains the main window shown at launch

================================================================================
main_window group
-----------------
Swallows:
	- toolbar1		// Toolbar1 buttons are swallowed by this
	- toolbar2		// Toolbar2 buttons are swallowed by this
	- toolbar3		// Toolbar3 buttons are swallowed by this

Textblocks:
	- error_dialog_text	// Error dialog's content is written here

Text parts:	
	-

Signals to emit:
	-

Signals to be caught:
	- Signal: "error_show" 
	  Source: "eyesight"
	  Action: Show error dialog

================================================================================

	