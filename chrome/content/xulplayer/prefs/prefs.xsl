<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE window SYSTEM "chrome://xulplayer/locale/xulplayer.dtd">
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" encoding="utf-8" doctype-public="-//MOZILLA//DTD XUL V1.0//EN"/>
  <xsl:template match="/MediaCoderPrefs">
    <!--?xml-stylesheet href="chrome://global/skin/" type="text/css"?-->
    <window id="prefs_tree"
        xmlns:html="http://www.w3.org/1999/xhtml"
        xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
        xmlns="http://www.mozilla.org/keymaster/gatekeeper/there.is.only.xul"
	flex="1"
        >
      <listbox flex="1">
        <listhead>
          <listheader label="&converter.parameter;"/>
          <listheader label="&converter.value;"/>
        </listhead>
        <listcols>
          <listcol/>
          <listcol flex="1"/>
        </listcols>
        <xsl:if test="node[@key='overall.video.enabled']='true'">
        <listitem>
          <listcell label="&player.video;" style="font-weight:bold"/>
        </listitem>
        <xsl:choose>
          <xsl:when test="node[@key='overall.video.format']='XviD'">
            <listitem>
              <listcell label="&player.codec;"/>
              <listcell><xsl:attribute name="label">MPEG-4 (XviD)</xsl:attribute></listcell>
            </listitem>
            <listitem>
              <listcell label="&converter.profile;"/>
              <listcell><xsl:attribute name="label"><xsl:value-of select="node[@key='videoenc.xvid.profile']"/></xsl:attribute></listcell>
            </listitem>
          </xsl:when>
          <xsl:when test="node[@key='overall.video.format']='H.264'">
            <listitem>
              <listcell label="&player.codec;"/>
              <listcell><xsl:attribute name="label">AVC (H.264)</xsl:attribute></listcell>
            </listitem>
            <listitem>
              <listcell label="&converter.profile;"/>
              <listcell><xsl:attribute name="label"><xsl:value-of select="node[@key='videoenc.x264.profile']"/></xsl:attribute></listcell>
            </listitem>
            <listitem>
              <listcell label="&converter.level;"/>
              <listcell><xsl:attribute name="label"><xsl:value-of select="node[@key='videoenc.x264.level']"/></xsl:attribute></listcell>
            </listitem>
          </xsl:when>
          <xsl:otherwise>
            <listitem>
              <listcell label="&player.codec;"/>
              <listcell><xsl:attribute name="label"><xsl:value-of select="node[@key='overall.video.format']"/></xsl:attribute></listcell>
            </listitem>
          </xsl:otherwise>
        </xsl:choose>
        <listitem>
          <listcell label="&player.bitrate;"/>
          <listcell>
            <xsl:attribute name="label"><xsl:value-of select="node[@key='overall.video.bitrate']"/> KBps</xsl:attribute>
          </listcell>
        </listitem>
        <listitem>
          <listcell label="&converter.resolution;"/>
          <listcell>
            <xsl:attribute name="label">
            <xsl:if test="node[@key='videofilter.scale.enabled'] = 'true'">
              <xsl:value-of select="node[@key='videofilter.scale.width']"/>x<xsl:value-of select="node[@key='videofilter.scale.height']"/>
            </xsl:if>
            </xsl:attribute>
          </listcell>
        </listitem>
        <listitem>
          <listcell label="&converter.framerate;"/>
          <listcell>
	    <xsl:attribute name="label">
	    <xsl:choose>
	      <xsl:when test="node[@key='videofilter.frame.fps'] = '30000'">29.97</xsl:when>
	      <xsl:when test="node[@key='videofilter.frame.fps'] = '24000'">23.976</xsl:when>
	      <xsl:when test="node[@key='videofilter.frame.fps'] = '15000'">14.985</xsl:when>
	      <xsl:otherwise>
		<xsl:value-of select="node[@key='videofilter.frame.fps']"/><xsl:if test="node[@key='videofilter.frame.fpsScale']!='1'">/<xsl:value-of select="node[@key='videofilter.frame.fpsScale']"/></xsl:if>
	      </xsl:otherwise>
	    </xsl:choose> fps</xsl:attribute>
          </listcell>
        </listitem>
        <listitem/>
        </xsl:if>
        <listitem>
          <listcell label="&player.audio;" style="font-weight:bold"/>
        </listitem>
        <xsl:choose>
          <xsl:when test="node[@key='overall.audio.encoder'] = 'LAME MP3'">
	    <listitem>
	      <listcell label="&player.codec;"/>
	      <listcell><xsl:attribute name="label">MP3</xsl:attribute></listcell>
	    </listitem>
            <listitem>
              <xsl:choose>
                <xsl:when test="node[@key='audioenc.lame.mode'] = 'CBR'">
                  <listcell label="&player.bitrate;"/>
                  <listcell>
                    <xsl:attribute name="label"><xsl:value-of select="node[@key='audioenc.lame.bitrate']"/> KBps</xsl:attribute>
                  </listcell>
                </xsl:when>
                <xsl:otherwise>
                  <listcell label="&converter.quality;"/>
                  <listcell>
                    <xsl:attribute name="label"><xsl:value-of select="node[@key='audioenc.lame.quality']"/></xsl:attribute>
                  </listcell>
                </xsl:otherwise>
              </xsl:choose>
            </listitem>
          </xsl:when>
          <xsl:when test="node[@key='overall.audio.encoder'] = 'FAAC'">
	    <listitem>
	      <listcell label="&player.codec;"/>
	      <listcell><xsl:attribute name="label">AAC</xsl:attribute></listcell>
	    </listitem>
            <listitem>
              <xsl:choose>
                <xsl:when test="node[@key='audioenc.faac.mode'] = 'Bitrate'">
                  <listcell label="&player.bitrate;"/>
                  <listcell>
                    <xsl:attribute name="label"><xsl:value-of select="node[@key='audioenc.faac.bitrate']"/> KBps</xsl:attribute>
                  </listcell>
                </xsl:when>
                <xsl:otherwise>
                  <listcell label="&converter.quality;"/>
                  <listcell>
                    <xsl:attribute name="label"><xsl:value-of select="node[@key='audioenc.faac.quality']"/></xsl:attribute>
                  </listcell>
                </xsl:otherwise>
              </xsl:choose>
            </listitem>
          </xsl:when>
	  <xsl:otherwise>
	    <listitem>
	      <listcell label="&player.codec;"/>
	      <listcell>
		<xsl:attribute name="label"><xsl:value-of select="node[@key='overall.audio.encoder']"/></xsl:attribute>
	      </listcell>
	    </listitem>
	  </xsl:otherwise>
        </xsl:choose>
	<xsl:if test="node[@key='audiofilter.resample.samplerate']!='0'">
        <listitem>
          <listcell label="&converter.samplerate;"/>
          <listcell>
            <xsl:attribute name="label"><xsl:value-of select="node[@key='audiofilter.resample.samplerate']"/> Hz</xsl:attribute>
          </listcell>
        </listitem>
	</xsl:if>
        <listitem/>
        <listitem>
          <listcell label="&converter.container;" style="font-weight:bold"/>
        </listitem>
        <listitem>
          <listcell label="&converter.format;"/>
          <listcell>
            <xsl:attribute name="label"><xsl:value-of select="node[@key='overall.container.format']"/></xsl:attribute>
          </listcell>
        </listitem>
        <listitem/>
        <listitem>
          <listcell label="&converter.picture;" style="font-weight:bold"/>
        </listitem>
        <listitem>
          <listcell label="&converter.cropping;"/>
          <listcell>
            <xsl:attribute name="label"><xsl:value-of select="node[@key='videofilter.crop.mode']"/></xsl:attribute>
          </listcell>
        </listitem>
        <listitem/>
        <listitem>
          <listcell label="&converter.device;" style="font-weight:bold"/>
        </listitem>
        <listitem>
          <listcell label="&converter.videoPath;"/>
          <listcell>
            <xsl:attribute name="label"><xsl:value-of select="node[@key='extension.device.videoPath']"/></xsl:attribute>
          </listcell>
        </listitem>
        <listitem>
          <listcell label="&converter.audioPath;"/>
          <listcell>
            <xsl:attribute name="label"><xsl:value-of select="node[@key='extension.device.audioPath']"/></xsl:attribute>
          </listcell>
        </listitem>
        <listitem>
          <listcell label="&converter.imagePath;"/>
          <listcell>
            <xsl:attribute name="label"><xsl:value-of select="node[@key='extension.device.imagePath']"/></xsl:attribute>
          </listcell>
        </listitem>
      </listbox>
    </window>
  </xsl:template>
</xsl:stylesheet>