# Sort-List

Creare un programma **sort-list.c** in linguaggio C che accetti invocazioni sulla riga di comando del tipo:

```bash
sort-list <file.txt>
```

Il programma deve sostanzialmente leggere il file di testo specificato, contenente una 
parola per ogni riga, e dare in output tale lista riordinata (ordine alfanumerico crescente  
e case-insensitive).

Per coordinarsi, i thread dovranno usare un certo numero di semafori (minimo a scelta dello studente).

Il thread **Sorter**, dovrà leggere il file di testo, estraendone la lista di parole (una per ogni riga) e  
dovrà   applicarvi   un   qualche   algoritmo   di   ordinamento   (ad   esempio:   bubble-sort   o 
insertion-sort); nel passo fondamentale di confronto di 2 stringhe, esso dovrà sempre 
richiedere l'aiuto del thread **Comparer** inviandogli le 2 stringhe di turno ed ottenendo 
indietro   il   risultato   del   confronto   (un   intero).   Una   volta   applicato   l'algoritmo   di 
ordinamento, il processo  Sorter  dovrà passare, parola per parola, la lista al thread **Padre** che 
provvederà a mandarla sullo standard-output.


Tutte le strutture dati utilizzate dovranno essere ripulite correttamente all'uscita e tutti i thread termineranno spontaneamente.

## Esempio di esecuzione
```bash
cat /usr/share/dict/italian | sort -R | head -n 100 > words.txt

gcc sort-list.c -o sort-list -pthread
./sort-list words.txt

# In alternativa: bash start.sh

abbonano
addizionavo
adirammo
adopereranno
agisco
agitavamo
ammortito
andarsene
antidiluviano
appostati

...

tronchereste
trucideremo
una infamia
valigie
volerla
zitto
```