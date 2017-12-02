# widigpio a Raspberry Pi Gpio PHP Extension
This extensions provides several functions to get control of gpios from within php.

Supports:
- export
- digital get and set
- get w1 bus clients
- read w1 client value

## Installation
	
	git clone https://github.com/dirkwinkhaus/widi-gpio.git
	cd widi-gpio
	phpize
	./configure
	make
	cp modules/widigpio.so [PATH_TO_YOUR_PHP_MODULES]
	tell php to load the module

## Sample

	// export gpio3
	gpio_export(3, true);
	// set gpio3 output mode
	gpio_direction(3, true);
	
	// export gpio17
	gpio_export(17, true);
	// set gpio17 iinput mode
	gpio_direction(17, false);

	// blink gpio3 3 times
	for ($i = 0  ; $i < 3; $i++) {
		gpio_set(3, true);
		sleep(1);
		gpio_set(3,false);
		sleep(1);
	}	
	
	echo 'gpio17 level is ';
	if (gpio_get(17)) {
		echo 'high'; 
	} else {
		echo 'low'; 
	}
	echo PHP_EOL;
	
	// sample of 3 18b20 temperature sensors on one wire bus
	// this list contains the sensor ids
	$deviceList = gpio_onewire_slaves_list();
	$device1 = $deviceList[0];
	$device2 = $deviceList[1];
	$device3 = $deviceList[2];

	// output of every sensors temperature as integer in micro degrees
	echo 'Selected device: ' . $device1 . ', value: ' . gpio_onewire_slave_value($device1) . PHP_EOL;
	echo 'Selected device: ' . $device2 . ', value: ' . gpio_onewire_slave_value($device2) . PHP_EOL;
	echo 'Selected device: ' . $device3 . ', value: ' . gpio_onewire_slave_value($device3) . PHP_EOL;
	echo PHP_EOL;
