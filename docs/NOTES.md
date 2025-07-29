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
H non deve venire trattato in `evolve()`, ma viene calcolato solo dopo in `get_states()` in quanto si calcola dai valori assoluti: conviene definire un tipo `Point` di cui `State` è derivata in modo tale da lavorare con `Point` in tutta la rappresentazione interna

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
  - solo "a richiesta" viene creato il corrispondente vettore di State e restituito all'utente: aggiornamento del metodo `get_states()`;
  *NOTA*: si è valutato di creare sin dall'inizio due vettori `rel_points_` e `abs_states_` riempendoli contemporaneamente ad ogni chiamata di `evolve()`, ma alla fine non fatto perché piu dispendioso a livello di memoria e di costo computazionale: meglio "trasformare" in State solo se e quando necessario (ad eventuale chiamata, che si presume non siano cosi frequenti).
  *IN SOSPESO* ci si riserva di cambiare idea, magari valutando tempo di esecuzione del programma con metodi di Doctest nei diversi casi.

3. _Implementation of `run()`_
- uso del metodo `std::ceil` per arrotondare al piu piccolo multiplo intero e di `static_cast` per convertire in `int` (pare non sia sconsigliato sulle coding conventions):
  ```cpp
  int steps                = static_cast<int>(std::ceil(duration / dt_));
  ```
4. _Draft solution for main_
*IN SOSPESO*: nel main usata soluzione temporanea di ripetere codice due volte nei blocchi if else: bisogna trovare un modo alternativo non ridondante.
In generale il main va migliorato nell'interfaccia utente. 

*IN SOSPESO*: i numeri di prede e predatori devono essere sempre interi!!!
