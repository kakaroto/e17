<?xml version='1.0' encoding='UTF-8'?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:output method="text"/>

<xsl:template name="copy-attr">
  <xsl:for-each select="@*">
    <xsl:text disable-output-escaping="yes"> </xsl:text>
    <xsl:value-of select="name()"/>=<xsl:value-of select="."/>
  </xsl:for-each>
</xsl:template>

<xsl:template name="copy">
  <xsl:text disable-output-escaping="yes">&lt;</xsl:text>
  <xsl:value-of select="name()"/>
  <xsl:call-template name="copy-attr"/>
  <xsl:text disable-output-escaping="yes">&gt;</xsl:text>
</xsl:template>

<xsl:template name="para">
  <xsl:choose>
  <xsl:when test="markup[@role='dox-center']">
    <xsl:text disable-output-escaping="yes">&lt;p align=50%&gt;</xsl:text>
  </xsl:when>
  <xsl:otherwise>
    <xsl:text disable-output-escaping="yes">&lt;p&gt;</xsl:text>
  </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="refentry">
  <xsl:apply-templates select="refsect1"/>
</xsl:template>

<xsl:template match="refsect1">
  <xsl:apply-templates select="refsect2"/>
</xsl:template>

<xsl:template match="refsect2">
  <xsl:text disable-output-escaping="yes">&lt;</xsl:text>
  <xsl:choose>
  <xsl:when test="@id = 'docindex'">
    <xsl:text>page columns=2 padding=16 name=</xsl:text><xsl:value-of select="@id"/><xsl:text> background=Edoc_bg.png linkcolor=#88dddd</xsl:text>
  </xsl:when>
  <xsl:otherwise>
    <xsl:text>page columns=1 padding=16 name=</xsl:text><xsl:value-of select="@id"/><xsl:text> background=Edoc_bg.png linkcolor=#88dddd</xsl:text>
  </xsl:otherwise>
  </xsl:choose>
  <xsl:text disable-output-escaping="yes">&gt;</xsl:text>
  <xsl:apply-templates/>
  <xsl:text disable-output-escaping="yes">&#xa;&#xa;</xsl:text>
</xsl:template>

<xsl:template match="para">
  <xsl:if test="name(preceding-sibling::*[1]) != 'para' and name(preceding-sibling::*[1]) != 'font' ">
    <xsl:text disable-output-escaping="yes">&lt;font face=Vera/11 color=#ffffff&gt;&#xa;</xsl:text>
  </xsl:if>
  <xsl:call-template name="para"/>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="member">
  <xsl:apply-templates/>
  <xsl:text disable-output-escaping="yes">&lt;br&gt;</xsl:text>
</xsl:template>

<xsl:template match="link">
  <xsl:text>_</xsl:text>
  <xsl:value-of select="translate(.,' ','_')"/>
  <xsl:text>(</xsl:text><xsl:value-of select="@linkend"/><xsl:text>)</xsl:text>
</xsl:template>

<xsl:template match="font|img">
  <xsl:call-template name="copy"/>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="refentryinfo | refmeta | refnamediv">
</xsl:template>

<xsl:template match="title">
  <xsl:if test="normalize-space(.)">
    <xsl:text disable-output-escaping="yes">&#xa;</xsl:text>
    <xsl:text disable-output-escaping="yes">&lt;font face=VeraBd/16 color=#ffffff&gt;&#xa;</xsl:text>
    <xsl:call-template name="para"/>
    <xsl:value-of select="."/>
  </xsl:if>
</xsl:template>

</xsl:stylesheet>
