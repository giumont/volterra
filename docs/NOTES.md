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
