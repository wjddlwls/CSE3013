#!/bin/bash
echo 'Working directory'
read -r dirname

if [ -n "$dirname" ]
then
	if [ -d "$dirname" ] && [ -x "$dirname" ]
	then
		cd "$dirname"
	else
		echo "error: directory $dirname does not exist."
		exit
	fi	
fi

for dir in *
do
newname=$(echo "$dir" | tr "[a-z] [A-Z]" "[A-Z] [a-z]")
mv $dir $newname
done

