# Laboratorio di Sistemi Operativi

* **Chiamate di sistema** (TRAP, modalità kernel)
* **Chiamate di libreria** (modalità utente)

Nel corso ci occuperemo dei seguenti sottosistemi:

* Gestione dei **file system** e dell'**I/O**
* Gestione dei **processi**
* Gestione dei **thread**
* **Comunicazione** e **sincronizzazione** di processi e thread

## Creazione di programmi eseguibili

* Compilazione diretta di un solo sorgente

    ```bash
    gcc -o nomeEseguibile sorgente.c
    ```

* Compilazione diretta da sorgenti multipli

    ```bash
    gcc -c file1.c; gcc -c file2.c;
    ```

## Terminazione di un processo

Bisogna inglobare la libreria stdlib.h

* **exit:** Restituisce 0 se la terminazione va a buon fine, >0 se ci sono stati errori

Permette di terminare in modo pulito il processo

## Apertura, Creazione e Chiusura di un file

* **open** apre e crea un file, ritorna -1 in caso di errore
    * **O_RDONLY / O_WRONLY / O_RDWR**
    * **O_APPEND:** Scrive alla fine del file
    * **O_CREAT:** Crea il file se non esiste usando i permessi indicagti in *mode*
    * **O_EXCL:** genera un errore se il file esiste già
    * **O_TRUNC:** Se il file esiste viene troncato ad una lunghezza pari a 0
* **creat** equivale ad **open(path, O_RDWR | O_CREAT | O_TRUNC, mode)**
* **close** chiude un file aperto