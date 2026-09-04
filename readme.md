# Math Calculus Project (MathLab)

Biblioteca modular de cálculo numérico com núcleo de alta performance em C++17 e fachada de alto nível em Python via `pybind11`.

---

## 🚀 Quickstart

### 1. Pré-requisitos
- Python 3.10+
- Compilador C++17 (`g++` ou `clang++`)
- CMake 3.15+

### 2. Instalação e Compilação
```bash
# Ativar virtualenv
source .venv/bin/activate

# Compilar extensão nativa (o setup baixa os headers da Boost sob demanda sem inflar o Git)
python setup.py build_ext --inplace
```

### 3. Execução dos Testes
```bash
# Suíte Python
pytest

# Suíte C++ via CMake
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

---

## 📚 Documentação

- [Guia de Configuração e Ambiente](docs/setup_guide.md): Instruções detalhadas de setup, política anti-inflação do Git e integração com a Boost.Contract.
- [Resumo da Arquitetura e Módulos](docs/resumo.md): Descrição dos componentes de integração, séries e Taylor.
- [Objetivos Técnicos do Workshop](OBJECTIVE.md): Padrões de projeto (Facade, Strategy, Adapter), guard clauses e métricas de qualidade.
- [Backlog e Tarefas](TODO.md): Lista de tarefas e status das entregas.
