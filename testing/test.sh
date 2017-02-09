#!/bin/bash
# Script for testing ZOS
orig_fat=empty.fat.orig
fat=empty.fat

# use clean fat
echo Using clean fat.
cp $orig_fat $fat

# print file tree
echo Printing file tree.
./semestralka $fat -p

# create some directories
echo Creatinig directories.
./semestralka $fat -m new-dir1 /direct-1/
./semestralka $fat -m new-dir2 /direct-1/new-dir1/
./semestralka $fat -m new-dir3 /direct-1/
echo Created.
./semestralka $fat -p

# import some data
echo Importing fairy tales.
./semestralka $fat -a pohadka-2.txt /direct-1/new-dir1/p1.txt
./semestralka $fat -a pohadka-2.txt /direct-1/p2.txt
./semestralka $fat -a pohadka-2.txt /direct-1/new-dir2/p3.txt
./semestralka $fat -a pohadka-2.txt /direct-1/new-dir3/p4.txt
./semestralka $fat -a pohadka-2.txt /direct-1/p3.txt
echo Imported.
./semestralka $fat -p

# delete a file and replace it
echo Deleting /direct-1/p2.txt and replacing it with another fairy tale.
./semestralka $fat -f /direct-1/p2.txt
./semestralka $fat -a pohadka-3.txt /direct-1/nova-p1.txt
echo Done.

echo Printing info about new fairy tale.
./semestralka $fat -c /direct-1/nova-p1.txt
./semestralka $fat -l /direct-1/nova-p1.txt
./semestralka $fat -p

# try to delete non-existent and non-empty directories
echo Attempt to delete /direct-1/new-dir2/
./semestralka $fat -r /direct-1/new-dir2/
echo Attempt to delete /direct-1/
./semestralka $fat -r /direct-1/

# replace file with directory and check that no garbage will be printed
echo Replacing /direct-1/nova-p1.txt with directory np-dir
./semestralka $fat -f /direct-1/nova-p1.txt
./semestralka $fat -m np-dir /direct-1/
./semestralka $fat -p

# try to access deleted file
echo Trying to read /direct-1/nova-p1.txt
./semestralka $fat -l /direct-1/nova-p1.txt

# try out the cluster printing functionality
echo Printing out clusters of some files.
./semestralka $fat -c /
./semestralka $fat -c /msg.txt
./semestralka $fat -c /direct-1/new-dir1/p1.txt

echo Done!



