<?php
/* tdu - TreeDiskUsage -
   copyright michael.heide@student.uni-siegen.de

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

$tdu_filename="/home/hede/outtest";
include 'tdu-config.php.inc';

/*function exitwitherror($text) 
{
  print($text."\n</body></html>");
  exit(-1);
}*/
?>

<html>
<head>
<title>TDU - show all</title>
<body>
<h2>Der komplette Inhalt der letzten Suchanfrage</h2>
<?php

  $tdu_file = fopen ($tdu_filename,"r");
  if (!$tdu_file) exitwitherror ("Konnte Datei nicht &ouml;ffnen");
  $dummys=fread($tdu_file,10);
  if ($dummys!="TDUv0.1---") exitwitherror ("Falscher Dateityp");

  $dummys=fread($tdu_file,4);
  $eintraege = ma_4int($dummys);
  print ("Eintr&auml;ge: ".$eintraege."<br>\n");

  $dummys=fread($tdu_file,4);
  $endzeit = ma_4int($dummys);
  
  $dummys=fread($tdu_file,4);
  $startzeit = ma_4int($dummys);
  
  $dauer = $endzeit - $startzeit;
  
  $endzeit = date ("d.m.Y H:i:s",$endzeit);
  $startzeit = date ("d.m.Y H:i:s",$startzeit);
  
  print ("Letzter Lauf ging von: ".$startzeit." bis: ".$endzeit
        ." und dauerte ".$dauer." Sekunden<br>\n");

  $dummys = fread($tdu_file,4);
  $size = ma_4int($dummys);
  $name = fread($tdu_file,$size);

  print ("Letzter Scan in folgendem Verzeichnis begonnen: ".$name."<br><br>\n");

  $dummys = fread($tdu_file,4);
  //$pointer = le_8int($dummys);
  $pointer = ma_4int($dummys);
  //print ("Springe zum ersten Eintrag: ".$pointer."<br><br>\n");
  //print ("pointer:".$pointer."<br>\n");
  rek_parseentrys_all($tdu_file,$pointer,0,array());
?>
</body>
</html>
