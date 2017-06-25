<?xml version="1.0" encoding="utf-8"?><!DOCTYPE xsl:stylesheet  [
	<!ENTITY nbsp   "&#160;">
	<!ENTITY copy   "&#169;">
	<!ENTITY reg    "&#174;">
	<!ENTITY trade  "&#8482;">
	<!ENTITY mdash  "&#8212;">
	<!ENTITY ldquo  "&#8220;">
	<!ENTITY rdquo  "&#8221;"> 
	<!ENTITY pound  "&#163;">
	<!ENTITY yen    "&#165;">
	<!ENTITY euro   "&#8364;">
]>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" encoding="utf-8"/>
<xsl:template match="/prefs">
<strong>Key: </strong>
<xsl:call-template name="GoBranch">
<xsl:with-param name="cur" select="."/>
</xsl:call-template>

</xsl:template>

<xsl:template name="GoBranch">
	<xsl:param name="cur"/>

	<xsl:value-of select="$cur/node/@key"/>

	<!--Loop to the deepest level-->
	<xsl:if test="$cur/node/node/@key">.<xsl:call-template name="GoBranch">
		<xsl:with-param name="cur" select="$cur/node"/>
		</xsl:call-template>
	</xsl:if>

	<xsl:if test="string-length($cur/node/node/@key)=0">
		<!--Here we come to the deepest level-->		
		<xsl:if test="$cur/node/@type='int' or $cur/node/@type='float'">
		<xsl:if test="$cur/node/@min or $cur/node/@max">
		&nbsp;<strong>Range: </strong><xsl:value-of select="$cur/node/@min"/>~<xsl:value-of select="$cur/node/@max"/>
		</xsl:if>
		</xsl:if>
		<xsl:if test="$cur/node/@type='string'">
		&nbsp;<strong>Max Length: </strong><xsl:value-of select="$cur/node/@max"/>
		</xsl:if>
		<xsl:if test="$cur/node/desc">
		<br /><strong>Description:</strong>
		&nbsp;<xsl:value-of select="$cur/node/desc"/>
		</xsl:if>
		<br/>&nbsp;
	</xsl:if>
</xsl:template>
</xsl:stylesheet>