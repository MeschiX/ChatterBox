#!/bin/bash

if [[ $# != 2 ]]; then
    echo "usa $0 config_file file_age"
    exit 1
fi


while IFS='' read -r line || [[ -n "$line" ]]; do
    if [[ $line == DirName* ]]; then
        DIR=$(echo $line | cut -d ' ' -f 3-)
    fi
done < "$1"

echo "$DIR"

COUNT=0
touch FILE_TO_COMPRESS
chmod 777 FILE_TO_COMPRESS

for filename in $DIR; do
    if [[ $(basename $filename) != .* ]]; then
        if test `find "$filename" -type f -mmin +$1`; then
            echo "$filename" >> FILE_TO_COMPRESS
            COUNT=$(($COUNT+1))
        elif test `find "$filename" -type d -mmin +$1`; then
            echo "$filename" >> FILE_TO_COMPRESS
            COUNT=$(($COUNT+1))
        fi
    fi
done

if [[ $COUNT != 0 ]]; then
    tar -cvf "FilesOlderThan$1Min.tar.gz" $(cat FILE_TO_COMPRESS)
fi

while read name; do
  rm "$name"
done < FILE_TO_COMPRESS

rm -rf FILE_TO_COMPRESS
