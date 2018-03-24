	.set noreorder

#include "pspimport.s"

	IMPORT_START	"scePower_driver",0x00010000
	IMPORT_FUNC	"scePower_driver",0xD3075926,scePowerIsLowBattery
	IMPORT_FUNC	"scePower_driver",0x2085D15D,scePowerGetBatteryLifePercent
