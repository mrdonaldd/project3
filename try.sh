#!/bin/bash
inpt_name="$1"
new_ext=".x"
opt_name="${inpt_name/.s/"$new_ext"}"    
# prints 'I love Sara and Marry'

# hexdump

cd spim_assembler
python3 ./asm2hex "../tests/$1"
cd ../src
make clean && make
./sim "../tests/$opt_name"


