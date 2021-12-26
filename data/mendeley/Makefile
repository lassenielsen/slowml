PCAPS = $(ls data/*.pcap)
SESSIONS = $(ls data/*.txt)

default: 
	echo "Usage:"
	echo "  make fetch   - downloads the datafile"
	echo "  make prepare - unpacks and preprocesses the datafile to create the datasets"
	echo "  make train   - trains the model"
	echo "  make test    - tests the model"

PHONY: clean mrpropper fetch prepare train test½½

clean:
	touch data/mapping.csv
	rm -Rf data/mapping.csv
	mkdir -p data/csv
	rm -Rf data/csv
	mkdir -p data/pcap
	rm -Rf data/pcap
	touch data/clean.tar.gz
	rm -Rf data/*.tar.gz
	touch data/clean.txt
	rm -Rf data/*.txt

mrpropper: clean
	mkdir -p data
	touch data/clean.pcap
	rm -Rf data/*.pcap
	mkdir -p trainingset
	rm -Rf trainingset
	mkdir -p testset
	rm -Rf testset
	mkdir -p models
	touch models/clean.mod
	rm models/*.mod

fetch: data/pcaps.zip

prepare: data/pcaps.zip extract # generates trainingset and testset
	echo Unzipping
	unzip data/pcaps.zip -d ./data/
	for x in `ls data/*.tar.gz`; do echo -n Preparing $$x; tar -xvzf $$x -C ./data/; echo .;done;
	./scripts/prepare.sh

train: sessions.mod

improve: sessions.mod
	slowml_train -gdr 150000 -c -o sessions.mod trainingset

test: sessions.mod testset
	slowml_test -m sessions.mod testset

data/pcaps.zip:
	mkdir -p data
	wget https://md-datasets-cache-zipfiles-prod.s3.eu-west-1.amazonaws.com/5pmnkshffm-1.zip -O ./data/pcaps.zip

sessions.mod: trainingset
	slowml_train -gdr 5000 -o sessions.mod trainingset
