#!/bin/bash

ls -l ./files | awk '{print $NF}' > ./files/names 
