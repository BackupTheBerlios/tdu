<?php
  //TODO: kontrolle, ob mittlerweiler neuer scan. STARTZEIT!

  //with this all configs and additional functions will be loaded
  include 'tdu-config.php.inc';

  //see if there are errors, here should be some error handling
  //redirect to some dump site for now...
  if (isset($_GET["error"])) {
    $error = (int) $_GET["error"];
    header("Location: dumperror.php?error=".$error);
    exit;
  }

  //see if we have to open a user specified file
  if (isset($_GET["fileno"])) {
    $fileno = (int) $_GET["fileno"];
  } else $fileno = 0;
  //will be used when error handling is implemented:
  /*
    if (!isset($tdu_files[$fileno])) $fileno=0;
    if (isset($tdu_files[$fileno])) $tdu_filename = $tdu_files[$fileno];
    else {
      header("Location: ".$startseite."?error=9");
      exit;
    }
  } else {  //no user file, open standard file
    if (isset($tdu_files[0])) {$fileno=0; $tdu_filename = $tdu_files[$fileno];}
    else {  //no files defined, goto starting page with error
      header("Location: ".$startseite."?error=9");
      exit;
    }
  }*/
  //if $tdu_files[$fileno] does not exist, search an existing one
  $warningchangedfile=false;
  if (!is_readable($tdu_files[$fileno])) {
    $warningchangedfile=true;
    for ($fileno=0;$fileno<count($tdu_files);$fileno++) {
      if (is_readable($tdu_files[$fileno])) break;
    }
    if (!is_readable($tdu_files[$fileno])) {
      header("Location: ".$startseite."?error=10");
      exit;
    }
  }
  $tdu_filename = $tdu_files[$fileno];
  //echo "tdufile:".$tdu_filename."<br>\n";
  
  //open file
  $fehlerlevel = error_reporting (0);
  $tdu_file = fopen ($tdu_filename,"r");
  error_reporting ($fehlerlevel);
  if (!$tdu_file) {
    header("Location: ".$startseite."?error=1"); //file open error - exists?
    exit;
  }
  $dummys=fread($tdu_file,10);
  if ($dummys!="TDUv0.1---") {
    header("Location: ".$startseite."?error=2"); // wrong filetype, no tdu file
    exit;
  }

  // read number of entrys
  $dummys=fread($tdu_file,4);
  $eintraege = ma_4int($dummys);
  // read finishing time, zero if scan is still running
  $dummys=fread($tdu_file,4);
  $endzeit = ma_4int($dummys);

  /*// forward to waiting page if scan is still running
  if ($endzeit == 0 || $eintraege == 0) {
    header("Location: ".$warteseite); 
    exit;
  }*/

  //starting time (scan)
  $dummys=fread($tdu_file,4);
  $startzeit = ma_4int($dummys);

  //scanroot
  $dummys = fread($tdu_file,4);
  $size = ma_4int($dummys);
  $scanroot = fread($tdu_file,$size);

  //pointer to first entry
  $dummys = fread($tdu_file,4);
  $pointer = ma_4int($dummys);

  //seek to first entry
  $seeked = fseek($tdu_file,$pointer);
  if ($seeked) {
    header("Location: ".$startseite."?error=4"); // Error seeking file
    exit;
  }

  //read directory size
  $dummys = fread($tdu_file,8);
  $fullsize = le_8int($dummys);

  //length of name
  $dummys = fread($tdu_file,4);
  $size = ma_4int($dummys);
  //if there is no name, call it .
  if ($size>0)
    $rootname = fread($tdu_file,$size);
  else
    $rootname = ".";

  //$fullsize=439808;

  //array of prefixes for sizes
  $prefixes = Array("","k","M","G","T","P","E");
  $fullsizehumanized = $fullsize;
  $j=0;
  //calculate human readable
  while ($fullsizehumanized>1024) {
    $fullsizehumanized = $fullsizehumanized / 1024;
    $j++;
  }

  //TODO: proof if this is necessarry, will be converted to string either way
  //      down there
  $fullsizehumanized = (int) $fullsizehumanized;

  //TODO: nicht noetig, wenn nicht eingerueckt
  /*
  if ($fullsizehumanized<100) {
    if ($fullsizehumanized<10)
      $fullsizehumanized = "_".$fullsizehumanized;
    $fullsizehumanized = "_".$fullsizehumanized;
  }*/

  //add prefix and suffix to size
  $fullsizehumanized = $fullsizehumanized.$prefixes[$j]."B";

  //if (strlen($_GET["sd"])<1)
  if (!isset($_GET["sd"]))
    $sizedisplay=2;
  else
    $sizedisplay=(int) $_GET["sd"];
  if ($sizedisplay<1 || $sizedisplay>3) $sizedisplay=2;
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
<title>tdu - TreeDiskUsage - Ajax-Applet</title>
<script src="tdu.js" type="text/javascript"></script>
<?php
if ($lang!="") {
?>
<script src="tdu-lang_<?php echo $lang ?>.js" type="text/javascript"></script>
<?php
}
?>
<link rel="stylesheet" type="text/css" href="tdu.css">
<meta http-equiv="Content-Type" content="text/html; charset=<?php echo $charset ?>" />
<?php
/* geben setInterval() den vorzug, da unterbrechbar
if ($endzeit==0)
  echo "<meta http-equiv=\"refresh\" content=\"5\">\n";
  */
?>
<script type="text/javascript">
if (typeof refresh !== 'undefined')
  window.clearTimeout(refresh);
<?php
  if ($endzeit==0) 
    echo "var refresh = "
         ."window.setTimeout('window.location.reload(true)', 5000);\n";
  echo "var startzeit=".$startzeit."\n";  // starttime of last scan
  echo "var fullsize=".$fullsize."\n"; // size of the scanned directory
  echo "var sizedisplay=".$sizedisplay."\n"; // display mode of "graphic"
  echo "var scanroot=\"".$scanroot."\"\n"; // path to scandir
  echo "var fileno=".$fileno."\n"; // file number in tdu_files array
  //echo "var lang=".$lang."\n"; //lang var in tdu-config.php
?>
</script>
<!--[if IE]>
  <script type="text/javascript" src="tdu-IEworkaround.js"></script>
<![endif]-->
</head>
<body>
<?php
//TODO: etwas huebscher waere schoen ;-)
if ($warningchangedfile) echo "<H3>".msg("Warning: Changed File")."</H3>";
?>
<table class="head">
 <tr>
  <td class="topleft"><img src="img/tdulogo.png" alt="TDU"></td>
  <td class="top">
<?php
  echo msg("select file to view");
?>
   <form>
    <select name="filename" size="1" onchange="ChangeFile(this.form)" <?php if ($endzeit==0) echo "onfocus=\"window.clearTimeout(refresh)\" onblur=\"refresh=window.setTimeout('window.location.reload(true)', 5000);\""; ?> >
<?php
 for ($i=0;$i<count($tdu_files);$i++) {
   if (is_readable($tdu_files[$i])) {
     /* if (strrchr($tdu_files[$i],"/"))
       $filename=substr(strrchr($tdu_files[$i],"/"),1);
     else 
       $filename=$tdu_files[$i];*/
     echo "     <option value=\"".$i."\"";
     if ($i==$fileno) echo " selected";
     echo ">".basename($tdu_files[$i],".tdu")."</option>\n"; 
   }
 }
?>
    </select>
   </form>
 </td>
  <td class="top">
 <?php
if ($endzeit!=0) {
echo msg("select reference for graphic display");
?>
   <form>
    <select name="sizedisplay" size="1" onchange="ChangeDisplay(this.form)">
     <option value="1" <?php if ($sizedisplay==1) echo "selected"; echo ">".msg("Parent as reference");?></option>
     <option value="2" <?php if ($sizedisplay==2) echo "selected"; echo ">".msg("Total size as reference");?></option>
     <option value="3" <?php if ($sizedisplay==3) echo "selected"; echo ">".msg("Biggest as reference");?></option>
    </select>
   </form>
<?php
}
?>
  </td>
  <td class="top">
  </td>
 </tr>
<tr><td class="left">
<p class="small">Alpha Version 17</p>
<p class="infos">
<?php
  echo msg("Entrys").":<br>&nbsp;&nbsp;".$eintraege."<br>";
  echo msg("starting time").":<br>&nbsp;&nbsp;".date(msg("Y-m-d H:i:s"),$startzeit)."<br>";
  if ($endzeit!=0) {
    echo msg("ending time").":<br>&nbsp;&nbsp;".date(msg("Y-m-d H:i:s"),$endzeit)."<br>";
    $dauers=$endzeit-$startzeit;
  } else { //if endzeit==0, still running
    $dauers=time()-$startzeit;
  }
  $dauerm=0;
  while ($dauers>60) {
    $dauers-=60;
    $dauerm++;
  }
  echo msg("runtime").":<br>&nbsp;&nbsp;".$dauerm."m:".$dauers."s"."<br>";
?>
</p>
<a href="tdu-scan.php">ReScan File</a><br>
</td><td class="inhalt" colspan="3">
<?php
if ($endzeit!=0) {
?>
 <table>
 <tr><th><?php echo msg("Directory"); ?></td><th><?php echo msg("Size"); ?></td><th><?php echo msg("Graphic"); ?></td></tr>
 <tr id="1" dirsize=<?php echo '"'.$fullsize.'"' ?>><td class="floated"><div class="linked" onclick="openPlus('<?php echo $rootname ?>','1')" openPlusFct="openPlus('<?php echo $rootname ?>','1')" closeMinusFct="closeMinus('1','<?php echo $rootname ?>','1')"><img src="plus.png" alt="+"></div><div class="linked" onclick="alert('<?php echo $rootname ?>')"><?php echo $rootname ?></div></td><td class="size"><div><?php echo $fullsizehumanized ?></div></td><td class="size"><img alt="100%" src="img/100.png" title="100%"></td></tr>
<!-- <tr id="1" dirsize=<?php echo '"'.$fullsize.'"' ?>><td class="floated"><div class="linked" onclick="openPlus('<?php echo $rootname ?>','1')" openPlus="openPlus('<?php echo $rootname ?>','1')" closeMinus="closeMinus('1','<?php echo $rootname ?>','1')"><img src="plus.png" alt="+"></div><div class="linked" onclick="alert('<?php echo $rootname ?>')"><?php echo $rootname ?></div></td><td class="size"><div><?php echo $fullsizehumanized ?></div></td><td class="size"><img alt="100" src="img/100.png"></td></tr>-->
 </table>
<?php
} else {
 echo msg("Scan still running, please wait...");
}
?>
</td></tr>
</table>
</body>
