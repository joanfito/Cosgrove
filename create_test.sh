#!/bin/bash

#Creem tots els directoris
mkdir -p ./TEST_ZONE/{Lionel/files,Telescope1/files,Telescope2/files,Telescope3/files,Telescope4/files,Telescope5/files,Scientist1,Scientist2,Scientist3}

#Copiem els arxius de Lionel
cp ./Lionel/lionel ./TEST_ZONE/Lionel
cp ./Lionel/ConfigLionel.dat ./TEST_ZONE/Lionel

#Copiem els arxius dels McGruders
cp ./McGruder/mcgruder ./TEST_ZONE/Telescope1
cp ./McGruder/ConfigT1.dat ./TEST_ZONE/Telescope1
cp ./McGruder/getnames.sh ./TEST_ZONE/Telescope1

cp ./McGruder/mcgruder ./TEST_ZONE/Telescope2
cp ./McGruder/ConfigT2.dat ./TEST_ZONE/Telescope2
cp ./McGruder/getnames.sh ./TEST_ZONE/Telescope2

cp ./McGruder/mcgruder ./TEST_ZONE/Telescope3
cp ./McGruder/ConfigT3.dat ./TEST_ZONE/Telescope3
cp ./McGruder/getnames.sh ./TEST_ZONE/Telescope3

cp ./McGruder/mcgruder ./TEST_ZONE/Telescope4
cp ./McGruder/ConfigT4.dat ./TEST_ZONE/Telescope4
cp ./McGruder/getnames.sh ./TEST_ZONE/Telescope4

cp ./McGruder/mcgruder ./TEST_ZONE/Telescope5
cp ./McGruder/ConfigT5.dat ./TEST_ZONE/Telescope5
cp ./McGruder/getnames.sh ./TEST_ZONE/Telescope5

#Copiem els dels McTavish
cp ./McTavish/mctavish ./TEST_ZONE/Scientist1
cp ./McTavish/ConfigS1.dat ./TEST_ZONE/Scientist1

cp ./McTavish/mctavish ./TEST_ZONE/Scientist2
cp ./McTavish/ConfigS2.dat ./TEST_ZONE/Scientist2

cp ./McTavish/mctavish ./TEST_ZONE/Scientist3
cp ./McTavish/ConfigS3.dat ./TEST_ZONE/Scientist3

#Copiem els arxius a enviar als diferents McGruders
cp ./test_files/Telescope1/* ./TEST_ZONE/Telescope1/files

cp ./test_files/Telescope2/* ./TEST_ZONE/Telescope2/files

cp ./test_files/Telescope3/* ./TEST_ZONE/Telescope3/files

cp ./test_files/Telescope4/* ./TEST_ZONE/Telescope4/files

cp ./test_files/Telescope5/* ./TEST_ZONE/Telescope5/files

