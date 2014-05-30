include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
add_avr_executable(
    nixie
    main.c
	USI_TWI_Master.c
	DS1820.c
	OneWire.c
	dht22.c
)

add_avr_executable(
    dht
    dht22_test.c
	USI_TWI_Master.c
	dht22.c
)
