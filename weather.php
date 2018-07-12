<?php

	$city = 'los angeles, ca';
	if(!empty($_GET['city'])) $city = $_GET['city'];
    
    $BASE_URL = "http://query.yahooapis.com/v1/public/yql";

	$yql_query = 'select wind, item.condition from weather.forecast where woeid in (select woeid from geo.places(1) where text="' . $city . '")';
	
    $yql_query_url = $BASE_URL . "?q=" . urlencode($yql_query) . "&format=json";

    // Make call with cURL
    $session = curl_init($yql_query_url);
    curl_setopt($session, CURLOPT_RETURNTRANSFER,true);
    $json = curl_exec($session);

    //echo $json;

   	// Convert JSON to PHP object
    $phpObj =  json_decode($json, true);
   	switch($phpObj['query']['results']['channel']['item']['condition']['code'])
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 23:
		case 24: 
		case 37:
		case 38:
		case 39:
		case 45:
		case 47: $weather = 'Description: Thunderstorm</br>ID: 16';//16
			break;
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 17:
		case 35:
		case 40: $weather = 'Description: Medium Showers</br>ID: 8';//8
			break;
		case 13:
		case 15:
		case 16:
		case 41: 
		case 42:
		case 43:
		case 5:
		case 6:
		case 7: 
		case 14:
		case 18:
		case 46: $weather = 'Description: Snowing</br>ID: 10';
			break;
		case 19: $weather = 'Description: Sand Storm</br>ID: 19';//19
			break;
		case 20:
		case 21:
		case 22: $weather = 'Description: Foggy</br>ID: 9';//9
			break;
		case 27:
		case 29:
		case 28:
		case 44: $weather = 'Description: Cloudy</br>ID: 15';//15
			break;
		case 25:
		case 26: $weather = 'Description: Smog</br>ID: 20';//20
			break;
		case 32:
		case 36: $weather = 'Description: Clear with periodic clouds</br>ID: 1';//1
			break;
		case 30:
		case 31:
		case 33:
		case 34: $weather = 'Description: Partly Cloudy</br>ID: 2';//2
			break;
		case 3200: $weather = 'Description: Not Available</br>ID: -1';//-1
		break;
	}
	$weather = $weather . '</br>Speed: ' . $phpObj['query']['results']['channel']['wind']['speed'] . '</br>Direction: ' . $phpObj['query']['results']['channel']['wind']['direction'];
	echo $weather;

?> 