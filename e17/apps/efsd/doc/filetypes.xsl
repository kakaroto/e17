<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:param name="letter" select="a"/>

  <xsl:template match="filetypes">

    <html>
      <head>
	<title>[insert your title here]</title>
	
	<script type="text/javascript" src="mtmcode.js">
	</script>
	
	<script type="text/javascript">
	  // Morten's JavaScript Tree Menu
	  // version 2.3.0, dated 2001-04-30
	  // http://www.treemenu.com/
	  
	  // Copyright (c) 2001, Morten Wang and contributors
	  // All rights reserved.
	  
	  // This software is released under the BSD License which should accompany
	  // it in the file "COPYING".  If you do not have this file you can access
	  // the license through the WWW at http://www.treemenu.com/license.txt
	  
	  /******************************************************************************
	  * User-configurable options.                                                  *
	  ******************************************************************************/
	  
	  // Menu table width, either a pixel-value (number) or a percentage value.
	  var MTMTableWidth = "100%";
	  
	  // Name of the frame where the menu is to appear.
	  var MTMenuFrame = "menu";
	  
	  // variable for determining whether a sub-menu always gets a plus-sign
	  // regardless of whether it holds another sub-menu or not
	  var MTMSubsGetPlus = "Always";
	  
	  // variable that defines whether the menu emulates the behaviour of
	  // Windows Explorer
	  var MTMEmulateWE = true;
	  
	  // Directory of menu images/icons
	  var MTMenuImageDirectory = "menu-images/";
	  
	  // Variables for controlling colors in the menu document.
	  // Regular BODY atttributes as in HTML documents.
	  var MTMBGColor = "#ffffff";
	  var MTMBackground = "";
	  var MTMTextColor = "#0000a0";
	  
	  // color for all menu items
	  var MTMLinkColor = "#000000";
	  
	  // Hover color, when the mouse is over a menu link
	  var MTMAhoverColor = "#0000ff";
	  
	  // Foreground color for the tracking and clicked submenu item
	  var MTMTrackColor ="#0000ff";
	  var MTMSubExpandColor = "#0000a0";
	  var MTMSubClosedColor = "#0000a0";
	  
	  // All options regarding the root text and it's icon
	  var MTMRootIcon = "menu_new_root.gif";
	  var MTMenuText = "Filetypes:";
	  var MTMRootColor = "#0000a0";
	  var MTMRootFont = "helvetica, arial";
	  var MTMRootCSSize = "10pt";
	  var MTMRootFontSize = "0";
	  
	  // Font for menu items.
	  var MTMenuFont = "Helvetica, Arial, sans-serif";
	  var MTMenuCSSize = "10pt";
	  var MTMenuFontSize = "0";
	  
	  // Variables for style sheet usage
	  // 'true' means use a linked style sheet.
	  var MTMLinkedSS = false;
	  var MTMSSHREF = "style/menu.css";
	  
	  // Additional style sheet properties if you're not using a linked style sheet.
	  // See the documentation for details on IDs, classes and elements used in the menu.
	  // Empty string if not used.
	  var MTMExtraCSS = "";
	  
	  // Header and footer, these are plain HTML.
	  // Leave them to be "" if you're not using them
	  
	  var MTMHeader = "";
	  var MTMFooter = "";
	  
	  // Whether you want an open sub-menu to close automagically
	  // when another sub-menu is opened.  'true' means auto-close
	  var MTMSubsAutoClose = false;
	  
	  // This variable controls how long it will take for the menu
	  // to appear if the tracking code in the content frame has
	  // failed to display the menu. Number if in tenths of a second
	  // (1/10) so 10 means "wait 1 second".
	  var MTMTimeOut = 25;
	  
	  // Cookie usage.  First is use cookie (yes/no, true/false).
	  // Second is cookie name to use.
	  // Third is how many days we want the cookie to be stored.
	  
	  var MTMUseCookies = false;
	  var MTMCookieName = "MTMCookie";
	  var MTMCookieDays = 3;
	  
	  // Tool tips.  A true/false-value defining whether the support
	  // for tool tips should exist or not.
	  var MTMUseToolTips = true;
	  
	  /******************************************************************************
	  * User-configurable list of icons.                                            *
	  ******************************************************************************/
	  
	  var MTMIconList = null;
	  MTMIconList = new IconList();
	  MTMIconList.addIcon(new MTMIcon("menu_link_external.gif", "http://", "pre"));
	  MTMIconList.addIcon(new MTMIcon("menu_link_pdf.gif", ".pdf", "post"));
	  
	  /******************************************************************************
	  * Menu stack                                                                  *
	  ******************************************************************************/
	  
	  function push(menuItem)
	  {
	  	var menu = null;
	  	menu = new MTMenu();
	  
	  	this.mstack[this.mstack.length - 1].MTMAddItem(menuItem);
	  	this.mstack.push(menu);
	  }
	  
	  function pop()
	  {
	  	var m = this.mstack.pop();
	  	var m2 = this.mstack[this.mstack.length - 1];
	  
	  	if (m.items.length > 0)
	  	{
	  		m2.items[m2.items.length - 1].MTMakeSubmenu(m);      
	  	}
	  }
	  
	  function MenuStack()
	  {
	  	this.push   = push;
	  	this.pop    = pop;
	  	this.mstack = new Array();
	  	this.menu   = new MTMenu();
	  
	  	this.mstack.push(this.menu);
	  }
	  
	  
	  /******************************************************************************
	  * Actual menu code                                                            *
	  ******************************************************************************/
	  
	  
	  var ms = null;
	  ms = new MenuStack();
	  menu = ms.menu;
	  
	    <xsl:choose>
	      <xsl:when test="$letter = 'other'">
		<xsl:apply-templates select="mtest[string-length(descr) = 0]">
		  <xsl:sort select="descr"/>
		</xsl:apply-templates>	
	      </xsl:when>
	      <xsl:otherwise>
		<xsl:apply-templates select="mtest[starts-with(descr, $letter)]">
		  <xsl:sort select="descr"/>
		</xsl:apply-templates>	
              </xsl:otherwise>
	    </xsl:choose>
	    
	</script>
      </head>
      <body onload="MTMStartMenu()" bgcolor="#ffffff" text="#0000a0">
      </body>
    </html>
  </xsl:template>

  <xsl:template match="mtest">
    <xsl:text>ms.push(new MTMenuItem("</xsl:text>
    <xsl:value-of select="descr"/>
    <xsl:text>", "table-</xsl:text>
    <xsl:value-of select="$letter"/>
    <xsl:text>.html#</xsl:text>
    <xsl:value-of select="@id"/>
    <xsl:text>", "text"));</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>ms.pop();</xsl:text>
  </xsl:template>    

<!--
  <xsl:template match="mtest">
    <xsl:text>ms.push(new MTMenuItem("</xsl:text>
    <xsl:value-of select="descr"/>
    <xsl:text>", "table-</xsl:text>
    <xsl:value-of select="$letter"/>
    <xsl:text>.html#</xsl:text>
    <xsl:value-of select="@id"/>
    <xsl:text>", "text"));</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>ms.pop();</xsl:text>
  </xsl:template>    
-->
  <xsl:template match="*">
  </xsl:template>

</xsl:stylesheet>
