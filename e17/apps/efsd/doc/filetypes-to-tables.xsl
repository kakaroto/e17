<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  
  <xsl:param name="letter" select="a"/>

  <xsl:template match="filetypes">
    <html>
      <head>
	<link rel="stylesheet" type="text/css" href="stylesheet.css"/>
      </head>
      <body bgcolor="#ffffff" text="#0000a0">	
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
      </body>
    </html>
  </xsl:template>
  
  <xsl:template match="mtest">
    <xsl:variable name="mask" select="mask"/>

    <xsl:element name="a"><xsl:attribute name="name"><xsl:value-of select="@id"/></xsl:attribute></xsl:element>
    
    <table border="0" width="600">
      <tr>
	<td class="title" colspan="6"><xsl:value-of select="descr"/></td>
      </tr>
      <tr>
	<td class="subtitle">Datatype</td>
	<td class="subtitle">Byteorder</td>
	<td class="subtitle">Test Type</td>
	<td class="subtitle">Offset</td>
	<td class="subtitle">Mask</td>
	<td class="subtitle">Magic</td>
      </tr>
      <tr>
	<td class="val"><xsl:value-of select="@data"/></td>
	<td class="val"><xsl:value-of select="@byteorder"/></td>
	<td class="val"><xsl:value-of select="@type"/></td>
	<td class="val"><xsl:value-of select="offset"/></td>
	<td class="val">
	  <xsl:choose>	    
	    <xsl:when test="string-length($mask) = 0"><xsl:text>(none)</xsl:text></xsl:when>
	    <xsl:otherwise><xsl:value-of select="mask"/></xsl:otherwise>
	  </xsl:choose>
	</td>
	<td class="val"><xsl:value-of select="magic"/></td>
      </tr>
    </table>

    <p>
    </p>

    <xsl:apply-templates/>

  </xsl:template>    
  
  <xsl:template match="*">
  </xsl:template>

</xsl:stylesheet>
