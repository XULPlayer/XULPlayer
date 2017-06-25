set APPNAME=xulplayer
md release
md release\bin
cd release
copy ..\xulplayer.exe
copy ..\application.ini
copy ..\changelog.txt
md defaults
md defaults\preferences
copy ..\defaults\preferences\prefs.js defaults\preferences
cd bin
copy ..\..\bin\npxulplayer.dll
copy ..\..\bin\libiconv.dll
copy ..\..\bin\mediainfo.dll
copy ..\..\bin\zlib1.dll
copy ..\..\bin\sysinfo.*
copy ..\..\bin\*.xml
md tools
md codecs
cd codecs
copy ..\..\..\bin\codecs\mplayer.exe
xcopy ..\..\..\bin\codecs\mplayer . /s /y
copy ..\..\..\bin\codecs\cook.dll
cd ..\..\..
del chrome\%APPNAME%.jar
cd chrome
md ..\release\chrome
7z a -tzip -r -mx=0 -x!.svn ..\release\chrome\%APPNAME%.jar -x!*.zip -x!medialib -x!tv_utility -x!skin -x!*.manifest -x!*.kpf
pause
rem Packing skins
cd skin\classic
del ..\Classic.zip
7z a -tzip -r -mx=0 -x!.svn -x!black* -x!thumbnail* -x!flag* ..\Classic.zip
rem cd ..\pinky
rem del ..\Pinky.zip
rem 7z a -tzip -r -mx=0 -x!.svn ..\Pinky.zip
rem cd ..\black
rem del ..\Black.zip
rem 7z a -tzip -r -mx=0 -x!.svn ..\Black.zip
cd ..\..
xcopy /y skin\*.zip ..\release\chrome\skin\
pause
cd ..
echo content %APPNAME% jar:xulplayer.jar!/content/xulplayer/> release\chrome\chrome.manifest
echo locale xulplayer en-US jar:%APPNAME%.jar!/locale/en-US/>> release\chrome\chrome.manifest
echo locale xulplayer zh-CN jar:%APPNAME%.jar!/locale/zh-CN/>> release\chrome\chrome.manifest
echo locale xulplayer cz-CZ jar:%APPNAME%.jar!/locale/cz-CZ/>> release\chrome\chrome.manifest
echo locale xulplayer de-DE jar:%APPNAME%.jar!/locale/de-DE/>> release\chrome\chrome.manifest
echo locale xulplayer ja-JP jar:%APPNAME%.jar!/locale/ja-JP/>> release\chrome\chrome.manifest
echo locale xulplayer ko-KR jar:%APPNAME%.jar!/locale/ko-KR/>> release\chrome\chrome.manifest
echo locale xulplayer sk-SK jar:%APPNAME%.jar!/locale/sk-SK/>> release\chrome\chrome.manifest
echo skin %APPNAME% classic/1.0 jar:skin/classic.zip!/ >> release\chrome\chrome.manifest
:end
cd release
rem Final packing...
rem del /q m:\xulplayer.7z
rem 7z a -r -mx=9 -ms -x!.svn -x!extensions -x!updates m:\xulplayer.7z
cd ..\installer
"D:\Program Files (x86)\NSIS\makensis.exe" installer-oc.nsi
pause