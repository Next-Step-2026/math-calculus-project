Nosso projeto basicamente consiste em coordenar duas equipes (uma em python e uma em cpp) para criar uma interface matematica. 

A parte matematica sera em cpp, enquanto a interface e coordenacao de testes sera em PYTHON.

Para isso temos algumas tarefas:

(V1) Código simples para testar integrações
TIME CPP:
- [X] codigo .cpp que recebe dois numeros e retorna a soma deles (receber inputs e outputs)- Joao
TIME PY:
- [X] codigo .py que chama o código do .cpp - Rafa
- [X] Teste em python (tests) - Rafa

Para testar:
1. Esteja em `math-calculus-project/` (a pasta raiz)
2. python setup.py build_ext --inplace
3. pytest

Quando terminarmos a v1, teremos encostado em todo o escopo do projeto:
(1) uma parte CPP (2) Uma parte Python (3) A integração entre elas (4) Os testes

(V2) TDD das funções matematicas
TIME PYTHON e CPP:
- [ ]  Definição da interface de chamada das funções (quais as entradas, quais as saídas, quais os erros)
TIME PYTHON:
- [ ] Testes em Python das chamadas da funcionalidade (que executa os binarios)
- [ ] Códigos em Python para chamar as funções, de forma efetiva
(precisa ser quebrado em subtarefas)
TIME CPP:
- [ ] Implementar os algoritmos de integration,series,taylor  (precisa ser quebrado em subtarefas)

TIMES:
1) Python: Rafa, Vitor
2) CPP: Gabi, Joao, Manuel