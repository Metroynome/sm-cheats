# libc-demo

Callable example using librac5's game libc wrappers: validates the boot build,
allocates 128 bytes, clears/copies/checks a string, formats and prints the detected
region, then frees the allocation. `libcDemoStatus` reports 0=not run, 1=success,
2=unsupported/wrong build, 3=allocation failure, 4=string check failure.

Run `make` here to build auto/ntscu/pal/ntscj relocatable modules. For a linked
payload, use `make payload REGION=pal ADDRESS=0xYOUR_VERIFIED_ADDRESS` with an
actual reserved address. See ../README.md for the loader/hook requirements.
The example should run once from a game thread after runtime initialization.
printf uses the game's logging route. No automatic injection or hook is installed.

This example and all regional jump targets were compile/link checked, but it
has not been run in PCSX2. Files named linkcheck are only linker-test artifacts;
their address is not a verified deployment location.
