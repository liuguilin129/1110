<?xml version="1.0" encoding="utf-16"?>

<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <!-- Unit Definition Template -->
  <xsl:template name="unit_definition">
    <!-- XPath searching for a DATAUNIT ancestor -->
    <xsl:if test="count(ancestor::DATAUNIT | ancestor::DATAUNITARRAY) > 0">
      <xsl:if test="position() > 1">
        <xsl:text>&#xa;&#x9;</xsl:text>
      </xsl:if>
    </xsl:if>
    <xsl:if test="count(ancestor::DATAUNIT | ancestor::DATAUNITARRAY) = 0">
      <xsl:text>&#xa;</xsl:text>
      <xsl:value-of select="@typequalifier"/>
      <xsl:text>&#xa;</xsl:text>
      <xsl:value-of select="@datatype"/>
      <xsl:text> </xsl:text>
      <xsl:if test="@vartype = string('PTR')">
        <xsl:text>* </xsl:text>
      </xsl:if>
      <xsl:value-of select="@name"/>
      <xsl:text> = </xsl:text>
    </xsl:if>
    <xsl:text>{ </xsl:text>
    <xsl:if test="count(ancestor::DATAUNIT | ancestor::DATAUNITARRAY) = 0">
      <xsl:text>&#xa;&#x9;</xsl:text>
    </xsl:if>
  </xsl:template>

  <!-- Array Definition Template -->
  <xsl:template name="array_definition">
    <!-- XPath searching for a DATAUNIT ancestor -->
    <xsl:if test="count(ancestor::DATAUNIT | ancestor::DATAUNITARRAY) > 0">
      <xsl:if test="position() > 1">
        <xsl:text>&#xa;&#x9;</xsl:text>
      </xsl:if>
    </xsl:if>
    <xsl:if test="count(ancestor::DATAUNIT | ancestor::DATAUNITARRAY) = 0">
      <xsl:text>&#xa;</xsl:text>
      <xsl:value-of select="@typequalifier"/>
      <xsl:text>&#xa;</xsl:text>
      <xsl:value-of select="@datatype"/>
      <xsl:if test="@vartype = string('PTR')">
        <xsl:text>* </xsl:text>
      </xsl:if>
      <xsl:text> </xsl:text>
      <xsl:value-of select="@name"/>
      <xsl:text>[] = </xsl:text>
    </xsl:if>
    <xsl:text>{ </xsl:text>
    <xsl:if test="count(ancestor::DATAUNIT | ancestor::DATAUNITARRAY) = 0">
      <xsl:text>&#xa;&#x9;</xsl:text>
    </xsl:if>
  </xsl:template>

  <!-- DATAELEMENT Template -->
  <xsl:template name="DATAELEMENT">
    <xsl:if test="name(current()) = 'DATAELEMENT'">
      <!-- start a new line for lengthy values -->
      <xsl:variable name="element_name">
        <xsl:value-of select="@value"/>
      </xsl:variable>
      <xsl:if test="string-length($element_name) > 10">
        <xsl:if test="position() > 1">
          <xsl:text>&#xa;&#x9;</xsl:text>
        </xsl:if>
      </xsl:if>

      <!-- use an & for reference type variables -->
      <xsl:if test="@valtype = 'REFERENCE'">
        <xsl:text disable-output-escaping="yes">&amp;</xsl:text>
      </xsl:if>

      <xsl:value-of select="@value"/>
      <xsl:call-template name="end_data_block"/>

      <!-- start new line for every 10 elements -->
      <xsl:if test="position() mod 8 = 0">
        <xsl:if test="position() != last()">
          <xsl:text>&#xa;&#x9;</xsl:text>
        </xsl:if>
      </xsl:if>
    </xsl:if>
    <xsl:if test="name(current()) = 'DATAELEMENTPTR'">
      <!-- start a new line for lengthy variables -->
      <xsl:variable name="element_name">
        <xsl:value-of select="@value"/>
      </xsl:variable>
      <xsl:if test="string-length($element_name) > 10">
        <xsl:if test="position() > 1">
          <xsl:text>&#xa;&#x9;</xsl:text>
        </xsl:if>
      </xsl:if>

      <!-- do typcasting if any -->
      <xsl:variable name="typecast">
        <xsl:value-of select="@typecast_type"/>
      </xsl:variable>
      <xsl:if test="string-length($typecast) > 0">
        <xsl:text>(</xsl:text>
        <xsl:value-of select="@typecast_type"/>
        <xsl:if test="@typecast_to = 'PTR'">
          <xsl:text>*</xsl:text>
        </xsl:if>
        <xsl:text>) </xsl:text>
      </xsl:if>

      <!-- use an & for reference type variables -->
      <xsl:if test="@vartype = 'REFERENCE'">
        <xsl:text disable-output-escaping="yes">&amp;</xsl:text>
      </xsl:if>
      <xsl:if test="@valtype = 'REFERENCE'">
        <xsl:text disable-output-escaping="yes">&amp;</xsl:text>
      </xsl:if>

      <xsl:value-of select="@value"/>

      <xsl:if test="count(parent::FEATURE_IF | 
                          parent::FEATURE_ELIF | 
                          parent::FEATURE_ELSE |
                          parent::FEATURE_IF_BINARY | 
                          parent::FEATURE_ELIF_BINARY) > 0">
        <xsl:if test="position() = last()">
          <xsl:text>, </xsl:text>
        </xsl:if>
      </xsl:if>

      <xsl:if test="position() != last()">
        <xsl:text>, </xsl:text>
      </xsl:if>

      <!-- start new line for every 10 elements -->
      <xsl:if test="position() mod 8 = 0">
        <xsl:if test="position() != last()">
          <xsl:text>&#xa;&#x9;</xsl:text>
        </xsl:if>
      </xsl:if>
    </xsl:if>
  </xsl:template>

  <!-- DATAUNIT Template -->
  <xsl:template name="DATAUNIT">
    <xsl:if test="name(current()) = 'DATAUNIT'">
      <xsl:call-template name="unit_definition"/>
      <xsl:call-template name="process_data"/>
      <xsl:text> }</xsl:text>
      <xsl:if test="count(ancestor::DATAUNIT | ancestor::DATAUNITARRAY) = 0">
        <xsl:text>;</xsl:text>
      </xsl:if>
      <xsl:if test="count(ancestor::DATAUNIT | ancestor::DATAUNITARRAY) > 0">
        <xsl:if test="count(parent::FEATURE_IF | 
                          parent::FEATURE_ELIF | 
                          parent::FEATURE_ELSE |
                          parent::FEATURE_IF_BINARY | 
                          parent::FEATURE_ELIF_BINARY) > 0">
          <xsl:if test="position() = last()">
            <xsl:text>, </xsl:text>
          </xsl:if>
        </xsl:if>

        <xsl:if test="position() != last()">
          <xsl:text>, </xsl:text>
        </xsl:if>
      </xsl:if>
    </xsl:if>
  </xsl:template>

  <!-- DATAUNITARRAY Template -->
  <xsl:template name="DATAUNITARRAY">
    <xsl:if test="name(current()) = 'DATAUNITARRAY'">
      <xsl:call-template name="array_definition"/>
      <xsl:call-template name="process_data"/>
      <xsl:text> }</xsl:text>
      <xsl:if test="count(ancestor::DATAUNIT) = 0">
        <xsl:text>;</xsl:text>
      </xsl:if>

      <!--if the unit array is contained in DATAUNIT-->
      <xsl:if test="count(ancestor::DATAUNIT) > 0">
        <xsl:call-template name="end_data_block"/>
      </xsl:if>
    </xsl:if>
  </xsl:template>

  <!-- DATAELEMENTARRAY Template -->
  <xsl:template name="DATAELEMENTARRAY">
    <xsl:if test="name(current()) = 'DATAELEMENTARRAY'">
      <xsl:call-template name="array_definition"/>
      <xsl:call-template name="process_data"/>
      <xsl:text> }</xsl:text>
      <xsl:if test="count(ancestor::DATAUNIT) = 0">
        <xsl:text>;</xsl:text>
      </xsl:if>
      <!--if the element array is contained in DATAUNIT-->
      <xsl:if test="count(ancestor::DATAUNIT) > 0">
        <xsl:call-template name="end_data_block"/>
      </xsl:if>
    </xsl:if>
  </xsl:template>

  <!-- CALUNIT_INSTANCE Template -->
  <xsl:template name="CALUNIT_INSTANCE">
    <xsl:if test="name(current()) = 'CALUNIT_INSTANCE'">
      <xsl:if test="position() > 1">
        <xsl:text>&#xa;&#x9;</xsl:text>
      </xsl:if>
      <!-- use an & for reference type variables -->
      <xsl:if test="@vartype = 'REFERENCE'">
        <xsl:text disable-output-escaping="yes">&amp;</xsl:text>
      </xsl:if>
      <xsl:value-of select="@name"/>
      <xsl:text>, </xsl:text>
    </xsl:if>
  </xsl:template>

  <!-- Process Data Template -->
  <xsl:template name="process_data">
    <xsl:for-each select="*">
      <xsl:call-template name="FEATURISATION"/>
      <!--Process DATAELEMENTARRAY -->
      <xsl:call-template name="DATAELEMENTARRAY"/>
      <!--Process DATAUNIT -->
      <xsl:call-template name="DATAUNIT"/>
      <!--Process DATAUNITARRAY -->
      <xsl:call-template name="DATAUNITARRAY"/>
      <!--Process DATAELEMENT -->
      <xsl:call-template name="DATAELEMENT"/>
      <!--Process CALUNIT_INSTANCE -->
      <xsl:call-template name="CALUNIT_INSTANCE"/>
    </xsl:for-each>
  </xsl:template>

  <!-- End Data with a ','-->
  <xsl:template name="end_data_block">
    <xsl:if test="count(parent::FEATURE_IF | 
                          parent::FEATURE_ELIF | 
                          parent::FEATURE_ELSE |
                          parent::FEATURE_IF_BINARY | 
                          parent::FEATURE_ELIF_BINARY) > 0">
      <xsl:if test="position() = last()">
        <xsl:text>, </xsl:text>
      </xsl:if>
    </xsl:if>
    <xsl:if test="position() != last()">
      <xsl:text>, </xsl:text>
    </xsl:if>
  </xsl:template>

  <xsl:template name="FEATURISATION">
    <xsl:if test="name(current()) = 'FEATURISATION'">
      <xsl:apply-templates select="FEATURE_IF"/>
      <xsl:apply-templates select="FEATURE_IF_BINARY"/>
      <xsl:apply-templates select="FEATURE_ELIF"/>
      <xsl:apply-templates select="FEATURE_ELIF_BINARY"/>
      <xsl:apply-templates select="FEATURE_ELSE"/>
      <xsl:text>&#xa;#endif /* </xsl:text>
      <xsl:value-of select="child::*/attribute::name"/>
      <xsl:variable name="OPR">
        <xsl:value-of select="child::FEATURE_IF_BINARY/BINARY_OPERATION/@opr"/>
      </xsl:variable>
      <xsl:for-each select="child::FEATURE_IF_BINARY/BINARY_OPERATION/OPERAND">
        <xsl:text>(</xsl:text>
        <xsl:value-of select="@name"/>
        <xsl:text>)</xsl:text>
        <xsl:if test="position() != last()">
          <xsl:if test="string($OPR)=string('OR')">
            <xsl:text disable-output-escaping="yes"> || </xsl:text>
          </xsl:if>
          <xsl:if test="string($OPR)=string('AND')">
            <xsl:text disable-output-escaping="yes"> &amp;&amp; </xsl:text>
          </xsl:if>
        </xsl:if>
      </xsl:for-each>
      <xsl:text> */&#xa;&#x9;</xsl:text>
    </xsl:if>
  </xsl:template>

  <!-- FEATURE_IF Template -->
  <xsl:template match="FEATURE_IF">
    <xsl:text>&#xa;#ifdef </xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:text>&#xa;&#x9;</xsl:text>
    <xsl:call-template name="process_data"/>
  </xsl:template>

  <!-- FEATURE_ELIF Template -->
  <xsl:template match="FEATURE_ELIF">
    <xsl:text>&#xa;#elif defined(</xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:text>)</xsl:text>
    <xsl:text>&#xa;&#x9;</xsl:text>
    <xsl:call-template name="process_data"/>
  </xsl:template>

  <!-- FEATURE_IF_BINARY Template -->
  <xsl:template match="FEATURE_IF_BINARY">
    <xsl:text>&#xa;#if </xsl:text>
    <xsl:call-template name="BINARY_OPERATION"/>
    <xsl:text>&#xa;&#x9;</xsl:text>
    <xsl:call-template name="process_data"/>
  </xsl:template>

  <xsl:template name="BINARY_OPERATION">
    <xsl:for-each select="*">
      <xsl:if test="name(current()) = 'BINARY_OPERATION'">

        <xsl:variable name="OPR">
          <xsl:value-of select="@opr"/>
        </xsl:variable>
        <!-- Go for operands now -->
        <xsl:for-each select="OPERAND">
          <xsl:text>defined(</xsl:text>
          <xsl:value-of select="@name"/>
          <xsl:text>)</xsl:text>
          <xsl:if test="position() != last()">
            <xsl:if test="string($OPR)=string('OR')">
              <xsl:text disable-output-escaping="yes"> || </xsl:text>
            </xsl:if>
            <xsl:if test="string($OPR)=string('AND')">
              <xsl:text disable-output-escaping="yes"> &amp;&amp; </xsl:text>
            </xsl:if>
          </xsl:if>
        </xsl:for-each>
        <!-- place the operator before any embedded binary opr -->
        <xsl:if test="count(current()/child::BINARY_OPERATION) > 0">
          <xsl:if test="string(@opr)=string('OR')">
            <xsl:text disable-output-escaping="yes"> || </xsl:text>
          </xsl:if>
          <xsl:if test="string(@opr)=string('AND')">
            <xsl:text disable-output-escaping="yes"> &amp;&amp; </xsl:text>
          </xsl:if>
        </xsl:if>
        <!-- Recursive Call -->
        <xsl:call-template name="BINARY_OPERATION"/>
      </xsl:if>
    </xsl:for-each>
  </xsl:template>

  <!-- FEATURE_ELIF_BINARY Template -->
  <xsl:template match="FEATURE_ELIF_BINARY">
    <xsl:variable name="OPR">
      <xsl:value-of select="BINARY_OPERATION/@opr"/>
    </xsl:variable>
    <xsl:text>&#xa;#elif </xsl:text>
    <xsl:for-each select="BINARY_OPERATION/OPERAND">
      <xsl:text>defined (</xsl:text>
      <xsl:value-of select="@name"/>
      <xsl:text>)</xsl:text>
      <xsl:if test="position() != last()">
        <xsl:if test="string($OPR)=string('OR')">
          <xsl:text disable-output-escaping="yes"> || </xsl:text>
        </xsl:if>
        <xsl:if test="string($OPR)=string('AND')">
          <xsl:text disable-output-escaping="yes"> &amp;&amp; </xsl:text>
        </xsl:if>
      </xsl:if>
    </xsl:for-each>
    <xsl:text>&#xa;&#x9;</xsl:text>
    <xsl:call-template name="process_data"/>
  </xsl:template>

  <!-- FEATURE_ELSE Template -->
  <xsl:template match="FEATURE_ELSE">
    <xsl:text>&#xa;#else&#xa;&#x9;</xsl:text>
    <xsl:call-template name="process_data"/>
  </xsl:template>

</xsl:stylesheet> 
