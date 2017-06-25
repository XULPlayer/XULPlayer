#!/bin/bash

BASEDIR=`dirname "$0"`

#absolute path
TMP=`echo "X${BASEDIR}" | grep "^X/"`
if test -z ${TMP}; then
    BASEDIR="${PWD}/${BASEDIR}"
fi

BINDIR="${BASEDIR}/bin"
LIBDIR="${BASEDIR}/lib"
PLUGINDIR="${BASEDIR}/plugins"

if test -z ${XULRUNNER}; then
	XULRUNNER="${BINDIR}/xulrunner"
    if test ! -f ${XULRUNNER}; then
        echo "#############################################"
        echo "# Please set XULRUNNER"
        echo "# eg. export XULRUNNER=/usr/bin/xulrunner-1.9"
        echo "#############################################"
        exit
    fi
fi

if test ! -f ${XULRUNNER}; then
    echo "#############################################"
    echo "# xulrunner ${XULRUNNER} can not be found"
    echo "#############################################"
    exit
fi

if test "${JSCONSOLE}" = "yes" -o "${JSCONSOLE}" = "y"; then
    JSCONSOLE="-jsconsole"
fi

if test "${LOG_CONSOLE}" = "yes" -o "${LOG_CONSOLE}" = "y"; then
	export XRE_CONSOLE_LOG="/tmp/signplayer.log"
fi

INIFILE="${BASEDIR}/application.ini"

export MOZ_PLUGIN_PATH=${PLUGINDIR}

if [ -z "${LD_LIBRARY_PATH}" ]; then
    export LD_LIBRARY_PATH=${LIBDIR}
else
    export LD_LIBRARY_PATH=${LIBDIR}:$LD_LIBRARY_PATH
fi

XINIT=

if [ "$1" != "" ]; then
   XINIT="xinit" 
fi

echo "LD_LIBRARY_PATH=${LD_LIBRARY_PATH}"
echo "MOZ_PLUGIN_PATH=${PLUGINDIR}"
echo "${XINIT} ${XULRUNNER} ${INIFILE}"

${XINIT} ${XULRUNNER} ${INIFILE} ${JSCONSOLE}
