read -p "Benvenuto! Inserisci il file da scansionare: " file

(gcc pari-dispari-file.c -o pari-dispari-file -pthread) && (./pari-dispari-file $file)