# Pari Dispari (Versione con i file)

Creare un programma **pari-dispari-file.c** in linguaggio C che accetti invocazioni sulla riga di comando del tipo

```bash
pari-dispari-file <file.txt>
```

Il programma sostanzialmente leggerà il file di testo indicato e riporterà le occorrenze di numeri pari o dispari riscontrate.

Tutti i thread condivideranno una struttura dati condivisa contenente 
- **numero**: un numero intero;
- **verifica**: un array di 2 interi inizialmente posti a zero;
- altri elementi ritenuti utili/necessari.

Per coordinarsi, i thread dovranno usare un certo numero di semafori (minimo a scelta dello studente).

Il thread **Controllore**, dopo le operazioni preliminari sopra indicate, leggerà il contenuto del file per poi depositarlo in **numero** ed attivare, rispettivamente, il thread **Pari** o **Dispari** a secondo del risultato dell'operazione di controllo. 

Il thread **Pari** (o **Dispari**) una volta attivato provvederà ad aggiornare il contatore corrispondente al numero ricevuto presente nell'array **verifica** 
(**verifica[0]**: numeri pari, **verifica[1]**: numeri dispari) .

Dopo che tutti i numeri del file saranno stati "analizzati" e le statistiche aggiornate, allora il thread **Controllore** provvederà a stampare a video le statistiche presenti all'interno di **verifica** e tutti i thread termineranno spontaneamente.

Tutte le strutture dati utilizzate dovranno essere ripulite correttamente all'uscita.

## Esempio di esecuzione
```bash
gcc pari-dispari-file.c -o pari-dispari-file -pthread
./pari-dispari-file numeri.txt

# In alternativa: bash start.sh (inserendo il nome del file "numeri.txt")

[C] Ho letto dal file il numero: 6
[P] Incrementato il contatore dei numeri pari
[C] Ho letto dal file il numero: 79
[D] Incrementato il contatore dei numeri dispari
[C] Ho letto dal file il numero: 35
[D] Incrementato il contatore dei numeri dispari
[C] Ho letto dal file il numero: 3
[D] Incrementato il contatore dei numeri dispari

...

[C] I risultati sono P = 42, D = 58 
```