// =========== [ External Includes ] ===========

#include <a_http>
#include <foreach>
#include <YSI\y_hooks>

// =========== [ Variables ] ===========

// Server+directory where weather.php is located
#define WEATHER_SERVER                  "127.0.0.1"

// Weather Observers
#define MAX_WEATHER_OBSERVERS           30
#define INVALID_WEATHER_OBSERVER        -1

enum WEATHER_OBSERVER_INFO {
	bool:observer,
	obsrv_url[128],
	obsrv_interval,
	obsrv_timer,
	obsrv_weather,
	obsrv_old_weather,
	Float:obsrv_speed,
	obsrv_direction
}
new WeatherObserver[MAX_WEATHER_OBSERVERS][WEATHER_OBSERVER_INFO];

// Weather Zones
#define MAX_WEATHER_ZONES               30
#define INVALID_WEATHER_ZONE            -1

enum WEATHER_ZONE_INFO {
	bool:weather_zone,
	bool:wz_movable,
	Float:wz_min[2],
	Float:wz_max[2],
	Float:wz_mv_min[2],
	Float:wz_mv_max[2],
	Float:wz_speed,
	wz_direction,
	wz_weather,
	wz_observer,
	wz_timer
}
new WeatherZone[MAX_WEATHER_ZONES][WEATHER_ZONE_INFO];

// Limiters
#define WEATHER_SPEED_LIMITER				5.0
#define WEATHER_LIMIT_POS_X					3070.00
#define WEATHER_LIMIT_POS_Y					3130.00
#define WEATHER_LIMIT_NEG_X					-3370.00
#define WEATHER_LIMIT_NEG_Y					-3110.00

// Weather types
#define WEATHER_UNAVAILABLE                 -1
#define WEATHER_CLEAR_PERIODIC_CLOUDS       1
#define WEATHER_PARTLY_CLOUDY               2
#define WEATHER_MEDIUM_SHOWERS              8
#define WEATHER_FOGGY                       9
#define WEATHER_SNOWING                     10
#define WEATHER_CLOUDY                      15
#define WEATHER_THUNDERSTORM                16
#define WEATHER_SANDSTORM                   19
#define WEATHER_SMOG                        20

// Player
new player_weather[MAX_PLAYERS],
    player_overide_weather[MAX_PLAYERS],
    player_zone[MAX_PLAYERS];

// =========== [ Hooks ] ===========

hook OnGameModeInit() {
    foreach(new i : Player) {
        player_weather[i] = WEATHER_UNAVAILABLE;
        player_overide_weather[i] = WEATHER_UNAVAILABLE;
        player_zone[i] = INVALID_WEATHER_ZONE;
    }
    return 1;
}

// =========== [ Callbacks ] ===========

forward OnWeatherObserverUpdate(observerid, oldweather, newweather);
forward OnPlayerWeatherZoneChange(playerid, oldzone, newzone);

// =========== [ Internal Functions ] ===========

// =========== [ Functions ] ===========

// Player
// native SetPlayerWeatherEx(playerid, weather);
stock SetPlayerWeatherEx(playerid, weather) {
    SetPlayerWeather(playerid, weather);
    player_weather[playerid] = weather;
    return 1;
}

// native GetPlayerWeather(playerid);
stock GetPlayerWeather(playerid) {
    return player_weather[playerid];
}

// native IsPlayerInWeatherZone(playerid, weatherzoneid);
stock IsPlayerInWeatherZone(playerid, weatherzoneid) {
	if(!WeatherZone[weatherzoneid][weather_zone])
		return 0;

    new Float:X, Float:Y, Float:Z;
    GetPlayerPos(playerid, X, Y, Z);

    if((X >= WeatherZone[weatherzoneid][wz_min][0] && X <= WeatherZone[weatherzoneid][wz_max][0] && Y >= WeatherZone[weatherzoneid][wz_min][1] && Y <= WeatherZone[weatherzoneid][wz_max][1]) || (X >= WeatherZone[weatherzoneid][wz_mv_min][0] && X <= WeatherZone[weatherzoneid][wz_mv_max][0] && Y >= WeatherZone[weatherzoneid][wz_mv_min][1] && Y <= WeatherZone[weatherzoneid][wz_mv_max][1])) {
        if(player_zone[playerid] != weatherzoneid) {
        	CallLocalFunction("OnPlayerWeatherZoneChange", "iii", playerid, player_zone[playerid], weatherzoneid);
        	player_zone[playerid] = weatherzoneid;
        	return 1;
        }
        else
        	return 1;
    } else {
    	if(player_zone[playerid] != -1) {
        	CallLocalFunction("OnPlayerWeatherZoneChange", "iii", playerid, player_zone[playerid], -1);
        	player_zone[playerid] = -1;
        	return 1;
        }
    }
    return 0;
}

stock IsPlayerInWeatherZoneEx(playerid, weatherzoneid) {
	if(!WeatherZone[weatherzoneid][weather_zone])
		return 0;

    new Float:X, Float:Y, Float:Z;
    GetPlayerPos(playerid, X, Y, Z);

    if((X >= WeatherZone[weatherzoneid][wz_min][0] && X <= WeatherZone[weatherzoneid][wz_max][0] && Y >= WeatherZone[weatherzoneid][wz_min][1] && Y <= WeatherZone[weatherzoneid][wz_max][1]) || (X >= WeatherZone[weatherzoneid][wz_mv_min][0] && X <= WeatherZone[weatherzoneid][wz_mv_max][0] && Y >= WeatherZone[weatherzoneid][wz_mv_min][1] && Y <= WeatherZone[weatherzoneid][wz_mv_max][1]))
        return 1;
    return 0;
}

// native IsPlayerInAnyWeatherZone(playerid);
stock IsPlayerInAnyWeatherZone(playerid) {
	for(new i = 0; i < MAX_WEATHER_ZONES; i++) {
		if(!WeatherZone[i][weather_zone])
			continue;

		if(IsPlayerInWeatherZoneEx(playerid, i)){
			return i;
		}
	}
	return -1;
}

// native bool:IsPlayerInMultipleZones(playerid);
stock bool:IsPlayerInMultipleZones(playerid) {
	new count = 0;
	for(new z = 0; z < MAX_WEATHER_ZONES; z++) {
		if(!WeatherZone[z][weather_zone])
			continue;

		if(IsPlayerInWeatherZoneEx(playerid, z)) {
			count++;
			continue;
		}

		else
			continue;
	}

	if(count > 1)
		return true;
	
	return false;
}

stock GetPriorityWeatherZone(playerid) {
	for(new i = 0; i < MAX_WEATHER_ZONES; i++) {
		if(!WeatherZone[i][weather_zone])
			continue;

		if(IsPlayerInWeatherZoneEx(playerid, i) && WeatherZone[i][wz_movable]) {
			return i;
		}

		else
			continue;
	}
	return -1;
}

// Weatherzone/observer
stock GetNextWeatherObserverID()
{
	for(new i = 0; i < MAX_WEATHER_OBSERVERS; i ++)
	{
	    if(!WeatherObserver[i][observer]) return i;
	}
	return -1;
}

stock GetNextWeatherZoneID()
{
	for(new i = 0; i < MAX_WEATHER_ZONES; i ++)
	{
	    if(!WeatherZone[i][weather_zone]) return i;
	}
	return -1;
}

// native AddWeatherObserver(interval, city[]);
stock AddWeatherObserver(interval, city[]) {
	new oid = GetNextWeatherObserverID();
	if(oid == -1)
		return -1;
	WeatherObserver[oid][observer] = true;
	format(WeatherObserver[oid][obsrv_url], 150, "%s/weather.php?city=%s", WEATHER_SERVER, city);
    strreplace(WeatherObserver[oid][obsrv_url], ' ', '-');
    WeatherObserver[oid][obsrv_interval] = interval * 60000;
    WeatherObserver[oid][obsrv_weather] = WEATHER_UNAVAILABLE;
    WeatherObserver[oid][obsrv_timer] = SetTimerEx("UpdateWeatherObserver", WeatherObserver[oid][obsrv_interval], true, "d", oid);
    UpdateWeatherObserver(oid);
	return oid;
}

// native AddWeatherZone(observerid, Float:minX, Float:minY, Float:maxX, Float:maxY, bool:moving = false);
stock AddWeatherZone(observerid, Float:minX, Float: minY, Float:maxX, Float:maxY, bool:moving = false) {
	new zid = GetNextWeatherZoneID();
	if(zid == -1)
		return -1;
	WeatherZone[zid][weather_zone] = true;

	WeatherZone[zid][wz_min][0] = minX;
	WeatherZone[zid][wz_min][1] = minY;
	WeatherZone[zid][wz_max][0] = maxX;
	WeatherZone[zid][wz_max][1] = maxY;

	if(moving) {
		WeatherZone[zid][wz_movable] = true;
	}

	WeatherZone[zid][wz_mv_min][0] = minX;
	WeatherZone[zid][wz_mv_min][1] = minY;
	WeatherZone[zid][wz_mv_max][0] = maxX;
	WeatherZone[zid][wz_mv_max][1] = maxY;

	WeatherZone[zid][wz_observer] = observerid;
	WeatherZone[zid][wz_timer] = SetTimerEx("UpdateWeatherZone", 2000, true, "i", zid);
	UpdateWeatherZone(zid);
	return zid;
}

// native RemoveWeatherObserver(observerid);
stock RemoveWeatherObserver(observerid) {
	if(WeatherObserver[observerid][observer]) {
		WeatherObserver[observerid][observer] = false;
		KillTimer(WeatherObserver[observerid][obsrv_timer]);
	}

	else
		return 0;
	return 1;
}

// native RemoveWeatherZone(weatherzoneid);
stock RemoveWeatherZone(weatherzoneid) {
	if(WeatherZone[weatherzoneid][weather_zone]) {
		WeatherZone[weatherzoneid][weather_zone] = false;
		KillTimer(WeatherZone[weatherzoneid][wz_timer]);
	}

	else
		return 0;
	return 1;
}

// native ResetWeatherZone(weatherzoneid);
stock ResetWeatherZone(weatherzoneid) {
	if(WeatherZone[weatherzoneid][weather_zone]) {
		WeatherZone[weatherzoneid][wz_mv_min][0] = WeatherZone[weatherzoneid][wz_min][0];
		WeatherZone[weatherzoneid][wz_mv_min][1] = WeatherZone[weatherzoneid][wz_min][1];
		WeatherZone[weatherzoneid][wz_mv_max][0] = WeatherZone[weatherzoneid][wz_max][0];
		WeatherZone[weatherzoneid][wz_mv_max][1] = WeatherZone[weatherzoneid][wz_max][1];
	}

	else
		return 0;
	return 1;
}

// native SetWeatherZoneMovable(weatherzoneid, bool:movable);
stock SetWeatherZoneMovable(weatherzoneid, bool:movable) {
	if(WeatherZone[weatherzoneid][weather_zone]) {
		if(movable) {
			WeatherZone[weatherzoneid][wz_speed] = 0.0;
			WeatherZone[weatherzoneid][wz_movable] = true;
		} else {
			WeatherZone[weatherzoneid][wz_movable] = false;
		}
	} 

	else
		return 0;
	return 1;
}

// native MoveWeatherZone(weatherzoneid, Float:speed);
stock MoveWeatherZone(weatherzoneid, Float:speed) {
	if(WeatherZone[weatherzoneid][weather_zone]) {
		WeatherZone[weatherzoneid][wz_speed] = speed;
		return 1;
	} else {
		return 0;
	}
}

forward UpdateWeatherObserver(observer_id);
public UpdateWeatherObserver(observer_id) {
	if(!WeatherObserver[observer_id][observer]) {
		KillTimer(WeatherObserver[observer_id][obsrv_timer]);
		return 0;
	} else {
		HTTP(observer_id + 300, HTTP_GET, WeatherObserver[observer_id][obsrv_url], "", "HTTPUpdateWeatherObserver");
	}
	return 1;
}

forward HTTPUpdateWeatherObserver(index, response_code, data[]);
public HTTPUpdateWeatherObserver(index, response_code, data[]) {
	new WeatherID[128], Speed[128], Direction[128], oid = index - 300;
    WeatherObserver[oid][obsrv_old_weather] = WeatherObserver[oid][obsrv_weather];
    if(response_code == 200)
    {
        format(WeatherID, sizeof(WeatherID), "%s", data);
        strdel(WeatherID, 0, strfind(WeatherID, "ID:")+3);
        WeatherObserver[oid][obsrv_weather] = strval(WeatherID);
        format(Speed, sizeof(Speed), "%s", data);
        strdel(Speed, 0, strfind(Speed, "Speed:")+6);
        WeatherObserver[oid][obsrv_speed] = floatdiv(strval(Speed), 4);
        format(Direction, sizeof(Direction), "%s", data);
        strdel(Direction, 0, strfind(Direction, "Direction:")+10);
        WeatherObserver[oid][obsrv_direction] = strval(Direction);
        CallLocalFunction("OnWeatherObserverUpdate", "iii", oid, WeatherObserver[oid][obsrv_old_weather], WeatherObserver[oid][obsrv_weather]);
        return 1;
    }
    else
    {
        WeatherObserver[oid][obsrv_weather] = WEATHER_UNAVAILABLE;
        CallLocalFunction("OnWeatherObserverUpdate", "iii", oid, WeatherObserver[oid][obsrv_old_weather], WeatherObserver[oid][obsrv_weather]);
        return 1;
    }
}

forward UpdateWeatherZone(zone_id);
public UpdateWeatherZone(zone_id) {
	if(!WeatherZone[zone_id][weather_zone]) {
		WeatherZone[zone_id][weather_zone] = false;
		KillTimer(WeatherZone[zone_id][wz_timer]);
		return 0;
	} else {

		new oid = WeatherZone[zone_id][wz_observer];

		if((WeatherZone[zone_id][wz_speed] > WEATHER_SPEED_LIMITER || WeatherObserver[oid][obsrv_speed] > WEATHER_SPEED_LIMITER) && WeatherZone[zone_id][wz_movable]) { // if RAW speed (before divison) is greater than 20, move zone in direction of wind at speed set by (SPEED / 4)
			new Float:speed = WeatherObserver[oid][obsrv_speed];
			if(WeatherZone[zone_id][wz_speed] > WeatherObserver[oid][obsrv_speed])
				speed = WeatherZone[zone_id][wz_speed];

			new Float:x = floatmul(speed, floatsin(-WeatherObserver[oid][obsrv_direction], degrees)),
                Float:y = floatmul(speed, floatcos(-WeatherObserver[oid][obsrv_direction], degrees));

            WeatherZone[zone_id][wz_mv_min][0] += x;
            WeatherZone[zone_id][wz_mv_min][1] += y;

            WeatherZone[zone_id][wz_mv_max][0] += x;
            WeatherZone[zone_id][wz_mv_max][1] += y;

            if((WeatherZone[zone_id][wz_mv_min][0] > WEATHER_LIMIT_POS_X && WeatherZone[zone_id][wz_mv_max][1] < WEATHER_LIMIT_NEG_Y) || (WeatherZone[zone_id][wz_mv_min][0] > WEATHER_LIMIT_POS_X && WeatherZone[zone_id][wz_mv_min][1] > WEATHER_LIMIT_POS_Y) || (WeatherZone[zone_id][wz_mv_max][0] < WEATHER_LIMIT_NEG_X && WeatherZone[zone_id][wz_mv_min][1] > WEATHER_LIMIT_POS_Y) || (WeatherZone[zone_id][wz_mv_max][0] < WEATHER_LIMIT_NEG_X && WeatherZone[zone_id][wz_mv_max][1] < WEATHER_LIMIT_NEG_Y)) {
                ResetWeatherZone(zone_id);
                return 1;
            }
		}

		new priority = -1;
		foreach(new i : Player) {
			if(!IsPlayerConnected(i)){
				continue;
			}

			if(IsPlayerInMultipleZones(i) && IsPlayerInWeatherZoneEx(i, zone_id)){
				priority = GetPriorityWeatherZone(i);
				if(priority == player_zone[i])
					continue;

				SetPlayerWeatherEx(i, WeatherObserver[priority][obsrv_weather]);
				CallLocalFunction("OnPlayerWeatherZoneChange", "iii", i, player_zone[i], priority);
				player_zone[i] = priority;
				continue;
			}

			if(IsPlayerInWeatherZone(i, zone_id)) {
				SetPlayerWeatherEx(i, WeatherObserver[oid][obsrv_weather]);
				continue;
			}

			else
				continue;
		}
	}
	return 1;
}

stock strreplace(string[], find, replace)
{
    for(new i = 0, j = strlen(string); i != j; i++)
    {
        if(string[i] == find)
        {
            string[i] = replace;
        }
    }
}