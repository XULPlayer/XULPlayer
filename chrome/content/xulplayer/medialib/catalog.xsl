<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE window SYSTEM "chrome://xulplayer/locale/xulplayer.dtd">
<?xml-stylesheet href="chrome://global/skin/" type="text/css"?>
<?xml-stylesheet href="chrome://dvdrip/skin/dvdrip.css" type="text/css"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="xml" encoding="utf-8" doctype-public="-//MOZILLA//DTD XUL V1.0//EN"/>

<xsl:template match="/items">
   
<window xmlns:html="http://www.w3.org/1999/xhtml"
        xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
        xmlns="http://www.mozilla.org/keymaster/gatekeeper/there.is.only.xul"
        style="background:none"
        class="mainwindow"
        flex="1"
        >

 <tree id="media-lib-list" flex="1" ondblclick="onMediaLibClick(this)">
   <treecols>
     <treecol id="file-title" label="Title" flex="1" primary="true" />
   </treecols>
   <treechildren>
    <treeitem id="medialib-video" container="true" open="true">
        <treerow>
          <treecell label="&medialib.videoFiles;"/>
        </treerow>
        <treechildren>
            <xsl:for-each select="dir[count(item[@type='1']) &gt; 0]">
                <treeitem container="true">
                    <treerow>
                        <treecell>
                            <xsl:attribute name="label">
                                <xsl:value-of select="@name"/> (<xsl:value-of select="count(item[@type='1'])"/>)
                            </xsl:attribute>
                        </treecell>
                    </treerow>
                    <treechildren>
                    <xsl:for-each select="item[@type='1']">
                        <xsl:sort select="@dir" order="descending"/>
                        <treeitem>
                          <xsl:attribute name="uri"><xsl:value-of select="@dir"/>\<xsl:value-of select="@path"/></xsl:attribute>
                          <treerow>
                            <treecell>
                                <xsl:attribute name="label"><xsl:value-of select="@name"/></xsl:attribute>
                            </treecell>
                          </treerow>
                        </treeitem>
                    </xsl:for-each>
                    </treechildren>
                </treeitem>
            </xsl:for-each>
        </treechildren>
    </treeitem>
    
    <treeitem id="medialib-audio" container="true" open="true">
        <treerow>
          <treecell label="&medialib.audioFiles;"/>
        </treerow>
        <treechildren>
            <xsl:for-each select="dir[count(item[@type='0']) &gt; 0]">
                <treeitem container="true">
                    <treerow>
                        <treecell>
                            <xsl:attribute name="label">
                                <xsl:value-of select="@name"/> (<xsl:value-of select="count(item[@type='0'])"/>)
                            </xsl:attribute>
                        </treecell>
                    </treerow>
                    <treechildren>
                        <xsl:for-each select="item[@type='0']">
                            <xsl:sort select="@dir" order="descending"/>
                            <treeitem>
                                <xsl:attribute name="uri"><xsl:value-of select="@dir"/>\<xsl:value-of select="@path"/></xsl:attribute>
                                    <treerow>
                                        <treecell>
                                            <xsl:attribute name="label"><xsl:value-of select="@name"/></xsl:attribute>
                                        </treecell>
                                      </treerow>
                            </treeitem>
                        </xsl:for-each>
                    </treechildren>
                </treeitem>
            </xsl:for-each>
        </treechildren>
    </treeitem>
    
    <treeitem id="medialib-image" container="true" open="true">
        <treerow>
          <treecell label="&medialib.imageFiles;"/>
        </treerow>
        <treechildren>
            <xsl:for-each select="dir[count(item[@type='2']) &gt; 0]">
                <treeitem container="true">
                    <treerow>
                        <treecell>
                            <xsl:attribute name="label">
                                <xsl:value-of select="@name"/>
                            </xsl:attribute>
                        </treecell>
                    </treerow>
                    <treechildren>
                        <xsl:for-each select="item[@type='2']">
                            <xsl:sort select="@dir" order="descending"/>
                            <treeitem>
                                <xsl:attribute name="uri"><xsl:value-of select="@dir"/>\<xsl:value-of select="@path"/></xsl:attribute>
                                    <treerow>
                                        <treecell>
                                            <xsl:attribute name="label"><xsl:value-of select="@name"/> (<xsl:value-of select="count(item[@type='2'])"/>)</xsl:attribute>
                                        </treecell>
                                      </treerow>
                            </treeitem>
                        </xsl:for-each>
                    </treechildren>
                </treeitem>
            </xsl:for-each>
        </treechildren>
    </treeitem>

   </treechildren>
 </tree>

</window>
</xsl:template>
    
</xsl:stylesheet>