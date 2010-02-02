<!--

Copyright (C) 2004 Peter J Jones (pjones@pmade.org)
All Rights Reserved

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in
the documentation and/or other materials provided with the
distribution.
3. Neither the name of the Author nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

-->
<!--=================================================================================================-->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<!--=================================================================================================-->
	<xsl:output 
		method="text" 
		indent="yes"
		omit-xml-declaration="yes"
		/>
	<!--=================================================================================================-->
	<xsl:param name="title"	    select="'To Do List'"/>
	<xsl:param name="category"  select="''"/>
	<!--=================================================================================================-->
	<xsl:template match="/">
<xsl:value-of select="$title" />
=================================

<xsl:apply-templates/>
</xsl:template>
	<!--=================================================================================================-->
<xsl:template match="gtodo">
<xsl:choose>
<xsl:when test="string-length($category) != 0">
<xsl:apply-templates select="category[@title = $category]"/>
</xsl:when>

<xsl:otherwise>
<xsl:for-each select="category">
    <xsl:if test="count(item/attribute[@done = '0'])">
<xsl:apply-templates select="."/>
</xsl:if>
</xsl:for-each>
</xsl:otherwise>
</xsl:choose>
</xsl:template>
	<!--=================================================================================================-->
<xsl:template match="category">
<xsl:value-of select="@title"/>
=======================================================================================
<xsl:apply-templates select="item">
<xsl:sort select="attribute/@enddate" data-type="number"/>
</xsl:apply-templates>
</xsl:template>
	<!--=================================================================================================-->
<xsl:template match="item">
	<xsl:if test="attribute/@done = 0">
 - <xsl:apply-templates select="summary"/>:
   <xsl:apply-templates select="comment" indent="yes"/>
<xsl:text>

</xsl:text>

</xsl:if>
</xsl:template>

</xsl:stylesheet>
<!--=================================================================================================-->
<!--
vim:tw=300
-->
