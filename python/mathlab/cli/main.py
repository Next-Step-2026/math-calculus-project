"""Definição de comandos e fluxos da CLI do Mathlab com Typer."""

from enum import Enum
from typing import Annotated

import typer
from rich.panel import Panel
from rich.prompt import FloatPrompt, IntPrompt, Prompt

import mathlab
from mathlab.cli.ui import (
    AVAILABLE_FUNCTIONS,
    console,
    render_error,
    render_integration_result,
    render_series_result,
    render_soma_result,
    render_taylor_result,
)
from mathlab.contracts import ContractError

_CLI_EXCEPTIONS = (ContractError, ValueError, TypeError, RuntimeError, ZeroDivisionError)

app = typer.Typer(
    help="CLI moderna para cálculo numérico C++ com fachada Python (Mathlab).",
    no_args_is_help=False,
)
series_app = typer.Typer(help="Comandos para convergência de séries numéricas.")
app.add_typer(series_app, name="series")


class IntegrationMethod(str, Enum):
    simpson = "simpson"
    trapezoidal = "trapezoidal"


class TaylorFunction(str, Enum):
    exp = "exp"
    sin = "sin"
    cos = "cos"
    ln = "ln"


class PredefinedFunction(str, Enum):
    x2 = "x^2"
    sin = "sin"
    cos = "cos"
    exp = "exp"
    ln = "ln"
    sqrt = "sqrt"


@app.command()
def soma(
    a: Annotated[float, typer.Argument(help="Primeiro número (operando A)")],
    b: Annotated[float, typer.Argument(help="Segundo número (operando B)")],
) -> None:
    """Calcula a soma de dois números utilizando o núcleo C++ (soma.cpp)."""
    try:
        resultado = mathlab.soma(a, b)
        render_soma_result(a, b, resultado)
    except _CLI_EXCEPTIONS as err:
        render_error(err, exit_on_error=True)


@series_app.command("geometric")
def series_geometric(
    a: Annotated[float, typer.Option(help="Termo inicial a")] = 1.0,
    r: Annotated[float, typer.Option(help="Razão r (|r| < 1.0)")] = 0.5,
    eps: Annotated[float, typer.Option(help="Tolerância de convergência")] = 1e-7,
    n_max: Annotated[int, typer.Option(help="Número máximo de iterações")] = 10000,
) -> None:
    """Calcula a soma de uma série geométrica sum(a * r^k) via C++."""
    try:
        resultado = mathlab.compute_series(a, r, eps, n_max)
        render_series_result(f"Série Geométrica: a={a:g}, r={r:g}", resultado)
    except _CLI_EXCEPTIONS as err:
        render_error(err, exit_on_error=True)


@series_app.command("p-series")
def series_p_series(
    p: Annotated[float, typer.Option(help="Expoente p (p > 1.0)")] = 2.0,
    eps: Annotated[float, typer.Option(help="Tolerância de convergência")] = 1e-6,
    n_max: Annotated[int, typer.Option(help="Número máximo de iterações")] = 10000,
) -> None:
    """Calcula a aproximação de uma p-série sum(1 / k^p) via C++."""
    try:
        resultado = mathlab.compute_p_series(p, eps, n_max)
        render_series_result(f"p-Série: p={p:g}", resultado)
    except _CLI_EXCEPTIONS as err:
        render_error(err, exit_on_error=True)


@app.command()
def integrate(
    func: Annotated[
        PredefinedFunction,
        typer.Option("--func", "-f", help="Função a integrar (x^2, sin, cos, exp, ln, sqrt)"),
    ] = PredefinedFunction.x2,
    a: Annotated[float, typer.Option(help="Limite inferior de integração")] = 0.0,
    b: Annotated[float, typer.Option(help="Limite superior de integração")] = 3.0,
    n: Annotated[int, typer.Option(help="Número de subintervalos")] = 100,
    method: Annotated[
        IntegrationMethod,
        typer.Option("--method", "-m", help="Método de quadratura ('simpson' ou 'trapezoidal')"),
    ] = IntegrationMethod.simpson,
) -> None:
    """Calcula a integral definida via quadratura numérica em C++."""
    try:
        fn, nome_f = AVAILABLE_FUNCTIONS[func.value]
        resultado = mathlab.integrate(fn, a, b, n, method=method.value)
        render_integration_result(nome_f, a, b, n, method.value, resultado)
    except _CLI_EXCEPTIONS as err:
        render_error(err, exit_on_error=True)


@app.command()
def taylor(
    func: Annotated[
        TaylorFunction,
        typer.Option("--func", "-f", help="Função analítica (exp, sin, cos, ln)"),
    ] = TaylorFunction.exp,
    x: Annotated[float, typer.Option(help="Ponto de avaliação")] = 1.0,
    x0: Annotated[float, typer.Option(help="Centro de expansão")] = 0.0,
    order: Annotated[int, typer.Option("--order", "-o", help="Ordem do polinômio (0 a 20)")] = 10,
) -> None:
    """Calcula a aproximação por Polinômio de Taylor via Horner em C++."""
    try:
        valor = mathlab.approximate_taylor(func.value, x, x0, order)
        render_taylor_result(func.value, x, x0, order, valor)
    except _CLI_EXCEPTIONS as err:
        render_error(err, exit_on_error=True)


def _interactive_soma() -> None:
    console.print("\n[bold cyan]--- [C++] Teste de Soma Básica ---[/]")
    try:
        a = FloatPrompt.ask("Digite o primeiro número", default=0.0)
        b = FloatPrompt.ask("Digite o segundo número", default=0.0)
        res = mathlab.soma(a, b)
        render_soma_result(a, b, res)
    except _CLI_EXCEPTIONS as err:
        render_error(err, exit_on_error=False)


def _interactive_series() -> None:
    console.print("\n[bold cyan]--- [C++] Teste de Séries Numéricas ---[/]")
    console.print("  [cyan]1[/] - Série Geométrica sum(a * r^k)")
    console.print("  [cyan]2[/] - p-Série sum(1 / k^p)")
    sub = Prompt.ask("Escolha o tipo", choices=["1", "2"], default="1")

    try:
        if sub == "1":
            a = FloatPrompt.ask("Termo inicial (a)", default=1.0)
            r = FloatPrompt.ask("Razão (|r| < 1)", default=0.5)
            eps = FloatPrompt.ask("Tolerância eps", default=1e-7)
            n_max = IntPrompt.ask("Iterações máximas", default=10000)
            res = mathlab.compute_series(a, r, eps, n_max)
            render_series_result(f"Série Geométrica: a={a:g}, r={r:g}", res)
        else:
            p = FloatPrompt.ask("Expoente p (p > 1.0)", default=2.0)
            eps = FloatPrompt.ask("Tolerância eps", default=1e-6)
            n_max = IntPrompt.ask("Iterações máximas", default=10000)
            res = mathlab.compute_p_series(p, eps, n_max)
            render_series_result(f"p-Série: p={p:g}", res)
    except _CLI_EXCEPTIONS as err:
        render_error(err, exit_on_error=False)


def _interactive_integracao() -> None:
    console.print("\n[bold cyan]--- [C++] Teste de Integração Numérica (Quadratura) ---[/]")
    console.print("Funções pré-definidas disponíveis:")
    keys = list(AVAILABLE_FUNCTIONS.keys())
    for idx, k in enumerate(keys, start=1):
        _, desc = AVAILABLE_FUNCTIONS[k]
        console.print(f"  [cyan]{idx}[/] - f(x) = {desc}")

    escolha_idx = Prompt.ask(
        "Escolha a função",
        choices=[str(i) for i in range(1, len(keys) + 1)],
        default="1",
    )
    selected_key = keys[int(escolha_idx) - 1]
    fn, nome_f = AVAILABLE_FUNCTIONS[selected_key]

    try:
        a = FloatPrompt.ask("Limite inferior (a)", default=0.0)
        b = FloatPrompt.ask("Limite superior (b)", default=3.0)
        n = IntPrompt.ask("Número de subintervalos n", default=100)
        metodo = Prompt.ask(
            "Método",
            choices=["simpson", "trapezoidal"],
            default="simpson",
        )
        res = mathlab.integrate(fn, a, b, n, method=metodo)
        render_integration_result(nome_f, a, b, n, metodo, res)
    except _CLI_EXCEPTIONS as err:
        render_error(err, exit_on_error=False)


def _interactive_taylor() -> None:
    console.print("\n[bold cyan]--- [C++] Teste de Aproximação de Taylor ---[/]")
    func_name = Prompt.ask(
        "Função analítica",
        choices=["exp", "sin", "cos", "ln"],
        default="exp",
    )
    try:
        x = FloatPrompt.ask("Ponto de avaliação x", default=1.0)
        x0 = FloatPrompt.ask("Centro de expansão x0", default=0.0)
        ordem = IntPrompt.ask("Ordem do polinômio (0 a 20)", default=10)
        res = mathlab.approximate_taylor(func_name, x, x0, ordem)
        render_taylor_result(func_name, x, x0, ordem, res)
    except _CLI_EXCEPTIONS as err:
        render_error(err, exit_on_error=False)


@app.command()
def interactive() -> None:
    """Abre o menu interativo com prompts guiados e renderização avançada."""
    while True:
        menu_text = (
            "[bold cyan]1[/] - Testar Soma (cpp/src/soma.cpp)\n"
            "[bold cyan]2[/] - Testar Séries Numéricas (cpp/src/series.cpp)\n"
            "[bold cyan]3[/] - Testar Integração Numérica (cpp/src/integration.cpp)\n"
            "[bold cyan]4[/] - Testar Polinômio de Taylor (cpp/src/taylor.cpp)\n"
            "[bold red]0[/] - Sair"
        )
        console.print(
            Panel(
                menu_text,
                title="[bold green]PAINEL INTERATIVO: NÚCLEO C++ VIA PYTHON[/]",
                border_style="bright_blue",
                expand=False,
            )
        )

        opcao = Prompt.ask("Digite a opção desejada", choices=["1", "2", "3", "4", "0"], default="0")
        if opcao == "1":
            _interactive_soma()
        elif opcao == "2":
            _interactive_series()
        elif opcao == "3":
            _interactive_integracao()
        elif opcao == "4":
            _interactive_taylor()
        elif opcao == "0":
            console.print("\n[bold yellow]Encerrando painel.[/]")
            break


@app.callback(invoke_without_command=True)
def main_callback(ctx: typer.Context) -> None:
    """Ponto de entrada callback: executa o menu interativo quando nenhum comando for especificado."""
    if ctx.invoked_subcommand is None:
        interactive()
