"""Bateria de testes automatizados para a interface de linha de comando (CLI)."""

from mathlab.cli.main import app
from typer.testing import CliRunner

runner = CliRunner()


def test_cli_help() -> None:
    """Verifica se a tela de ajuda da CLI é renderizada corretamente."""
    result = runner.invoke(app, ["--help"])
    assert result.exit_code == 0
    assert "soma" in result.output
    assert "series" in result.output
    assert "integrate" in result.output
    assert "taylor" in result.output
    assert "interactive" in result.output


def test_cli_soma_success() -> None:
    """Testa execução de soma com argumentos nominais."""
    result = runner.invoke(app, ["soma", "15.5", "4.5"])
    assert result.exit_code == 0
    assert "20" in result.output


def test_cli_soma_invalid_arg() -> None:
    """Testa tratamento de erro de conversão de tipos na CLI."""
    result = runner.invoke(app, ["soma", "invalido", "4.5"])
    assert result.exit_code != 0


def test_cli_series_geometric_success() -> None:
    """Testa comando series geometric convergente."""
    result = runner.invoke(app, ["series", "geometric", "--a", "1.0", "--r", "0.5"])
    assert result.exit_code == 0
    assert "Sim (Convergiu)" in result.output
    assert "1.9999" in result.output


def test_cli_series_geometric_divergent_precondition() -> None:
    """Testa captura de violação de pré-condição com razão |r| >= 1."""
    result = runner.invoke(app, ["series", "geometric", "--r", "1.5"])
    assert result.exit_code == 1
    assert "Violação de Pré-condição" in result.output
    assert "Geometric series diverges for |r| >= 1.0" in result.output


def test_cli_series_pseries_success() -> None:
    """Testa comando series p-series convergente."""
    result = runner.invoke(app, ["series", "p-series", "--p", "2.0"])
    assert result.exit_code == 0
    assert "Sim (Convergiu)" in result.output
    assert "1.64" in result.output


def test_cli_series_pseries_divergent_precondition() -> None:
    """Testa captura de violação de pré-condição com p <= 1.0."""
    result = runner.invoke(app, ["series", "p-series", "--p", "0.5"])
    assert result.exit_code == 1
    assert "Violação de Pré-condição" in result.output
    assert "p-series diverges for p <= 1.0" in result.output


def test_cli_integrate_simpson_success() -> None:
    """Testa comando integrate com método Simpson."""
    result = runner.invoke(
        app,
        ["integrate", "--func", "x^2", "--a", "0.0", "--b", "3.0", "--n", "10", "--method", "simpson"],
    )
    assert result.exit_code == 0
    assert "9.0000" in result.output
    assert "Simpson" in result.output


def test_cli_integrate_simpson_odd_intervals_precondition() -> None:
    """Testa captura de erro DbC quando n é ímpar para Simpson."""
    result = runner.invoke(
        app,
        ["integrate", "--func", "sin", "--a", "0.0", "--b", "3.0", "--n", "11", "--method", "simpson"],
    )
    assert result.exit_code == 1
    assert "Violação de Pré-condição" in result.output
    assert "Simpson requires an even number of intervals" in result.output


def test_cli_integrate_trapezoidal_success() -> None:
    """Testa comando integrate com método trapezoidal."""
    result = runner.invoke(
        app,
        ["integrate", "--func", "exp", "--a", "0.0", "--b", "1.0", "--n", "50", "--method", "trapezoidal"],
    )
    assert result.exit_code == 0
    assert "Trapezoidal" in result.output


def test_cli_taylor_exp_success() -> None:
    """Testa comando taylor para função exponencial."""
    result = runner.invoke(app, ["taylor", "--func", "exp", "--x", "1.0", "--order", "10"])
    assert result.exit_code == 0
    assert "2.71828" in result.output


def test_cli_taylor_invalid_order_precondition() -> None:
    """Testa captura de erro DbC para ordem superior ao permitido (> 20)."""
    result = runner.invoke(app, ["taylor", "--func", "exp", "--order", "25"])
    assert result.exit_code == 1
    assert "Violação de Pré-condição" in result.output
    assert "order must be between 0 and 20" in result.output


def test_cli_interactive_exit() -> None:
    """Testa saída graciosa no modo interativo."""
    result = runner.invoke(app, ["interactive"], input="0\n")
    assert result.exit_code == 0
    assert "Encerrando painel." in result.output


def test_cli_interactive_soma_workflow() -> None:
    """Testa fluxo completo de soma via modo interativo."""
    result = runner.invoke(app, ["interactive"], input="1\n7.5\n2.5\n0\n")
    assert result.exit_code == 0
    assert "C++ : Soma Básica" in result.output
    assert "10" in result.output
    assert "Encerrando painel." in result.output
