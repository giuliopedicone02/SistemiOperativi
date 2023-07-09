read -p "Benvenuto! Inserisci il file da scansionare: " file

(gcc alpha-stats-file.c -o alpha-stats-file -pthread) && (./alpha-stats-file $file)