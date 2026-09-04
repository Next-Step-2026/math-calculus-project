Nosso projeto basicamente consiste em coordenar duas equipes (uma em python e uma em cpp) para criar uma interface matematica.

A parte matematica sera em cpp, enquanto a interface e coordenacao de testes sera em PYTHON.

Para isso temos algumas tarefas:

(V1) Código simples para testar integrações
TIME CPP:

- [X] codigo .cpp que recebe dois numeros e retorna a soma deles (receber inputs e outputs)- Joao
TIME PY:
- [X] codigo .py que chama o código do .cpp - Rafa
- [X] Teste em python (tests) - Rafa

Para validar:

1. Esteja em `math-calculus-project/` (a pasta raiz).
2. Compile extensão Python:

   ```bash
   python setup.py build_ext --inplace
   ```

3. Execute checks Python:

   ```bash
   ruff check .
   deal lint
   deal prove
   pyright
   pytest
   ```

4. Execute checks C++:

   ```bash
   clang-tidy
   cmake -S . -B build -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined"
   cmake --build build
   ctest --test-dir build
   ```

5. Para componente formal selecionado, execute a partir de `cpp/`:

   ```bash
   esbmc src/maximum.cpp
   ```

Quando terminarmos a v1, teremos encostado em todo o escopo do projeto:
(1) uma parte CPP (2) Uma parte Python (3) A integração entre elas (4) Os testes

Regra V2: marcar tarefa como concluída somente após checks relevantes passarem. Python exige `ruff check .`, `deal lint`, `deal prove`, `pyright` e `pytest`. C++ exige `clang-tidy`, build com sanitizers e `ctest`; `maximum.cpp` exige também ESBMC.

(V2) TDD das funções matematicas
TIME PYTHON e CPP:

- [x]  Definição da interface de chamada das funções (quais as entradas, quais as saídas, quais os erros)
TIME PYTHON:
- [x] Testes em Python das chamadas da funcionalidade (que executa os binarios)
  - [x] Integration
  - [x] Taylor
  - [x] Series
- [x] Códigos em Python para chamar as funções, de forma efetiva
TIME CPP:
- [x] Implementar os algoritmos de integration,series,taylor

TIMES:

1) Python: Rafa, Vitor, Sampaio, Amanda
2) CPP: Gabi, Joao, Manuel, Rodrigues
