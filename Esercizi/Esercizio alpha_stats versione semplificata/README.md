# Alpha Stats (Versione Semplificata)

Creare un programma **alpha-stats.c** in linguaggio C che accetti invocazioni sulla riga di comando del tipo

```bash
alpha-stats <numero lettere da generare>
```

Il programma sostanzialmente genererà lettere casuali e riporterà il numero di occorrenze riscontrate per tutte le lettere dell'alfabeto inglese.

Tutti i thread condivideranno una struttura dati condivisa contenente 
- **c**: un char/byte;
- **alf**: un array di 26 interi inizialmente posti a zero;
- altri elementi ritenuti utili/necessari.

Per coordinarsi, i thread dovranno usare un certo numero di semafori (minimo a scelta dello studente).

Il thread **Controllore**, dopo le operazioni preliminari sopra indicate, genererà una lettera causale per poi depositarla in **c** ed attivare, rispettivamente, il thread **AL** o **MZ** a secondo dell'intervallo di competenza. 

Il thread **AL** (o **MZ**) una volta attivato provvederà ad aggiornare il contatore corrispondente alla lettera ricevuta presente nell'array **alf**.

Dopo che tutte le lettere generate saranno state "analizzate" e le statistiche aggiornate, allora il thread **Controllore** provvederà a stampare a video le statistiche presenti all'interno di **alf** e tutti i thread termineranno spontaneamente.

Tutte le strutture dati utilizzate dovranno essere ripulite correttamente all'uscita.

## Esempio di esecuzione
```bash
gcc alpha-stats.c -o alpha-stats -pthread
./alpha-stats 5000

# In alternativa: bash start.sh

[C] Ho generato la lettera: l
[AL] Ho incrementato il contatore della lettera: l
[C] Ho generato la lettera: w
[MZ] Ho incrementato il contatore della lettera: w
[C] Ho generato la lettera: n
[MZ] Ho incrementato il contatore della lettera: n
[C] Ho generato la lettera: g
[AL] Ho incrementato il contatore della lettera: g

...

[C] Statistiche: 
a: 186  b: 186  c: 180  d: 190
e: 205  f: 197  g: 204  h: 193
i: 191  j: 197  k: 206  l: 196
m: 169  n: 180  o: 214  p: 194
q: 195  r: 188  s: 182  t: 187
u: 203  v: 202  w: 192  x: 191
y: 175  z: 197
```