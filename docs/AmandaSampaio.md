# Guia de Trabalho e Estado do Projeto — Amanda Sampaio & Time Python

**Data:** 03/09/2026  
**Escopo:** Camada Python (`mathlab`), Integração FFI e Testes (`pytest`)  
**Documento de Referência:** `OBJECTIVE.md`

---

## 1. Visão Geral da Arquitetura

O projeto adota arquitetura híbrida:
1. **Núcleo Numérico (C++17):** Funções puras de alta performance em `cpp/src/` e contratos em `cpp/include/`.
2. **Ponte FFI (`pybind11`):** Módulo nativo `mathlab._mathcore` gerado via `cpp/bindings.cpp` e `setup.py`.
3. **Fachada Pública (Python 3.10+):** `python/mathlab/api.py` atua como **Facade** e barreira de proteção defensiva (**Guard Clauses**).
4. **Bateria de Testes (`pytest`):** Cobertura nominal, de borda e de exceções em `tests/`.

```text
Usuário / Testes (Python)
       │
       ▼
[python/mathlab/api.py]   -> Fachada + Validação Defensiva (Guard Clauses)
       │ (dados validados)
       ▼
[_mathcore]              -> FFI pybind11 (tradução tipos & C++ exceptions -> Python exceptions)
       │
       ▼
[cpp/src/*.cpp]          -> Motor matemático puro C++17 (sem estado, sem alocação dinâmica)
```

---

## 2. O Que Já Foi Feito

### 2.1 Prova de Conceito e Setup (V1)
- Pipeline inicial de integração C++ ↔ Python com `pybind11` e `setup.py`.
- Módulo `soma` funcional de ponta a ponta:
  - `cpp/src/soma.cpp` + `cpp/include/soma.hpp`
  - `python/mathlab/soma.py`
  - `tests/test_soma.py` (passando)

### 2.2 Núcleo C++ (Time C++)
- **Integração (`cpp/src/integration.cpp`):**
  - Implementadas regras dos Trapézios e Simpson 1/3 composta.
  - Funções de validação de argumentos nativas (`validate_trapezoidal_parameters`, `validate_simpson_parameters`).
  - Estrutura de retorno `IntegrationResult`.
- **Séries (`cpp/src/series.cpp`):**
  - Implementada soma de série geométrica `computarSerie`.
  - Verificação de parâmetros `verify_parameters` (validação de $\epsilon > 0$, $N_{max} \ge 1$, e divergência para $|r| \ge 1.0$).
  - Estrutura de retorno `ResultadoSerie`.
- **Taylor (`cpp/src/taylor.cpp`):**
  - Classe `TaylorPolynomial` com avaliação via Algoritmo de Horner $O(n)$.
  - Enum `Function` (`Exp`, `Sin`, `Cos`, `Ln`).
  - Função pública `approximate_taylor`.

### 2.3 Camada Python e Testes (Time Python)
- `python/mathlab/api.py`:
  - Fachadas `integrate_simpson` e `integrate_trapezoidal` chamando `_mathcore`.
- `tests/test_integration.py`:
  - 5 testes automatizados cobrindo casos básicos (integral de $x$ e $x^2$), limites coincidentes ($a = b$) e disparo de `ValueError` para intervalos inválidos e $n$ ímpar em Simpson.
  - Testes passando via `pytest` (`6 passed`).

---

## 3. Ponto de Atenção Crítico: Desalinhamento C++ ↔ Bindings

Existe um binário compilado antigo `python/mathlab/_mathcore.*.so` que faz os testes de integração passarem hoje, mas o código atual **não compila** se rodar `python setup.py build_ext --inplace`.

**Causa:**
1. `cpp/bindings.cpp` e os arquivos de cabeçalho C++ divergiram em nomes e tipos:
   - Bindings esperam `SeriesResult` e `compute_series`, mas C++ implementou `ResultadoSerie` e `computarSerie`.
   - Bindings esperam `TaylorResult` global e funções soltas, mas Taylor foi encapsulado no namespace `taylor::` e enum `taylor::Function`.
2. `setup.py` só referencia `cpp/src/integration.cpp`, deixando de fora `series.cpp` e `taylor.cpp`.

> **Ação Conjunta com Time C++:** Ajustar `cpp/bindings.cpp` e `setup.py` para exportar os três domínios com contratos padronizados em inglês conforme `OBJECTIVE.md`.

--- 

## 4. O Que Falta Fazer (Backlog do Time Python)

### Tarefa 1: Implementar Fachada Completa em `python/mathlab/api.py`
Substituir os stubs (`pass`) e aplicar os padrões de projeto exigidos pelo `OBJECTIVE.md`:

1. **Guard Clauses e Validação Defensiva (§4 e §8.2):**
   - **Regra:** Nunca deixar condicionais aninhadas (*nested ifs*). Validar no topo da função e fazer *early return* ou disparar `ValueError`/`TypeError`.
   - `integrate_simpson` e `integrate_trapezoidal`:
     - Validar se `func` é chamável (`callable`).
     - Validar $n$ como inteiro estritamente positivo ($n \ge 1$).
     - Em Simpson, validar $n$ par ($n \% 2 == 0$).
   - `compute_series(a, r, eps=1e-7, n_max=10000)`:
     - Validar se $\epsilon > 0$ (lançar `ValueError`).
     - Validar se $n_{max} \ge 1$ (lançar `ValueError`).
     - Validar se $|r| < 1.0$ (se a série geométrica divergir matematicamente, lançar `ValueError` ou retornar status claro).
   - `approximate_taylor(func_name, x, x0, order)`:
     - Validar $0 \le \text{order} \le 20$ (lançar `ValueError`).
     - Validar se `func_name` pertence a `{"exp", "sin", "cos", "ln"}` (lançar `ValueError`).
     - Se `func_name == "ln"`, validar domínio do logaritmo natural ($x > 0$ e $x_0 > 0$).
2. **Tipagem Estática Estrita:**
   - Adicionar Type Hints em todas as entradas e saídas de `api.py`.
3. **Padrão Strategy para Integração (Opcional/Recomendado por §4):**
   - Criar função unificada `integrate(func, a, b, n, method="simpson")` que delega para a estratégia solicitada.
4. **Exportação Pública (`__all__`):**
   - Atualizar `__all__` em `python/mathlab/api.py` e em `python/mathlab/__init__.py`.

---

### Tarefa 2: Criar Bateria de Testes em `tests/`
O `OBJECTIVE.md` exige testes 100% verdes com cenários nominais, de borda e de exceção:

1. **`tests/test_series.py`:**
   - *Nominal:* Séries conhecidas analiticamente (ex: $a=1, r=0.5 \Rightarrow S = \frac{a}{1-r} = 2.0$; $a=3, r=1/3 \Rightarrow S = 4.5$).
   - *Borda:* $N_{max} = 1$, tolerâncias finas ($\epsilon = 10^{-10}$).
   - *Exceções:* `pytest.raises(ValueError)` para $\epsilon \le 0$, $N_{max} < 1$ e $|r| \ge 1.0$.
2. **`tests/test_taylor.py`:**
   - *Nominal:* Comparar aproximação de $e^x$, $\sin(x)$, $\cos(x)$ contra biblioteca padrão `math` em pontos próximos a $x_0$ com `pytest.approx`.
   - *Borda:* Ordem $n = 0$, ponto de avaliação exatamente igual ao centro $x = x_0$.
   - *Exceções:* `pytest.raises(ValueError)` para ordem negativa ($n < 0$), ordem excessiva ($n > 20$) e função não suportada (ex: `"tan"`).
3. **`tests/test_integration.py` (Refinamento):**
   - Adicionar testes de funções trigonométricas e polinomiais com valor analítico exato conhecido.

---

### Tarefa 3: Qualidade de Código e Conformidade de Linters (§8.2 e §11)
- Rodar o linter **Ruff** no código Python:
  ```bash
  ruff check python/ tests/
  ruff format python/ tests/
  ```
- Garantir complexidade ciclomática baixa (sem alertas C901).
- Garantir ausência total de falhas silenciosas (não retornar `None`, `0.0` ou `NaN` em caso de erro de entrada).

---

## 5. Roteiro Passo a Passo de Execução

1. **Alinhamento do binding com Time C++:**
   - Definir se no Python vamos chamar `mathlab.compute_series` e `mathlab.approximate_taylor`.
   - Corrigir `cpp/bindings.cpp` e `setup.py`.
   - Recompilar com `./.venv/bin/python setup.py build_ext --inplace`.
2. **Desenvolvimento Orientado a Testes (TDD):**
   - Escrever `tests/test_series.py` e `tests/test_taylor.py` com os cenários esperados.
   - Implementar validações defensivas e chamadas FFI em `python/mathlab/api.py`.
3. **Validação Contínua:**
   - Executar `./.venv/bin/pytest`.
   - Executar análise com `ruff`.
