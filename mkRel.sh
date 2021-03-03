#!/usr/bin/env sh

# Stupid temporary release script
version=$(cat version.txt)
echo "Building $version"
mkdir -p doxyYoda/css
cp version.txt doxyYoda
cp -r src/html doxyYoda
cp -r src/xml doxyYoda
minify src/styles/doxyYoda.css -o doxyYoda/css/doxyYoda.min.css
echo "Apache 2 licensed Doxygen theme by Rohit Goswami <https://rgoswami.me>. \n See: https://github.com/HaoZeke/doxyYoda for details" > doxyYoda/README
tar -czf "doxyYoda_$version.tar.gz" doxyYoda
rm -rf doxyYoda
echo "Done"
