ls T* |
while read arquivo
do
        echo $arquivo
	cp $arquivo.asc $arquivo
	./conv2asc -t $arquivo  -l 133 -d 1
        mv $arquivo.asc $arquivo
done
