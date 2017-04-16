<?php
if(isset($_GET["table"])) {
	$table = $_GET["table"];

	$input = fopen("table.in", "w") or die("Unable to open input file!");
	fwrite($input, $table);
	fclose($input);
	
	exec("./chess");
	
	$output = fopen("table.out", "r") or die("Unable to open output file!");
	$result = fread($output,filesize("table.out"));
	fclose($output);
	
	echo $result;
} else
	echo "No input!";
?>