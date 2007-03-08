<?php
  $tdu_filename="outtest"; //data file
  //$startseite="php-scan.php"; //where to go incase of a restart
  $startseite="tdu.php"; //where to go incase of a restart
  $warteseite="tdu-wait.php"; //here you can wait for scan to finish
  $tdu_files=array("outtest", // only the filename will be shown, no path
                   "outtest2",
                   "backup5.tdu",
                   "/somewhere/outtest",
                   "itserver.tdu");
  $charset="iso-8859-15"; // system charset, e.g. utf-8 ascii iso-8859-1 etc.
  //$charset="utf-8";
  //setlocale (LC_ALL, 'de_DE');
  $lang="de"; //language. supportet: "" (empty means en) and "de"


  //nothing to configure below here


  include 'tdu_fkt.php.inc';
  //include ${"tdu-lang_".$lang.".php.inc"};
  include "tdu-lang_".$lang.".php.inc";
  //include $tdulang;
?>
