#!/usr/bin/env python3
"""Ponto de entrada executável para a CLI do Mathlab."""

import sys
from pathlib import Path

# Assegura que o diretório python/ esteja no sys.path para execução direta da raiz
_ROOT = Path(__file__).resolve().parent
_PY_DIR = _ROOT / "python"
if _PY_DIR.exists() and str(_PY_DIR) not in sys.path:
    sys.path.insert(0, str(_PY_DIR))

from mathlab.cli.main import app

if __name__ == "__main__":
    app()
