<?php

/* 
 * thanks go to http://www.onlamp.com/pub/a/php/2002/11/28/php_i18n.html
 * derived work
 */
function msg($s) {
    global $messages;
    
    if (isset($messages[$s])) {
        return $messages[$s];
    } else {
        return $s;
        //error_log("l10n error:LANG:" . 
        //    "$lang,message:'$s'");
    }
}

function exitwitherror ($num,$text){
  echo "error\n".$num."\n".$text;
  exit();
}

function ma_4int($string)
{
  if (strlen($string)!=4) return 0;
  $entpackt = unpack("l",$string);
  return $entpackt[1];
}

function le_8int($string)
{
  if (strlen($string)!=8) return 0;
  $entpackt = unpack("C*",$string);
  $number = $entpackt[1];
  $number += $entpackt[2]*256;
  $number += $entpackt[3]*65536;
  $number += $entpackt[4]*16777216;
  $number += $entpackt[5]*4294967296;
  $number += $entpackt[6]*1099511627776;
  $number += $entpackt[7]*281474976710656;
  $number += $entpackt[8]*72057594037927936;
  return $number;
}

function dirstoarray($dirs) {
  /*$ret = array();
  while ($dirs != "") {
    
  }
  */
  return explode("/",$dirs);
  //TODO: erster eintrag vllt "" ??
}

/*
 * returns array(Name of dir,size of dir,1 if hassubdirs else 0)
 */
function get_name_size_hasdirs($tdu_file,$pointer) {
  $seeked = fseek($tdu_file,$pointer); 
  if ($seeked) 
    exitwitherror (4,msg("Pointererror").": ".msg("Error seeking file"));

  $dummys = fread($tdu_file,8);
  $dsize = le_8int($dummys);

  $dummys = fread($tdu_file,4);
  $size = ma_4int($dummys);

  $name = fread($tdu_file,$size);
  if ($charset!=="utf-8") $name=utf8_encode($name);

  $dummys = fread($tdu_file,4);
  $numsubdirs = ma_4int($dummys);

  if ($numsubdirs>0) return array($name,$dsize,1);
  return array($name,$dsize,0);
}

function a_cmp($a,$b) { // TODO: effizient??? nein! ;-)
  if ($a[1]>$b[1]) return -1;
  if ($a[1]<$b[1]) return 1;
  if ($a[2]==2) return -1;
  if ($b[2]==2) return 1;
  if ($a[0]>$b[0]) return 1;
  return -1;
}

function rek_getentry($tdu_file,$aktpointer,$aktdeep,$dirs) {
  $seeked = fseek($tdu_file,$aktpointer);
  if ($seeked) exitwitherror (4,msg("Error seeking file"));
  
  $dummys = fread($tdu_file,8);
  $dsize = le_8int($dummys);

  $dummys = fread($tdu_file,4);
  //echo "dummys:$dummys!<br>\n";
  $size = ma_4int($dummys);
  //echo "size:$size!<br>\n";
  if ($size>0)
    $name = fread($tdu_file,$size);
  else
    $name = ".";

  //echo $name." ist noch nicht durch<br>\n";
  if ($dirs[0]=="." && $aktdeep==1) $dirs[0]=$name;
  //echo "namen: ".$size.$name."/".$dirs[$aktdeep-1]."!<br>\n";
  if ($name!==$dirs[$aktdeep-1]) return false;
  //echo $name." ist durch<br>\n";

  $dummys = fread($tdu_file,4);
  $numsubdirs = ma_4int($dummys);
  
  $subdirptrs=array();
  for ($i=0;$i<$numsubdirs;$i++) {
    $dummys = fread($tdu_file,4);
    array_push($subdirptrs,ma_4int($dummys));
  }
  //echo $aktdeep."/".count($dirs);
  if ($aktdeep<count($dirs)) {
    for ($i=0;$i<$numsubdirs;$i++) {
      $rekreturn = rek_getentry($tdu_file,$subdirptrs[$i],$aktdeep+1,$dirs);
      if ($rekreturn) {
        return $rekreturn;
      }
    }
  }
  else {
    //$subdat=array(array("this dir",$dsize,2));
    //if ($dsize<0) 
    //echo "ERROR!: dsize:".$dsize."<br>\n";
    $subdat=array(array(".",$dsize,2));
    for ($i=0;$i<$numsubdirs;$i++) {
      $entry=get_name_size_hasdirs($tdu_file,$subdirptrs[$i]);
      //echo "groessen - gesamt:".$dsize." dieses subdir:".$entry[1]."!<br>\n";
      $subdat[0][1]-=$entry[1];
      array_push($subdat,$entry);
    }
    return $subdat;
  }
  return false;
}

function rek_parseentrys_all($tdu_file,$aktpointer,$aktdeep,$dirs) {
  $seeked = fseek($tdu_file,$aktpointer);
  if ($seeked) {
            echo msg("Error seeking file").": ".msg("exiting")."<br>\n";
            exit(1);
  }
  //print ("seeked: ".intval($seeked)."<br><br>\n");
  
  $dummys = fread($tdu_file,8);
  $dsize = le_8int($dummys);
  //print ("Verzeichnisgroesse: ".$dsize."<br><br>\n");

  $dummys = fread($tdu_file,4);
  $size = ma_4int($dummys);
  if ($size>0)
    $name = fread($tdu_file,$size);
  else 
    $name = "";
  //print ("Verzeichnisname: ".$name."<br><br>\n");
  
  $dummys = fread($tdu_file,4);
  $numsubdirs = ma_4int($dummys);
  
  for ($i=0;$i<$aktdeep;$i++) {
    echo "&nbsp;";
  }
  if ($numsubdirs>0) 
    echo "T ";
  else
    echo "L ";
  echo $name."\t".$dsize." byte<br>\n"; // ".$numsubdirs." subdirs<br>\n";
  $subdirptrs=array();
  for ($i=0;$i<$numsubdirs;$i++) {
    $dummys = fread($tdu_file,4);
    //$pointer = ma_4int($dummys);
    array_push($subdirptrs,ma_4int($dummys));
    //echo "going to ".$i.". subdir at:".$pointer." ".$aktdeep." deep!<br>\n";
    //rek_parseentrys($tdu_file,$pointer,$aktdeep+1,$dirs);
  }
  for ($i=0;$i<$numsubdirs;$i++) {
    rek_parseentrys_all($tdu_file,$subdirptrs[$i],$aktdeep+1,$dirs);
  }
}

?>
