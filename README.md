# Garasje
Arduino-kode som kjøres på en ESP8266 i garasjen.
Statuser og handlinger publiseres til en MQTT-server og diverse
MQTT topics som igjen benttes videre i smarthuset.

## Foreløpig gjør programmet følgende
* Overvåker garasjeportens status
* Henter inn temperaturen i garasjen
* Henter inn luftfuktigheten i garasjen

## Garasjeportens status
Garasjeporten har reed-kontakter som trigges dersom garasjeporten enten er helt åpen eller
helt stengt. 

## config.h
Denne filen beskriver konfigurasjonen. Det er lagt ut et eksempel på en slik fil.
Denne finner du [her](config.h.example)
