# weather

[![sampctl](https://shields.southcla.ws/badge/sampctl-weather-2f2f2f.svg?style=for-the-badge)](https://github.com/Cezear/samp-weather)

This is a recreate of the mweather include by SA-MP forum user [Mauzen](http://forum.sa-mp.com/member.php?u=10237).

It should be noted that while I call this a recreate, it is only a recreate of the idea mweather originally introduced; which were weather zones that get real-life weather data.

Now, abit about the include. This include adds the ability to create weather zones within the GTA:SA game that area sized according to what you'd specify.

There are two parts to this system.

> Observers

Observers are the backbone of the system. These handy little things are what send the HTTP request to the weather.php file; which in turns uses the Yahoo API to get the weather of the specified area.

> Weather Zones

Weather zones link to observers and use the information gathered by the Yahoo API to set any player who is within the borders of the zone's weather.

Weather zones also come with the ability to move across the map. This can be toggled off when creating the zone.

If a zone is set to be movable, it will move when the wind speed of the API area is greater than WEATHER_SPEED_LIMITER (by default 5.0); and in the direction the wind is blowing.

## Installation

Simply install to your project:

```bash
sampctl package install Cezear/samp-weather
```

Include in your code and begin using the library:

```pawn
#include <weather>
```
In addition to installing the package within your SAMP server, there is a PHP file that you will also need to upload to a web server. 

To install this, simply copy **weather.php** into your webservers root directory and update the WEATHER_SERVER variable.

## Usage

For usage instructions, view the [Wiki](https://github.com/Cezear/samp-weather/wiki)

## Testing

To test, simply run the package:

```bash
sampctl package run
```
By default, test.pwn comes with the GTA:SA map fully zoned out. None of these zones are configured to move.
