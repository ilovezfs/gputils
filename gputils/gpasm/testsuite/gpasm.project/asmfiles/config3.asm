     list p=18c242,f=inhx32
     include "p18c242.inc"

     org 0
     movlw 0Xaa

     __CONFIG    _CONFIG0, _CP_OFF_0
     __CONFIG    _CONFIG1, _OSCS_ON_1 & _RCIO_OSC_1
     __CONFIG    _CONFIG2, _BOR_OFF_2 & _BORV_25_2 & _PWRT_ON_2
     __CONFIG    _CONFIG3, _WDT_ON_3 & _WDTPS_128_3
     __CONFIG    _CONFIG5, _CCP2MX_OFF_5
     __CONFIG    _CONFIG6, _STVR_OFF_6
     __CONFIG    _DEVID1, 0x55
     __CONFIG    _DEVID2, 0xAA


     org 0x100

     movlw 0x5
     fill 0x23, 14
     fill 0xff, 58
     fill 0x5534, 0xF00
     
     eNd
