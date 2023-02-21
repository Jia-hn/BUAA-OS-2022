#!/bin/bash
make
touch hello_os
mkdir hello_os_dir
cp ./os_hello ./hello_os_dir
cp ./os_hello ./hello_os_dir/hello_os
make clean
grep -ni os_hello hello_os.c > hello_os.txt
