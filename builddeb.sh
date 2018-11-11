#!/bin/sh

fakeroot ./debian/rules clean

rm -rf ./releasearm/*

dpkg-buildpackage $*
