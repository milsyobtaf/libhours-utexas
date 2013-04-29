<?php
//verybeginning of your page
$start = microtime(true);

/* Location Variables */
$locationurl = $_SERVER['REQUEST_URI'];
$locationabbreviation;
/* Find Location 
 * Find the location by the URL and set the $apilibrary ID and the $locationabbreviation
 * $apilibrary is the ID given to the locations in the Lib Hours module
 * $locationabbreviation will be output in the widget, needs to be as short as possible
*/
if ((strpos($locationurl, "/apl") !== false) || (strpos($locationurl, "/apl/") !== false)) {
	$apilibrary = 9;
	$locationabbreviation = "APL";
}
else if ((strpos($locationurl, "/benson") !== false) || (strpos($locationurl, "/benson/") !== false)) {
	$apilibrary = 11;
	$locationabbreviation = "BLAC";
}
else if ((strpos($locationurl, "/chem") !== false) || (strpos($locationurl, "/chem/") !== false)) {
	$apilibrary = 13;
	$locationabbreviation = "CHEM";
}
else if ((strpos($locationurl, "/classics") !== false) || (strpos($locationurl, "/classics/") !== false)) {
	$apilibrary = 14;
	$locationabbreviation = "Classics";
}
else if ((strpos($locationurl, "/engin") !== false) || (strpos($locationurl, "/engin/") !== false)) {
	$apilibrary = 16;
	$locationabbreviation = "ENG";
}
else if ((strpos($locationurl, "/fal") !== false) || (strpos($locationurl, "/fal/") !== false)) {
	$apilibrary = 17;
	$locationabbreviation = "FAL";
}
else if ((strpos($locationurl, "/geo") !== false) || (strpos($locationurl, "/geo/") !== false)) {
	$apilibrary = 20;
	$locationabbreviation = "GEO";
}
else if ((strpos($locationurl, "/lsl") !== false) || (strpos($locationurl, "/lsl/") !== false)) {
	$apilibrary = 21;
	$locationabbreviation = "LSL";
}
else if ((strpos($locationurl, "/pma") !== false) || (strpos($locationurl, "/pma/") !== false)) {
	$apilibrary = 22;
	$locationabbreviation = "PMA";
}
else {
	$apilibrary = 1;
	$locationabbreviation = "PCL";
}

/* Retrieve API Function 
 * This is necessary because the simpler file_get_contents won't traverse domains without mucking about with php.ini settings.  Maybe try file_get_contents again when you get everything under one roof?
*/
function get_content($URL){
	$ch = curl_init();
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
	curl_setopt($ch, CURLOPT_URL, $URL);
	curl_setopt($ch, CURLOPT_TIMEOUT_MS, 1000);
	curl_setopt($ch, CURLOPT_CONNECTTIMEOUT_MS, 1000);
	curl_setopt($ch, CURLOPT_FAILONERROR, true);
	$data = curl_exec($ch);
	curl_close($ch);
	return $data;
}

/* API Variables */
$apihourslocation = "&library=" . $apilibrary;
$apihoursaction = "&action=hours";
$apiopenaction = "&action=open";
$apiformat = "format=json";
$apiurl = "http://localhost:8888/d6/hours/api?";

/* JSON Retrieval Variables */
$hoursurl = $apiurl . $apiformat . $apihourslocation . $apihoursaction;
$openurl = $apiurl . $apiformat . $apiopenaction;

/* Retrieve JSON */
$hoursjsonfile = get_content($hoursurl);
$openjsonfile = get_content($openurl);

/* Decode JSON */
$decodedhoursjson = json_decode($hoursjsonfile, true);
$decodedopenjson = json_decode($openjsonfile, true);

/* 1. Check JSON Status And Print Out Only Today
 * This check ensures that if the api doesn't return for some reason, nothing is printed
 * 2. Check Whether Location Is Open
 * This sets the class variables for styling the output based on the current status of the location
 * The "date(w)" in the meat of the calls to $decodedjson pulls the 0-index day of the week and feeds it to the array, so it only shows today's data
 */

if ($decodedopenjson["status"] === 'ok'){
	$i = 0;
	while ($i < count($decodedopenjson["locations"])){
		if ($apilibrary == $decodedopenjson["locations"][$i]["id"]){
			$libhoursstyle = " class='libhours-header-widget-open'";
			return $libhoursstyle;
		}
		elseif ($apilibrary !== $decodedopenjson["locations"][$i]["id"]){
			$libhoursstyle = " class='libhours-header-widget-closed'";
			return $libhoursstyle;
		}
	$i++;
	}
	echo "<div" . $libhoursstyle . " id='libhours-header-widget'><span id='libhours-header-widget-clockicon'>[</span><a href='http://drupal.lib.utexas.edu/hours/" . $apilibrary . "'>" . $locationabbreviation . " Hours Today: <strong>" . $decodedhoursjson["hours"][date("w")]["hour"] . "</strong></span></a></div>";
} else {
	echo "<div id='libhours-header-widget' class='libhours-header-widget-error'><span id='libhours-header-widget-clockicon'>[</span><strong><a href='http://drupal.lib.utexas.edu/hours'>Library Hours</a></strong></div>";
}

//very end of your page
$end = microtime(true);
print "Page generated in ".round(($end - $start), 4)." seconds";

?>
