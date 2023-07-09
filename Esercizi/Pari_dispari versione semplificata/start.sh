read -p "Benvenuto! Inserisci quantita di numeri da generare: " numero

(gcc pari-dispari.c -o pari-dispari -pthread) && (./pari-dispari $numero)