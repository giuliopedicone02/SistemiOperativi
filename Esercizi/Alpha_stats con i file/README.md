# Alpha Stats (Versione con file)

Creare un programma **alpha-stats-file.c** in linguaggio C che accetti invocazioni sulla riga di comando del tipo

```bash
alpha-stats-file <file.txt>
```

Il programma sostanzialmente leggerà il file di testo indicato e riporterà il numero di occorrenze riscontrate per tutte le lettere dell'alfabeto inglese.

Tutti i thread condivideranno una struttura dati condivisa contenente 
- **c**: un char/byte;
- **alf**: un array di 26 interi inizialmente posti a zero;
- altri elementi ritenuti utili/necessari.

Per coordinarsi, i thread dovranno usare un certo numero di semafori (minimo a scelta dello studente).

Il thread **Controllore**, dopo le operazioni preliminari sopra indicate, leggerà il contenuto del file per poi depositarla in **c** ed attivare, rispettivamente, il thread **AL** o **MZ** a secondo dell'intervallo di competenza. 

Il thread **AL** (o **MZ**) una volta attivato provvederà ad aggiornare il contatore corrispondente alla lettera ricevuta presente nell'array **alf**.

Dopo che tutte le lettere del file saranno state "analizzate" e le statistiche aggiornate, allora il thread **Controllore** provvederà a stampare a video le statistiche presenti all'interno di **alf** e tutti i thread termineranno spontaneamente.

Tutte le strutture dati utilizzate dovranno essere ripulite correttamente all'uscita.

## Esempio di esecuzione
```bash
gcc alpha-stats-file.c -o alpha-stats-file -pthread
./alpha-stats lettere.txt

# In alternativa: bash start.sh (inserendo il nome del file "lettere.txt")

[C] Ho letto dal file la lettera: i
[AL] Ho incrementato il contatore della lettera: i
[C] Ho letto dal file la lettera: t
[MZ] Ho incrementato il contatore della lettera: t
[C] Ho letto dal file la lettera: y
[MZ] Ho incrementato il contatore della lettera: y
[C] Ho letto dal file la lettera: f
[AL] Ho incrementato il contatore della lettera: f

...

[C] Statistiche sul file lettere.txt: 
a: 47   b: 7    c: 30   d: 24
e: 73   f: 20   g: 14   h: 12
i: 53   j: 1    k: 5    l: 41
m: 17   n: 44   o: 56   p: 25
q: 3    r: 44   s: 32   t: 60
u: 30   v: 7    w: 7    x: 1
y: 6    z: 1
```