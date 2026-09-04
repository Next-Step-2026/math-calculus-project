"""Módulo de interface e apresentação visual da CLI com Rich."""

import math
from collections.abc import Callable
from typing import Any

import typer
from rich import box
from rich.console import Console
from rich.panel import Panel
from rich.table import Table

from mathlab.contracts import (
    InvariantError,
    PostconditionError,
    PreconditionError,
)

console = Console()

AVAILABLE_FUNCTIONS: dict[str, tuple[Callable[[float], float], str]] = {
    "x^2": (lambda x: x**2, "x²"),
    "sin": (math.sin, "sin(x)"),
    "cos": (math.cos, "cos(x)"),
    "exp": (math.exp, "exp(x)"),
    "ln": (math.log, "ln(x)"),
    "sqrt": (math.sqrt, "sqrt(x)"),
}


def render_soma_result(a: float, b: float, resultado: float) -> None:
    """Renderiza o resultado da soma."""
    table = Table(title="[bold cyan]C++ : Soma Básica[/]", box=box.ROUNDED)
    table.add_column("Operando A", justify="right", style="cyan")
    table.add_column("Operando B", justify="right", style="cyan")
    table.add_column("Resultado C++", justify="right", style="bold green")
    table.add_row(f"{a:g}", f"{b:g}", f"{resultado:g}")
    console.print(table)


def render_series_result(title: str, res: Any) -> None:
    """Renderiza a tabela com o resultado do cálculo de séries numéricas."""
    table = Table(title=f"[bold cyan]{title}[/]", box=box.ROUNDED)
    table.add_column("Métrica", style="cyan")
    table.add_column("Valor", style="bold")

    status_str = "[green]Sim (Convergiu)[/]" if res.converged else "[red]Não (Divergiu)[/]"
    table.add_row("Status de Convergência", status_str)
    table.add_row("Soma Acumulada", f"[green]{res.sum:.8f}[/]")
    table.add_row("Iterações Executadas", str(res.iterations))
    console.print(table)


def render_integration_result(
    nome_f: str, a: float, b: float, n: int, method: str, resultado: float
) -> None:
    """Renderiza a tabela com o resultado da integração numérica."""
    table = Table(title="[bold cyan]C++ : Integração Numérica (Quadratura)[/]", box=box.ROUNDED)
    table.add_column("Parâmetro", style="cyan")
    table.add_column("Valor", style="bold green")

    table.add_row("Função Integrada f(x)", nome_f)
    table.add_row("Intervalo [a, b]", f"[{a:g}, {b:g}]")
    table.add_row("Número de Subintervalos (n)", str(n))
    table.add_row("Método de Quadratura", method.capitalize())
    table.add_row("Valor Aproximado", f"{resultado:.8f}")
    console.print(table)


def render_taylor_result(func_name: str, x: float, x0: float, order: int, valor: float) -> None:
    """Renderiza a tabela com o resultado da aproximação de Taylor."""
    table = Table(title="[bold cyan]C++ : Polinômio de Taylor[/]", box=box.ROUNDED)
    table.add_column("Parâmetro", style="cyan")
    table.add_column("Valor", style="bold green")

    table.add_row("Função Base", func_name)
    table.add_row("Ponto de Avaliação (x)", f"{x:g}")
    table.add_row("Centro de Expansão (x0)", f"{x0:g}")
    table.add_row("Ordem do Polinômio (n)", str(order))
    table.add_row("Aproximação Polinomial", f"{valor:.10f}")
    console.print(table)


def render_error(err: Exception, exit_on_error: bool = False) -> None:
    """Apresenta mensagens de erro estilizadas com suporte aos contratos DbC."""
    if isinstance(err, PreconditionError):
        title = "[bold red]Violação de Pré-condição (Contrato DbC)[/]"
        msg = f"[yellow]{err}[/]"
    elif isinstance(err, (PostconditionError, InvariantError)):
        title = "[bold red]Violação de Integridade Numérica (Contrato DbC)[/]"
        msg = f"[red]{err}[/]"
    else:
        title = "[bold red]Erro de Execução[/]"
        msg = f"[white]{err}[/]"

    console.print(Panel(msg, title=title, border_style="red", expand=False))
    if exit_on_error:
        raise typer.Exit(code=1)
