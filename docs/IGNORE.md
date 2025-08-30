# Development diary - Volterra
_This is a personal set of notes used during the development of the project. Please ignore it, as it could be unclear, not correct or updated_.

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

2. _Small upgrades_
- Cambiato da `int` a `std::size_t` il tipo dell'indice in `get_abs_states()`. 
- Implementato test per verificare funzionamento di un singolo evolve() all'interno di run()
*IN SOSPESO*: sembra che non funzioni perché i valori di y a volte rimangono uguali dopo uno step, ma non è detto sia un problema...da capire se il test si puo levare oppure se c'è un prob reale

3. _Preparation to use SFML graphic library_
- Provato a installare SFML su Ubuntu con:
```bash
sudo apt install libsfml-dev
```
ma a quanto pare era già installata sul mio PC, infatti ha returnato:
```bash
Reading package lists... Done
Building dependency tree... Done
Reading state information... Done
libsfml-dev is already the newest version (2.6.1+dfsg-2build2).
0 upgraded, 0 newly installed, 0 to remove and 131 not upgraded.
```

### 02/08/2025
1. _Alternative constructor for Simulation and small upgrades + first draft of graphic window_
- Il metodo `size()` rinominato in `num_steps()` per maggiore chiarezza.
*NOTA*: si è scelto di lasciarlo anche se non fa altro che ritornare la dimensione di `rel_points_` senza altre gestioni particolari di eccezioni ecc, perché puo essere meglio e piu immediata la lettura del codice cosi. Inoltre si usa la size di `rel_points_` e NON di `get_abs_states()` perché è piu veloce e efficiente (NON deve .chiamare ogni volta il metodo per restituire la size, semplicemente legge una cosa gia salvata privatamente nel metodo: questo è utile ed è stato usato ad esempio in alcuni dei test fatti)

- Aggiunto costruttore alternativo di `Simulation` con array di parametri
*NOTA*: si è scelto di usare array e non vettore perché un array non puo essere modificato in dimensione, come è giusto che sia per i parametri. Nel costruttore, NON si può scrivere direttamente
```cpp
  Simulation(Point const initial_abs_point  = {10.0, 5.0},
             std::array<double, 4> const params = {1.0, 0.1, 0.1, 1.0},
             double dt                      = 0.001);
```
perché il costruttore non è in grado di passare automaticamente da una LISTA {} ad un array. La soluzione migliore è definire nello stesso namespace pf ma PRIMA di Simulation (così che la classe posso usarla) l'espressione:
```cpp
inline constexpr std::array<double, 4> const def_params {1.0, 0.1, 0.1, 1.0};
```
e poi nel costruttore:
```cpp
  Simulation(Point const initial_abs_point  = {10.0, 5.0},
             std::array<double, 4> const params = def_params,
             double dt                      = 0.001);
```
dove:
  - `inline` serve a dire "questa variabile può essere definita in più traduzioni di unità (file .cpp), ma il linker ne deve usare una sola": questo fa in modo che non ci siano problemi anche se l'header è inserito in più file .cpp;
  - `constexpr` vuol dire che la variabile è costante ed è noto a compile time: serve al compilatore per ottimizzare il codice. La differenza con `const` è che in questo secondo caso NON necessariamente la variabile è nota a compile time.
*MA*: ho dovuto commentare il default per questa seconda versione del costruttore, perché puo essercene solo una nel programma altrimenti è ambiguo.

- Riscritti i test in volterra.test.cpp usando questo nuovo costruttore per maggiore chiarezza.

- Implementata la prima bozza di finestra grafica, ho scaricato un file per il font e lo ho messo in una nuova cartella.
*IN SOSPESO*: capisci come rendere piu user friendly e efficiente possibile questo passaggio. Inoltre la grafica va molto migliorata.

### 03/08/2025
1. _Major changes to main_
- Metodo `run_visual()` di graph_renderer dichiarato const: l'unico membro privato di Graph_Renderer è sim che è const, quindi siamo sicuri;
- Implementazione di un `std::thread` nel main per fare sì che la finestra grafica possa rimanere aperta ad oltranza e non blocchi l'esecuzione del programma: 
   - Ora sim in Graph Renderer è passata PER VALORE e non più per riferimento: questo permette di evitare errori se l'esecuzione del main termina e le operazioni su questo thread ancora no (si avrebbe comportamento indefinito con riferimento non valido ad un oggetto cancellato)
   - ...
*IN SOSPESO*: questa implementazione è ora come ora un po inutilmente complicata, se non dovesse rivelarsi utile in futuro (cioè se ci son progrmami che devono effettivamente continuare l'esecuzione con la finestra grafica in background) conviene toglierla
- Implementazione funzioni private drawAxes(), initializeFont() e dei corrispondenti metodi pubblici
*IN SOSPESO*: da capire per bene come funzionano + PROVARE AD AGGIUNGERE SALVATAGGIO IMMAGINI
- Modificato il main: adesso ha un menu con tutte le funzioni, che è possibile usare separatamente grazie al fatto che la simulazione viene salvata in una variabile
```cpp
std::unique_ptr<pf::Simulation> sim = nullptr;
```
che all'inizio è dichiarata nullptr. La soluzione `std::unique_ptr` è stata scelta in quanto: 
  - per come strutturato il menu c'è bisogno di ridefinire l'oggetto Simulation dinamicamente a runtime....
  - ....ma l'alternativa (usare delle copie, scrivere cioé `sim = pf::Simulation(ecc)` nei diversi scope) non funziona perché dovrei definire l'operatore = per Simulation, che non si puo fare (almeno non in modo facile, cioé usando `default`) perché Simulation ha dei membri const (i parametri vari)
  - la soluzione equivale a scrivere `pf::Simulation* sim = nullptr` ma con vantaggi aggiuntivi, tipo non dover gestire manualmente il delete e le eccezioni (piu sicuro)
Adesso le varie funzioni chiamate nel main prendono in input una reference (si chiama cosi giusto?) ad un oggetto Simulation e non direttamente un oggetto. 
- Modificato il main: adesso se vengono inseriti parametri (visibilmente) invalidi un loop fa in modo che il programma non termini ma che chieda di inserirne altri. In particolare si usano:
  - `std::cin.clear()`: cancella lo stato di errore di cin (se si ha `std::cin.fail() = true`)
  - `std::cin.ignore(1000,'\n')`: Scarta (ignora) fino a 1000 caratteri oppure fino al primo newline (\n), a seconda di quale condizione si verifica prima (per evitare i vari caratteri non validi che potrebbero essere stati inseriti)
*NOTA*: si è valutato di inserire gestione errori basilari per i parametri (non numerici o negativi) direttamente nel main, lasciando alla classe Simulation la gestione di eccezioni piu complesse e meno banali. Questa scelta serve ad evitare che il programma venga runnato inutilmente quando facilmente evitabile.

### 05/08/2025
_Pausa_

### 06/08/2025
1. _Changes in main design_
- From Point to SpeciesCount (x -> prey and y -> predator) and from State to SpeciesState;
- Helpers del main spostati in appositi file (pro design e modularità): main gestisce Input che viene dato in input dove necessario agli handler, che si occupano di parte computazionale e specifica.
*NOTA*: Per gli errori viene lanciato un throw gestito nel main, mentre per le notifiche all'utente dovrebbe andar bene far stampare direttamente in questi file
- Miglioramento dei messaggi su terminale usando convenzione di [Error], [Warning] e [Info];
- Helper `askInput` implementato: è un TEMPLATE, quindi nel main lo chiamo sia per definire double sia per definire stringhe come feedback 
*NOTA*: i template come questo vanno definiti nel file .hpp e non nel .cpp!!
*IN SOSPESO*: per i parametri sarebbe il caso di definire anche un tetto massimo visto che altrimenti impazzisce il programma (vedi file test): si potrebbe implementare anche min, max come parametri in inpute della funzione nel template T

### 08-09/08/2025
1. _Changes in classes design_
- Definita una struct Parameters in volterra.hpp:
- Aggiunti metodi validatePositive insieme a validateParameters e validateInitialConditions che la usano al suo interno: chiamate nel costruttore di Simulation, rendono il tutto meno verboso
validatePositive è un metodo privato STATICO, ovvero è legato logicamente alla classe Simulation ma NON ad una sua specifica istanza (non ha un puntatore this), infatti ha firma:
```cpp
void static validatePositive(const std::vector<std::pair<std::string, std::double>>& items)
```
ovvero puo prendere in input un QUALSIASI vettore items di questo tipo, non necessariamente legato a membri della classe (nel mio codice viene usato poi da funzioni validateInitialConditions e validateParameters che invece NON sono static perché chiamate nel costruttore per costruire uno specifico oggetto Simulation). 
*NOTA*: in volterra.cpp NON va ripetuto `static` nella definizione.
*IN SOSPESO*: si potrebbe pensare di spostare validatePositive al di fuori di Simulation magari in un file di helpers, ma prob non sarebbe giusto metterlo in helpers.hpp perché lì ci sono esclusivamente funzioni usate dal main e queste responsabilità non andrebbero mischiate

*NOTA*: nei costruttori di Simulation, i double NON sono stati deferenziati perché sono tipi che occupano molta poca memoria e non conviene farlo
