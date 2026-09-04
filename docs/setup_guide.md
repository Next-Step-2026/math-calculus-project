# Guia de Configuração e Ambiente — Math Calculus Project

Este guia orienta o setup do ambiente de desenvolvimento, instalação de dependências, compilação dos módulos FFI e execução dos testes e verificações de qualidade.

---

## 1. Pré-requisitos do Sistema

- **Python:** Versão 3.10 ou superior (recomendado 3.13 com virtualenv ativo).
- **C++:** Compilador com suporte completo a **C++17** (`g++ >= 9.0` ou `clang++ >= 10.0`).
- **CMake:** Versão 3.15 ou superior (`cmake --version`).
- **Git:** Para controle de versão e submissão de alterações.

---

## 2. Dependências C++ e Boost.Contract

O núcleo numérico em C++ utiliza a biblioteca **Boost.Contract** (`<boost/contract.hpp>`) para implementar *Design by Contract* (pré-condições, pós-condições e invariantes de classe).

### 2.1 Política Anti-Inflação do Git
Para evitar inflar o histórico do repositório Git com os ~140 MB e dezenas de milhares de arquivos da Boost, o projeto adota as seguintes regras:
1. **Pasta ignorada no Git:** `cpp/include/boost/` está explicitamente incluída no `.gitignore`.
2. **Modo Header-Only:** A Boost.Contract opera automaticamente em modo *header-only* quando macros de link dinâmico não são passadas. Isso elimina a necessidade de compilar ou linkar bibliotecas `.so` adicionais, prevenindo problemas de `ImportError` em runtime no Python.
3. **Instalação Automática Sob Demanda:**
   - O arquivo `setup.py` detecta automaticamente se `cpp/include/boost/contract.hpp` está ausente e dispara o download transparente.
   - Alternativamente, o download e extração rápida dos cabeçalhos podem ser executados a qualquer momento pelo script dedicado:
     ```bash
     python3 scripts/fetch_boost.py
     ```

---

## 3. Passo a Passo de Instalação e Build

### 3.1 Clonar o Repositório e Ativar o Ambiente Virtual

```bash
git clone <URL_DO_REPOSITORIO>
cd math-calculus-project

# Ativar virtualenv existente ou criar um novo:
source .venv/bin/activate
```

### 3.2 Instalar Dependências Python

```bash
pip install -r requirements.txt
# ou caso utilize uv:
uv sync
```

### 3.3 Compilar Extensões C++ via Python (In-Place)

Este comando compila as extensões `_mathcore` e `_soma_cpp` diretamente na árvore do pacote Python:

```bash
python setup.py build_ext --inplace
```

*(Se a pasta `cpp/include/boost/` ainda não existir, o script fará o download e extração automaticamente antes de compilar).*

### 3.4 Compilação Alternativa via CMake

Para compilar e gerar alvos nativos de teste C++ com sanitizers:

```bash
cmake -S . -B build -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined"
cmake --build build
```

---

## 4. Execução de Testes e Checks de Qualidade

### 4.1 Bateria de Testes em Python (`pytest`)

Executa a suíte de testes de integração, séries, Taylor e soma:

```bash
pytest
```

Para saída detalhada:
```bash
pytest -v
```

### 4.2 Linter e Formatador Python (`ruff`)

Garante conformidade de estilo, imports e ausência de complexidade ciclomática excessiva (C901):

```bash
ruff check .
ruff format --check .
```

### 4.3 Testes Nativos C++ (`ctest`)

Executa a suíte registrada no CMake:

```bash
ctest --test-dir build --output-on-failure
```

### 4.4 Verificação Formal Limitada (ESBMC)

Para verificação formal dos contratos e limites do componente `maximum.cpp`:

```bash
cd cpp
esbmc src/maximum.cpp
cd ..
```

---

## 5. Como Utilizar Contratos em C++

Você pode incluir `<boost/contract.hpp>` diretamente nos arquivos em `cpp/include/` ou `cpp/src/`:

```cpp
#include <boost/contract.hpp>

double calcular_quadratura(double a, double b, int n) {
    boost::contract::check c = boost::contract::function()
        .precondition([&] {
            BOOST_CONTRACT_ASSERT(a <= b);
            BOOST_CONTRACT_ASSERT(n > 0);
        })
        .postcondition([&] {
            // checagens de pós-condição
        });

    // Lógica do algoritmo puro
    return 0.0;
}
```

O projeto também disponibiliza asserções leves em `cpp/include/contracts.hpp` (`contracts::Expects`, `contracts::Ensures`, `contracts::Invariant`).
