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