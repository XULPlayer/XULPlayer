<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE window SYSTEM "chrome://xulplayer/locale/xulplayer.dtd">
<?xml-stylesheet href="chrome://global/skin/" type="text/css"?>
<?xml-stylesheet href="chrome://dvdrip/skin/dvdrip.css" type="text/css"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="xml" encoding="utf-8" doctype-public="-//MOZILLA//DTD XUL V1.0//EN"/>
  
<xsl:template match="/info">
   
<window xmlns:html="http://www.w3.org/1999/xhtml"
        xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
        xmlns="http://www.mozilla.org/keymaster/gatekeeper/there.is.only.xul"
        style="background:none"
        class="mainwindow"
        >

<hbox>
    <groupbox>
        <caption><label>&player.video;</label></caption>
        <grid flex="1">
        <columns>
          <column/>
          <column/>
        </columns>
        <rows>
          <row>
            <hbox align="center"><label value="&player.codec;:"/></hbox>
            <textbox readonly="true">
                <xsl:attribute name="value">
                    <xsl:choose>
                        <xsl:when test="video/codec='mpeg12'">
                            <xsl:text>MPEG</xsl:text>
                        </xsl:when>
                        <xsl:when test="video/codec='odivx'">
                            <xsl:text>MPEG-4</xsl:text>
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:value-of select="translate(video/codec,'abcdefghijklmnopqrstuvwxyz','ABCDEFGHIJKLMNOPQRSTUVWXYZ')"/>
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:attribute>
            </textbox>
          </row>
          <row>
            <hbox align="center"><label value="&player.bitrate;:"/></hbox>
            <textbox readonly="true">
                <xsl:attribute name="value">
                <xsl:if test="video/bitrate &gt; 0">
                <xsl:value-of select="video/bitrate"/> bps
                </xsl:if>
                <xsl:if test="video/bitrate = 0">N/A</xsl:if>
                </xsl:attribute>
            </textbox>
          </row>
          <row>
            <hbox align="center"><label value="&player.resolution;:"/></hbox>
            <textbox readonly="true">
                <xsl:attribute name="value">
                    <xsl:if test="video/width">
                    <xsl:value-of select="video/width"/>x<xsl:value-of select="video/height"/>
                    </xsl:if>
                </xsl:attribute>
            </textbox>
          </row>
          <row>
            <hbox align="center"><label value="&player.fps;:"/></hbox>
            <textbox readonly="true">
                <xsl:attribute name="value">
                <xsl:for-each select="video/fps">
                    <xsl:value-of select="."/> fps
                </xsl:for-each>
                </xsl:attribute>
            </textbox>
          </row>
          <row>
            <hbox align="center"><label value="&player.aspect;:"/></hbox>
            <textbox readonly="true">
                <xsl:attribute name="value">
                    <xsl:value-of select="video/aspect"/>
                </xsl:attribute>
            </textbox>
          </row>
        </rows>
        </grid>
        <spacer flex="1"/>
    </groupbox>
    <groupbox>
        <caption><label>&player.audio;</label></caption>
        <grid flex="1">
        <columns>
          <column/>
          <column/>
        </columns>
        <rows>
          <row>
            <hbox align="center"><label value="&player.codec;:"/></hbox>
            <textbox readonly="true">
                <xsl:attribute name="value">
                    <xsl:choose>
                        <xsl:when test="audio/codec='faad'">
                            <xsl:text>AAC/AAC+</xsl:text>
                        </xsl:when>
                        <xsl:when test="audio/codec='a52'">
                            <xsl:text>AC-3</xsl:text>
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:value-of select="translate(audio/codec,'abcdefghijklmnopqrstuvwxyz','ABCDEFGHIJKLMNOPQRSTUVWXYZ')"/>
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:attribute>
            </textbox>
          </row>
          <row>
            <hbox align="center"><label value="&player.bitrate;:"/></hbox>
            <textbox readonly="true">
                <xsl:attribute name="value">
                <xsl:for-each select="audio/bitrate">
                    <xsl:value-of select="."/> bps
                </xsl:for-each>
                </xsl:attribute>
            </textbox>
          </row>
          <row>
            <hbox align="center"><label value="&player.srate;:"/></hbox>
            <textbox readonly="true">
                <xsl:attribute name="value">
                <xsl:for-each select="audio/srate">
                    <xsl:value-of select="."/> Hz
                </xsl:for-each>
                </xsl:attribute>
            </textbox>
          </row>
          <row>
            <hbox align="center"><label value="&player.channels;:"/></hbox>
            <textbox readonly="true">
                <xsl:attribute name="value">
                    <xsl:choose>
                    <xsl:when test="audio/nch='1'">Mono</xsl:when>
                    <xsl:when test="audio/nch='2'">Stereo</xsl:when>
                    <xsl:otherwise><xsl:value-of select="audio/nch"/></xsl:otherwise>
                    </xsl:choose>
                </xsl:attribute>
            </textbox>
          </row>
        </rows>
        </grid>
        <spacer flex="1"/>
    </groupbox>
</hbox>

</window>
</xsl:template>
    
</xsl:stylesheet>