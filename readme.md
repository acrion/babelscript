# Debugging

## Preperation

- build Y:\git\zelph\build-vs\network.sln
- open windows command line
- cd Y:\git\zelph\sample_scripts

## sample 1

- ..\build-vs\bin\RelWithDebInfo\network_app.exe wikidata.bls
- .wikidata Y:/_excludedFromBackup/part2.json

## sample 2

- ..\build-vs\bin\RelWithDebInfo\network_app.exe simple.bls
- open Mac terminal in same path
- dot -Tpng white.dot >white.png
- open white.png
