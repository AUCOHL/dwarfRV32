#!/bin/sh
CAPH=4096 #memory bank size
tests_path="../tests/"
tmp_path="./tmp/"
toolchain_path=""

[ ! -d "$tmp_path" ] && mkdir "$tmp_path"

[ ! -d "${tmp_path}reg" ] && mkdir "${tmp_path}reg" 

