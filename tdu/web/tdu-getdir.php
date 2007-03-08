<?
  //TODO: Fehlerausgabe von php unterbinden, wenn alle abgefangen werden.

  //echo "./1\na/12\nb/15\nc/19";
  //echo "error\n0\nTestfehler";
  //exit();
  include 'tdu-config.php.inc';
  $dir = (string) $_GET["dir"];
  $line = (string) $_GET["line"];
  $fileno = (int) $_GET["fileno"];

  //if (!isset($_GET["dir"]) || !preg_match("/^[\s\d\w\/\.]*$/",$dir))
  //echo "dir: ".$_GET["dir"]."\n<br>";
  if (!isset($_GET["dir"]) || preg_match("/[\x01-\x19]/",$dir))
    exitwitherror (5,msg("wrong parameter")." dir");
  if (!isset($_GET["line"]) || !preg_match("/^[\d\.]*$/",$line))
    exitwitherror (6,msg("wrong parameter")." line");
  if (!isset($_GET["fileno"])|| !preg_match("/^[\d]*$/",$_GET["fileno"]))
    exitwitherror (7,msg("wrong Parameter")." fileno");
  //echo "fileno: ".$_GET["fileno"]."\n<br>";
  $dirs = explode ("/",$dir);
//print_r($dirs);
//exit();

  $fehlerlevel = error_reporting (0);
  $tdu_file = fopen ($tdu_files[$fileno],"r");
  error_reporting ($fehlerlevel);
  if (!$tdu_file) exitwitherror (1,msg("file open error").msg(", does it exist?"));
  $dummys=fread($tdu_file,10);
  if ($dummys!="TDUv0.1---") exitwitherror (2,msg("wrong filetype").msg(", no tdu file"));

  $dummys=fread($tdu_file,4);
  $eintraege = ma_4int($dummys);

  $dummys=fread($tdu_file,4);
  $endzeit = ma_4int($dummys);

  if ($endzeit == 0 || $eintraege == 0) exitwitherror (3,msg("Work in Progress"));

  $dummys=fread($tdu_file,4);
  $startzeit = ma_4int($dummys);

  $dummys = fread($tdu_file,4);
  $size = ma_4int($dummys);
  $scanroot = fread($tdu_file,$size);

  $dummys = fread($tdu_file,4);
  //$pointer = le_8int($dummys);
  $pointer = ma_4int($dummys);

  $entrys = rek_getentry($tdu_file,$pointer,1,$dirs);
  if (!is_array($entrys)) {
    exitwitherror (8,$dir.msg(" does not exist or has no subdirs"));
  }

  echo $startzeit."\n".$dir."\n".$line."\n";

  usort($entrys,"a_cmp");
  foreach ($entrys as $entry) {
    echo $entry[0]."/".$entry[1]."/".$entry[2]."\n";
  }
?>
