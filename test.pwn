#include <a_samp>
#include "weather.inc"

new TR_Observer,
    LV_Observer,
    LS_Observer,
    SF_Observer,
    BC_Observer,
    BS_Observer,
    AP_Observer,
    FC_Observer,
    RC_Observer,
    FORT_CARSON_Observer;

main() {
    TR_Observer = AddWeatherObserver(10, "Sierra, NV");
    LV_Observer = AddWeatherObserver(10, "Caribou, ME");
    LS_Observer = AddWeatherObserver(10, "Los Angeles, CA");
    SF_Observer = AddWeatherObserver(10, "San Francisco, CA");
    BC_Observer = AddWeatherObserver(10, "Mojave, CA");
    BS_Observer = AddWeatherObserver(10, "Sausalito, CA");
    AP_Observer = AddWeatherObserver(10, "Contra Costa, CA");
    FC_Observer = AddWeatherObserver(10, "Inyo County, CA");
    RC_Observer = AddWeatherObserver(10, "Orange County, CA");
    FORT_CARSON_Observer = AddWeatherObserver(10, "Fountain, CO");

	AddWeatherZone(TR_Observer,  -2149.03125, 1598, -461.03125, 3001, false);
    AddWeatherZone(LV_Observer,  865, 546, 3000.015625, 3000, false);
    AddWeatherZone(LS_Observer, 65, -2967, 2999, -662, false);
    AddWeatherZone(SF_Observer, -2998.03125, -884.015625, -1261.03125, 1596.984375, true);
    AddWeatherZone(BC_Observer, -463, 1593, 866, 3000, false);
    AddWeatherZone(BS_Observer, -3000, 1599, -2151, 3000, true);
    AddWeatherZone(AP_Observer, -2998.03125, -2969.015625, -1264.03125, -884.015625, false);
    AddWeatherZone(FC_Observer, -1264.03125, -2967, 61.96875, 549, false);
    AddWeatherZone(RC_Observer, 64.96875, -663, 3003, 549, false);
    AddWeatherZone(FORT_CARSON_Observer,  -1261.03125, 549, 865.96875, 1596, false);
}

public OnGameModeExit() {
    RemoveWeatherObserver(LS_Observer);
}