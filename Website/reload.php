<?php
	//echo '<table><tr><td>TEST</td></tr></table>';
	
    //$rows = file('log1.csv');
	//$last_row = array_pop($rows);
	//$data = str_getcsv($last_row);
	//$headers = str_getcsv($rows[0]);
	

	$path = '/home/cancorder';
	$latest_ctime = 0;
	$latest_filename = '';    

	$d = dir($path);
	while (false !== ($entry = $d->read())) {
		$filepath = "{$path}/{$entry}";
		// could do also other checks than just checking whether the entry is a file
		$ext = pathinfo($filepath, PATHINFO_EXTENSION);
		if(strcmp($ext, "csv") == 0)
		{
			if (is_file($filepath) && fileatime($filepath) > $latest_ctime) {
				$latest_ctime = fileatime($filepath);
				$latest_filename = $filepath;
			}
		}
	}	
	$fp = fopen($latest_filename, "r");
	if(!feof($fp))
	{
		$headers = fgets($fp);
	}
	$last_line = tailCustom($latest_filename);
	$headers = str_getcsv($headers);
	$data = str_getcsv($last_line);
	fclose($fp);
	
	$BIM_array = array();
	$temp_array = array();
	$cell_array = array();
	$error_array = array();
	$other_array = array();
	$gps_array = array();
	$heartbeats_array = array();
	
	$i = 1;
	
	$head_value = array_shift( $headers );
	$data_value = array_shift($data);
	echo "<p><b>" . $head_value . "</b>&ensp;" . $data_value . "&ensp;&ensp;&ensp;&ensp;";
	
	$head_value = array_shift( $headers );
	$data_value = array_shift($data);
	echo "<b>" . $head_value . "</b>&ensp;" . $data_value . "</p>";
	while($i < sizeof($headers))
	{
		$head_value = array_shift( $headers );
		$data_value = array_shift($data);
		if(strpos($head_value,'Heartbeat') !== false ||  strpos($head_value, 'StatusInformation') !== false)
		{
			$heartbeats_array[$head_value] = $data_value;
		}
		else if (strpos($head_value,'BIM') !== false) {
			$BIM_array[$head_value] = $data_value;
		}
		else if (strpos($head_value,'C)') !== false)
		{
			$temp_array[$head_value] = $data_value;
		}
		else if (strpos($head_value, 'Cell') !== false)
		{
			$cell_array[$head_value] = $data_value;
		}
		else if(strpos($head_value,'error') !== false || strpos($head_value,'limit') !== false || strpos($head_value,'Flags') !== false || strpos($head_value,'Status') !== false)
		{
			$error_array[$head_value] = $data_value;
		}
		else if(strpos($head_value,'GPS') !== false || strpos($head_value, 'Axis') !== false)
		{
			$gps_array[$head_value] = $data_value;
		}
		
		else
		{
			$other_array[$head_value] = $data_value;
		}
	}
	echo "<div class=\"tables\"><h2>BIM Data</h2><table>";
	$i = 0;
	while($i < sizeof($BIM_array))
	{
		echo "<tr>";
		$head_value = key($BIM_array);
		$data_value = array_shift( $BIM_array );
		//echo $head_value . $data_value;
		//echo "<br />";
		//$data_value = array_shift($data);
		echo "<td><b>" . $head_value . "</b></td><td>" . $data_value . "</td>";
		
		$head_value = key($BIM_array);
		$data_value = array_shift($BIM_array);
		echo "<td><b>" . $head_value . "</b></td><td>" . $data_value . "</td>";
		echo "</tr>";
		//echo "<br />";
	}
	echo "</table></div>";
	echo "<div class=\"tables\"><h2>Temperatures</h2><table>";
	$i = 0;
	while($i < sizeof($temp_array))
	{
		echo "<tr>";
		$head_value = key($temp_array);
		$data_value = array_shift( $temp_array );
		//echo $head_value . $data_value;
		//echo "<br />";
		//$data_value = array_shift($data);
		echo "<td><b>" . $head_value . "</b></td><td>" . $data_value . "</td>";
		
		$head_value = key($temp_array);
		$data_value = array_shift($temp_array);
		echo "<td><b>" . $head_value . "</b></td><td>" . $data_value . "</td>";
		echo "</tr>";
		//echo "<br />";
	}
	echo "</table></div>";
	echo "<div class=\"tables\"><h2>GPS Data</h2><table>";
	$i = 0;
	while($i < sizeof($gps_array))
	{
		echo "<tr>";
		$head_value = key($gps_array);
		$data_value = array_shift( $gps_array );
		//echo $head_value . $data_value;
		//echo "<br />";
		//$data_value = array_shift($data);
		echo "<td><b>" . $head_value . "</b></td><td>" . $data_value . "</td>";
		
		$head_value = key($gps_array);
		$data_value = array_shift($gps_array);
		echo "<td><b>" . $head_value . "</b></td><td>" . $data_value . "</td>";
		echo "</tr>";
		//echo "<br />";
	}
	echo "</table></div>";
	echo "<div class=\"tables\"><h2>Heartbeats</h2><table>";
	$i = 0;
	while($i < sizeof($heartbeats_array))
	{
		echo "<tr>";
		$head_value = key($heartbeats_array);
		$data_value = array_shift( $heartbeats_array );
		//echo $head_value . $data_value;
		//echo "<br />";
		//$data_value = array_shift($data);
		echo "<td><b>" . $head_value . "</b></td><td>" . $data_value . "</td>";
		
		$head_value = key($heartbeats_array);
		$data_value = array_shift($heartbeats_array);
		echo "<td><b>" . $head_value . "</b></td><td>" . $data_value . "</td>";
		echo "</tr>";
		//echo "<br />";
	}
	echo "</table></div>";
	echo "<div class=\"tables\"><h2>Errors</h2><table>";
	$i = 0;
	while($i < sizeof($error_array))
	{
		echo "<tr>";
		$head_value = key($error_array);
		$data_value = array_shift( $error_array );
		if($data_value == "1")
		{
			echo "<td><b>" . $head_value . "</b></td><td><font color=\"red\"><b>" . $data_value . "</b></font></td>";
		}
		else
		{
			echo "<td><b>" . $head_value . "</b></td><td>" . $data_value . "</td>";
		}
		//echo $head_value . $data_value;
		//echo "<br />";
		//$data_value = array_shift($data);
		
		
		$head_value = key($error_array);
		$data_value = array_shift($error_array);
		if($data_value == "1")
		{
			echo "<td><b>" . $head_value . "</b></td><td><font color=\"red\"><b>" . $data_value . "</b></font></td>";
		}
		else
		{
			echo "<td><b>" . $head_value . "</b></td><td>" . $data_value . "</td>";
		}
		echo "</tr>";
		//echo "<br />";
	}
	echo "</table></div>";
	echo "<div class=\"tables\"><h2>Other data</h2><table>";
	$i = 0;
	while($i < sizeof($other_array))
	{
		echo "<tr>";
		$head_value = key($other_array);
		$data_value = array_shift( $other_array );
		//echo $head_value . $data_value;
		//echo "<br />";
		//$data_value = array_shift($data);
		echo "<td><b>" . $head_value . "</b></td><td>" . $data_value . "</td>";
		
		$head_value = key($other_array);
		$data_value = array_shift($other_array);
		echo "<td><b>" . $head_value . "</b></td><td>" . $data_value . "</td>";
		echo "</tr>";
		//echo "<br />";
	}
	echo "</table></div>";
	echo "<div class=\"tables\"><h2>Cell data</h2><table>";
	$i = 0;
	while($i < sizeof($cell_array))
	{
		echo "<tr>";
		$head_value = key($cell_array);
		$data_value = array_shift( $cell_array );
		//echo $head_value . $data_value;
		//echo "<br />";
		//$data_value = array_shift($data);
		echo "<td><b>" . $head_value . "</b></td><td>" . $data_value . "</td>";
		
		$head_value = key($cell_array);
		$data_value = array_shift($cell_array);
		echo "<td><b>" . $head_value . "</b></td><td>" . $data_value . "</td>";
		echo "</tr><tr>";
		$head_value = key($cell_array);
		$data_value = array_shift( $cell_array );
		//echo $head_value . $data_value;
		//echo "<br />";
		//$data_value = array_shift($data);
		echo "<td><b>" . $head_value . "</b></td><td>" . $data_value . "</td>";
		
		$head_value = key($cell_array);
		$data_value = array_shift($cell_array);
		echo "<td><b>" . $head_value . "</b></td><td>" . $data_value . "</td>";
		echo "</tr>";
		//echo "<br />";
	}
	echo "</table></div>";
	//echo $data;
	
?>
<?php
function tailCustom($filepath, $lines = 1, $adaptive = true) {
 
		// Open file
		$f = @fopen($filepath, "rb");
		if ($f === false) return false;
 
		// Sets buffer size
		if (!$adaptive) $buffer = 8096;
		else $buffer = ($lines < 2 ? 64 : ($lines < 10 ? 512 : 8096));
 
		// Jump to last character
		fseek($f, -1, SEEK_END);
 
		// Read it and adjust line number if necessary
		// (Otherwise the result would be wrong if file doesn't end with a blank line)
		if (fread($f, 1) != "\n") $lines -= 1;
		
		// Start reading
		$output = '';
		$chunk = '';
 
		// While we would like more
		while (ftell($f) > 0 && $lines >= 0) {
 
			// Figure out how far back we should jump
			$seek = min(ftell($f), $buffer);
 
			// Do the jump (backwards, relative to where we are)
			fseek($f, -$seek, SEEK_CUR);
 
			// Read a chunk and prepend it to our output
			$output = ($chunk = fread($f, $seek)) . $output;
 
			// Jump back to where we started reading
			fseek($f, -mb_strlen($chunk, '8bit'), SEEK_CUR);
 
			// Decrease our line counter
			$lines -= substr_count($chunk, "\n");
 
		}
 
		// While we have too many lines
		// (Because of buffer size we might have read too many)
		while ($lines++ < 0) {
 
			// Find first newline and remove all text before that
			$output = substr($output, strpos($output, "\n") + 1);
 
		}
 
		// Close file and return
		fclose($f);
		return trim($output);
 
	}
 
?>
