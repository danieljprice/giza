Changes in v1.2.0 compared to v1.1.0
------------------------------------
Features:
- new giza_set_number_format() sets format to use by giza_box() (thanks to chafar)
- giza_open() now detects and handles errors occurring during setup of the new device
- PGEND()/cpgend() now closes all open devices, as per documentation (thanks to H. Verkouter)
- giza build includes freetype and fontconfig libraries
- improved font memory management (thanks to H. Verkouter)

Bug fixes:
- build failure caused by giza_box_time fixed (#21)
- bug fix with internal logic in naming png files if blank page exists (thanks to Aard Keimpema, #28)
- seg fault fixed when multiple devices are opened and closed in random order (thanks to H. Verkouter)
- fixed possible buffer overflow in giza_format_number (thanks to H. Verkouter)
- avoid the 'initial declarations are only allowed in C99 mode' error (thanks to M. Hutchison)

Other:
- deleted unused _giza_change_size() function
- updated documentation for github
- automated build checking, api documentation and release-on-tag via github actions
