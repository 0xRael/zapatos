#!/bin/bash
gcc $(pkg-config --cflags gtk+-3.0) main2.c $(pkg-config --libs gtk+-3.0) -o app
