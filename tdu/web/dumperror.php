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

?>
<html>
<title>tdu - TreeDiskUsage</title>
<body><p>There is no error handling for this error, here is the dump:<br>
<?php
  if (isset($_GET["error"])) {
      $error = (int) $_GET["error"];
      echo "your error code is: ".$error."<br>";
      $fehlerlevel = error_reporting (0);
      $file = fopen ("errorcodes.txt","r");
      error_reporting ($fehlerlevel);
      if (!$file) {
        echo "For an explanation see errorcodes.txt in source directory.";
      } else {
        $zeile = fgets($file);
        while ($zeile) {
          echo $zeile."<br>\n";
          $zeile = fgets($file);
        }
      }
  } else {
    echo "hugh? there is no error to display...";
  }
                
?>
</p>
<p>
Fix the problem and then reload <a href="tdu.php">tdu.php</a>
</p>
</body>
</html>
