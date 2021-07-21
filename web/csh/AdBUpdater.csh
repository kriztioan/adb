#! /bin/csh

set BASE   = $argv[1]
set PLUGIN = $argv[2]
set CGIBIN = $cwd

echo "<br />Downloading update<br />"
/usr/bin/wget --quiet http://www.astro.rug.nl/~boersma/software/AdB.tgz -O ${BASE}download/AdB.tgz
echo "Unpacking update<br />"
/bin/tar C ${BASE}download/ -xvzf ${BASE}download/AdB.tgz > /dev/null
echo "Doing backup<br/>"
/bin/rm --force ${BASE}backup/*
cd ${BASE}
/bin/tar -cvzf ${BASE}backup/backup.tgz * > /dev/null
echo "Running update script<br />"
echo "<br />"
${BASE}download/update.csh ${BASE} ${PLUGIN} ${CGIBIN}
echo "<br />"
echo "<b>Update completed</b><br />"
echo "<br/>"
echo "<i>Please reload main window to make new version active</i><br />"
echo "<hr>"
echo "<hr>"
