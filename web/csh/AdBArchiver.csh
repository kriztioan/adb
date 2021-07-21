#!/bin/csh
set nonomatch
set BASE = $argv[1]
set URL  = $argv[2]
set DEST = $argv[3]
/usr/bin/curl -s -L -o ${DEST} ${URL}
if ( `/usr/bin/file -b --mime-type ${DEST}` != 'application/pdf' ) then
  /bin/rm ${DEST}
endif
