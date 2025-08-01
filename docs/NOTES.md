# Diario di sviluppo - Lotka Volterra (corso "Programmazione per la fisica")
## Diario giornaliero
### 27/07/25

- CMakeLists.txt, .clang-format and doctest.h imported from shared repo using
  ```bash
  curl https://raw.githubusercontent.com/Programmazione-per-la-Fisica/progetto2024/main/filename.ext -o filename.ext

- Initialized CMake for project using
  ```bash
  cmake -S . -B build -G "Ninja Multi-Config"
  ```
  where:
  - `-S .`: _Start directory_: specifica la cartella dei sorgenti, in questo caso . (la directory corrente):
  - `-B build`: _Binary directory_: indica dove salvare i file di build generati. Crea una cartella `build/`;
  - `-G "Ninja Multi-Config` _Generatore_: dice a CMake di usare Ninja come sistema di build, in modalità "multi-config" (per gestire sia `Debug`che `Release`).

- File CMakeLists.txt edited: sostituiti nomi generici del file con quelli del progetto. 
*DUBBIO*: `add_executable(progetto.t volterra.test.cpp` dovrebbe servire per creare l'eseguibile dei test, non sono sicurissima che ci vada il già creato file `volterra.test.cpp` al secondo termine

- Modifiche a `volterra.hpp`: 
prima bozza della struttura della classe `Simulation`.
  *IN SOSPESO*: 
  - costruttore `Simulation`: se ne potrebbero usare diverse versioni a seconda di quali parametri vengono dati in input (es: solo i parametri delle ode e non le condizioni iniziali, viceversa, ecc)
  - metodo `abs`: nome troppo generico
  - in generale non si è ancora mai provata la riuscita del programma e potrebbero esserci parecchi errore nelle implementazioni

- Modifiche a `volterra.cpp`;
prima bozza dell'implementazione dei metodi di `Simulation`.
  *IN SOSPESO*:
  - decidere come trattare l'integrale primo in `evolve` e non solo (_puo avere senso eliminarlo totalmente da `State`?_)

### 28/07/25
*OBIETTIVI*
- *IN SOSPESO* di ieri;
- terminare prima stesura del codice (ricontrollare logica)
- inserire eccezioni
- (implementare test)

1. _Structure of class Simulation changed + debug for compilation_ 

- Cambiata la nomenclatura in `Simulation`: ora usata convenzione dei `getter`, i `to` ecc.

- Cambiata la struttura di `Simulation`:
  - adesso gli stati sono conservati in `states_` in forma assoluta e non relativa;
  - modificato `to_abs()`: modifica solo uno `State` per volte e non tutto il vettore;
  - gli stati vengono convertiti in forma relativa con `to_rel()` direttamente in `evolve` e poi riconvertiti in assoluti con `to_abs()` per essere aggiunti a `states_`

- Provata prima compilazione con CMake. Usato `cmake --build build --config Debug`.
Errori riscontrati: 
```bash
/usr/bin/ld: /usr/lib/gcc/x86_64-linux-gnu/13/../../../x86_64-linux-gnu/Scrt1.o: in function `_start':
(.text+0x1b): undefined reference to `main'
```
Modifiche apportate:
  - aggiunta bozza funzione `main()` in main.cpp:
  - aggiunto `#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN` in volterra.hpp;

Problema risolto.

2. _Internal representation with relative values in class Simulation_

- Cambiata la struttura di `Simulation`:
  - adesso gli stati sono conservati in `states_` di nuovo in forma relativa;
  - gli stati vengono convertiti in forma assoluta con `to_abs()` direttamente in `states()`
  - eliminata `to_rel()`

3. _Constructor and function `compute_H()` added_

- Riaggiunta `to rel()` (serve per implementare il costruttore);
- Implementato il costruttore di `Simulation`;
- Implementata funzione `compute_H()`

*IN SOSPESO*
H non deve venire trattato in `evolve()`, ma viene calcolato solo dopo in `get_abs_states()` in quanto si calcola dai valori assoluti: conviene definire un tipo `Point` di cui `State` è derivata in modo tale da lavorare con `Point` in tutta la rappresentazione interna

4. _Ecceptions for constructor added_ 

- Eccezione nel costruttore: tutti i parametri di input devono essere positivi.
`std::invalid_argument` è una sottoclasse di `std::logic_error`, che insieme a `std::runtime_error` (e altri) costituiscono la classe <stdexcept>  
*IN SOSPESO*: si potrebbe usare un `bool` con l'unione delle condizioni piuttosto che elencarle nell'`if`.

*PROBLEMA*: mi serve il dt nel main per runnare `evolve()` in un ciclo il numero corretto di volte: potrei creare un nuovo metodo di `Simulation` che fa tutto e nel main lo chiamo e basta (il metodo potrebbe accedere senza problemi a `dt_`) oppure potrei implementare un getter per `dt_` e usare quello nel main.

### 29/07/2025

1. _I/O implementation draft_
*NOTA*: Valutato di creare un metodo privato booleano `is_valid_duration` da chiamare in `run()` per la gestione delle eccezioni: alla fine valutato non fosse il caso, perché non ci sono altre situazioni in cui verrebbe usato (si era pensato di renderlo magari una funzione esterna alla classe, ma ha bisogno di accedere a `dt_` e usare una `friend` sarebbe stato overkill in questo caso, quindi al momento si è scelta la strata più lineare). Si gestisce quindi l'eccezione direttamente dentro `run`
*IN SOSPESO*: ci si riserva per il futuro di valutare di cambiare l'accesso a `dt_`, tipo di renderla una variabile accessibile tramite un getter. Sospetto però che sia una inutile complicazione.

- Modifiche al main: gestione di input/output:
  - Modificato costruttore per prendere in input Point con stato iniziale e non State: l'utente NON deve inserire il valore iniziale di H, in quanto esso può essere calcolato da programma a partire da altri input. 
  *IN SOSPESO*: va modificato ulteriormente il costruttore e la classe in generale per farlo lavorare internamente in Points e non in States (`states_` dovrebbe diventare un `points_`)

2. _Simulation now internally works with Point instead of State_
- Gestione separata di Point internamente e di State per chiamata esterna: 
  - modificata struttura del costruttore, ora inizializza un vettore `points_` e non piu `states_`;
  - `evolve()` lavora in Point;
  - `to_rel()` e `to_abs()` lavorano in Point;
  - `compute_H()` lavora in Point;
  - `get_last()` cambiato in `get_last_point()`: ora lavora in Point
  *IN SOSPESO*: Ha senso far sopravvivere questo metodo? del resto non fa altro che usare `.back()`, ha senso tenerlo se in piu gestisce delle eccezioni, ma in questo caso il vettore `rel_states_` NON è mai vuoto perché viene riempito gia dal costruttore in poi...
  *IN SOSPESO*: è giusto che il costruttore faccia gia una azione (riempire `rel_states_`) oltre alla lista di inizializzazione??
  - solo "a richiesta" viene creato il corrispondente vettore di State e restituito all'utente: aggiornamento del metodo `get_abs_states()`;
  *NOTA*: si è valutato di creare sin dall'inizio due vettori `rel_points_` e `abs_states_` riempendoli contemporaneamente ad ogni chiamata di `evolve()`, ma alla fine non fatto perché piu dispendioso a livello di memoria e di costo computazionale: meglio "trasformare" in State solo se e quando necessario (ad eventuale chiamata, che si presume non siano cosi frequenti).
  *IN SOSPESO* ci si riserva di cambiare idea, magari valutando tempo di esecuzione del programma con metodi di Doctest nei diversi casi.

3. _Implementation of `run()`_
- uso del metodo `std::ceil` per arrotondare al piu piccolo multiplo intero e di `static_cast` per convertire in `int` (pare non sia sconsigliato sulle coding conventions):
  ```cpp
  int steps                = static_cast<int>(std::ceil(duration / dt_));
  ```
4. _Draft solution for main_
- Miglioramento della implementazione: evitata ripetizione di codice nei blocchi if else creando funzione ausiliaria `execute_simulation`
- Aggiunto blocco try catch per la gestione delle eccezioni riscontrate: l'uso di catch per `std::exception` assicura che tutti i diversi tipi di errori chiamati nel classe Simulation siano trattati correttamente in quanto rientrano in questa categoria.

*NOTA*: i numeri di prede e predatori devono essere sempre interi? NO, nella traccia è specificato che debbano essere REALI positivi, in quanto possono rappresentare anche una densità di popolazione o simile

5. _Exceptions solutions added_
- Errore valori negativi aggiunta a `evolve`
*NOTA*: NON dovrebbe mai succedere se controllo a priori di valori positivi iniziali funziona, serve più per trovare eventuali bug, ad esempio se l'utente modifica il `dt_` usandone uno molto grande (es: 0.5) la simulazione potrebbe diventare instabile.

*IN SOSPESO*: al momento si è deciso di non mettere nessun controllo aggiuntivo nella funzione `compute_H` per il discorso dei logaritmi, in quanto si presume che i valori siano sicuramente positivi perché controllati precedentemente. Ci si riserva di cambiare la gestione delle eccezioni per i logaritmi in `compute_H` se si valutasse di rendere questo metodo pubblico, in modifiche successive del codice.

- Errore durata negativa aggiunta a `run` (e rimossa dal main)

*IN SOSPESO*: nel main, per ora la gestion delle eccezioni è tutta fatta in `main()`, anche se l'eccezione potrebbe essere sollevata all'interno della funzione ausiliaria `execute_simuation()`. Questo forse rende il codice non abbastanza modulare e riutilizzabile. Da rivedere. 
Si potrebbe ad esempio mettere come return di `execute_simulation()` un bool che dice se ci sono stati errori o qualcosa di simile.

### 30/07/2025
_Pausa_

### 31/07/2025

1. _Design update: output moved from implementation file to main_
- Output di avviso ("durata arrotondata a...") spostata nella funzione del main `execute_simulation()` dal metodo `run()`: è un miglioramento di design, che rispetta i principi di "divisione dei ruoli" (I/O va fatta nel main). In particolare:
  - uso di `std::pair` per returnare dalla funzione `run()` la durata effettiva della simulazione;
  *NOTA*: si è scelto infatti di returnare anche il numero di steps, anche se per ora non è utile è effortless e migliora riusabilità
  - nel main si usa anche il metodo `get_dt()`, che quindi ora ha un suo senso
  - uso della struttura moderna
  ``` cpp
  auto [steps, adjusted_duration] = sim.run(duration);
  ```
  visto che run ha come output un `std::pair`.

2. _Draft output on file_
- Creazione funzione ausiliaria nel main `write_on_file()` che usa std::fstream.
*IN SOSPESO*: stesso discorso gestione errori e modularità di funzione `execute_simulation()` (vedi 29/07/2025)

*PROBLEMA*: la simulazione sembra diventare instabile per valori grandi (c.a 10 s): si rimanda ai test per capire meglio

*IN SOSPESO*: non so quanto abbia senso che i valori iniziali assoluti vengano convertiti in relativi per poi dover essere riconvertiti in assoluti....

3. _Tests on constructor and run() implemented_
- Implementati test su costruttore per valori di default e per valori sbagliati (nulli o negativi).
*IN SOSPESO*: si potrebbe migliorare la leggibilità definendo un vettore con i valori di default in modo da mettere in evidenza quelli modificati volta per volta (creare un getter dei valori di default sembra overkill)

- Implementati test su run() per verificarne la stabilità
*IN SOSPESO*: alcuni test commentati perché danno problemi (non stabile per quei casi): da capire se scrivere in relazione/non fare niente/provare a mascherare la cosa
*IN SOSPESO*:  pensa se è corretto o se nasconde errore implementazione

### 01/08/2025

1. _Time inserted in declaration of State_
- Adesso in ogni `State` c'è anche un double che è il tempo corrispondente ai punti considerati. Serve perché nell'output è giusto che ci sia per ogni coppia di punti preda/predatore il corrispondnete tempo dell'evoluzione.
*NOTA*: si era valutato di aggiungere il tempo alla dichiarazione di `Point`, ma sarebbe stato un appesantimento sostanzialmente inutile visto che i `Point` sono usati solo per i calcoli interni del metodo, in cui non serve conoscere mai il tempo. L'"appesantimento" dei calcoli temporali si fanno cosi solo se necessari. 

- Modificata gestione dell'eccezione per duration < dt
*IN SOSPESO*: test per duration < dt dà due valori invece che uno: questo dipende dall'uso di `std::ceil` che arrotonda per eccesso e quindi in questa situazione steps è uguale ad 1, dunque nel vettore degli stati è salvato la condizione iniziale + 1 evoluzione (al tempo dt_ * 1), quindi in tutto ci sono due elementi. 
Da valutare se è preferibile che per duration < dt NON venga stampato nessuno stato oltre alla condizione iniziale (si potrebbe usare solo in questo caso limite `std::floor` che arrotonda per difetto). 

2.
- Cambiato da `int` a `std::size_t` il tipo dell'indice in `get_abs_states()`. 
- Implementato test per verificare funzionamento di un singolo evolve() all'interno di run()
*IN SOSPESO*: sembra che non funzioni perché i valori di y a volte rimangono uguali dopo uno step, ma non è detto sia un problema...da capire se il test si puo levare oppure se c'è un prob reale