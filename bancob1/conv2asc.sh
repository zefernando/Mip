cp $1 $1.ebc
./conv2asc -t $1  -l 133 -d 1
cp $1.asc $1 
