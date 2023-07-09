read -p "Benvenuto! Inserisci il numero di lettere da generare: " numero

(gcc alpha-stats.c -o alpha-stats -pthread) && (./alpha-stats $numero)