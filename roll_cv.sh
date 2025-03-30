#!/bin/bash

./NRPS
# TODO: Check that projects.tex was generated
xelatex -synctex=1 -interaction=nonstopmode "CV".tex
rm ./*.aux ./*.out ./*.synctex.gz ./*.log
