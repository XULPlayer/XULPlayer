<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="xml" encoding="utf-8" doctype-public="-//MOZILLA//DTD XUL V1.0//EN"/>
<xsl:template match="/prefs/node/node/node">

<window xmlns:html="http://www.w3.org/1999/xhtml"
        xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
        xmlns="http://www.mozilla.org/keymaster/gatekeeper/there.is.only.xul"
        >
<hbox>
<xsl:choose>
<xsl:when test="@type='bool'">

	<vbox>
	<radiogroup orient="horizontal">
	<radio label="Yes" oncommand="SaveValue('true')">
	<xsl:if test="value='true'"><xsl:attribute name="selected">true</xsl:attribute></xsl:if>
	</radio>
	<radio label="No" oncommand="SaveValue('false')">
	<xsl:if test="value!='true'"><xsl:attribute name="selected">true</xsl:attribute></xsl:if>
	</radio>
	</radiogroup>
	</vbox>
	<vbox flex="1"/>

</xsl:when>

<xsl:when test="@type='enum'">

	<vbox>
	<menulist flex="false" oncommand="SaveValue(selectedItem.getAttribute('label'))">
        <menupopup>
			<xsl:for-each select="enum">
            <menuitem>
				<xsl:attribute name="label"><xsl:value-of select="."/></xsl:attribute>
				<xsl:if test=".=../value"><xsl:attribute name="selected">true</xsl:attribute></xsl:if>
			</menuitem>
			</xsl:for-each>
        </menupopup>
	</menulist>
	</vbox>
	<vbox flex="1"/>

</xsl:when>

<xsl:when test="@type='int' or @type='float'">

	<vbox>
	<button label="Apply"/>
	</vbox>
	<vbox>
	<textbox onchange="SaveValue(this.value)" size="8">
	<xsl:attribute name="value"><xsl:value-of select="value"/></xsl:attribute>
	</textbox>
	</vbox>
	<vbox flex="1"/>

</xsl:when>

<xsl:otherwise>

	<vbox>
	<button label="Apply"/>
	</vbox>
	<vbox flex="1">
	<textbox onchange="SaveValue(this.value)">
	<xsl:attribute name="value"><xsl:value-of select="value"/></xsl:attribute>
	</textbox>
	</vbox>

</xsl:otherwise>

</xsl:choose>	

</hbox>

</window>
</xsl:template>

</xsl:stylesheet>