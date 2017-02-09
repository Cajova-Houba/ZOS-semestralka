#!/bin/bash
# Script for testing ZOS
orig_fat=empty.fat.orig
fat=empty.fat

bad_c_orig_fat=bad_c.fat.orig
bad_c_fat=bad_c.fat

exec=semestralka

# use clean fat
echo Using clean fat.
cp $orig_fat $fat
cp $bad_c_orig_fat $bad_c_fat

# print file tree
echo Printing file tree.
./$exec $fat -p

# create some directories
echo Creatinig directories.
./$exec $fat -m new-dir1 /direct-1/
./$exec $fat -m new-dir2 /direct-1/new-dir1/
./$exec $fat -m new-dir3 /direct-1/
echo Created.
./$exec $fat -p

# import some data
echo Importing fairy tales.
./$exec $fat -a pohadka-2.txt /direct-1/new-dir1/p1.txt
./$exec $fat -a pohadka-2.txt /direct-1/p2.txt
./$exec $fat -a pohadka-2.txt /direct-1/new-dir2/p3.txt
./$exec $fat -a pohadka-2.txt /direct-1/new-dir3/p4.txt
./$exec $fat -a pohadka-2.txt /direct-1/p3.txt
echo Imported.
./$exec $fat -p

# delete a file and replace it
echo Deleting /direct-1/p2.txt and replacing it with another fairy tale.
./$exec $fat -f /direct-1/p2.txt
./$exec $fat -a pohadka-3.txt /direct-1/nova-p1.txt
echo Done.

echo Printing info about new fairy tale.
./$exec $fat -c /direct-1/nova-p1.txt
./$exec $fat -l /direct-1/nova-p1.txt
./$exec $fat -p

# try to delete non-existent and non-empty directories
echo Attempt to delete /direct-1/new-dir2/
./$exec $fat -r /direct-1/new-dir2/
echo Attempt to delete /direct-1/
./$exec $fat -r /direct-1/

# replace file with directory and check that no garbage will be printed
echo Replacing /direct-1/nova-p1.txt with directory np-dir
./$exec $fat -f /direct-1/nova-p1.txt
./$exec $fat -m np-dir /direct-1/
./$exec $fat -p

# try to access deleted file
echo Trying to read /direct-1/nova-p1.txt
./$exec $fat -l /direct-1/nova-p1.txt

# try out the cluster printing functionality
echo Printing out clusters of some files.
./$exec $fat -c /
./$exec $fat -c /msg.txt
./$exec $fat -c /direct-1/new-dir1/p1.txt

# try to create a dir which already exists
echo Trying to create new-dir1 in /direct-1/
./$exec $fat -m new-dir1 /direct-1/
echo Trying to create a p1.txt in /direct-1/new-dir1/
./$exec $fat -a pohadka-2.txt "/direct-1/new-dir1/p1.txt"
./$exec $fat -p

echo Done!

# fat with bad cluster should have 2 bad cluster
# both of them in files
echo Testing bad clusters.
echo Printing files with bad clusters.
./$exec $bad_c_fat -c /cisla.txt
#./$exec $bad_c_fat -l /cisla.txt

./$exec $bad_c_fat -c /pohadka.txt
#./$exec $bad_c_fat -l /pohadka.txt

echo Fixing bad blocks.
./$exec $bad_c_fat -v

echo Printing files again
./$exec $bad_c_fat -c /cisla.txt
#./$exec $bad_c_fat -l /cisla.txt

./$exec $bad_c_fat -c /pohadka.txt
#./$exec $bad_c_fat -l /pohadka.txt

# no bad blocks should be detected now
echo Fixing bad blocks again
./$exec $bad_c_fat -v

echo Done!
