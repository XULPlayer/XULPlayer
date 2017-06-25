#!/bin/sh

BUILD="release"

BINDIR="${BUILD}/bin"
LIBDIR="${BUILD}/lib"

CHROME_SRC_DIR="chrome"
CHROME_DST_DIR="${BUILD}/${CHROME_SRC_DIR}"

CHROME_EXT_DIRS="skin icons"

LOCALES="en-US"
LOCALE_SRC_DIR="${CHROME_SRC_DIR}/locale"
LOCALE_DST_DIR="${BUILD}/${LOCALE_SRC_DIR}"

PREFS_SRC_DIR="defaults/preferences"
PREFS_SRC_FILE="${PREFS_SRC_DIR}/prefs.js"
PREFS_DST_DIR="${BUILD}/${PREFS_SRC_DIR}"

XULAPP="xulplayer"
XULAPP_SRC_DIR="chrome/content/${XULAPP}"
XULAPP_DST_DIR="${BUILD}/${XULAPP_SRC_DIR}"

MANIFEST_SRC_FILE="${CHROME_SRC_DIR}/chrome.${XULAPP}.manifest"

XULRUNNER_SDK="xulrunner-sdk"
XULRUNNER_VERSION="1.9.0.11"
XULRUNNER_FILENAME="xulrunner-${XULRUNNER_VERSION}.en-US.linux-i686.sdk.tar.bz2"
XULRUNNER_DOWNLOAD_URL="http://releases.mozilla.org/pub/mozilla.org/xulrunner/releases/${XULRUNNER_VERSION}/sdk/${XULRUNNER_FILENAME}"

LAUNCHER_SRC="launcher.sh"
LAUNCHER_DST="${BUILD}/${XULAPP}"

EXT_PLUGIN_SRC_DIR="${HOME}/.mozilla/plugins"
PLUGIN_DST_DIR="${BUILD}/plugins"

EXT_LIBS_SRC="lib/libiconv.*"

#################################helper function####################################################
check_file()
{
    if [ -z $1 ]; then
        return
    fi

    if test ! -f $1; then
        echo "file $1 can not be found!"
        exit
    fi
}

check_dir()
{
    if [ -z $1 ]; then
        return
    fi

    if test ! -d $1; then
        echo "dir $1 can not be found!"
        exit
    fi
}

#################################copy xunrunner bin to bin dir######################################
mkdir -p ${BINDIR}
mkdir -p ${LIBDIR}

if test ! -d ${XULRUNNER_SDK}; then
    wget ${XULRUNNER_DOWNLOAD_URL}
    tar xvf ${XULRUNNER_FILENAME}
    rm -rf ${XULRUNNER_FILENAME}
fi

check_dir ${XULRUNNER_SDK}

cp -rf ${XULRUNNER_SDK}/bin/* ${BINDIR}


#################################copy xulapp to target dir######################################
check_dir ${XULAPP_SRC_DIR}
mkdir -p ${XULAPP_DST_DIR}

###
echo "copying ${XULAPP} ..."
cp -a ${XULAPP_SRC_DIR}/* ${XULAPP_DST_DIR}
#remove .svn dir in xulapp
find ${XULAPP_DST_DIR} -name .svn -type d 2>/dev/null | xargs rm -rf

###
check_dir ${LOCALE_SRC_DIR}
echo "copying locale files ..."
mkdir -p ${LOCALE_DST_DIR}
for locale in ${LOCALES};
do
    if test -d ${LOCALE_SRC_DIR}/${locale}; then
        mkdir -p ${LOCALE_DST_DIR}/${locale}
        cp -a ${LOCALE_SRC_DIR}/${locale}/* ${LOCALE_DST_DIR}/${locale}
        find ${LOCALE_DST_DIR}/${locale} -name .svn -type d 2>/dev/null | xargs rm -rf
    fi
done

###
for d in ${CHROME_EXT_DIRS}; 
do
    if test -d ${CHROME_SRC_DIR}/$d; then
        mkdir -p ${CHROME_DST_DIR}/$d
        cp -a ${CHROME_SRC_DIR}/$d/* ${CHROME_DST_DIR}/$d
        find ${CHROME_DST_DIR}/$d -name .svn -type d 2>/dev/null | xargs rm -rf
    fi
done

###
if test -d ${EXT_PLUGIN_SRC_DIR}; then
    echo "copying ext plugins"
    mkdir -p ${PLUGIN_DST_DIR}
    cp -a ${EXT_PLUGIN_SRC_DIR}/* ${PLUGIN_DST_DIR}
fi

###
if [ "X${EXT_LIBS_SRC}" != "X" ]; then
    echo "copying ext libs"
    cp -a ${EXT_LIBS_SRC} ${LIBDIR}
fi

###
check_file ${MANIFEST_SRC_FILE}
echo "copying manifest file ..."
mkdir -p ${CHROME_DST_DIR}
cp ${MANIFEST_SRC_FILE} ${CHROME_DST_DIR}

###
check_file ${PREFS_SRC_FILE}
echo "copying prefs.js"
mkdir -p ${PREFS_DST_DIR}
cp ${PREFS_SRC_FILE} ${PREFS_DST_DIR}
sed -i "s/xulplayer/${XULAPP}/g" ${PREFS_DST_DIR}/prefs.js

###
check_file application.ini
echo "copying application.ini"
cp application.ini ${BUILD}

if test -f ${LAUNCHER_SRC}; then
    cp -f ${LAUNCHER_SRC} ${LAUNCHER_DST}
    chmod +x ${LAUNCHER_DST}
fi
