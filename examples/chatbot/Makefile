default: data_extract

data_extract:
	g++ -o data_extract data_extract.cpp

data/simple_train: data_extract
	mkdir -p data/simple_train
	./data_extract data/raw/simpledata1.txt data/simple_train

simplebot.mod: data/simple_train
	stutterml -n '513->[[1000'*[all]],[512*[all]]]' -ms 256 -gda 0 -gdr 1 -gdl 0.01 -o simplebot_1.mod
	stutterml -m simplebot_1.mod -ms 256 -gda 0 -gdr 99 -gdl 0.01 -o simplebot_100.mod
	cp simplebot_100.mod simplebot.mod
