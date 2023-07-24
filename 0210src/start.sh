#!/bin/bash

# directory containing input files
input_dir="../inputs/"

# function to delete the first line of a file
delete_first_line() {
    sed -i '1d' "$1"
}

# prepare the .x input files
cd ../spim_assembler/
# collect all input files with the .s extension in the input directory
input_files=("${input_dir}"*.s)
# complie input files
for file in "${input_files[@]}"; do
    echo "Compiling $input_file..."
    python3 asm2hex "${file}"
done
# collect all the complied input files with the .text.dat extension in the input directory
input_files=("${input_dir}"*.text.dat)
# replace all the compiled input files with .text.dat extension with .x extension
for file in "${input_files[@]}"; do
    if [ -f "$file" ]; then
        echo "Processing $file..."
        delete_first_line "$file"
    fi
    mv "$file" "${file%.text.dat}.x"
done
# check if hex directory exists and create if it does not exist
if [ ! -d "${input_dir}hex" ]; then
    mkdir "${input_dir}hex"
fi
# move all the compiled input files with .x extension to the hex directory
mv "${input_dir}"*.x "${input_dir}"hex/

# collect all input files with the .x extension in the input directory
input_files=("${input_dir}"hex/*.x)

# compile the program
cd ../src/
make

# test with all input files in one command
./sim "${input_files[@]}"
