"""Módulo de Contratos (Design by Contract - DbC) para Python.

Inspirado nas diretrizes e conceitos do Boost.Contract:
https://www.boost.org/doc/libs/latest/libs/contract/doc/html/index.html

Conceitos implementados:
- Pré-condições (Preconditions): obrigações do chamador antes da execução.
- Pós-condições (Postconditions): garantias do fornecedor após a execução bem-sucedida.
- Invariantes de Classe (Class Invariants): condições de integridade de estado.
"""

import functools
import inspect
from collections.abc import Callable
from typing import Any, TypeVar

F = TypeVar("F", bound=Callable[..., Any])


class ContractError(Exception):
    pass


class PreconditionError(ValueError, ContractError):
    pass


class PostconditionError(RuntimeError, ContractError):
    pass


class InvariantError(RuntimeError, ContractError):
    pass


def require(condition: bool, message: str = "Precondition failed") -> None:
    if not condition:
        raise PreconditionError(message)


def ensure(condition: bool, message: str = "Postcondition failed") -> None:
    if not condition:
        raise PostconditionError(message)


def check_invariant(condition: bool, message: str = "Invariant failed") -> None:
    if not condition:
        raise InvariantError(message)


def precondition(
    predicate: Callable[..., bool], message: str = "Precondition failed"
) -> Callable[[F], F]:
    def decorator(func: F) -> F:
        sig = inspect.signature(func)

        @functools.wraps(func)
        def wrapper(*args: Any, **kwargs: Any) -> Any:
            bound = sig.bind(*args, **kwargs)
            bound.apply_defaults()
            try:
                valid = predicate(**bound.arguments)
            except TypeError:
                valid = predicate(*args, **kwargs)
            if not valid:
                raise PreconditionError(message)
            return func(*args, **kwargs)

        return wrapper  # type: ignore

    return decorator


def postcondition(
    predicate: Callable[[Any], bool], message: str = "Postcondition failed"
) -> Callable[[F], F]:
    def decorator(func: F) -> F:
        @functools.wraps(func)
        def wrapper(*args: Any, **kwargs: Any) -> Any:
            result = func(*args, **kwargs)
            if not predicate(result):
                raise PostconditionError(message)
            return result

        return wrapper  # type: ignore

    return decorator


def invariant(
    predicate: Callable[[Any], bool], message: str = "Invariant failed"
) -> Callable[[type], type]:
    def decorator(cls: type) -> type:
        orig_init = cls.__init__

        @functools.wraps(orig_init)
        def wrapped_init(self: Any, *args: Any, **kwargs: Any) -> None:
            orig_init(self, *args, **kwargs)
            if not predicate(self):
                raise InvariantError(message)

        cls.__init__ = wrapped_init

        for attr_name, attr_value in list(cls.__dict__.items()):
            if callable(attr_value) and not attr_name.startswith("_"):

                def make_wrapped_method(
                    method: Callable[..., Any],
                ) -> Callable[..., Any]:
                    @functools.wraps(method)
                    def wrapped_method(self: Any, *args: Any, **kwargs: Any) -> Any:
                        if not predicate(self):
                            raise InvariantError(
                                f"Pre-call invariant violated: {message}"
                            )
                        res = method(self, *args, **kwargs)
                        if not predicate(self):
                            raise InvariantError(
                                f"Post-call invariant violated: {message}"
                            )
                        return res

                    return wrapped_method

                setattr(cls, attr_name, make_wrapped_method(attr_value))

        return cls

    return decorator
