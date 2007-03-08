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
