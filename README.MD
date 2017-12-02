# widigpio a Raspberry Pi Gpio PHP Extension
This extensions provides the Gpio object to get control of gpios from within php. Export and digital functions need root privileges, the "one-wire" bus does not.

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
	
	<?php

	echo 'TEST GPIO EXT' . PHP_EOL;

	$gpio = new Gpio();

	echo 'set export' . PHP_EOL;
	$gpio->setExport(24, false);
	$gpio->setExport(27, false);
	var_dump($gpio->setExport(24, true));
	var_dump($gpio->setExport(27, true));

	echo 'set direction' . PHP_EOL;
	var_dump($gpio->setDirection(27, true));
	var_dump($gpio->setDirection(24,false));

	echo 'get' . PHP_EOL;
	var_dump($gpio->getIo(24));

	for ($i = 0  ; $i < 3; $i++) {
  		$gpio->setIo(27, true);
  		sleep(1);
  		$gpio->setIo(27, false);
		sleep(1);
	}

	$deviceList = $gpio->getOneWireDeviceList();
	$device1 = $deviceList[0];
	$device2 = $deviceList[1];
	$device3 = $deviceList[2];

	echo 'Selected device: ' . $device1 . ', value: ' . $gpio->getOneWireDeviceValue($device1) . PHP_EOL;
	echo 'Selected device: ' . $device2 . ', value: ' . $gpio->getOneWireDeviceValue($device2) . PHP_EOL;
	echo 'Selected device: ' . $device3 . ', value: ' . $gpio->getOneWireDeviceValue($device3) . PHP_EOL;
