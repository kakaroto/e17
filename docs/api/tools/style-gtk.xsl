<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
  xmlns:exsl="http://exslt.org/common"
  extension-element-prefixes="exsl" >
  
  <!-- parameter "dir" must be set from the command line: it represents the output directory -->
  <xsl:param name="dir"/>
  
  <xsl:output name="index" method="html" indent="yes"/>
  
  <xsl:template match="doxygen">

    <xsl:variable name="libname">
      <xsl:value-of select="substring-before(./compounddef[@kind='file']/compoundname, '.')"/>
    </xsl:variable>
    
    <exsl:document href="./index.html" method="html" indent="yes">
      <xsl:element name="html">
        <xsl:call-template name="html_header"/>
        <xsl:element name="body">

          <xsl:call-template name="doc_header"/>
          <xsl:call-template name="doc_image"/>

          <xsl:call-template name="doc_synopsis"/>
          
          <xsl:call-template name="doc_description"/>
          <xsl:call-template name="doc_todo"/>

          <xsl:call-template name="doc_details"/>
          
        </xsl:element>
      </xsl:element>
    </exsl:document>
  </xsl:template>

  <!-- Header of the HTML file -->

  <xsl:template name="html_header">
    <xsl:element name="head">
      <xsl:element name="link">
        <xsl:attribute name="rel">
          <xsl:text>stylesheet</xsl:text>
        </xsl:attribute>
        <xsl:attribute name="type">
          <xsl:text>text/css</xsl:text>
        </xsl:attribute>
        <xsl:attribute name="href">
          <xsl:text>style.css</xsl:text>
        </xsl:attribute>
      </xsl:element>
      <xsl:element name="title">
        <xsl:value-of select="substring-before(./compounddef[@kind='file']/compoundname, '.')"/>
      </xsl:element>
    </xsl:element>
  </xsl:template>
  
  <!-- Header of the documentation -->

  <xsl:template name="doc_header">
    <xsl:element name="div">
      <xsl:attribute name="class">
        <xsl:text>header</xsl:text>
      </xsl:attribute>
      <xsl:element name="table">
        <xsl:element name="tr">
          <xsl:element name="td">
            <xsl:attribute name="width">
              <xsl:text>16px</xsl:text>
            </xsl:attribute>
            <xsl:element name="img">
              <xsl:attribute name="src">
                <xsl:text>e_mini.png</xsl:text>
              </xsl:attribute>
              <xsl:attribute name="alt">
                <xsl:text>E</xsl:text>
              </xsl:attribute>
            </xsl:element>
          </xsl:element>
          <xsl:element name="td">
            <xsl:attribute name="width">
              <xsl:text>100%</xsl:text>
            </xsl:attribute>
            <xsl:element name="h2">
              <xsl:value-of select="//compounddef[@id='indexpage']/title"/>
            </xsl:element>
          </xsl:element>
          <xsl:element name="td">
            <xsl:attribute name="width">
              <xsl:text>16px</xsl:text>
            </xsl:attribute>
            <xsl:element name="img">
              <xsl:attribute name="src">
                <xsl:text>e_mini.png</xsl:text>
              </xsl:attribute>
              <xsl:attribute name="alt">
                <xsl:text>E</xsl:text>
              </xsl:attribute>
            </xsl:element>
          </xsl:element>
        </xsl:element>
      </xsl:element>
    </xsl:element>
  </xsl:template>

  <!-- Synopsis -->

  <xsl:template name="doc_synopsis">
    <xsl:element name="div">
      <xsl:attribute name="class">
        <xsl:text>ref_synopsis</xsl:text>
      </xsl:attribute>
      <xsl:element name="h2">
        <xsl:text>Synopsis</xsl:text>
      </xsl:element>
      <xsl:element name="pre">
        <xsl:attribute name="class">
          <xsl:text>synopsis</xsl:text>
        </xsl:attribute>
        <xsl:element name="table">
          <xsl:apply-templates select="./compounddef/sectiondef/memberdef[@kind='enum']" mode="synopsis"/>
        </xsl:element>
        <xsl:element name="table">
          <xsl:apply-templates select="./compounddef/sectiondef/memberdef[@kind='define']" mode="synopsis"/>
        </xsl:element>
        <xsl:element name="table">
          <xsl:apply-templates select="./compounddef/sectiondef/memberdef[@kind='function']" mode="synopsis"/>
        </xsl:element>
      </xsl:element>
    </xsl:element>
  </xsl:template>

  <!-- Details -->

  <xsl:template name="doc_details">
    <xsl:element name="div">
      <xsl:attribute name="class">
        <xsl:text>ref_details</xsl:text>
      </xsl:attribute>
      <xsl:element name="h2">
        <xsl:text>Details</xsl:text>
      </xsl:element>
      <xsl:apply-templates select="./compounddef/sectiondef/memberdef[@kind='enum']" mode="details"/>
      <xsl:apply-templates select="./compounddef/sectiondef/memberdef[@kind='define']" mode="details"/>
      <xsl:apply-templates select="./compounddef/sectiondef/memberdef[@kind='function']" mode="details"/>
    </xsl:element>
  </xsl:template>

  <!-- Enum stylesheet -->

  <!-- Enum synopsis stylesheet -->
  
  <xsl:template match="memberdef[@kind='enum']" mode="synopsis">
    <xsl:element name="tr">
      <xsl:element name="td">
        <xsl:attribute name="class">
          <xsl:text>type</xsl:text>
        </xsl:attribute>
        <xsl:text>enum&#xA0;</xsl:text>
      </xsl:element>
      <xsl:element name="td">
        <xsl:attribute name="class">
          <xsl:text>name</xsl:text>
        </xsl:attribute>
        <xsl:element name="a">
          <xsl:attribute name="href">
            <xsl:value-of select="concat ('#', ./@id)"/>
          </xsl:attribute>
          <xsl:value-of select="./name"/>
        </xsl:element>
        <xsl:text>;&#xA;</xsl:text>
      </xsl:element>
    </xsl:element>
  </xsl:template>

  <!-- Enum details stylesheet -->

  <xsl:template match="memberdef[@kind='enum']" mode="details">
    <xsl:element name="hr"/>
    <xsl:element name="div">
      <xsl:attribute name="class">
        <xsl:text>details</xsl:text>
      </xsl:attribute>
      <xsl:element name="h3">
        <xsl:attribute name="id">
          <xsl:value-of select="./@id"/>
        </xsl:attribute>
        <xsl:text>enum </xsl:text>
        <xsl:value-of select="./name"/>
      </xsl:element>
      <xsl:element name="pre">
        <xsl:attribute name="class">
          <xsl:text>api</xsl:text>
        </xsl:attribute>
        <xsl:text>typedef enum&#xA;</xsl:text>
        <xsl:text>{&#xA;</xsl:text>
        <xsl:for-each select="./enumvalue">
          <xsl:text>  </xsl:text>
          <xsl:value-of select="./name"/>
          <xsl:choose>
            <xsl:when test="not(position()=last())">
              <xsl:text>,&#xA;</xsl:text>
            </xsl:when>
            <xsl:otherwise>
              <xsl:text>&#xA;</xsl:text>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:for-each>
        <xsl:text>}</xsl:text>
        <xsl:value-of select="./name"/>
        <xsl:text>;</xsl:text>
      </xsl:element>
      <xsl:element name="p">
        <xsl:value-of select="./briefdescription/para"/>
      </xsl:element>
      <xsl:element name="p">
        <xsl:value-of select="./detaileddescription/para"/>
      </xsl:element>
    </xsl:element>
  </xsl:template>

  <!-- Define stylesheet -->

  <!-- Define synopsis stylesheet -->

  <xsl:template match="memberdef[@kind='define']" mode="synopsis">
    <xsl:choose>
      <xsl:when test="./param">
        <xsl:for-each select="./param">
          <xsl:choose>
            <xsl:when test="position()=1">
              <xsl:element name="tr">
                <xsl:element name="td">
                  <xsl:attribute name="id">
                    <xsl:text>preprocessor</xsl:text>
                  </xsl:attribute>
                  <xsl:text>#define&#xA0;</xsl:text>
                </xsl:element>
                <xsl:element name="td">
                  <xsl:attribute name="id">
                    <xsl:text>name</xsl:text>
                  </xsl:attribute>
                  <xsl:element name="a">
                    <xsl:attribute name="href">
                      <xsl:value-of select="concat('#', ../@id)"/>
                    </xsl:attribute>
                    <xsl:value-of select="../name"/>
                  </xsl:element>
                </xsl:element>
                <xsl:element name="td">
                  <xsl:attribute name="id">
                    <xsl:text>separator</xsl:text>
                  </xsl:attribute>
                  <xsl:text>(</xsl:text>
                </xsl:element>
                <xsl:element name="td">
                  <xsl:attribute name="id">
                    <xsl:text>argument</xsl:text>
                  </xsl:attribute>
                  <xsl:value-of select="./defname"/>
                  <xsl:choose>
                    <xsl:when test="position()=last()">
                      <xsl:text>)</xsl:text>
                    </xsl:when>
                    <xsl:otherwise>
                      <xsl:text>,</xsl:text>
                    </xsl:otherwise>
                  </xsl:choose>
                </xsl:element>
              </xsl:element>
            </xsl:when>
            <xsl:otherwise>
              <xsl:element name="tr">
                <xsl:element name="td">
                  <xsl:attribute name="id">
                    <xsl:text>preprocessor</xsl:text>
                  </xsl:attribute>
                </xsl:element>
                <xsl:element name="td">
                  <xsl:attribute name="id">
                    <xsl:text>name</xsl:text>
                  </xsl:attribute>
                </xsl:element>
                <xsl:element name="td">
                  <xsl:attribute name="id">
                    <xsl:text>separator</xsl:text>
                  </xsl:attribute>
                </xsl:element>
                <xsl:element name="td">
                  <xsl:attribute name="id">
                    <xsl:text>argument</xsl:text>
                  </xsl:attribute>
                  <xsl:value-of select="./defname"/>
                  <xsl:choose>
                    <xsl:when test="position()=last()">
                      <xsl:text>)</xsl:text>
                    </xsl:when>
                    <xsl:otherwise>
                      <xsl:text>,</xsl:text>
                    </xsl:otherwise>
                  </xsl:choose>
                </xsl:element>
              </xsl:element>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:for-each>
      </xsl:when>
      <xsl:otherwise>
        <xsl:element name="tr">
          <xsl:element name="td">
            <xsl:attribute name="id">
              <xsl:text>preprocessor</xsl:text>
            </xsl:attribute>
            <xsl:text>#define</xsl:text>
          </xsl:element>
          <xsl:element name="td">
            <xsl:attribute name="id">
              <xsl:text>name</xsl:text>
            </xsl:attribute>
            <xsl:element name="a">
              <xsl:attribute name="href">
                <xsl:value-of select="concat('#', ./@id)"/>
              </xsl:attribute>
              <xsl:value-of select="./name"/>
            </xsl:element>
          </xsl:element>
        </xsl:element>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Define details stylesheet -->

  <xsl:template match="memberdef[@kind='define']" mode="details">
      <xsl:element name="hr"/>
      <xsl:element name="div">
        <xsl:attribute name="class">
          <xsl:text>details</xsl:text>
        </xsl:attribute>
        <xsl:element name="h3">
          <xsl:attribute name="id">
            <xsl:value-of select="./@id"/>
          </xsl:attribute>
          <xsl:value-of select="./name"/>
        </xsl:element>
        <xsl:element name="p">
          <xsl:value-of select="./briefdescription/para"/>
        </xsl:element>
        <xsl:element name="div">
          <xsl:attribute name="class">
            <xsl:text>api</xsl:text>
          </xsl:attribute>
          <xsl:choose>
            <xsl:when test="not(./param)">
              <xsl:text>#define </xsl:text>
              <xsl:value-of select="./name"/>
              <xsl:text> </xsl:text>
              <xsl:value-of select="substring-before(./initializer, '/**')"/>
            </xsl:when>
            <xsl:otherwise>
              <xsl:element name="table">
                <xsl:for-each select="./param">
                  <xsl:element name="tr">
                    <xsl:choose>
                      <xsl:when test="position()=1">
                        <xsl:element name="td">
                          <xsl:text>#define </xsl:text>
                          <xsl:value-of select="../name"/>
                          <xsl:text> (</xsl:text>
                        </xsl:element>
                        <xsl:element name="td">
                          <xsl:value-of select="./defname"/>
                          <xsl:choose>
                            <xsl:when test="not(position()=last())">
                              <xsl:text>,</xsl:text>
                            </xsl:when>
                            <xsl:otherwise>
                              <xsl:text>);</xsl:text>
                            </xsl:otherwise>
                          </xsl:choose>
                        </xsl:element>
                      </xsl:when>
                      <xsl:otherwise>
                        <xsl:element name="td"/>
                        <xsl:element name="td">
                          <xsl:if test="not(./defname='void')">
                            <xsl:text> </xsl:text>
                            <xsl:value-of select="./defname"/>
                          </xsl:if>
                          <xsl:choose>
                            <xsl:when test="not(position()=last())">
                              <xsl:text>,</xsl:text>
                            </xsl:when>
                            <xsl:otherwise>
                              <xsl:text>);</xsl:text>
                            </xsl:otherwise>
                          </xsl:choose>
                        </xsl:element>
                      </xsl:otherwise>
                    </xsl:choose>
                  </xsl:element>
                </xsl:for-each>        
              </xsl:element>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:element>
        <xsl:apply-templates select="./detaileddescription/para"/>
      </xsl:element>
      <xsl:choose>
        <xsl:when test="./param">
          <xsl:element name="div">
            <xsl:attribute name="class">
              <xsl:text>arguments</xsl:text>
            </xsl:attribute>
            <xsl:element name="table">
              <xsl:apply-templates select="./detaileddescription/para/parameterlist/parametername"/>
            </xsl:element>
          </xsl:element>
        </xsl:when>
      </xsl:choose>
  </xsl:template>

  <!-- Function stylesheet -->

  <!-- Function synopsis stylesheet -->

  <xsl:template match="memberdef[@kind='function']" mode="synopsis">
    <xsl:apply-templates select="param" mode="synopsis"/>
  </xsl:template>

  <xsl:template match="param" mode="synopsis">
    <xsl:choose>
      <xsl:when test="position()=1">
        <xsl:element name="tr">
          <xsl:element name="td">
            <xsl:attribute name="id">
              <xsl:text>return</xsl:text>
            </xsl:attribute>
            <xsl:value-of select="../type"/>
          </xsl:element>
          <xsl:element name="td">
            <xsl:attribute name="id">
              <xsl:text>name</xsl:text>
            </xsl:attribute>
            <xsl:element name="a">
              <xsl:attribute name="href">
                <xsl:text>#</xsl:text>
                <xsl:value-of select="../@id"/>
              </xsl:attribute>
              <xsl:value-of select="../name"/>
            </xsl:element>
          </xsl:element>
          <xsl:element name="td">
            <xsl:attribute name="id">
              <xsl:text>separator</xsl:text>
            </xsl:attribute>
            <xsl:text>(</xsl:text>
          </xsl:element>
          <xsl:element name="td">
            <xsl:attribute name="id">
              <xsl:text>argument</xsl:text>
            </xsl:attribute>
            <xsl:value-of select="./type"/>
            <xsl:text> </xsl:text>
            <xsl:value-of select="./declname"/>
            <xsl:choose>
              <xsl:when test="position()=last()">
                <xsl:text>);</xsl:text>
              </xsl:when>
              <xsl:otherwise>
                <xsl:text>,</xsl:text>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:element>
        </xsl:element>
      </xsl:when>
      <xsl:otherwise>
        <xsl:element name="tr">
          <xsl:element name="td">
            <xsl:attribute name="id">
              <xsl:text>return</xsl:text>
            </xsl:attribute>
          </xsl:element>
          <xsl:element name="td">
            <xsl:attribute name="id">
              <xsl:text>name</xsl:text>
            </xsl:attribute>
          </xsl:element>
          <xsl:element name="td">
            <xsl:attribute name="id">
              <xsl:text>separator</xsl:text>
            </xsl:attribute>
          </xsl:element>
          <xsl:element name="td">
            <xsl:attribute name="id">
              <xsl:text>argument</xsl:text>
            </xsl:attribute>
            <xsl:value-of select="./type"/>
            <xsl:text> </xsl:text>
            <xsl:value-of select="./declname"/>
            <xsl:choose>
              <xsl:when test="position()=last()">
                <xsl:text>);</xsl:text>
              </xsl:when>
              <xsl:otherwise>
                <xsl:text>,</xsl:text>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:element>
        </xsl:element>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Function details stylesheet -->

  <xsl:template match="memberdef[@kind='function']" mode="details">
    <xsl:element name="hr"/>
    <xsl:element name="div">
      <xsl:attribute name="class">
        <xsl:text>details</xsl:text>
      </xsl:attribute>
      <xsl:element name="h3">
        <xsl:attribute name="id">
          <xsl:value-of select="./@id"/>
        </xsl:attribute>
        <xsl:value-of select="./name"/>
      </xsl:element>
      <xsl:element name="p">
        <xsl:value-of select="./briefdescription/para"/>
      </xsl:element>
      <xsl:element name="div">
        <xsl:attribute name="class">
          <xsl:text>api</xsl:text>
        </xsl:attribute>
        <xsl:element name="table">
          <xsl:for-each select="./param">
            <xsl:element name="tr">
              <xsl:choose>
                <xsl:when test="position()=1">
                  <xsl:element name="td">
                    <xsl:value-of select="../type"/>
                    <xsl:text> </xsl:text>
                    <xsl:value-of select="../name"/>
                    <xsl:text> (</xsl:text>
                  </xsl:element>
                  <xsl:element name="td">
                    <xsl:value-of select="./type"/>
                    <xsl:text> </xsl:text>
                    <xsl:value-of select="./declname"/>
                    <xsl:choose>
                      <xsl:when test="not(position()=last())">
                        <xsl:text>,</xsl:text>
                      </xsl:when>
                      <xsl:otherwise>
                        <xsl:text>);</xsl:text>
                      </xsl:otherwise>
                    </xsl:choose>
                  </xsl:element>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:element name="td"/>
                  <xsl:element name="td">
                    <xsl:value-of select="./type"/>
                    <xsl:if test="not(./declname='void')">
                      <xsl:text> </xsl:text>
                      <xsl:value-of select="./declname"/>
                    </xsl:if>
                    <xsl:choose>
                      <xsl:when test="not(position()=last())">
                        <xsl:text>,</xsl:text>
                      </xsl:when>
                      <xsl:otherwise>
                        <xsl:text>);</xsl:text>
                      </xsl:otherwise>
                    </xsl:choose>
                  </xsl:element>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:element>
          </xsl:for-each>        
        </xsl:element>
      </xsl:element>
      <xsl:apply-templates select="./detaileddescription/para"/>
    </xsl:element>
    <xsl:element name="div">
      <xsl:attribute name="class">
        <xsl:text>arguments</xsl:text>
      </xsl:attribute>
      <xsl:element name="table">
        <xsl:apply-templates select="./detaileddescription/para/parameterlist/parametername"/>
        <xsl:element name="tr">
          <xsl:element name="td">
            <xsl:attribute name="id">
              <xsl:text>arglist</xsl:text>
            </xsl:attribute>
            <xsl:text>Return:</xsl:text>
          </xsl:element>
          <xsl:element name="td">
            <xsl:attribute name="id">
              <xsl:text>description</xsl:text>
            </xsl:attribute>
            <xsl:value-of select="./detaileddescription/para/simplesect"/>
          </xsl:element>
        </xsl:element>
      </xsl:element>
    </xsl:element>
  </xsl:template>

  <!-- Detailed description of functions and define -->
  
  <xsl:template match="detaileddescription/para">
    <xsl:element name="p">
      <xsl:apply-templates/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="parametername">
    <xsl:variable name="pos" select="position()"/>
    <xsl:element name="tr">
      <xsl:element name="td">
        <xsl:attribute name="id">
          <xsl:text>arglist</xsl:text>
        </xsl:attribute>
        <xsl:value-of select="."/>
        <xsl:text>:</xsl:text>
      </xsl:element>
      <xsl:element name="td">
        <xsl:attribute name="id">
          <xsl:text>description</xsl:text>
        </xsl:attribute>
        <xsl:value-of select="../parameterdescription[position()=$pos]/para"/>
      </xsl:element>
    </xsl:element>
  </xsl:template>
  
  <!-- Main image for the introduction -->

  <xsl:template name="doc_image">
    <xsl:element name="div">
      <xsl:attribute name="class">
        <xsl:text>image</xsl:text>
      </xsl:attribute>
      <xsl:element name="center">
        <xsl:element name="img">
          <xsl:attribute name="src">
            <xsl:value-of select="./compounddef/detaileddescription/para/image/@name"/>
          </xsl:attribute>
          <xsl:attribute name="alt">
            <xsl:text>E</xsl:text>
          </xsl:attribute>
          <xsl:attribute name="class">
            <xsl:text>imagetitle</xsl:text>
          </xsl:attribute>
        </xsl:element>
      </xsl:element>
    </xsl:element>
  </xsl:template>

  <!-- Description -->

  <xsl:template name="doc_description">
    <xsl:element name="div">
      <xsl:attribute name="class">
        <xsl:text>ref_description</xsl:text>
      </xsl:attribute>
      <xsl:element name="h2">
        <xsl:text>Description</xsl:text>
      </xsl:element>
      <xsl:apply-templates select="./compounddef[@id='indexpage']"/>
      </xsl:element>
  </xsl:template>

  <!-- Todo -->

  <xsl:template name="doc_todo">
    <xsl:element name="div">
      <xsl:attribute name="class">
        <xsl:text>ref_todo</xsl:text>
      </xsl:attribute>
      <xsl:element name="h2">
        <xsl:value-of select="./compounddef[@id='todo']/title"/>
      </xsl:element>
      
      <xsl:element name="ul">
        <xsl:apply-templates select="./compounddef[@id='todo']//listitem/para"/>
      </xsl:element>
    </xsl:element>
  </xsl:template>

  <!-- References -->
  
  <xsl:template match="ref">
    <xsl:element name="a">
      <xsl:attribute name="href">
        <xsl:text>#</xsl:text>
        <xsl:value-of select="./@refid"/>
      </xsl:attribute>
      <xsl:value-of select="."/>
    </xsl:element>
  </xsl:template>
  
  <!-- Program Listing -->

  <xsl:template match="programlisting">
    <xsl:element name="div">
      <xsl:attribute name="class">
        <xsl:text>programlisting</xsl:text>
      </xsl:attribute>
      <xsl:apply-templates/>
    </xsl:element>
  </xsl:template>
  
  <xsl:template match="sp">
    <xsl:text>&#xA0;</xsl:text>
  </xsl:template>
  
  <xsl:template match="highlight">
    <xsl:element name="span">
      <xsl:attribute name="class">
        <xsl:value-of select="./@class"/>
      </xsl:attribute>
      <xsl:apply-templates/>
    </xsl:element>
  </xsl:template>
  
  <xsl:template match="codeline">
    <xsl:apply-templates/>
    <xsl:element name="br"/>
  </xsl:template>

  <!-- for the description -->

  <xsl:template match="compoundname">
  </xsl:template>

  <xsl:template match="xrefsect">
  </xsl:template>
  
  <xsl:template match="parameterlist">
  </xsl:template>

  <xsl:template match="ulink">
    <xsl:element name="a">
      <xsl:attribute name="href">
        <xsl:value-of select="./@url"/>
      </xsl:attribute>
      <xsl:apply-templates/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="image[@type='latex']">
  </xsl:template>

  <xsl:template match="simplesect[@kind='author']">
    <xsl:element name="p">
      <xsl:choose>
        <xsl:when test="count(./para)=1">
          <xsl:text>Author:</xsl:text>
        </xsl:when>
        <xsl:otherwise>
          <xsl:text>Authors:</xsl:text>
        </xsl:otherwise>
      </xsl:choose>
      <xsl:element name="div">
        <xsl:attribute name="class">
          <xsl:text>author</xsl:text>
        </xsl:attribute>
        <xsl:element name="dl">
          <xsl:apply-templates select="para">
          </xsl:apply-templates>
        </xsl:element>
      </xsl:element>
    </xsl:element>
  </xsl:template>

  <xsl:template match="simplesect[@kind='author']/para">
    <xsl:element name="dl">
      <xsl:apply-templates/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="simplesect[@kind='date']/para">
    <xsl:element name="p">
      <xsl:text>Copyright</xsl:text>
      <xsl:element name="div">
        <xsl:attribute name="class">
          <xsl:text>date</xsl:text>
        </xsl:attribute>
        <xsl:text>(</xsl:text>
        <xsl:apply-templates/>
        <xsl:text>)</xsl:text>
      </xsl:element>
    </xsl:element>
  </xsl:template>

  <xsl:template match="title">
    <xsl:element name="h3">
      <xsl:attribute name="id">
        <xsl:text>#</xsl:text>
        <xsl:value-of select="../@id"/>
      </xsl:attribute>
      <xsl:apply-templates/>
    </xsl:element>
  </xsl:template>
  
  <xsl:template match="itemizedlist">
    <xsl:element name="ul">
      <xsl:apply-templates/>
    </xsl:element>
  </xsl:template>
  
  <xsl:template match="listitem">
    <xsl:element name="li">
      <xsl:apply-templates/>
    </xsl:element>
  </xsl:template>
  
  <xsl:template match="variablelist/listitem/para">
    <xsl:element name="li">
      <xsl:apply-templates/>
    </xsl:element>
  </xsl:template>
  
  <xsl:template match="verbatim">
    <xsl:element name="pre">
      <xsl:attribute name="class">
        <xsl:text>verbatim</xsl:text>
      </xsl:attribute>
      <xsl:apply-templates/>
    </xsl:element>
  </xsl:template>

</xsl:stylesheet>