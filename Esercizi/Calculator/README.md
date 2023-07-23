# Calculator

Creare un programma **calculator.c** in linguaggio C che accetti invocazioni sulla riga di comando del tipo:

```bash
calculator <list.txt>
```

Il programma dovrà fondamentalmente leggere una sequenza di operazioni numericheda eseguire in sequenza a partire da un valore iniziale nullo (0) e presentare il risultato finale. Ogni riga del file contiene l'operatore da applicare (tra somma, moltiplicazione esottrazione) e il secondo operando (tipo  +2,  *10,  -9, ...). Il primo operando è sempre il valore dell'operazione precedente.

Per coordinarsi, i thread dovranno usare un certo numero di semafori (minimo a scelta dello studente).

Il thread **MNG**, coordinerà tutto il calcolo: leggerà riga-per-riga la lista e per ognioperazione passerà il risultato parziale (partenzo da 0) e l'operando appena letto al processo associato all'operazione attuale (+, * o  -); 

Il thread **ADD**, **MUL** e **SUB**, una volta attivati applicheranno la propria operazione ai   due   operandi   presenti   nel   segmento   condiviso,   depositando   nello   stesso   il risultato..

Ogni thread dovrà dare sullo standard output un feedback sulle operazioni via via eseguite.

Tutte le strutture dati utilizzate dovranno essere ripulite correttamente all'uscita e tutti i thread termineranno spontaneamente.

## Esempio di esecuzione
```bash
gcc calculator.c -o calculator -pthread
./calculator list-1.txt

# In alternativa: bash start.sh

[MNG]: risultato intermedio: 0; letto "+41"
[ADD]: 0+41=41
[MNG]: risultato intermedio: 41; letto "-56"
[SUB]: 41-56=-15 
[MNG]: risultato intermedio: -15; letto "+23"
[ADD]: -15+23=8
[MNG]: risultato intermedio: 8; letto "+26"
[ADD]: 8+26=34

...

[MNG]: risultato intermedio: 831757330; letto "+4"
[ADD]: 831757330+4=831757334
[MNG]: risultato intermedio: 831757334; letto "+92"
[ADD]: 831757334+92=831757426
[MNG]: risultato finale: 831757426
```